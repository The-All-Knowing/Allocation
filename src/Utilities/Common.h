#include "Precompile.h"


namespace Allocation
{

    std::chrono::year_month_day operator+(const std::chrono::year_month_day& ymd, const std::chrono::days& days);
    std::chrono::year_month_day operator+(const std::chrono::days& days, const std::chrono::year_month_day& ymd);
    
}