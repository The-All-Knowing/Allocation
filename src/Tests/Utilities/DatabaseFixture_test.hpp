#pragma once

#include <gtest/gtest.h>

#include "Adapters/Database/Mappers/ProductMapper.hpp"
#include "Adapters/Database/Session/DatabaseSessionPool.hpp"
#include "ServiceLayer/MessageBus/Handlers/Handlers.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "Utilities/ConfigReaders.hpp"


namespace Allocation::Tests
{
    /// @brief Фикстура для инициализации подключения к БД.
    class Database_Fixture : public testing::Test
    {
    public:
        /// @brief Инициализирует пул сессий.
        static void SetUpTestSuite()
        {
            if (auto& sessionPool = Adapters::Database::DatabaseSessionPool::Instance();
                !sessionPool.IsConfigured())
            {
                auto config = ReadDatabaseConfigurations();
                sessionPool.Configure(config);
            }
        }

    protected:
        /// @brief Открывает транзакцию перед каждым тестом.
        void SetUp() override
        {
            _session = Adapters::Database::DatabaseSessionPool::Instance().GetSession();
            _session.begin();
        }

        /// @brief Откатывает транзакцию после каждого теста.
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

        Poco::Data::Session _session{
            Adapters::Database::DatabaseSessionPool::Instance().GetSession()};
    };

    /// @brief Фикстура для работы с Unit of Work с поддержкой автоматической очистки продуктов.
    class UoW_Fixture : public testing::Test
    {
    public:
        /// @brief Инициализирует пул сессий.
        static void SetUpTestSuite()
        {
            if (auto& sessionPool = Adapters::Database::DatabaseSessionPool::Instance();
                !sessionPool.IsConfigured())
            {
                auto config = ReadDatabaseConfigurations();
                sessionPool.Configure(config);
            }
        }

    protected:
        /// @brief RAII-хелпер для удаления агрегата-продукта из БД по артикулу продукции.
        class ProductCleanup
        {
        public:
            /// @param sku Артикул удаляемого агрегата-продукта.
            explicit ProductCleanup(std::string sku) : _sku(std::move(sku)) {}

            /// @brief Удаляет агрегат-продукт из базы при выходе из области видимости.
            ~ProductCleanup()
            {
                try
                {
                    auto session = Adapters::Database::DatabaseSessionPool::Instance().GetSession();
                    session.begin();
                    Adapters::Database::Mapper::ProductMapper productMapper(session);
                    if (auto product = productMapper.FindBySKU(_sku); product)
                        productMapper.Delete(product);
                    session.commit();
                }
                catch (...)
                {
                }
            }

        private:
            std::string _sku;
        };

        /// @brief Создаёт RAII-объект для удаления агрегата-продукта по артикулу.
        /// @param sku Артикул продукции.
        /// @return RAII-объект, который удалит агрегат-продукт при уничтожении.
        ProductCleanup CleanupProduct(const std::string& sku) const { return ProductCleanup(sku); }
    };

    /// @brief Фикстура для тестирования read-model с поддержкой очистки записей по ссылке на партию
    /// поставки.
    class Views_Fixture : public UoW_Fixture
    {
    public:
        /// @brief Настройка тестового окружения.
        ///
        /// Выполняется один раз для всего набора тестов:
        /// 1. Инициализирует пул соединений с базой данных.
        /// 2. Инициализация шины сообщений:
        ///    - Подписка на события аллокаций (Allocated, Deallocated) с обработчиками для
        ///    обновления read-model.
        ///    - Настройка командных обработчиков для команд (Allocate, CreateBatch,
        ///    ChangeBatchQuantity).
        static void SetUpTestSuite()
        {
            if (auto& sessionPool = Adapters::Database::DatabaseSessionPool::Instance();
                !sessionPool.IsConfigured())
            {
                auto config = ReadDatabaseConfigurations();
                sessionPool.Configure(config);
            }

            auto& messagebus = ServiceLayer::MessageBus::Instance();
            messagebus.ClearHandlers();
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
        /// @brief RAII-хелпер для удаления записей read-model по ссылке на партию поставки.
        class ViewCleanup
        {
        public:
            /// @brief Конструктор.
            /// @param reference Ссылка на удаляемую партию.
            explicit ViewCleanup(std::string reference) : _reference(std::move(reference)) {}

            /// @brief Удаляет записи read-model по ссылке на партию поставки при выходе из области
            /// видимости.
            ~ViewCleanup()
            {
                try
                {
                    auto session = Adapters::Database::DatabaseSessionPool::Instance().GetSession();
                    session.begin();
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

        /// @brief Создаёт RAII-объект для удаления записей read-model по ссылке на партию поставки.
        /// @param reference Ссылка на партию поставки.
        /// @return RAII-объект, который удалит записи read-model при уничтожении.
        ViewCleanup CleanupViewModel(const std::string& reference) const
        {
            return ViewCleanup(reference);
        }
    };
}