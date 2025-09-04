#pragma once

#include "Precompile.hpp"

#include "Domain/Ports/IRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief Репозиторий для отслеживания изменений продуктов.
    class TrackingRepository final : public Domain::IRepository
    {
    public:
        /// @brief Конструктор.
        /// @param repo Отслеживаемый репозиторий.
        TrackingRepository(Domain::IRepository& repo);

        /// @brief Добавляет продукт в репозиторий и отслеживает его.
        /// @param product Продукт для добавления.
        void Add(Domain::ProductPtr product) override;

        /// @brief Получает продукт по его SKU.
        /// @param SKU Артикул продукта.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr Get(std::string_view SKU) override;

        /// @brief Получает продукт по ссылке партии.
        /// @param batchRef Ссылка на партию.
        /// @return Найденный продукт или пустой nullptr.
        [[nodiscard]] Domain::ProductPtr GetByBatchRef(std::string_view batchRef) override;

        /// @brief Получает все продукты, которые были добавлены в репозиторий.
        /// @return Список всех добавленных продуктов.
        [[nodiscard]] std::vector<Domain::ProductPtr> GetSeen() const noexcept;

        /// @brief Получает все измененные версии продуктов.
        /// @return Список кортежей с информацией об измененных версиях.
        [[nodiscard]] std::vector<std::tuple<std::string, size_t, size_t>> GetChangedVersions()
            const noexcept;

    private:
        Domain::IRepository& _repo;
        std::unordered_map<std::string, Domain::ProductPtr> _seen;
        std::unordered_map<std::string, size_t> _seenObjByOldVersion;
    };
}