#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct ServerList
    {
        explicit ServerList(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string name = {};
        std::string desc = {};
        std::string ip = {};
    };

    class ServerListTable final : public ISoxTable, public std::enable_shared_from_this<ServerListTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const ServerList*;
        auto Get() const -> const std::vector<ServerList>&;

    private:
        std::unordered_map<int32_t, ServerList*> _umap;
        std::vector<ServerList> _vector;

    };
}