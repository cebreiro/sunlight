#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct MinigameTypingWord
    {
        explicit MinigameTypingWord(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string typingWord = {};
    };

    class MinigameTypingWordTable final : public ISoxTable, public std::enable_shared_from_this<MinigameTypingWordTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const MinigameTypingWord*;
        auto Get() const -> const std::vector<MinigameTypingWord>&;

    private:
        std::unordered_map<int32_t, MinigameTypingWord*> _umap;
        std::vector<MinigameTypingWord> _vector;

    };
}
