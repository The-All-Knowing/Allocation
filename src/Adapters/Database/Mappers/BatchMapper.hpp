#pragma once

#include "Precompile.hpp"

#include "Domain/Product/Batch.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    /// @brief Маппер для работы с партиями заказ в базе данных. 
    class BatchMapper
    {
    public:
        /// @brief Конструктор.
        /// @param session Сессия базы данных.
        explicit BatchMapper(const Poco::Data::Session& session);

        /// @brief Находит партии заказа по артикулу.
        /// @param SKU Артикул.
        /// @return Партии заказов.
        [[nodiscard]] std::vector<Domain::Batch> Find(std::string SKU) const;

        /// @brief Удаляет партии заказа по ссылкам на партии.
        /// @param batchRefs Ссылки на партии.
        void Delete(std::vector<std::string> batchRefs);

        /// @brief Создаёт партии заказа.
        /// @param batches Партии заказа.
        void Insert(const std::vector<Domain::Batch>& batches);
        
    private:
        /// @brief Получает строки заказов партии. 
        /// @param batchPk PK партии заказа.
        /// @return Строки заказа.
        [[nodiscard]] std::vector<Domain::OrderLine> GetAllocations(int batchPk) const;

        /// @brief Создаёт строки заказов партии.
        /// @param orders Строки заказа.
        /// @param batchPk PK партии заказа.
        void InsertOrderLines(const std::vector<Domain::OrderLine>& orders, int batchPk);

        /// @brief Удаляет строки заказов партии.
        /// @param batchRefs Ссылки на партии.
        void DeleteOrderLines(std::vector<std::string> batchRefs);

        mutable Poco::Data::Session _session;
    };
}