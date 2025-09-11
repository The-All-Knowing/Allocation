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

        EXPECT_EQ(product.Allocate(line).value_or(""), "in-stock-batch");

        EXPECT_TRUE(product.IsModified());
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

        EXPECT_EQ(product.Allocate(line).value_or(""), "speedy-batch");

        EXPECT_TRUE(product.IsModified());
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
        EXPECT_TRUE(product.IsModified());
        EXPECT_EQ(allocation.value_or(""), inStockBatch.GetReference());
    }

    TEST(Product, test_outputs_allocated_event)
    {
        Domain::Batch batch("batchref", "RETRO-LAMPSHADE", 100);
        Domain::OrderLine line("oref", "RETRO-LAMPSHADE", 10);
        Domain::Product product("RETRO-LAMPSHADE", {batch});
        product.Allocate(line);

        EXPECT_TRUE(product.IsModified());
        auto& actual = product.Messages();
        ASSERT_EQ(actual.size(), 1);
        auto event = std::dynamic_pointer_cast<Domain::Events::Allocated>(actual.back());
        ASSERT_TRUE(event);
        EXPECT_EQ("batchref", batch.GetReference());
        EXPECT_EQ("oref", line.reference);
        EXPECT_EQ("RETRO-LAMPSHADE", line.SKU);
        EXPECT_EQ(10, line.quantity);
    }

    TEST(Product, test_records_out_of_stock_event_if_cannot_allocate)
    {
        Domain::Batch batch("batch1", "SMALL-FORK", 10, today);
        Domain::Product product("SMALL-FORK", {batch});
        product.Allocate(Domain::OrderLine("order1", "SMALL-FORK", 10));
        auto allocation = product.Allocate(Domain::OrderLine("order1", "SMALL-FORK", 10));
        ASSERT_EQ(allocation, std::nullopt);

        EXPECT_TRUE(product.IsModified());
        auto& actual = product.Messages();
        ASSERT_EQ(actual.size(), 2);
        auto rawEvent = actual.back();
        ASSERT_EQ(rawEvent->Name(), "OutOfStock");
        auto event = std::dynamic_pointer_cast<Domain::Events::OutOfStock>(rawEvent);
        ASSERT_TRUE(event);
        EXPECT_EQ(event->SKU, batch.GetSKU());
    }

    TEST(Product, test_increments_version_number)
    {
        Domain::OrderLine line("oref", "SCANDI-PEN", 10);
        Domain::Product product("SCANDI-PEN", {Domain::Batch("b1", "SCANDI-PEN", 100)}, 7);
        product.Allocate(line);
        EXPECT_EQ(product.GetVersion(), 8);
        EXPECT_TRUE(product.IsModified());
    }

    TEST(Product, test_add_batch_marks_modified)
    {
        Domain::Batch batch("b-add", "DESK", 100);
        Domain::Product product("DESK");

        EXPECT_TRUE(product.AddBatch(batch));
        EXPECT_TRUE(product.IsModified());
        EXPECT_TRUE(product.GetBatch("b-add").has_value());

        auto modified = product.GetModifiedBatches();
        ASSERT_EQ(modified.size(), 1);
        EXPECT_EQ(modified.back(), "b-add");
    }
}