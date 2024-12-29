#include "nes_script_token.h"

namespace sunlight
{
    NesScriptToken::NesScriptToken(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        const NesScriptTokenType type = static_cast<NesScriptTokenType>(reader.Read<int32_t>());
        if (!IsValid(type))
        {
            throw std::runtime_error(fmt::format("invalid nest script token type. type: {}", static_cast<int32_t>(type)));
        }

        _type = type;

        switch (_type)
        {
        case NesScriptTokenType::Int1:
        {
            _intValue1 = reader.Read<int32_t>();
        }
        break;
        case NesScriptTokenType::Int2:
        {
            _intValue2 = reader.Read<int32_t>();
        }
        break;
        case NesScriptTokenType::Variable:
        {
            _variable.type = reader.Read<int32_t>();
            _variable.index = reader.Read<int32_t>();
        }
        break;
        case NesScriptTokenType::Float:
        {
            _floatValue = reader.Read<float>();
        }
        break;
        case NesScriptTokenType::String:
        {
            _strValue = reader.ReadString(reader.Read<int32_t>());
        }
        break;
        }
    }

    auto NesScriptToken::GetType() const -> NesScriptTokenType
    {
        return _type;
    }
}
