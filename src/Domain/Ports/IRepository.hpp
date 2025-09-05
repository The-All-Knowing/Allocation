#pragma once

#include "Domain/Product/Product.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс репозитория для работы с продуктами.
    /// @note Необходимо использовать данный интерфейс при раборте с внешними системами.
    class IRepository
    {
    public:
        /// @brief Конструктор.
        IRepository() = default;

        /// @brief Деструктор.
        virtual ~IRepository() = default;

        /// @brief Добавляет/обновляет продукт в репозиторий.
        /// @param product Продукт для добавления.
        virtual void Add(Domain::ProductPtr product) = 0;

        /// @brief Получает продукт из репозитория.
        /// @param SKU Артикул продукта.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] virtual Domain::ProductPtr Get(const std::string& SKU) = 0;

        /// @brief Получает продукт из репозитория по ссылке партии.
        /// @param batchRef Ссылка на партию.
        /// @return Продукт с заданной ссылкой партии, иначе nullptr.
        [[nodiscard]] virtual Domain::ProductPtr GetByBatchRef(const std::string& batchRef) = 0;

    private:
        /// @brief Удалённый конструктор копирования.
        IRepository(const IRepository&) = delete;

        /// @brief Удалённый конструктор перемещения.
        IRepository(IRepository&&) = delete;

        /// @brief Удалённый оператор присвоения копирования.
        IRepository& operator=(const IRepository&) = delete;

        /// @brief Удалённый оператор присвоения перемещения.
        IRepository& operator=(IRepository&&) = delete;
    };
}