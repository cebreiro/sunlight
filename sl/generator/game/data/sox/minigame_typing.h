#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct MinigameTyping
    {
        explicit MinigameTyping(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t monsterId = {};
        std::string monsterName = {};
        int32_t monsterModelId = {};
        int32_t monsterSpeed = {};
        int32_t monsterAdd = {};
        int32_t minWordLength = {};
        int32_t maxWordLength = {};
    };

    class MinigameTypingTable final : public ISoxTable, public std::enable_shared_from_this<MinigameTypingTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const MinigameTyping*;
        auto Get() const -> const std::vector<MinigameTyping>&;

    private:
        std::unordered_map<int32_t, MinigameTyping*> _umap;
        std::vector<MinigameTyping> _vector;

    };
}