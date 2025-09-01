#pragma once

#include "Precompile.hpp"

#include "Domain/Product/Product.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    /// @brief Маппер для работы с продуктами в базе данных.
    class ProductMapper
    {
    public:
        /// @brief Конструктор маппера.
        /// @param session Сессия базы данных.
        explicit ProductMapper(Poco::Data::Session& session);

        /// @brief Проверяет, существует ли продукт с заданным SKU.
        /// @param SKU Идентификатор продукта.
        /// @return true, если продукт существует, иначе false.
        [[nodiscard]] bool IsExists(std::string SKU);
        [[nodiscard]] Domain::ProductPtr FindBySKU(std::string SKU);
        [[nodiscard]] Domain::ProductPtr FindByBatchRef(std::string ref);
        void Update(const Domain::Product& product);
        void Insert(const Domain::Product& product);
        [[nodiscard]] bool UpdateVersion(std::string SKU, size_t oldVersion, size_t newVersion);

    private:
        Poco::Data::Session& _session;
    };
}