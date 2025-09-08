#pragma once

#include <gtest/gtest.h>

#include "Adapters/Database/Mappers/ProductMapper.hpp"
#include "Adapters/Database/Session/SessionPool.hpp"
#include "ServiceLayer/MessageBus/Handlers/Handlers.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "Utilities/ConfigReaders.hpp"


namespace Allocation::Tests
{
    /// @brief Фикстура для инициализации БД.
    class Database_Fixture : public testing::Test
    {
    public:
        /// @brief Настройка пула сессий и регистрация PostgreSQL коннектора.
        static void SetUpTestSuite()
        {
            if (auto& sessionPool = Adapters::Database::SessionPool::Instance();
                !sessionPool.IsConfigured())
            {
                auto config = ReadDatabaseConfigurations();
                sessionPool.Configure(config);
                Poco::Data::PostgreSQL::Connector::registerConnector();
            }
        }

    protected:
        void SetUp() override
        {
            _session = Adapters::Database::SessionPool::Instance().GetSession();
            _session.begin();
        }

        void TearDown() override
        {
            try
            {
                _session.rollback();
            }
            catch (...)
            {
            }
        }

        Poco::Data::Session _session{Adapters::Database::SessionPool::Instance().GetSession()};
    };

    /// @brief Фикстура для работы с Unit of Work с поддержкой автоматической очистки продуктов.
    class UoW_Fixture : public testing::Test
    {
    public:
        /// @brief Настройка пула сессий и регистрация PostgreSQL коннектора.
        static void SetUpTestSuite()
        {
            if (auto& sessionPool = Adapters::Database::SessionPool::Instance();
                !sessionPool.IsConfigured())
            {
                auto config = ReadDatabaseConfigurations();
                sessionPool.Configure(config);
                Poco::Data::PostgreSQL::Connector::registerConnector();
            }
        }

    protected:
        /// @brief RAII-хелпер для удаления продукта из БД по SKU.
        class ProductCleanup
        {
        public:
            /// @param sku Артикул удаляемого продукта.
            explicit ProductCleanup(std::string sku) : _sku(std::move(sku)) {}

            /// @brief Удаляет продукт из базы при выходе из области видимости.
            ~ProductCleanup()
            {
                try
                {
                    auto session = Adapters::Database::SessionPool::Instance().GetSession();
                    Adapters::Database::Mapper::ProductMapper productMapper(session);
                    productMapper.Delete(productMapper.FindBySKU(_sku));
                    session.commit();
                }
                catch (...)
                {
                }
            }

        private:
            std::string _sku;
        };

        /// @brief Создаёт RAII-объект для удаления продукта по SKU.
        /// @param sku Артикул продукта.
        /// @return RAII-объект, который удалит продукт при уничтожении.
        ProductCleanup CleanupForSku(const std::string& sku) const { return ProductCleanup(sku); }
    };

    class Views_Fixture : public UoW_Fixture
    {
    public:
        /// @brief Настройка тестового окружения.
        ///
        /// Выполняется один раз для всего набора тестов:
        /// 1. Настройка пула соединений с базой данных и регистрация PostgreSQL-коннектора.
        /// 2. Инициализация шины сообщений:
        ///    - Подписка на события аллокаций (Allocated, Deallocated) с обработчиками для
        ///    обновления read-model.
        ///    - Настройка командных обработчиков для бизнес-команд (Allocate, CreateBatch,
        ///    ChangeBatchQuantity).
        static void SetUpTestSuite()
        {
            if (auto& sessionPool = Adapters::Database::SessionPool::Instance();
                !sessionPool.IsConfigured())
            {
                auto config = ReadDatabaseConfigurations();
                sessionPool.Configure(config);
                Poco::Data::PostgreSQL::Connector::registerConnector();
            }

            auto& messagebus = ServiceLayer::MessageBus::Instance();
            messagebus.SubscribeToEvent<Allocation::Domain::Events::Allocated>(
                ServiceLayer::Handlers::AddAllocationToReadModel);
            messagebus.SubscribeToEvent<Allocation::Domain::Events::Deallocated>(
                ServiceLayer::Handlers::RemoveAllocationFromReadModel);
            messagebus.SubscribeToEvent<Allocation::Domain::Events::Deallocated>(
                ServiceLayer::Handlers::Reallocate);

            messagebus.SetCommandHandler<Allocation::Domain::Commands::Allocate>(
                ServiceLayer::Handlers::Allocate);
            messagebus.SetCommandHandler<Allocation::Domain::Commands::CreateBatch>(
                ServiceLayer::Handlers::AddBatch);
            messagebus.SetCommandHandler<Allocation::Domain::Commands::ChangeBatchQuantity>(
                ServiceLayer::Handlers::ChangeBatchQuantity);
        }

    protected:
        /// @brief RAII-хелпер для удаления записей read-model (CQRS) по reference.
        class ViewCleanup
        {
        public:
            /// @param reference Ссылка на удаляемую партию.
            explicit ViewCleanup(std::string reference) : _reference(std::move(reference)) {}

            /// @brief Удаляет записи read-model по reference при выходе из области видимости.
            ~ViewCleanup()
            {
                try
                {
                    auto session = Adapters::Database::SessionPool::Instance().GetSession();
                    session << "DELETE FROM allocation.allocations_view WHERE batchref = $1",
                        Poco::Data::Keywords::use(_reference), Poco::Data::Keywords::now;
                    session.commit();
                }
                catch (...)
                {
                }
            }

        private:
            std::string _reference;
        };

        /// @brief Создаёт RAII-объект для удаления записей read-model по reference.
        /// @param reference Ссылка на удаляемую партию.
        /// @return RAII-объект, который удалит записи read-model при уничтожении.
        ViewCleanup CleanupForReference(const std::string& reference) const
        {
            return ViewCleanup(reference);
        }
    };
}