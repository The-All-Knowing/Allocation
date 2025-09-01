#pragma once

#include "Domain/Product/Product.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс репозитория для работы с продуктами.
    class IRepository
    {
    public:
        /// @brief Деструктор.
        virtual ~IRepository() = default;

        /// @brief Добавляет продукт в репозиторий.
        /// @param product Продукт для добавления.
        virtual void Add(const Product& product) = 0;

        /// @brief Получает продукт из репозитория.
        /// @param SKU Артикул продукта.
        /// @return Продукт с заданным артикулом.
        [[nodiscard]] virtual Domain::ProductPtr Get(std::string_view SKU) = 0;

        /// @brief Получает продукт из репозитория по ссылке партии.
        /// @param batchRef Ссылка на партию.
        /// @return Продукт с заданной ссылкой партии.
        [[nodiscard]] virtual Domain::ProductPtr GetByBatchRef(std::string_view batchRef) = 0;
    };
}