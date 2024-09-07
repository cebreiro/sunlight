#pragma once

namespace sunlight::execution
{
    class ICancelable
    {
    public:
        virtual ~ICancelable() = default;

        virtual bool Cancel() = 0;
        virtual bool IsCanceled() const = 0;
    };
}
