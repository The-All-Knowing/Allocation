#pragma once

#include "Precompile.hpp"

#include "Domain/Ports/IUpdatableRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief Репозиторий для отслеживания изменений продуктов.
    class TrackingRepository final : public Domain::IUpdatableRepository
    {
    public:
        /// @brief Конструктор.
        /// @param repo Отслеживаемый репозиторий.
        TrackingRepository(Domain::IUpdatableRepository& repo);

        /// @brief Добавляет или обновляет продукт в репозиторий и отслеживает его.
        /// @param product Продукт для добавления или обновления.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        void Add(Domain::ProductPtr product) override;

        /// @brief Получает продукт из наблюдаемых по его SKU или загружает его.
        /// @param SKU Артикул продукта.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr Get(const std::string& SKU) override;

        /// @brief Получает продукт по ссылке партии из наблюдаемых или загружает его.
        /// @param batchRef Ссылка на партию.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override;

        /// @brief Получает все продукты, которые отслеживались в репозиторий.
        /// @return Отслеживаемые продукты и их изначальные версии.
        [[nodiscard]] std::vector<std::pair<Domain::ProductPtr, int>> GetSeen() const noexcept;

        /// @brief Очищает все наблюдаемые продукты.
        void Clear() noexcept;

        /// @brief Обновляет продукт в репозитории.
        /// @param product Продукт для обновления.
        /// @param oldVersion Прошлая версия продукта.
        void Update(Domain::ProductPtr product, int oldVersion) override;

        Domain::IUpdatableRepository& _repo;
        std::unordered_map<std::string, std::pair<Domain::ProductPtr, int>> _seenAndOldVersion;
    };
}