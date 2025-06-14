#include <gtest/gtest.h>

#include "CommonFunctions.h"
#include "Services/UoW/FakeUnitOfWork.h"
#include "Services/MessageBus/MessageBus.h"
#include "Services/Exceptions/InvalidSku.h"
#include "Domain/Events/OutOfStock.h"
#include "Domain/Events/BatchCreated.h"
#include "Domain/Events/AllocationRequired.h"
#include "Domain/Events/BatchQuantityChanged.h"


namespace Allocation::Tests
{
    TEST(Handlers, test_for_new_product)
    {
        auto uow = std::make_shared<Services::UoW::FakeUnitOfWork>();
        auto FakeUowFactory = [uow]() {return uow;};

        Services::MessageBus::Instance().Handle(FakeUowFactory, std::make_shared<Domain::Events::BatchCreated>("b1", "CRUNCHY-ARMCHAIR", 100));

        EXPECT_TRUE(uow->GetProductRepository().Get("CRUNCHY-ARMCHAIR"));
        EXPECT_TRUE(uow->IsCommited());
    }

    TEST(Handlers, test_for_existing_product)
    {
        auto uow = std::make_shared<Services::UoW::FakeUnitOfWork>();
        auto FakeUowFactory = [uow]() {return uow;};

        auto& messagebus = Services::MessageBus::Instance();
        messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::BatchCreated>("b1", "GARISH-RUG", 100));
        messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::BatchCreated>("b2", "GARISH-RUG", 99));

        auto product = uow->GetProductRepository().Get("GARISH-RUG");
        auto batches = product->GetBatches();
        auto references = batches | 
                        std::views::transform([](const auto& b) { return b.GetReference(); });

        EXPECT_NE(std::ranges::find(references, "b2"), references.end());
    }

    TEST(Handlers, test_returns_allocation)
    {
        auto uow = std::make_shared<Services::UoW::FakeUnitOfWork>();
        auto FakeUowFactory = [uow]() {return uow;};

        auto& messagebus = Services::MessageBus::Instance();
        messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::BatchCreated>("b1", "COMPLICATED-LAMP", 100));

        auto result = messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::AllocationRequired>("o1", "COMPLICATED-LAMP", 10));

        EXPECT_EQ(result.front(), "b1");
    }

    TEST(Handlers, test_errors_for_invalid_sku)
    {
        auto uow = std::make_shared<Services::UoW::FakeUnitOfWork>();
        auto FakeUowFactory = [uow]() {return uow;};

        Services::MessageBus::Instance().Handle(
            FakeUowFactory, 
            std::make_shared<Domain::Events::BatchCreated>("b1", "AREALSKU", 100)
        );

        EXPECT_TRUE(ThrowsWithMessage<Services::Exceptions::InvalidSku>(
            [&]() {
                Services::MessageBus::Instance().Handle(
                    FakeUowFactory,
                    std::make_shared<Domain::Events::AllocationRequired>("o1", "NONEXISTENTSKU", 10)
                );
            },
            "Invalid sku NONEXISTENTSKU"
        ));
    }

    TEST(Handlers, test_commits)
    {
        auto uow = std::make_shared<Services::UoW::FakeUnitOfWork>();
        auto FakeUowFactory = [uow]() {return uow;};

        auto& messagebus = Services::MessageBus::Instance();
        messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::BatchCreated>("b1", "OMINOUS-MIRROR", 100));
        messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::AllocationRequired>("o1", "OMINOUS-MIRROR", 10));

        EXPECT_TRUE(uow->IsCommited());
    }

    TEST(Handlers, test_sends_email_on_out_of_stock_error)
    {
        auto handler = [](std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Events::OutOfStock> event) -> std::optional<std::string>
        {
            EXPECT_EQ(event->SKU, "POPULAR-CURTAINS");

            return std::nullopt;
        };

        auto& messagebus = Services::MessageBus::Instance();

        messagebus.Subscribe<Domain::Events::OutOfStock>(handler);

        auto uow = std::make_shared<Services::UoW::FakeUnitOfWork>();
        auto FakeUowFactory = [uow]() {return uow;};

        messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::BatchCreated>("b1", "POPULAR-CURTAINS", 9));
        messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::AllocationRequired>("o1", "POPULAR-CURTAINS", 10));
    }

    TEST(Handlers, test_changes_available_quantity)
    {
        auto uow = std::make_shared<Services::UoW::FakeUnitOfWork>();
        auto FakeUowFactory = [uow]() {return uow;};

        auto& messagebus = Services::MessageBus::Instance();

        messagebus.Handle(FakeUowFactory,  std::make_shared<Domain::Events::BatchCreated>("batch1", "ADORABLE-SETTEE", 100));
        auto batch = uow->GetProductRepository().Get("ADORABLE-SETTEE")->GetBatches().front();
        EXPECT_EQ(batch.GetAvailableQuantity(), 100);

        messagebus.Handle(FakeUowFactory, std::make_shared<Domain::Events::BatchQuantityChanged>("batch1", 50));
        batch = uow->GetProductRepository().Get("ADORABLE-SETTEE")->GetBatches().front();
        EXPECT_EQ(batch.GetAvailableQuantity(), 50);
    }

    TEST(Handlers, test_reallocates_if_necessary)
    {
        using namespace std::chrono;
        const year_month_day today(2020y, January, 31d);

        auto uow = std::make_shared<Services::UoW::FakeUnitOfWork>();
        auto FakeUowFactory = [uow]() {return uow;};

        std::vector<Domain::Events::IEventPtr> eventHistory{
            std::make_shared<Domain::Events::BatchCreated>("batch1", "INDIFFERENT-TABLE", 50),
            std::make_shared<Domain::Events::BatchCreated>("batch2", "INDIFFERENT-TABLE", 50, today),
            std::make_shared<Domain::Events::AllocationRequired>("order1", "INDIFFERENT-TABLE", 20),
            std::make_shared<Domain::Events::AllocationRequired>("order2", "INDIFFERENT-TABLE", 20)
        };

        for(auto& event : eventHistory)
            Services::MessageBus::Instance().Handle(FakeUowFactory, event);

        auto batches = uow->GetProductRepository().Get("INDIFFERENT-TABLE")->GetBatches();
        EXPECT_EQ(batches[0].GetAvailableQuantity(), 10);
        EXPECT_EQ(batches[1].GetAvailableQuantity(), 50);

        Services::MessageBus::Instance().Handle(FakeUowFactory, std::make_shared<Domain::Events::BatchQuantityChanged>("batch1", 25));

        batches = uow->GetProductRepository().Get("INDIFFERENT-TABLE")->GetBatches();
        EXPECT_EQ(batches[0].GetAvailableQuantity(), 5);
        EXPECT_EQ(batches[1].GetAvailableQuantity(), 30);
    }
}