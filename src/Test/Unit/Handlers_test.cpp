#include <gtest/gtest.h>

#include "Domain/Commands/Allocate.hpp"
#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "Domain/Commands/CreateBatch.hpp"
#include "Domain/Events/OutOfStock.hpp"
#include "Infrastructure/Services/Exceptions/Errors.hpp"
#include "Infrastructure/Services/MessageBus/MessageBus.hpp"
#include "Test/Utilities/Common_test.hpp"
#include "Test/Utilities/FakeUnitOfWork_test.hpp"


namespace Allocation::Tests
{
    TEST(TestAddBatch, test_for_new_product)
    {
        FakeUnitOfWork uow;
        Services::MessageBus::Instance().Handle(
            std::make_shared<Domain::Commands::CreateBatch>("b1", "CRUNCHY-ARMCHAIR", 100), uow);
        EXPECT_TRUE(uow.GetProductRepository().Get("CRUNCHY-ARMCHAIR"));
        EXPECT_TRUE(uow.IsCommited());
    }

    TEST(TestAddBatch, test_for_existing_product)
    {
        FakeUnitOfWork uow;
        auto& messagebus = Services::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("b1", "GARISH-RUG", 100), uow);
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("b2", "GARISH-RUG", 99), uow);
        auto product = uow.GetProductRepository().Get("GARISH-RUG");
        auto batches = product->GetBatches();
        auto references =
            batches | std::views::transform([](const auto& b) { return b.GetReference(); });
        EXPECT_NE(std::ranges::find(references, "b2"), references.end());
    }

    TEST(TestAllocate, test_allocates)
    {
        FakeUnitOfWork uow;
        auto& messagebus = Services::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("batch1", "COMPLICATED-LAMP", 100),
            uow);
        messagebus.Handle(
            std::make_shared<Domain::Commands::Allocate>("o1", "COMPLICATED-LAMP", 10), uow);
        auto batches = uow.GetProductRepository().Get("COMPLICATED-LAMP")->GetBatches();
        EXPECT_EQ(batches.size(), 1);
        EXPECT_EQ(batches.front().GetAvailableQuantity(), 90);
    }

    TEST(TestAllocate, test_errors_for_invalid_sku)
    {
        FakeUnitOfWork uow;
        auto& messagebus = Services::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("batch1", "COMPLICATED-LAMP", 100),
            uow);

        EXPECT_TRUE(ThrowsWithMessage<Services::Exceptions::InvalidSku>(
            [&]()
            {
                messagebus.Handle(
                    std::make_shared<Domain::Commands::Allocate>("o1", "NONEXISTENTSKU", 10), uow);
            },
            "Invalid sku NONEXISTENTSKU"));
    }

    TEST(TestAllocate, test_commits)
    {
        FakeUnitOfWork uow;
        auto& messagebus = Services::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("b1", "OMINOUS-MIRROR", 100), uow);
        messagebus.Handle(
            std::make_shared<Domain::Commands::Allocate>("o1", "OMINOUS-MIRROR", 10), uow);
        EXPECT_TRUE(uow.IsCommited());
    }

    TEST(TestAllocate, test_sends_email_on_out_of_stock_error)
    {
        FakeUnitOfWork uow;
        auto& messagebus = Services::MessageBus::Instance();
        //messagebus.SubscribeToEvent<Domain::Events::OutOfStock>();
    }

    TEST(TestChangeBatchQuantity, test_changes_available_quantity)
    {
        FakeUnitOfWork uow;
        auto& messagebus = Services::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("batch1", "ADORABLE-SETTEE", 100), uow);
        auto batches = uow.GetProductRepository().Get("ADORABLE-SETTEE")->GetBatches();
        EXPECT_EQ(batches.size(), 1);
        EXPECT_EQ(batches.front().GetAvailableQuantity(), 100);

        messagebus.Handle(
            std::make_shared<Domain::Commands::ChangeBatchQuantity>("batch1", 50), uow);
        EXPECT_EQ(batches.size(), 1);
        EXPECT_EQ(batches.front().GetAvailableQuantity(), 50);
    }

    TEST(TestChangeBatchQuantity, test_reallocates_if_necessary)
    {
        using namespace std::chrono;
        const year_month_day today{2007y, October, 18d};
        FakeUnitOfWork uow;
        auto& messagebus = Services::MessageBus::Instance();
        std::vector<Domain::Commands::CommandPtr> history{
            std::make_shared<Domain::Commands::CreateBatch>("batch1", "INDIFFERENT-TABLE", 50),
            std::make_shared<Domain::Commands::CreateBatch>(
                "batch2", "INDIFFERENT-TABLE", 50, today),
            std::make_shared<Domain::Commands::Allocate>("order1", "INDIFFERENT-TABLE", 20),
            std::make_shared<Domain::Commands::Allocate>("order2", "INDIFFERENT-TABLE", 20),
        };
        for (auto& command : history)
            messagebus.Handle(command, uow);
        auto batches = uow.GetProductRepository().Get("INDIFFERENT-TABLE")->GetBatches();
        EXPECT_EQ(batches.size(), 2);
        EXPECT_EQ(batches[0].GetAvailableQuantity(), 10);
        EXPECT_EQ(batches[1].GetAvailableQuantity(), 50);

        messagebus.Handle(
            std::make_shared<Domain::Commands::ChangeBatchQuantity>("batch1", 25), uow);

        batches = uow.GetProductRepository().Get("INDIFFERENT-TABLE")->GetBatches();
        EXPECT_EQ(batches.size(), 2);
        EXPECT_EQ(batches[0].GetAvailableQuantity(), 5);
        EXPECT_EQ(batches[1].GetAvailableQuantity(), 30);
    }
}