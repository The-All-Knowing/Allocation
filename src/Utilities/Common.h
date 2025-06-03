#pragma once

#include "Precompile.h"


namespace Allocation
{
    std::chrono::year_month_day Convert(Poco::DateTime date);
    Poco::DateTime Convert(std::chrono::year_month_day date);

    std::chrono::year_month_day operator+(const std::chrono::year_month_day& ymd, const std::chrono::days& days);
    std::chrono::year_month_day operator+(const std::chrono::days& days, const std::chrono::year_month_day& ymd);
    
}