#include "stored_procedure.h"

namespace sunlight
{
    tbb::concurrent_unordered_map<std::string, std::optional<boost::mysql::statement>> StoredProcedure::_preparedStatements;

    StoredProcedure::StoredProcedure(db::ConnectionPool::Borrowed& conn)
        : _conn(conn)
    {
    }

    auto StoredProcedure::ExecuteAsync() noexcept -> Future<DatabaseError>
    {
        try
        {
            std::string_view sql = this->GetSQL();

            std::optional<boost::mysql::statement>& stmt = _preparedStatements[MakeKey(*_conn, sql)];
            if (!stmt.has_value())
            {
                stmt = _conn->prepare_statement(sql);
            }

            Promise<boost::mysql::error_code> promise;
            Future<boost::mysql::error_code> future = promise.GetFuture();

            const boost::container::small_vector<boost::mysql::field, 16>& input = this->GetInput();

            _conn->async_execute(stmt->bind(input.begin(), input.end()), _executeResult, _dbDiagnostics,
                [p = std::move(promise)](boost::mysql::error_code ec) mutable
                {
                    p.Set(std::move(ec));
                });

            const boost::system::error_code ec = co_await future;
            throw_on_error(ec, _dbDiagnostics);

            this->SetOutput(_executeResult);

        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            co_return DatabaseError(e);
        }
        catch (const std::exception& e)
        {
            co_return DatabaseError(e);
        }
        catch (...)
        {
            co_return DatabaseError();
        }

        co_return {};
    }

    auto StoredProcedure::MakeKey(const boost::mysql::tcp_ssl_connection& connection,
        std::string_view sql) -> std::string
    {
        return fmt::format("{}_{}", sql, reinterpret_cast<size_t>(&connection));
    }

    namespace
    {
        auto ExecuteAsync(db::ConnectionPool::Borrowed& conn, std::string_view sql) -> Future<DatabaseError>
        {
            boost::mysql::diagnostics diagnostics = {};
            boost::mysql::results executeResult = {};

            Promise<boost::system::error_code> promise;
            Future<boost::system::error_code> future = promise.GetFuture();

            conn->async_execute(sql, executeResult, diagnostics,
                [p = std::move(promise)](boost::system::error_code ec) mutable
                {
                    p.Set(ec);
                });
            boost::system::error_code ec = co_await future;

            try
            {
                throw_on_error(ec, diagnostics);
            }
            catch (const boost::mysql::error_with_diagnostics& e)
            {
                co_return DatabaseError(e);
            }

            co_return{};
        }
    }

    auto StartTransaction(db::ConnectionPool::Borrowed& conn) -> Future<DatabaseError>
    {
        return ExecuteAsync(conn, "START TRANSACTION");
    }

    auto Commit(db::ConnectionPool::Borrowed& conn) -> Future<DatabaseError>
    {
        return ExecuteAsync(conn, "COMMIT");
    }

    auto Rollback(db::ConnectionPool::Borrowed& conn) -> Future<DatabaseError>
    {
        return ExecuteAsync(conn, "ROLLBACK");
    }
}
