#pragma once

#include "Precompile.hpp"

#include "Domain/Ports/IRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief SQL реализация репозитория.
    class SqlRepository final : public Domain::IRepository
    {
    public:
        /// @brief Конструктор.
        /// @param session Сессия базы данных.
        explicit SqlRepository(Poco::Data::Session session);

        /// @brief Добавляет продукт в репозиторий.
        /// @param product Продукт для добавления.
        void Add(Domain::ProductPtr product) override;

        /// @brief Получает продукт по его артикулу.
        /// @param SKU Артикул продукта.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr Get(std::string_view SKU) override;

        /// @brief Получает продукт по ссылке партии.
        /// @param batchRef Ссылка на партию.
        /// @return Найденный продукт или пустой nullptr.
        [[nodiscard]] Domain::ProductPtr GetByBatchRef(std::string_view batchRef) override;

        /// @brief Обновляет версию продукта.
        /// @param SKU Артикул продукта.
        /// @param oldVersion Старая версия продукта.
        /// @param newVersion Новая версия продукта.
        void UpdateVersion(std::string_view SKU, size_t oldVersion, size_t newVersion);

    private:
        Poco::Data::Session _session;
    };
}