#include <gtest/gtest.h>

#include "CommonFunctions.h"
#include "Utilities/Common.h"
#include "Model.h"


namespace Allocation::Tests
{
    using namespace Allocation::Domain;

    /// @Todo: Надо поставить константу
    const auto today = GetCurrentDate();
    const auto tomorrow = today + std::chrono::days(1);
    const auto later = tomorrow + std::chrono::days(1);

    TEST(Domain, test_prefers_current_stock_batches_to_shipments)
    {
        std::vector<Batch> batches
        {
            Batch("in-stock-batch", "RETRO-CLOCK", 100),
            Batch("shipment-batch", "RETRO-CLOCK", 100, tomorrow)
        };
        OrderLine line ("oref", "RETRO-CLOCK", 10);
        Allocate(line, batches.begin(), batches.end());
        EXPECT_EQ(batches[0].GetAvailableQuantity(), 90);
        EXPECT_EQ(batches[1].GetAvailableQuantity(), 100);
    }

    TEST(Domain, test_prefers_earlier_batches)
    {
        std::vector<Batch> batches
        {
            Batch("speedy-batch", "MINIMALIST-SPOON", 100, today),
            Batch("normal-batch", "MINIMALIST-SPOON", 100, tomorrow),
            Batch("slow-batch", "MINIMALIST-SPOON", 100, later)
        };
        OrderLine line("order1", "MINIMALIST-SPOON", 10);
        Allocate(line, batches.begin(), batches.end());

        EXPECT_EQ(batches[0].GetAvailableQuantity(), 90);
        EXPECT_EQ(batches[1].GetAvailableQuantity(), 100);
        EXPECT_EQ(batches[2].GetAvailableQuantity(), 100);
    }

    TEST(Domain, test_returns_allocated_batch_ref)
    {
        std::vector<Batch> batches
        {
            Batch("in-stock-batch-ref", "HIGHBROW-POSTER", 100),
            Batch("shipment-batch-ref", "HIGHBROW-POSTER", 100, tomorrow)
        };
        OrderLine line("oref", "HIGHBROW-POSTER", 10);
        auto allocation = Allocate(line, batches.begin(), batches.end());
        EXPECT_EQ(allocation, batches[0]);
    }

    TEST(Domain, test_raises_out_of_stock_exception_if_cannot_allocate)
    {
        std::list<Batch> batches
        {
            Batch("batch1", "SMALL-FORK", 10, today)
        };
        Allocate(OrderLine("order1", "SMALL-FORK", 10), batches.begin(), batches.end());
        
        EXPECT_THROW(
            Allocate(OrderLine("order1", "SMALL-FORK", 10), batches.begin(), batches.end()),
            OutOfStock
        );

        EXPECT_TRUE(
        ThrowsWithMessage<OutOfStock>(
            [&]() { Allocate(OrderLine("order1", "SMALL-FORK", 10), batches.begin(), batches.end());},
            "The article SMALL-FORK is out of stock")
        );
    }


}