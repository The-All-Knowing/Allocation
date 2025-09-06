#pragma once

#include <gtest/gtest.h>

#include "Adapters/Database/Session/SessionPool.hpp"
#include "Utilities/ConfigReaders.hpp"


namespace Allocation::Tests
{
    /// @brief Фикстура для инициализации БД.
    class Database_Fixture : public testing::Test
    {
    public:
        /// @brief Настройка пула сессий и регистрация PostgreSQL коннектора. Выполняется один раз
        /// для всего набора тестов.
        static void SetUpTestSuite()
        {
            if (auto& sessionPool = Adapters::Database::SessionPool::Instance();
                !sessionPool.IsConfigured())
            {
                auto config = ReadSystemDatabaseConfigs();
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
    class UoW_Fixture : public Database_Fixture
    {
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
                    session << "DELETE FROM allocation.products WHERE sku = $1",
                        Poco::Data::Keywords::use(_sku), Poco::Data::Keywords::now;
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
        [[nodiscard]] ProductCleanup CleanupForSku(const std::string& sku) const
        {
            return ProductCleanup(sku);
        }
    };
}