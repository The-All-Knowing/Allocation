#include <gtest/gtest.h>

#include "Precompile.h"
#include "CommonFunctions.h"
#include "Utilities/Common.h"
#include "Domain/Exceptions/OutOfStock.h"
#include "Domain/Product/Product.h"


namespace Allocation::Tests
{
    using namespace std::chrono;

    const year_month_day today(2020y, January, 31d);
    const auto tomorrow = today + days(1);
    const auto later = tomorrow + days(10);

    TEST(Domain, test_prefers_warehouse_batches_to_shipments)
    {
        Domain::Batch inStockBatch("in-stock-batch", "RETRO-CLOCK", 100);
        Domain::Batch shipmentBatch("shipment-batch", "RETRO-CLOCK", 100, tomorrow);
        Domain::Product product("RETRO-CLOCK", {inStockBatch, shipmentBatch});
        Domain::OrderLine line("oref", "RETRO-CLOCK", 10);

        product.Allocate(line);

        EXPECT_EQ(product.GetBatches()[0].GetAvailableQuantity(), 90);
        EXPECT_EQ(product.GetBatches()[1].GetAvailableQuantity(), 100);
    }

    TEST(Domain, test_prefers_earlier_batches)
    {
        Domain::Batch earliest("speedy-batch", "MINIMALIST-SPOON", 100, today);
        Domain::Batch medium("normal-batch", "MINIMALIST-SPOON", 100, tomorrow);
        Domain::Batch latest("slow-batch", "MINIMALIST-SPOON", 100, later);
        Domain::Product product("MINIMALIST-SPOON", {medium, earliest, latest});
        Domain::OrderLine line("order1", "MINIMALIST-SPOON", 10);

        product.Allocate(line);

        EXPECT_EQ(product.GetBatches()[0].GetAvailableQuantity(), 100);
        EXPECT_EQ(product.GetBatches()[1].GetAvailableQuantity(), 90);
        EXPECT_EQ(product.GetBatches()[2].GetAvailableQuantity(), 100);
    }

    TEST(Domain, test_returns_allocated_batch_ref)
    {
        Domain::Batch inStockBatch("in-stock-batch-ref", "HIGHBROW-POSTER", 100);
        Domain::Batch shipmentBatch("shipment-batch-ref", "HIGHBROW-POSTER", 100, tomorrow);
        Domain::OrderLine line("oref", "HIGHBROW-POSTER", 10);
        Domain::Product product("HIGHBROW-POSTER", {inStockBatch, shipmentBatch});
        auto allocation = product.Allocate(line);
        EXPECT_EQ(allocation, inStockBatch.GetReference());
    }

    TEST(Domain, test_raises_out_of_stock_exception_if_cannot_allocate)
    {
        Domain::Batch batch("batch1", "SMALL-FORK", 10, today);
        Domain::Product product("SMALL-FORK", {batch});
        product.Allocate(Domain::OrderLine("order1", "SMALL-FORK", 10));

        EXPECT_TRUE(
        ThrowsWithMessage<Domain::Exceptions::OutOfStock>(
            [&]() { product.Allocate(Domain::OrderLine("order1", "SMALL-FORK", 10));},
            "The article SMALL-FORK is out of stock")
        );
    }

    TEST(Domain, test_increments_version_number)
    {
        Domain::OrderLine line("oref", "SCANDI-PEN", 10);
        Domain::Product product(
            "SCANDI-PEN",
            {Domain::Batch("b1", "SCANDI-PEN", 100)},
            7
        );

        product.Allocate(line);
        
        EXPECT_EQ(product.GetVersion(), 8);
    }
}