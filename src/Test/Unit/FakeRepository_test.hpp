#pragma once

#include "Precompile.hpp"

#include "Domain/Ports/IRepository.hpp"
#include "Domain/Product/Product.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief Фейковый репозиторий для тестирования
    class FakeRepository final : public Domain::IRepository
    {
    public:
        /// @brief Конструктор.
        FakeRepository() = default;

        /// @brief Конструктор.
        /// @param init Инициализирующий список продуктов.
        FakeRepository(const std::vector<Domain::Product>& init);

        /// @brief Добавляет продукт в репозиторий.
        /// @param product Продукт для добавления.
        void Add(const Domain::Product& product) override;

        /// @brief Получает продукт из репозитория.
        /// @param SKU Артикул продукта.
        /// @return Продукт с заданным артикулом.
        [[nodiscard]] Domain::ProductPtr Get(std::string_view SKU) override;

        /// @brief Получает продукт из репозитория по ссылке партии.
        /// @param batchRef Ссылка на партию.
        /// @param byBatch Флаг, указывающий, нужно ли искать по партии.
        /// @return Продукт с заданной ссылкой партии.
        [[nodiscard]] Domain::ProductPtr Get(std::string_view batchRef, bool byBatch) override;

    private:
        std::unordered_map<std::string, Domain::ProductPtr> _skuByProduct;
    };
}