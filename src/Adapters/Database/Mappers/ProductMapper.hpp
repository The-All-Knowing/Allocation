#pragma once

#include "BatchMapper.hpp"
#include "Domain/Product/Product.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    /// @brief Маппер для отображения агрегата-продукт в базе данных и обратно.
    class ProductMapper
    {
    public:
        /// @brief Конструктор.
        /// @param session Сессия подключения к базе данных.
        explicit ProductMapper(const Poco::Data::Session& session);

        /// @brief Находит агрегат-продукт по артикулу.
        /// @param sku Артикул товара.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr FindBySKU(const std::string& sku) const;

        /// @brief Находит агрегат-продукт по идентификатору партии включённого в него.
        /// @param ref Идентификатор партии.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @return Найденный продукт или nullptr.
        [[nodiscard]] Domain::ProductPtr FindByBatchRef(const std::string& ref) const;

        /// @brief Обновляет агрегат-продукт.
        /// @param product Обновляемый агрегат-продукт.
        /// @param oldVersion Исходная прочитанная версия продукта.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        /// @return true - успешное обновление, иначе false.
        [[nodiscard]] bool Update(Domain::ProductPtr product, int oldVersion);

        /// @brief Сохраняет агрегат-продукт.
        /// @param product Сохраняемый агрегат-продукт.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        void Insert(Domain::ProductPtr product);

        /// @brief Удаляет агрегат-продукт.
        /// @param product Удаляемый агрегат-продукт.
        /// @throw std::invalid_argument Выбрасывается, если передан nullptr вместо продукта.
        /// @return true - успешное удаление, иначе false.
        bool Delete(Domain::ProductPtr product);

    private:
        /// @brief Обновляет партии заказов, содержащиеся в агрегате.
        /// @param batches Коллекция изменённых партий заказов, которые требуется сохранить.
        /// @param batchRefs Идентификаторы партий заказов, которые были изменены или удалены.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        void UpdateBatches(
            const std::vector<Domain::Batch>& batches, const std::vector<std::string>& batchRefs);

        mutable Poco::Data::Session _session;
        BatchMapper _batchMapper;
    };
}