#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct ServerListInternal
    {
        explicit ServerListInternal(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string name = {};
        std::string desc = {};
        std::string ip = {};
    };

    class ServerListInternalTable final : public ISoxTable, public std::enable_shared_from_this<ServerListInternalTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const ServerListInternal*;
        auto Get() const -> const std::vector<ServerListInternal>&;

    private:
        std::unordered_map<int32_t, ServerListInternal*> _umap;
        std::vector<ServerListInternal> _vector;

    };
}