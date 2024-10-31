#pragma once

namespace sunlight
{
    ENUM_CLASS(NesScriptTokenType, int32_t,
        (Int1, 0)
        (Int2, 1)
        (Variable, 2)
        (Float, 4)
        (String, 5)
    )

    struct NesScriptTokenVariable
    {
        static constexpr auto TYPE = NesScriptTokenType::Variable;

        int32_t type = 0;
        int32_t index = 0;
    };

    template <NesScriptTokenType E>
    struct NesScriptTokenTypeMapper;

    template <> struct NesScriptTokenTypeMapper<NesScriptTokenType::Int1> { using value_type = int32_t; };
    template <> struct NesScriptTokenTypeMapper<NesScriptTokenType::Int2> { using value_type = int32_t; };
    template <> struct NesScriptTokenTypeMapper<NesScriptTokenType::Variable> { using value_type = NesScriptTokenVariable; };
    template <> struct NesScriptTokenTypeMapper<NesScriptTokenType::Float> { using value_type = float; };
    template <> struct NesScriptTokenTypeMapper<NesScriptTokenType::String> { using value_type = const std::string&; };

    class NesScriptToken
    {
    public:
        NesScriptToken() = default;
        explicit NesScriptToken(StreamReader<std::vector<char>::const_iterator>& reader);

        auto GetType() const -> NesScriptTokenType;

        template <NesScriptTokenType E>
        auto Get() const -> typename NesScriptTokenTypeMapper<E>::value_type;

    private:
        NesScriptTokenType _type = NesScriptTokenType::Int1;

        int32_t _intValue1 = 0;
        int32_t _intValue2 = 0;
        NesScriptTokenVariable _variable = {};
        float _floatValue = 0.f;
        std::string _strValue;
    };

    template <NesScriptTokenType E>
    auto NesScriptToken::Get() const -> typename NesScriptTokenTypeMapper<E>::value_type
    {
        if constexpr (E == NesScriptTokenType::Int1)
        {
            return _intValue1;
        }
        else if constexpr (E == NesScriptTokenType::Int2)
        {
            return _intValue2;
        }
        else if constexpr (E == NesScriptTokenType::Variable)
        {
            return _variable;
        }
        else if constexpr (E == NesScriptTokenType::Float)
        {
            return _floatValue;
        }
        else if constexpr (E == NesScriptTokenType::String)
        {
            return _strValue;
        }
        else
        {
            static_assert(false, "not implemented");

            return 0;
        }
    }
}
