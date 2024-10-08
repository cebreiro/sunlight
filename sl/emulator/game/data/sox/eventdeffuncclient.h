#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct Eventdeffuncclient
    {
        explicit Eventdeffuncclient(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t classId = {};
        int32_t trigger = {};
        int32_t deffuncId = {};
    };

    class EventdeffuncclientTable final : public ISoxTable, public std::enable_shared_from_this<EventdeffuncclientTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const Eventdeffuncclient*;
        auto Get() const -> const std::vector<Eventdeffuncclient>&;

    private:
        std::unordered_map<int32_t, Eventdeffuncclient*> _umap;
        std::vector<Eventdeffuncclient> _vector;

    };
}
