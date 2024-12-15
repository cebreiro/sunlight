#include "character_create.h"

#include <boost/locale.hpp>

#include "sl/generator/server/server_constant.h"

namespace sunlight::req
{
    void to_json(nlohmann::json& j, const CharacterCreate& req)
    {
        j = nlohmann::json
        {
            { "id", req.id },
            { "aid", req.aid },
            { "sid", req.sid },
            { "slot", req.slot },
            { "name",  boost::locale::conv::to_utf<char>(req.name, ServerConstant::TEXT_ENCODING) },
            { "hair", req.hair },
            { "hairColor", req.hairColor },
            { "face", req.face },
            { "skinColor", req.skinColor },
            { "zone", req.zone },
            { "x", req.x },
            { "y", req.y },
            { "gender", req.gender },
            { "str", req.str },
            { "dex", req.dex },
            { "accr", req.accr },
            { "health", req.health },
            { "intell", req.intell },
            { "wis", req.wis },
            { "will", req.will },
            { "water", req.water },
            { "fire", req.fire },
            { "lightning", req.lightning },
            { "job", req.job },
            { "items", req.items },
            { "skills", req.skills },
        };
    }
}
