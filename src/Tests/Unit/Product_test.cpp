#include "Domain/Product/Product.hpp"

#include <gtest/gtest.h>

#include "Precompile.hpp"

#include "Domain/Events/Allocated.hpp"
#include "Domain/Events/OutOfStock.hpp"
#include "Tests/Utilities/Common_test.hpp"
#include "Utilities/Common.hpp"


namespace Allocation::Tests
{
    using namespace std::chrono;

    const year_month_day today(2020y, January, 31d);
    const auto tomorrow = today + days(1);
    const auto later = tomorrow + days(10);

    TEST(Product, test_prefers_warehouse_batches_to_shipments)
    {
        Domain::Batch inStockBatch("in-stock-batch", "RETRO-CLOCK", 100);
        Domain::Batch shipmentBatch("shipment-batch", "RETRO-CLOCK", 100, tomorrow);
        Domain::Product product("RETRO-CLOCK", {inStockBatch, shipmentBatch});
        Domain::OrderLine line("oref", "RETRO-CLOCK", 10);

        product.Allocate(line);

        inStockBatch = product.GetBatch("in-stock-batch").value();
        shipmentBatch = product.GetBatch("shipment-batch").value();
        EXPECT_EQ(inStockBatch.GetAvailableQuantity(), 90);
        EXPECT_EQ(shipmentBatch.GetAvailableQuantity(), 100);
    }

    TEST(Product, test_prefers_earlier_batches)
    {
        Domain::Batch earliest("speedy-batch", "MINIMALIST-SPOON", 100, today);
        Domain::Batch medium("normal-batch", "MINIMALIST-SPOON", 100, tomorrow);
        Domain::Batch latest("slow-batch", "MINIMALIST-SPOON", 100, later);
        Domain::Product product("MINIMALIST-SPOON", {medium, earliest, latest});
        Domain::OrderLine line("order1", "MINIMALIST-SPOON", 10);

        product.Allocate(line);

        earliest = product.GetBatch("speedy-batch").value();
        medium = product.GetBatch("normal-batch").value();
        latest = product.GetBatch("slow-batch").value();
        EXPECT_EQ(earliest.GetAvailableQuantity(), 90);
        EXPECT_EQ(medium.GetAvailableQuantity(), 100);
        EXPECT_EQ(latest.GetAvailableQuantity(), 100);
    }

    TEST(Product, test_returns_allocated_batch_ref)
    {
        Domain::Batch inStockBatch("in-stock-batch-ref", "HIGHBROW-POSTER", 100);
        Domain::Batch shipmentBatch("shipment-batch-ref", "HIGHBROW-POSTER", 100, tomorrow);
        Domain::OrderLine line("oref", "HIGHBROW-POSTER", 10);
        Domain::Product product("HIGHBROW-POSTER", {inStockBatch, shipmentBatch});
        auto allocation = product.Allocate(line);
        EXPECT_EQ(allocation, inStockBatch.GetReference());
    }

    TEST(Product, test_outputs_allocated_event)
    {
        Domain::Batch batch("batchref", "RETRO-LAMPSHADE", 100);
        Domain::OrderLine line("oref", "RETRO-LAMPSHADE", 10);
        Domain::Product product("RETRO-LAMPSHADE", {batch});
        product.Allocate(line);
        Domain::Events::Allocated expected(
            "oref", "RETRO-LAMPSHADE", 10, std::string(batch.GetReference()));
        auto& actual = product.Messages();
        EXPECT_TRUE(!actual.empty());
        auto msg = actual.back();
        auto event = std::dynamic_pointer_cast<Domain::Events::Allocated>(msg);
        EXPECT_TRUE(event);
        EXPECT_EQ(event->batchref, std::string(batch.GetReference()));
        EXPECT_EQ(event->orderid, line.reference);
        EXPECT_EQ(event->SKU, line.SKU);
        EXPECT_EQ(event->qty, line.quantity);
    }

    TEST(Product, test_records_out_of_stock_event_if_cannot_allocate)
    {
        Domain::Batch batch("batch1", "SMALL-FORK", 10, today);
        Domain::Product product("SMALL-FORK", {batch});
        product.Allocate(Domain::OrderLine("order1", "SMALL-FORK", 10));
        auto allocation = product.Allocate(Domain::OrderLine("order1", "SMALL-FORK", 10));

        EXPECT_FALSE(product.Messages().empty());
        EXPECT_EQ(product.Messages().back()->Name(), "OutOfStock");
        auto event =
            std::dynamic_pointer_cast<Domain::Events::OutOfStock>(product.Messages().back());
        EXPECT_TRUE(event);
        EXPECT_EQ(event->SKU, batch.GetSKU());
        EXPECT_EQ(allocation, std::nullopt);
    }

    TEST(Product, test_increments_version_number)
    {
        Domain::OrderLine line("oref", "SCANDI-PEN", 10);
        Domain::Product product("SCANDI-PEN", {Domain::Batch("b1", "SCANDI-PEN", 100)}, 7);
        product.Allocate(line);
        EXPECT_EQ(product.GetVersion(), 8);
    }
}