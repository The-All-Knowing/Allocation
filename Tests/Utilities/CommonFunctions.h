#pragma once

#include "Precompile.h"
#include "gtest/gtest-assertion-result.h"
#include "Batch.h"


namespace Allocation::Tests
{
    std::chrono::year_month_day GetCurrentDate();

    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& SKU, size_t batchQty, size_t lineQty);

    template <class Func>
    testing::AssertionResult ThrowsWithMessage(
        Func&& func,
        const std::string& expectedMsg) 
    {
        try 
        {
            func();
            return testing::AssertionFailure() << "No exception thrown";
        } 
        catch (const std::exception& e)
        {
            if (e.what() == expectedMsg)
                return testing::AssertionSuccess();
                
            return testing::AssertionFailure() 
                << "Wrong message. Actual: " << e.what();
        }
    }
}