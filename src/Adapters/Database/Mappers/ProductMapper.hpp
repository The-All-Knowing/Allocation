#pragma once

#include "Precompile.hpp"

#include "Domain/Product/Product.hpp"
#include "BatchMapper.hpp"


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
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr FindBySKU(std::string SKU) const;

        /// @brief Получает продукт по ссылке партии.
        /// @param ref Ссылка на партию.
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr FindByBatchRef(std::string ref) const;

        /// @brief Обновляет продукт.
        /// @param product Продукт для обновления.
        /// @param oldVersion Прошлая версия продукта.
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        /// @return true - успешное обновление, иначе false.
        [[nodiscard]] bool Update(Domain::ProductPtr product, int oldVersion);

        /// @brief Создаёт продукт.
        /// @param product Продукт для создания.
        /// @throw Poco::Data::DataExceptionn Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        void Insert(Domain::ProductPtr product);

        /// @brief Удаляет продукт.
        /// @param product Удаляемый продукт.
        /// @return true - успешное удаление, иначе false.
        bool Delete(Domain::ProductPtr product);

    private:
        /// @brief Обновляет партии продукта.
        /// @param product Продукт с партиями для обновления.
        void UpdateBatches(Domain::ProductPtr product);

        mutable Poco::Data::Session _session;
        BatchMapper _batchMapper;
    };
}