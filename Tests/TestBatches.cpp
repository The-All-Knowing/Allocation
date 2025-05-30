#include <gtest/gtest.h>

#include "CommonFunctions.h"
#include "Batch.h"


namespace Allocation::Tests
{
    using namespace Allocation::Domain;

    TEST(Domain, test_allocating_to_a_batch_reduces_the_available_quantity)
    {
        Batch batch("batch-001", "SMALL-TABLE", 20, GetCurrentDate());
        OrderLine line("order-ref", "SMALL-TABLE", 2);
        batch.Allocate(line);
        EXPECT_EQ(batch.GetAvailableQuantity(), 18);
    }
    
    TEST(Domain, test_can_allocate_if_available_greater_than_required)
    {
        auto [largeBatch, smallLine] = MakeBatchAndLine("ELEGANT-LAMP", 20, 2);
        EXPECT_TRUE(largeBatch.CanAllocate(smallLine));
    }

    TEST(Domain, test_cannot_allocate_if_available_smaller_than_required)
    {
        auto [smallBatch,largeLine] = MakeBatchAndLine("ELEGANT-LAMP", 2, 20);
        EXPECT_FALSE(smallBatch.CanAllocate(largeLine));
    }

    TEST(Domain, test_can_allocate_if_available_equal_to_required)
    {
        auto [batch, line] = MakeBatchAndLine("ELEGANT-LAMP", 2, 2);
        EXPECT_TRUE(batch.CanAllocate(line));
    }

    TEST(Domain, test_cannot_allocate_if_skus_do_not_match)
    {
        Batch batch("batch-001", "UNCOMFORTABLE-CHAIR", 100);
        OrderLine differentSkuLine("order-123", "EXPENSIVE-TOASTER", 10);
        EXPECT_FALSE(batch.CanAllocate(differentSkuLine));
    }

    TEST(Domain, test_can_only_deallocate_allocated_lines)
    {
        auto [batch, unallocatedLine] = MakeBatchAndLine("DECORATIVE-TRINKET", 20, 2);
        batch.Deallocate(unallocatedLine);
        EXPECT_EQ(batch.GetAvailableQuantity(), 20);
    }

    TEST(Domain, test_allocation_is_idempotent)
    {
        auto [batch, line] = MakeBatchAndLine("ANGULAR-DESK", 20, 2);
        batch.Allocate(line);
        batch.Allocate(line);
        EXPECT_EQ(batch.GetAvailableQuantity(), 18);
    }

    TEST(Domain, test_deallocate)
    {
        auto [batch, line] = MakeBatchAndLine("EXPENSIVE-FOOTSTOOL", 20, 2);
        batch.Allocate(line);
        batch.Deallocate(line);
        EXPECT_EQ(batch.GetAvailableQuantity(), 20);
    }

}