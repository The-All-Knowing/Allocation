#pragma once

#include "Precompile.h"
#include "gtest/gtest-assertion-result.h"
#include "Product/Batch.h"


namespace Allocation::Tests
{
    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& SKU, size_t batchQty, size_t lineQty);

    template <class Exception, class Func>
    testing::AssertionResult ThrowsWithMessage(Func&& func, const std::string& expectedMsg)
    {
        try 
        {
            func();
            return testing::AssertionFailure() << "No exception thrown";
        } 
        catch (const Exception& e)
        {
            if (e.what() == expectedMsg)
                return testing::AssertionSuccess();
                    
            return testing::AssertionFailure() 
                << "Wrong message. Actual: " << e.what();
        }
        catch (...)
        {
            return testing::AssertionFailure() << "Unexpected exception type thrown";
        }
    }

    int InsertBatch(Poco::Data::Session& session, std::string batchRef, std::string sku = "GENERIC-SOFA", int qty = 100, int version = 0);

    std::string RandomSku(const std::string& name = "");
    std::string RandomBatchRef(const std::string& name = "");
    std::string RandomOrderId(const std::string& name = "");
    std::string GetAllocatedBatchRef(Poco::Data::Session& session, std::string orderid, std::string sku);
}