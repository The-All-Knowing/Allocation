#include "Common.h"


namespace Allocation
{

    std::chrono::year_month_day operator+(const std::chrono::year_month_day& ymd, const std::chrono::days& days)
    {
        return std::chrono::sys_days(ymd) + days;
    }

    std::chrono::year_month_day operator+(const std::chrono::days& days, const std::chrono::year_month_day& ymd)
    {
        return ymd + days;
    }
    
}