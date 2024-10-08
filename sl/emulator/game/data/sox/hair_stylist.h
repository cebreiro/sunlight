#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct HairStylist
    {
        explicit HairStylist(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t coloringPrice = {};
        int32_t id01 = {};
        int32_t sex01 = {};
        int32_t price01 = {};
        int32_t id02 = {};
        int32_t sex02 = {};
        int32_t price02 = {};
        int32_t id03 = {};
        int32_t sex03 = {};
        int32_t price03 = {};
        int32_t id04 = {};
        int32_t sex04 = {};
        int32_t price04 = {};
        int32_t id05 = {};
        int32_t sex05 = {};
        int32_t price05 = {};
        int32_t id06 = {};
        int32_t sex06 = {};
        int32_t price06 = {};
        int32_t id07 = {};
        int32_t sex07 = {};
        int32_t price07 = {};
        int32_t id08 = {};
        int32_t sex08 = {};
        int32_t price08 = {};
        int32_t id09 = {};
        int32_t sex09 = {};
        int32_t price09 = {};
        int32_t id10 = {};
        int32_t sex10 = {};
        int32_t price10 = {};
        int32_t id11 = {};
        int32_t sex11 = {};
        int32_t price11 = {};
        int32_t id12 = {};
        int32_t sex12 = {};
        int32_t price12 = {};
        int32_t id13 = {};
        int32_t sex13 = {};
        int32_t price13 = {};
        int32_t id14 = {};
        int32_t sex14 = {};
        int32_t price14 = {};
        int32_t id15 = {};
        int32_t sex15 = {};
        int32_t price15 = {};
        int32_t id16 = {};
        int32_t sex16 = {};
        int32_t price16 = {};
        int32_t id17 = {};
        int32_t sex17 = {};
        int32_t price17 = {};
        int32_t id18 = {};
        int32_t sex18 = {};
        int32_t price18 = {};
        int32_t id19 = {};
        int32_t sex19 = {};
        int32_t price19 = {};
        int32_t id20 = {};
        int32_t sex20 = {};
        int32_t price20 = {};
        int32_t id21 = {};
        int32_t sex21 = {};
        int32_t price21 = {};
        int32_t id22 = {};
        int32_t sex22 = {};
        int32_t price22 = {};
        int32_t id23 = {};
        int32_t sex23 = {};
        int32_t price23 = {};
        int32_t id24 = {};
        int32_t sex24 = {};
        int32_t price24 = {};
        int32_t id25 = {};
        int32_t sex25 = {};
        int32_t price25 = {};
        int32_t id26 = {};
        int32_t sex26 = {};
        int32_t price26 = {};
        int32_t id27 = {};
        int32_t sex27 = {};
        int32_t price27 = {};
        int32_t id28 = {};
        int32_t sex28 = {};
        int32_t price28 = {};
        int32_t id29 = {};
        int32_t sex29 = {};
        int32_t price29 = {};
        int32_t id30 = {};
        int32_t sex30 = {};
        int32_t price30 = {};
        int32_t id31 = {};
        int32_t sex31 = {};
        int32_t price31 = {};
        int32_t id32 = {};
        int32_t sex32 = {};
        int32_t price32 = {};
        int32_t id33 = {};
        int32_t sex33 = {};
        int32_t price33 = {};
        int32_t id34 = {};
        int32_t sex34 = {};
        int32_t price34 = {};
        int32_t id35 = {};
        int32_t sex35 = {};
        int32_t price35 = {};
        int32_t id36 = {};
        int32_t sex36 = {};
        int32_t price36 = {};
        int32_t id37 = {};
        int32_t sex37 = {};
        int32_t price37 = {};
        int32_t id38 = {};
        int32_t sex38 = {};
        int32_t price38 = {};
        int32_t id39 = {};
        int32_t sex39 = {};
        int32_t price39 = {};
        int32_t id40 = {};
        int32_t sex40 = {};
        int32_t price40 = {};
    };

    class HairStylistTable final : public ISoxTable, public std::enable_shared_from_this<HairStylistTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const HairStylist*;
        auto Get() const -> const std::vector<HairStylist>&;

    private:
        std::unordered_map<int32_t, HairStylist*> _umap;
        std::vector<HairStylist> _vector;

    };
}
