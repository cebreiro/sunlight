#pragma once
#include "sl/emulator/service/database/transaction/item/item_log.h"

namespace sunlight::db
{
    struct ItemTransaction
    {
        std::vector<ItemLog> logs;
    };
}
