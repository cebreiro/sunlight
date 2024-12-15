#include "npc_shop_data_provider.h"

#include "sl/generator/game/data/sox_table_set.h"
#include "sl/generator/game/data/sox/merchant2.h"
#include "sl/generator/game/data/sox/hair_stylist.h"

namespace sunlight
{
    NPCShopDataProvider::NPCShopDataProvider(const SoxTableSet& tableSet)
    {
        for (const sox::HairStylist& sox : tableSet.Get<sox::HairStylistTable>().Get())
        {
            HairShopData& hairShopData = _hairShopData[sox.index];

            hairShopData.coloringPrice = sox.coloringPrice;

            const std::initializer_list<std::tuple<int32_t, int32_t, int32_t>> list{
                std::tuple(sox.id01, sox.sex01, sox.price01),
                std::tuple(sox.id02, sox.sex02, sox.price02),
                std::tuple(sox.id03, sox.sex03, sox.price03),
                std::tuple(sox.id04, sox.sex04, sox.price04),
                std::tuple(sox.id05, sox.sex05, sox.price05),
                std::tuple(sox.id06, sox.sex06, sox.price06),
                std::tuple(sox.id07, sox.sex07, sox.price07),
                std::tuple(sox.id08, sox.sex08, sox.price08),
                std::tuple(sox.id09, sox.sex09, sox.price09),
                std::tuple(sox.id10, sox.sex10, sox.price10),
                std::tuple(sox.id11, sox.sex11, sox.price11),
                std::tuple(sox.id12, sox.sex12, sox.price12),
                std::tuple(sox.id13, sox.sex13, sox.price13),
                std::tuple(sox.id14, sox.sex14, sox.price14),
                std::tuple(sox.id15, sox.sex15, sox.price15),
                std::tuple(sox.id16, sox.sex16, sox.price16),
                std::tuple(sox.id17, sox.sex17, sox.price17),
                std::tuple(sox.id18, sox.sex18, sox.price18),
                std::tuple(sox.id19, sox.sex19, sox.price19),
                std::tuple(sox.id20, sox.sex20, sox.price20),
                std::tuple(sox.id21, sox.sex21, sox.price21),
                std::tuple(sox.id22, sox.sex22, sox.price22),
                std::tuple(sox.id23, sox.sex23, sox.price23),
                std::tuple(sox.id24, sox.sex24, sox.price24),
                std::tuple(sox.id25, sox.sex25, sox.price25),
                std::tuple(sox.id26, sox.sex26, sox.price26),
                std::tuple(sox.id27, sox.sex27, sox.price27),
                std::tuple(sox.id28, sox.sex28, sox.price28),
                std::tuple(sox.id29, sox.sex29, sox.price29),
                std::tuple(sox.id30, sox.sex30, sox.price30),
                std::tuple(sox.id31, sox.sex31, sox.price31),
                std::tuple(sox.id32, sox.sex32, sox.price32),
                std::tuple(sox.id33, sox.sex33, sox.price33),
                std::tuple(sox.id34, sox.sex34, sox.price34),
                std::tuple(sox.id35, sox.sex35, sox.price35),
                std::tuple(sox.id36, sox.sex36, sox.price36),
                std::tuple(sox.id37, sox.sex37, sox.price37),
                std::tuple(sox.id38, sox.sex38, sox.price38),
                std::tuple(sox.id39, sox.sex39, sox.price39),
                std::tuple(sox.id40, sox.sex40, sox.price40),
            };

            for (const auto& [hair, sex, price] : list)
            {
                if (hair == 0)
                {
                    continue;
                }

                hairShopData.hairs.emplace_back(hair, sex == 0, price);
            }

            hairShopData.hairs.shrink_to_fit();
        }

        for (const sox::Merchant2& sox : tableSet.Get<sox::Merchant2Table>().Get())
        {
            if (sox.type != 0)
            {
                // hair shop
                continue;
            }

            ItemShopData& itemShopData = _itemShopData[sox.index];

            itemShopData.id = sox.index;
            itemShopData.name = sox.name;
            itemShopData.buyingFactor = std::max(0.f, static_cast<float>(sox.buyingFactor) / 100.f);
            itemShopData.sellingFactor = std::max(0.f, static_cast<float>(sox.sellingFactor) / 100.f);

            const std::initializer_list<int32_t> list{
                sox.item01, sox.item02, sox.item03, sox.item04, sox.item05, sox.item06, sox.item07, sox.item08, sox.item09, sox.item10,
                sox.item11, sox.item12, sox.item13, sox.item14, sox.item15, sox.item16, sox.item17, sox.item18, sox.item09, sox.item20,
                sox.item21, sox.item22, sox.item23, sox.item24, sox.item25, sox.item26, sox.item27, sox.item28, sox.item09, sox.item30,
                sox.item31, sox.item32, sox.item33, sox.item34, sox.item35, sox.item36, sox.item37, sox.item38, sox.item09, sox.item40,
                sox.item41, sox.item42, sox.item43, sox.item44, sox.item45, sox.item46, sox.item47, sox.item48, sox.item09, sox.item50,
                sox.item51, sox.item52, sox.item53, sox.item54, sox.item55, sox.item56, sox.item57, sox.item58, sox.item09, sox.item60,
                sox.item61, sox.item62, sox.item63, sox.item64, sox.item65, sox.item66, sox.item67, sox.item68, sox.item09, sox.item70,
                sox.item71, sox.item72, sox.item73, sox.item74, sox.item75, sox.item76, sox.item77, sox.item78, sox.item09, sox.item80,
            };

            for (int32_t itemId : list)
            {
                if (itemId == 0)
                {
                    continue;
                }

                itemShopData.saleItems.push_back(itemId);
            }

            itemShopData.saleItems.shrink_to_fit();
        }
    }

    auto NPCShopDataProvider::FindHairShopData(int32_t id) const -> const HairShopData*
    {
        const auto iter = _hairShopData.find(id);

        return iter != _hairShopData.end() ? &iter->second : nullptr;

    }

    auto NPCShopDataProvider::FindItemShopData(int32_t id) const -> const ItemShopData*
    {
        const auto iter = _itemShopData.find(id);

        return iter != _itemShopData.end() ? &iter->second : nullptr;
    }
}
