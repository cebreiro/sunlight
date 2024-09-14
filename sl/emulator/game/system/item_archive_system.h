#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;
}

namespace sunlight
{
    class ItemArchiveSystem final : public GameSystem
    {
    public:
        explicit ItemArchiveSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    private:
        void HandleMessage(const ZoneMessage& message) const;

    private:
        const ServiceLocator& _serviceLocator;
    };
}
