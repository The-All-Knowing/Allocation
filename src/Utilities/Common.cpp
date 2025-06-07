#include "Common.h"


namespace Allocation
{
    std::chrono::year_month_day Convert(Poco::DateTime date)
    {
        return std::chrono::year_month_day{
            std::chrono::year{date.year()},
            std::chrono::month{static_cast<unsigned>(date.month())},
            std::chrono::day{static_cast<unsigned>(date.day())}};
    }

    Poco::DateTime Convert(std::chrono::year_month_day date)
    {
        int year = int(date.year());
        int month = static_cast<unsigned>(date.month());
        int day = static_cast<unsigned>(date.day());

        return Poco::DateTime{year, month, day};
    }

    std::chrono::year_month_day operator+(const std::chrono::year_month_day& ymd, const std::chrono::days& days)
    {
        return std::chrono::sys_days(ymd) + days;
    }

    std::chrono::year_month_day operator+(const std::chrono::days& days, const std::chrono::year_month_day& ymd)
    {
        return ymd + days;
    }
}