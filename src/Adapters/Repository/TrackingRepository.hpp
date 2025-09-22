#pragma once

#include "Domain/Ports/IUpdatableRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief Репозиторий для отслеживания изменений агрегатов продуктов.
    class TrackingRepository final : public Domain::IUpdatableRepository
    {
    public:
        /// @brief Конструктор.
        /// @param repo Отслеживаемый репозиторий.
        TrackingRepository(Domain::IUpdatableRepository& repo);

        /// @brief Общий интерфейс для добавления или обновления агрегат-продукта.
        /// @param product Агрегат-продукт для добавления или обновления.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        void Add(Domain::ProductPtr product) override;

        /// @brief Возвращает агрегат-продукт по его артикулу.
        /// @param sku Артикул товара.
        /// @return Агрегат-продукт, если найден, иначе nullptr.
        [[nodiscard]] Domain::ProductPtr Get(const std::string& sku) override;

        /// @brief Возвращает агрегат-продукт по идентификатору партии включённого в него.
        /// @param batchRef Ссылка на партию.
        /// @return Агрегат-продукт, если найден, иначе nullptr.
        [[nodiscard]] Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override;

        /// @brief Получает все отслеживаемые агрегаты.
        /// @return Отслеживаемые продукты и их изначальные версии.
        [[nodiscard]] std::vector<std::pair<Domain::ProductPtr, size_t>> GetSeen() const noexcept;

        /// @brief Очищает все наблюдаемые продукты.
        void Clear() noexcept;

        /// @brief Обновляет агрегат-продукт в репозитории.
        /// @param product Агрегат-продукт для обновления.
        /// @param oldVersion Изначальная версия агрегата, загруженная из репозитория.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        void Update(Domain::ProductPtr product, size_t oldVersion) override;

    private:
        Domain::IUpdatableRepository& _repo;
        std::unordered_map<std::string, std::pair<Domain::ProductPtr, size_t>>
            _skuToProductAndOldVersion;
    };
}