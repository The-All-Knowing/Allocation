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
        std::chrono::sys_days days = date;
        std::time_t time = std::chrono::system_clock::to_time_t(days);
        std::tm* tm = std::gmtime(&time);

        return Poco::DateTime(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
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