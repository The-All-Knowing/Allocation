#include <gtest/gtest.h>

#include "CommonFunctions.h"
#include "Adapters/Repository/FakeRepository.h"
#include "Services.h"
#include "Services/UoW/FakeUnitOfWork.h"
#include "Services/EventBus/EventBus.h"
#include "Services/Exceptions/InvalidSku.h"
#include "Domain/Events/OutOfStock.h"


namespace Allocation::Tests
{
    TEST(Services, test_add_batch_for_new_product)
    {
        Services::UoW::FakeUnitOfWork uow;
        Services::AddBatch(uow, "b1", "CRUNCHY-ARMCHAIR", 100);

        EXPECT_TRUE(uow.GetProductRepository().Get("CRUNCHY-ARMCHAIR"));
        EXPECT_TRUE(uow.IsCommited());
    }

    TEST(Services, test_add_batch_for_existing_product)
    {
        Services::UoW::FakeUnitOfWork uow;
        Services::AddBatch(uow, "b1", "GARISH-RUG", 100);
        Services::AddBatch(uow, "b2", "GARISH-RUG", 99);

        auto product = uow.GetProductRepository().Get("GARISH-RUG");
        auto batches = product->GetBatches();
        auto references = batches | 
                        std::views::transform([](const auto& b) { return b.GetReference(); });

        EXPECT_NE(std::ranges::find(references, "b2"), references.end());
    }

    TEST(Services, test_allocate_returns_allocation)
    {
        Services::UoW::FakeUnitOfWork uow;
        Services::AddBatch(uow, "b1", "COMPLICATED-LAMP", 100);
        auto result = Services::Allocate(uow, "o1", "COMPLICATED-LAMP", 10);

        EXPECT_EQ(result, "b1");
    }

    TEST(Services, test_allocate_errors_for_invalid_sku)
    {
        Services::UoW::FakeUnitOfWork uow;
        Services::AddBatch(uow, "b1", "AREALSKU", 100);

        EXPECT_TRUE(
        ThrowsWithMessage<Services::Exceptions::InvalidSku>(
            [&]() {Services::Allocate(uow, "o1", "NONEXISTENTSKU", 10);},
            "Invalid sku NONEXISTENTSKU")
        );
    }

    TEST(Services, test_commits)
    {
        Services::UoW::FakeUnitOfWork uow;
        Services::AddBatch(uow, "b1", "OMINOUS-MIRROR", 100);
        Services::Allocate(uow, "o1", "OMINOUS-MIRROR", 10);

        EXPECT_TRUE(uow.IsCommited());
    }

    TEST(Services, test_sends_email_on_out_of_stock_error)
    {
        auto handler = [](std::shared_ptr<Domain::Events::OutOfStock> event)
        {
            EXPECT_EQ(event->SKU, "POPULAR-CURTAINS");
        };

        Services::EventBus::Instance().Subscribe<Domain::Events::OutOfStock>(handler);

        Services::UoW::FakeUnitOfWork uow;
        Services::AddBatch(uow, "b1", "POPULAR-CURTAINS", 9);

        Services::Allocate(uow, "o1", "POPULAR-CURTAINS", 10);
    }
}