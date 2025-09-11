#include "ServiceLayer/Views.hpp"

#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include "Adapters/Database/Mappers/ProductMapper.hpp"
#include "Adapters/Database/Session/SessionPool.hpp"
#include "Domain/Commands/Allocate.hpp"
#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "Domain/Commands/CreateBatch.hpp"
#include "ServiceLayer/MessageBus/Handlers/Handlers.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "Tests/Utilities/DatabaseFixture_test.hpp"


namespace Allocation::Tests
{
    using ::testing::UnorderedElementsAre;
    using namespace std::chrono;
    const year_month_day today(2020y, January, 31d);

    TEST_F(Views_Fixture, test_allocations_view)
    {
        auto& messageBus = ServiceLayer::MessageBus::Instance();
        messageBus.Handle(std::make_shared<Domain::Commands::CreateBatch>("sku1batch", "sku1", 50));
        messageBus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("sku2batch", "sku2", 50, today));
        messageBus.Handle(std::make_shared<Domain::Commands::Allocate>("order1", "sku1", 20));
        messageBus.Handle(std::make_shared<Domain::Commands::Allocate>("order1", "sku2", 20));

        messageBus.Handle(
            std::make_shared<Domain::Commands::CreateBatch>("sku1batch-later", "sku1", 50, today));
        messageBus.Handle(std::make_shared<Domain::Commands::Allocate>("otherorder", "sku1", 30));
        messageBus.Handle(std::make_shared<Domain::Commands::Allocate>("otherorder", "sku2", 10));

        ServiceLayer::UoW::SqlUnitOfWork uow;
        auto views = ServiceLayer::Views::Allocations("order1", uow);

        EXPECT_THAT(
            views, UnorderedElementsAre(std::pair<std::string, std::string>("sku1", "sku1batch"),
                       std::pair<std::string, std::string>("sku2", "sku2batch")));

        CleanupForReference("sku1batch");
        CleanupForReference("sku2batch");
        CleanupForSku("sku1");
        CleanupForSku("sku2");
    }

    TEST_F(Views_Fixture, test_deallocation)
    {
        const year_month_day today(2020y, January, 31d);
        auto& messageBus = ServiceLayer::MessageBus::Instance();

        messageBus.Handle(std::make_shared<Domain::Commands::CreateBatch>("b1", "sku1", 50));
        messageBus.Handle(std::make_shared<Domain::Commands::CreateBatch>("b2", "sku1", 50, today));
        messageBus.Handle(std::make_shared<Domain::Commands::Allocate>("o1", "sku1", 40));
        messageBus.Handle(std::make_shared<Domain::Commands::ChangeBatchQuantity>("b1", 10));

        ServiceLayer::UoW::SqlUnitOfWork uow;
        auto views = ServiceLayer::Views::Allocations("o1", uow);

        EXPECT_THAT(views, UnorderedElementsAre(std::pair<std::string, std::string>("sku1", "b1"),
                               std::pair<std::string, std::string>("sku1", "b2")));

        CleanupForReference("b1");
        CleanupForReference("b2");
        CleanupForSku("sku1");
    }
}