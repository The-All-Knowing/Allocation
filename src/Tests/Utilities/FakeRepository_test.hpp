#pragma once

#include "Domain/Ports/IUpdatableRepository.hpp"
#include "Domain/Product/Product.hpp"


namespace Allocation::Tests
{
    /// @brief Фейковый репозиторий для тестирования.
    class FakeRepository final : public Domain::IUpdatableRepository
    {
    public:
        /// @brief Конструктор.
        FakeRepository() = default;

        /// @brief Конструктор.
        /// @param init Инициализирующий список продуктов.
        FakeRepository(const std::vector<Domain::ProductPtr>& init)
        {
            for (const auto& prod : init)
                _skuByProduct.insert({prod->GetSKU(), prod});
        }

        /// @brief Добавляет или обновляет продукт в репозиторий.
        /// @param product Продукт для добавления.
        void Add(Domain::ProductPtr product) override
        {
            _skuByProduct.insert_or_assign(product->GetSKU(), product);
        }

        /// @brief Получает продукт из репозитория.
        /// @param sku Артикул продукта.
        /// @return Продукт с заданным артикулом.
        [[nodiscard]] Domain::ProductPtr Get(const std::string& sku) override
        {
            auto it = _skuByProduct.find(sku);
            if (it != _skuByProduct.end())
                return it->second;
            return nullptr;
        }

        /// @brief Получает продукт из репозитория по ссылке партии.
        /// @param batchRef Ссылка на партию.
        /// @return Продукт с заданной ссылкой партии.
        [[nodiscard]] virtual Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override
        {
            for (const auto& [_, product] : _skuByProduct)
                if (product->GetBatch(batchRef) != std::nullopt)
                    return product;
            return nullptr;
        }

        /// @brief Обновляет продукт.
        /// @param product Продукт для добавления.
        /// @note Реализовано для работы с TrackingRepository.
        virtual void Update(Domain::ProductPtr product, size_t) override
        {
            _skuByProduct.insert_or_assign(product->GetSKU(), product);
        }

        std::unordered_map<std::string, Domain::ProductPtr> _skuByProduct;
    };
}