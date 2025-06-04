#include <gtest/gtest.h>

#include "Forwards.h"
#include "CommonFunctions.h"
#include "Services.h"
#include "InvalidSku.h"
#include "Adapters/Repository/FakeRepository.h"
#include "Adapters/Database/Session/FakeSessionImpl.h"


namespace Allocation::Tests
{

    TEST(Services, test_add_batch)
    {
        IRepositoryPtr repo = std::make_shared<Adapters::Repository::FakeRepository>();
        auto session = GetFakeSession();

        Services::AddBatch(repo, session, "b1", "CRUNCHY-ARMCHAIR", 100);

        EXPECT_TRUE(repo->Get("b1"));
        EXPECT_FALSE(session.isTransaction());
    }

    TEST(Services, test_allocate_returns_allocation)
    {
        IRepositoryPtr repo = std::make_shared<Adapters::Repository::FakeRepository>();
        auto session = GetFakeSession();
        Services::AddBatch(repo, session, "b1", "COMPLICATED-LAMP", 100);

        auto result = Services::Allocate(repo, session, "o1", "COMPLICATED-LAMP", 10);

        EXPECT_EQ(result, "b1");
    }

    TEST(Services, test_allocate_errors_for_invalid_sku)
    {
        IRepositoryPtr repo = std::make_shared<Adapters::Repository::FakeRepository>();
        auto session = GetFakeSession();
        Services::AddBatch(repo, session, "b1", "AREALSKU", 100);

        EXPECT_TRUE(
        ThrowsWithMessage<Services::InvalidSku>(
            [&]() {Services::Allocate(repo, session, "o1", "NONEXISTENTSKU", 10);},
            "Invalid sku NONEXISTENTSKU")
        );
    }

    TEST(Services, test_commits)
    {
        IRepositoryPtr repo = std::make_shared<Adapters::Repository::FakeRepository>();
        auto session = GetFakeSession();

        Services::AddBatch(repo, session, "b1", "OMINOUS-MIRROR", 100);
        Services::Allocate(repo, session, "o1", "OMINOUS-MIRROR", 10);

        EXPECT_FALSE(session.isTransaction());
    }

}
