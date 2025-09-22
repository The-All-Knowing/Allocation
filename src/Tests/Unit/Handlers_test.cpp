#include "ServiceLayer/MessageBus/Handlers/Handlers.hpp"

#include <gtest/gtest.h>

#include "Domain/Commands/Allocate.hpp"
#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "Domain/Commands/CreateBatch.hpp"
#include "Domain/Events/OutOfStock.hpp"
#include "ServiceLayer/Exceptions.hpp"
#include "ServiceLayer/MessageBus/Handlers/NotificationHandler.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "Tests/Utilities/Common_test.hpp"
#include "Tests/Utilities/FakeUnitOfWork_test.hpp"


namespace Allocation::Tests
{
    class Handlers_TestAddBatch : public testing::Test
    {
    public:
        static void SetUpTestSuite()
        {
            auto& messagebus = ServiceLayer::MessageBus::Instance();
            messagebus.ClearHandlers();
            messagebus.SetCommandHandler<Allocation::Domain::Commands::CreateBatch>(
                ServiceLayer::Handlers::AddBatch);
        }
    };

    TEST_F(Handlers_TestAddBatch, test_for_new_product)
    {
        FakeUnitOfWork uow;
        ServiceLayer::MessageBus::Instance().Handle(
            std::make_shared<Domain::Commands::CreateBatch>("b1", "CRUNCHY-ARMCHAIR", 100), uow);
        EXPECT_TRUE(uow.GetProductRepository().Get("CRUNCHY-ARMCHAIR"));
        EXPECT_TRUE(uow.IsCommitted());
    }

    TEST_F(Handlers_TestAddBatch, test_for_existing_product)
    {
        FakeUnitOfWork uow;
        auto& messagebus = ServiceLayer::MessageBus::Instance();
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

    class Handlers_TestAllocate : public testing::Test
    {
    public:
        static void SetUpTestSuite()
        {
            auto& messagebus = ServiceLayer::MessageBus::Instance();
            messagebus.ClearHandlers();
            messagebus.SetCommandHandler<Allocation::Domain::Commands::CreateBatch>(
                ServiceLayer::Handlers::AddBatch);
            messagebus.SetCommandHandler<Allocation::Domain::Commands::Allocate>(
                ServiceLayer::Handlers::Allocate);
        }
    };

    TEST_F(Handlers_TestAllocate, test_allocates)
    {
        FakeUnitOfWork uow;
        auto& messagebus = ServiceLayer::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("batch1", "COMPLICATED-LAMP", 100),
            uow);
        messagebus.Handle(
            std::make_shared<Domain::Commands::Allocate>("o1", "COMPLICATED-LAMP", 10), uow);
        auto batches = uow.GetProductRepository().Get("COMPLICATED-LAMP")->GetBatches();
        EXPECT_EQ(batches.size(), 1);
        EXPECT_EQ(batches.front().GetAvailableQuantity(), 90);
    }

    TEST_F(Handlers_TestAllocate, test_errors_for_invalid_sku)
    {
        FakeUnitOfWork uow;
        auto& messagebus = ServiceLayer::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("batch1", "COMPLICATED-LAMP", 100),
            uow);

        EXPECT_TRUE(ThrowsWithMessage<ServiceLayer::Exceptions::InvalidSku>(
            [&]()
            {
                messagebus.Handle(
                    std::make_shared<Domain::Commands::Allocate>("o1", "NONEXISTENTSKU", 10), uow);
            },
            "Invalid sku: NONEXISTENTSKU"));
    }

    TEST_F(Handlers_TestAllocate, test_commits)
    {
        FakeUnitOfWork uow;
        auto& messagebus = ServiceLayer::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("b1", "OMINOUS-MIRROR", 100), uow);
        messagebus.Handle(
            std::make_shared<Domain::Commands::Allocate>("o1", "OMINOUS-MIRROR", 10), uow);
        EXPECT_TRUE(uow.IsCommitted());
    }

    TEST_F(Handlers_TestAllocate, test_sends_email_on_out_of_stock_error)
    {
        std::unordered_map<std::string, std::vector<std::string>> sentEmails;
        auto mockEmailSender = [&sentEmails](std::string to, std::string message)
        { sentEmails[to].emplace_back(std::move(message)); };

        using FakeNotifications =
            Allocation::ServiceLayer::Handlers::NotificationHandler<Domain::Events::OutOfStock,
                decltype(mockEmailSender)>;

        FakeUnitOfWork uow;
        auto& messagebus = ServiceLayer::MessageBus::Instance();
        messagebus.SubscribeToEvent<Domain::Events::OutOfStock>(FakeNotifications(mockEmailSender));
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("b1", "POPULAR-CURTAINS", 9), uow);
        messagebus.Handle(
            std::make_shared<Domain::Commands::Allocate>("o1", "POPULAR-CURTAINS", 10), uow);

        ASSERT_TRUE(sentEmails.contains("stock@made.com"));
        EXPECT_EQ(sentEmails["stock@made.com"],
            std::vector<std::string>{"Out of stock for POPULAR-CURTAINS"});
    }

    class Handlers_TestChangeBatchQuantity : public testing::Test
    {
    public:
        static void SetUpTestSuite()
        {
            auto& messagebus = ServiceLayer::MessageBus::Instance();
            messagebus.ClearHandlers();
            messagebus.SetCommandHandler<Allocation::Domain::Commands::Allocate>(
                ServiceLayer::Handlers::Allocate);
            messagebus.SetCommandHandler<Allocation::Domain::Commands::CreateBatch>(
                ServiceLayer::Handlers::AddBatch);
            messagebus.SetCommandHandler<Allocation::Domain::Commands::ChangeBatchQuantity>(
                ServiceLayer::Handlers::ChangeBatchQuantity);

            messagebus.SubscribeToEvent<Allocation::Domain::Events::Deallocated>(
                ServiceLayer::Handlers::Reallocate);
        }
    };

    TEST_F(Handlers_TestChangeBatchQuantity, test_changes_available_quantity)
    {
        FakeUnitOfWork uow;
        auto& messagebus = ServiceLayer::MessageBus::Instance();
        messagebus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("batch1", "ADORABLE-SETTEE", 100), uow);
        auto batches = uow.GetProductRepository().Get("ADORABLE-SETTEE")->GetBatches();
        EXPECT_EQ(batches.size(), 1);
        EXPECT_EQ(batches.front().GetAvailableQuantity(), 100);

        messagebus.Handle(
            std::make_shared<Domain::Commands::ChangeBatchQuantity>("batch1", 50), uow);
        batches = uow.GetProductRepository().Get("ADORABLE-SETTEE")->GetBatches();
        EXPECT_EQ(batches.size(), 1);
        EXPECT_EQ(batches.front().GetAvailableQuantity(), 50);
    }

    TEST_F(Handlers_TestChangeBatchQuantity, test_reallocates_if_necessary)
    {
        using namespace std::chrono;
        const year_month_day today{2007y, October, 18d};
        FakeUnitOfWork uow;
        auto& messagebus = ServiceLayer::MessageBus::Instance();
        std::vector<Domain::Commands::CommandPtr> history{
            std::make_shared<Domain::Commands::CreateBatch>("batch1", "INDIFFERENT-TABLE", 50),
            std::make_shared<Domain::Commands::CreateBatch>(
                "batch2", "INDIFFERENT-TABLE", 50, today),
            std::make_shared<Domain::Commands::Allocate>("order1", "INDIFFERENT-TABLE", 20),
            std::make_shared<Domain::Commands::Allocate>("order2", "INDIFFERENT-TABLE", 20),
        };
        for (auto& command : history)
            messagebus.Handle(command, uow);
        auto product = uow.GetProductRepository().Get("INDIFFERENT-TABLE");
        EXPECT_EQ(product->GetBatch("batch1").value().GetAvailableQuantity(), 10);
        EXPECT_EQ(product->GetBatch("batch2").value().GetAvailableQuantity(), 50);

        messagebus.Handle(
            std::make_shared<Domain::Commands::ChangeBatchQuantity>("batch1", 25), uow);

        product = uow.GetProductRepository().Get("INDIFFERENT-TABLE");
        EXPECT_EQ(product->GetBatch("batch1").value().GetAvailableQuantity(), 5);
        EXPECT_EQ(product->GetBatch("batch2").value().GetAvailableQuantity(), 30);
    }
}