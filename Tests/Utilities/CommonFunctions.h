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
    testing::AssertionResult ThrowsWithMessage(Func&& func, const std::string& expectedMsg) 
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

    int InsertOrderLine(Poco::Data::Session& session, std::string orderId = "order1", std::string sku = "GENERIC-SOFA", int qty = 12);

    int InsertBatch(Poco::Data::Session& session, std::string batchRef, std::string sku = "GENERIC-SOFA", int qty = 100);

    void InsertAllocation(Poco::Data::Session& session, int orderlineId, int batchId);

    std::vector<std::string> GetAllocations(Poco::Data::Session& session, std::string batchRef);
}