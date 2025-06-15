#include <gtest/gtest.h>

#include "Precompile.h"
#include "CommonFunctions.h"
#include "Utilities/Common.h"
#include "Domain/Product/Product.h"
#include "Domain/Events/OutOfStock.h"


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

    TEST(Domain, test_records_out_of_stock_event_if_cannot_allocate)
    {
        Domain::Batch batch("batch1", "SMALL-FORK", 10, today);
        Domain::Product product("SMALL-FORK", {batch});
        product.Allocate(Domain::OrderLine("order1", "SMALL-FORK", 10));

        product.Allocate(Domain::OrderLine("order1", "SMALL-FORK", 10));
        EXPECT_FALSE(product.Messages().empty());
        EXPECT_EQ(product.Messages().front()->Name(), "OutOfStock");
        auto event = std::dynamic_pointer_cast<Domain::Events::OutOfStock>(product.Messages().front());
        EXPECT_TRUE(event);
        EXPECT_EQ(event->SKU, "SMALL-FORK");
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