#include <gtest/gtest.h>

#include "Precompile.hpp"

#include "Adapters/Database/Session/SessionPool.hpp"
#include "Adapters/Repository/SqlRepository.hpp"
#include "Utilities/Common.hpp"


namespace Allocation::Tests
{
    TEST(Repository, test_get_by_batchref)
    {
        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        Adapters::Repository::SqlRepository repo(session);
        Domain::Batch b1("b1", "sku1", 100);
        Domain::Batch b2("b2", "sku1", 100);
        Domain::Batch b3("b3", "sku2", 100);
        Domain::Product p1("sku1", {b1, b2});
        Domain::Product p2("sku2", {b3});
        repo.Add(p1);
        repo.Add(p2);
        EXPECT_EQ(*repo.GetByBatchRef("b2"), p1);
        EXPECT_EQ(*repo.GetByBatchRef("b3"), p2);
    }
}