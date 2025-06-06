#include <gtest/gtest.h>

#include "Forwards.h"
#include "CommonFunctions.h"
#include "Services.h"
#include "Services/UoW/FakeUnitOfWork.h"
#include "Adapters/Repository/FakeRepository.h"
#include "InvalidSku.h"


namespace Allocation::Tests
{

    TEST(Services, test_add_batch)
    {
        Adapters::Repository::FakeRepository repo;
        Services::UoW::FakeUnitOfWork uow(repo);
        Services::AddBatch(uow, "b1", "CRUNCHY-ARMCHAIR", 100);

        EXPECT_TRUE(uow.GetBatchRepository().Get("b1"));
        EXPECT_TRUE(uow.IsCommited());
    }

    TEST(Services, test_allocate_returns_allocation)
    {
        Adapters::Repository::FakeRepository repo;
        Services::UoW::FakeUnitOfWork uow(repo);
        Services::AddBatch(uow, "b1", "COMPLICATED-LAMP", 100);

        Services::UoW::FakeUnitOfWork uowSecond(repo);
        auto result = Services::Allocate(uowSecond, "o1", "COMPLICATED-LAMP", 10);

        EXPECT_EQ(result, "b1");
    }

    TEST(Services, test_allocate_errors_for_invalid_sku)
    {
        Adapters::Repository::FakeRepository repo;
        Services::UoW::FakeUnitOfWork uow(repo);
        Services::AddBatch(uow, "b1", "AREALSKU", 100);

        Services::UoW::FakeUnitOfWork uowSecond(repo);
        EXPECT_TRUE(
        ThrowsWithMessage<Services::InvalidSku>(
            [&]() {Services::Allocate(uowSecond, "o1", "NONEXISTENTSKU", 10);},
            "Invalid sku NONEXISTENTSKU")
        );
    }

    TEST(Services, test_commits)
    {
        Adapters::Repository::FakeRepository repo;
        Services::UoW::FakeUnitOfWork uow(repo);
        Services::AddBatch(uow, "b1", "OMINOUS-MIRROR", 100);
        Services::UoW::FakeUnitOfWork uowSecond(repo);
        Services::Allocate(uowSecond, "o1", "OMINOUS-MIRROR", 10);

        EXPECT_TRUE(uowSecond.IsCommited());
    }

}
