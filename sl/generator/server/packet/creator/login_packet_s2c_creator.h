#pragma once

namespace sunlight
{
    ENUM_CLASS(LoginResult, int8_t,
        (Success, 0)
        (CantConnectServer, 1)
        (InvalidIDPassword, 2)
        (ReleaseExistingConnection, 7)
        (AgeRestriction, 12)
        (TooMuchUser, 15)
        (NotPaidAccount, 18)
        (Blank, 19)
    )
}

namespace sunlight
{
    class LoginPacketS2CCreator
    {
    public:
        LoginPacketS2CCreator() = delete;

        static auto CreateHello(uint32_t key1, uint32_t key2) -> Buffer;

        static auto CreateServerList(const std::vector<std::pair<int8_t, std::string>>& list) -> Buffer;
        static auto CreateLobbyPass(int32_t key1, int32_t key2, int8_t unk) -> Buffer;

        static auto CreateLoginFail(LoginResult result) -> Buffer;
    };
}
