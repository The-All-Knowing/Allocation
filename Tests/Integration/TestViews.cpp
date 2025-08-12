#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Domain/Commands/Allocate.hpp"
#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "Domain/Commands/CreateBatch.hpp"
#include "Services/MessageBus/MessageBus.hpp"
#include "Services/UoW/SqlUnitOfWork.hpp"
#include "Services/Views.hpp"
#include "Utilities/SqlFixture.hpp"


namespace Allocation::Tests
{
    using ::testing::UnorderedElementsAre;

    TEST_F(SqlFixture, test_allocations_view)
    {
        try
        {
            using namespace std::chrono;
            const year_month_day today(2020y, January, 31d);
            auto& messageBus = Services::MessageBus::Instance();

            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::CreateBatch>("sku1batch", "sku1", 50));
            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::CreateBatch>("sku2batch", "sku2", 50, today));
            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::Allocate>("order1", "sku1", 20));
            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::Allocate>("order1", "sku2", 20));

            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::CreateBatch>(
                    "sku1batch-later", "sku1", 50, today));
            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::Allocate>("otherorder", "sku1", 30));
            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::Allocate>("otherorder", "sku2", 10));

            auto views = Services::Views::Allocations(
                "order1", std::make_shared<Services::UoW::SqlUnitOfWork>());

            EXPECT_THAT(views,
                UnorderedElementsAre(std::pair<std::string, std::string>("sku1", "sku1batch"),
                    std::pair<std::string, std::string>("sku2", "sku2batch")));
        }
        catch (const Poco::Exception& e)
        {
            FAIL() << e.displayText();
        }
    }

    TEST_F(SqlFixture, test_deallocation)
    {
        try
        {
            using namespace std::chrono;
            const year_month_day today(2020y, January, 31d);
            auto& messageBus = Services::MessageBus::Instance();

            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::CreateBatch>("b1", "sku1", 50));
            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::CreateBatch>("b2", "sku1", 50, today));
            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::Allocate>("o1", "sku1", 40));
            messageBus.Handle(Allocation::Services::UoW::SqlUowFactory,
                std::make_shared<Domain::Commands::ChangeBatchQuantity>("b1", 10));

            auto views = Services::Views::Allocations(
                "o1", std::make_shared<Services::UoW::SqlUnitOfWork>());

            ///@todo: поправить
            EXPECT_THAT(
                views, UnorderedElementsAre(std::pair<std::string, std::string>("sku1", "b1"),
                           std::pair<std::string, std::string>("sku1", "b2")));
        }
        catch (const Poco::Exception& e)
        {
            FAIL() << e.displayText();
        }
    }
}