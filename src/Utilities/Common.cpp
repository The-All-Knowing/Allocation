#include "Common.hpp"


namespace Allocation
{
    std::optional<std::chrono::year_month_day> Convert(Poco::Nullable<Poco::DateTime> date) noexcept
    {
        std::optional<std::chrono::year_month_day> result;
        if (date.isNull())
            return result;

        auto temp = date.value();
        result = std::chrono::year_month_day{std::chrono::year{temp.year()},
            std::chrono::month{static_cast<unsigned>(temp.month())},
            std::chrono::day{static_cast<unsigned>(temp.day())}};
        return result;
    }

    Poco::Nullable<Poco::DateTime> Convert(std::optional<std::chrono::year_month_day> date) noexcept
    {
        Poco::Nullable<Poco::DateTime> result;
        if (!date.has_value())
            return result;

        auto temp = date.value();
        int year = int(temp.year());
        int month = static_cast<unsigned>(temp.month());
        int day = static_cast<unsigned>(temp.day());
        result = Poco::DateTime{year, month, day};
        return result;
    }

    std::chrono::year_month_day operator+(
        const std::chrono::year_month_day& ymd, const std::chrono::days& days) noexcept
    {
        return std::chrono::sys_days(ymd) + days;
    }

    std::chrono::year_month_day operator+(
        const std::chrono::days& days, const std::chrono::year_month_day& ymd) noexcept
    {
        return ymd + days;
    }
}