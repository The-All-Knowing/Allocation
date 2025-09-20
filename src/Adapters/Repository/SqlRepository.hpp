#pragma once

#include "Adapters/Database/Mappers/ProductMapper.hpp"
#include "Domain/Ports/IUpdatableRepository.hpp"


namespace Allocation::Adapters::Repository
{
    /// @brief Реализация репозитория для работы с PostgreSQL СУБД.
    class SqlRepository final : public Domain::IUpdatableRepository
    {
    public:
        /// @brief Конструктор.
        /// @param session Сессия подключения к базе данных.
        explicit SqlRepository(const Poco::Data::Session& session);

        /// @brief Добавляет новый агрегат-продукт в репозиторий.
        /// @param product Агрегат-продукт для добавления.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        void Add(Domain::ProductPtr product) override;

        /// @brief Возвращает агрегат-продукт по его артикулу.
        /// @param sku Артикул товара.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @return Агрегат-продукт, если найден, иначе nullptr.
        [[nodiscard]] Domain::ProductPtr Get(const std::string& sku) override;

        /// @brief Возвращает агрегат-продукт по идентификатору партии включённого в него.
        /// @param batchRef Ссылка на партию заказа.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @return Найденный агрегат-продукт, если найден, иначе nullptr.
        [[nodiscard]] Domain::ProductPtr GetByBatchRef(const std::string& batchRef) override;

        /// @brief Обновляет агрегат-продукт в репозитории.
        /// @param product Агрегат-продукт для обновления.
        /// @param oldVersion Изначальная версия агрегата, загруженная из репозитория.
        /// @throw std::runtime_error Если не удалось обновить агрегат из-за конфликта версий.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        void Update(Domain::ProductPtr product, int oldVersion) override;

    private:
        Database::Mapper::ProductMapper _mapper;
    };
}