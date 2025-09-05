#pragma once

#include "Precompile.hpp"

#include "Domain/Product/Product.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    /// @brief Маппер для работы с продуктами в базе данных.
    class ProductMapper
    {
    public:
        /// @brief Конструктор маппера.
        /// @param session Сессия базы данных.
        explicit ProductMapper(const Poco::Data::Session& session);

        /// @brief Возвращает продукт по артикулу.
        /// @param SKU Артикул.
        /// @throw Poco::Data::DataExceptionn Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr FindBySKU(std::string SKU);

        /// @brief Получает продукт по ссылке партии.
        /// @param ref Ссылка на партию.
        /// @throw Poco::Data::DataExceptionn Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr FindByBatchRef(std::string ref);

        /// @brief Обновляет продукт.
        /// @param product Продукт для обновления.
        /// @param oldVersion Прошлая версия продукта.
        /// @throw Poco::Data::DataExceptionn Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        /// @return true - успешное обновление, иначе false.
        [[nodiscard]] bool Update(Domain::ProductPtr product, int oldVersion);

        /// @brief Создаёт продукт.
        /// @param product Продукт для создания.
        /// @throw Poco::Data::DataExceptionn Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        void Insert(Domain::ProductPtr product);

    private:
        Poco::Data::Session _session;
    };
}