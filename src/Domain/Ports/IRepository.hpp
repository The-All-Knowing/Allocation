#pragma once

#include "Domain/Product/Product.hpp"


namespace Allocation::Adapters::Repository
{
    class TrackingRepository;
}

namespace Allocation::Domain
{
    /// @brief Интерфейс репозитория для работы с продуктами.
    /// @note Дочернии классы должны использоваться с TrackingRepository.
    class IRepository
    {
    public:
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
        /// @brief Обновляет продукт.
        /// @param product Продукт для добавления.
        /// @param oldVersion Прошлая версия продукта.
        /// @note Необходимо реализовать для работы с TrackingRepository.
        virtual void Update(
            Domain::ProductPtr product, std::optional<int> oldVersion = std::nullopt) = 0;

        friend Allocation::Adapters::Repository::TrackingRepository;
    };
}