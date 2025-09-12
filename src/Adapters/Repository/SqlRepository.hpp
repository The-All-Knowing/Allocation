#pragma once

#include "Precompile.hpp"

#include "Adapters/Database/Mappers/ProductMapper.hpp"
#include "Domain/Ports/IUpdatableRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief SQL реализация репозитория.
    /// @note Метод Update предназначен для использования только с TrackingRepository
    /// и не должен вызываться извне.
    class SqlRepository final : public Domain::IUpdatableRepository
    {
    public:
        /// @brief Конструктор.
        /// @param session Сессия базы данных.
        explicit SqlRepository(const Poco::Data::Session& session);

        /// @brief Добавляет новый продукт в репозитории.
        /// @param product Продукт для добавления.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        void Add(Domain::ProductPtr product) override;

        /// @brief Получает продукт по его артикулу.
        /// @param SKU Артикул продукта.
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        /// @return Продукт, если найден, иначе nullptr.
        [[nodiscard]] Domain::ProductPtr Get(const std::string& SKU) override;

        /// @brief Получает продукт по ссылке партии.
        /// @param batchRef Ссылка на партию.
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        /// @return Найденный продукт или пустой nullptr.
        [[nodiscard]] Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override;

        /// @brief Обновляет продукт в репозитории.
        /// @param product Продукт для добавления или обновления.
        /// @param oldVersion Прошлая версия продукта для проверки репозиторием
        /// версии для контроля конкуренции.
        /// @throw std::runtime_error Выбрасывается, если не удалось обновить продукт из-за
        /// конфликта версий.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        virtual void Update(Domain::ProductPtr product, int oldVersion) override;

        Database::Mapper::ProductMapper _mapper;
    };
}