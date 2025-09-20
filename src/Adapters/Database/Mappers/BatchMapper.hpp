#pragma once

#include "Domain/Product/Batch.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    /// @brief Маппер для отображения партий заказов товаров в базе данных и из неё.
    class BatchMapper
    {
    public:
        /// @brief Конструктор.
        /// @param session Сессия подключения к базе данных.
        explicit BatchMapper(const Poco::Data::Session& session);

        /// @brief Находит партии заказов по артикулу.
        /// @param sku Артикул товара.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @return Найденные партии заказов.
        [[nodiscard]] std::vector<Domain::Batch> Find(const std::string& sku) const;

        /// @brief Удаляет партии заказов по их идентификаторам.
        /// @param batchRefs Идентификаторы партий для удаления.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        void Delete(std::vector<std::string> batchRefs);

        /// @brief Сохраняет партии заказов в базе данных.
        /// @param batches Сохраняемые партии заказов.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        void Insert(const std::vector<Domain::Batch>& batches);

    private:
        /// @brief Находит позиции заказов, закреплённые за партией.
        /// @param batchPk Первичный ключ партии заказа.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @return Позиции заказов партии.
        [[nodiscard]] std::vector<Domain::OrderLine> FindOrderLines(int batchPk) const;

        /// @brief Сохраняет позиции заказов для партии.
        /// @param orders Позиции заказов.
        /// @param batchPk Первичный ключ партии заказа.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        void InsertOrderLines(const std::vector<Domain::OrderLine>& orders, int batchPk);

        /// @brief Удаляет все позиции заказов, связанные с партиями.
        /// @param batchRefs Идентификаторы партий заказов.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        void DeleteAllOrderLines(std::vector<std::string> batchRefs);

        mutable Poco::Data::Session _session;
    };
}