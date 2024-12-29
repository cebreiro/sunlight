#pragma once
#include "tbb/concurrent_hash_map.h"
#include "sl/data/sox/sox_table_interface.h"

namespace sunlight
{
    class SoxTableSet
    {
    public:
        SoxTableSet() = delete;
        SoxTableSet(const SoxTableSet& other) = delete;
        SoxTableSet(SoxTableSet&& other) noexcept = delete;
        SoxTableSet& operator=(const SoxTableSet& other) = delete;
        SoxTableSet& operator=(SoxTableSet&& other) noexcept = delete;

    public:
        explicit SoxTableSet(const std::filesystem::path& path);
        ~SoxTableSet();

        void Clear();

        template <typename T> requires std::derived_from<T, sox::ISoxTable>
        auto Get() const -> const T&;

    private:
        template <typename T> requires std::derived_from<T, sox::ISoxTable>
        void LoadSoxTable();

    private:
        std::filesystem::path _path;

        tbb::concurrent_hash_map<int64_t, SharedPtrNotNull<sox::ISoxTable>> _tables;
    };

    template <typename T> requires std::derived_from<T, sox::ISoxTable>
    auto SoxTableSet::Get() const -> const T&
    {
        const int64_t id = RuntimeTypeId<SoxTableSet>::Get<T>();

        std::shared_ptr<sox::ISoxTable> target;
        {
            decltype(_tables)::accessor accessor;

            if (_tables.find(accessor, id))
            {
                target = accessor->second;
            }
        }

        if (target)
        {
            const T* item = static_cast<const T*>(target.get());
            assert(dynamic_cast<const T*>(target.get()));

            return *item;
        }

        // lazy load
        const_cast<SoxTableSet*>(this)->LoadSoxTable<T>();

        return Get<T>();
    }

    template <typename T> requires std::derived_from<T, sox::ISoxTable>
    void SoxTableSet::LoadSoxTable()
    {
        const int64_t id = RuntimeTypeId<SoxTableSet>::Get<T>();

        auto table = std::make_shared<T>();
        table->LoadFromFile(_path);

        decltype(_tables)::accessor accessor;

        if (_tables.insert(accessor, id))
        {
            accessor->second = std::move(table);
        }
    }
}
