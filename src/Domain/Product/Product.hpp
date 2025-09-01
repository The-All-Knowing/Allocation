#pragma once

#include "Precompile.hpp"

#include "Batch.hpp"
#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain
{
    /// @brief Представляет продукт с его партиями и логикой распределения.
    class Product
    {
    public:
        /// @brief Конструктор продукта.
        /// @param SKU Артикул продукта.
        /// @param batches Список партий продукта.
        /// @param versionNumber Номер версии продукта.
        explicit Product(
            std::string_view SKU, const std::vector<Batch>& batches = {}, size_t versionNumber = 0);

        /// @brief Добавляет партию к продукту.
        /// @param batch Партия для добавления.
        void AddBatch(const Batch& batch) noexcept;

        /// @brief Добавляет партии к продукту.
        /// @param batches Партии для добавления.
        void AddBatches(const std::vector<Batch>& batches) noexcept;

        /// @brief Аллоцирует продукт на основе строки заказа.
        /// @param line Строка заказа.
        /// @return Ссылка на партию, если аллокация прошла успешно, иначе std::nullopt.
        std::optional<std::string> Allocate(const OrderLine& line);

        /// @brief Изменяет количество партии продукта.
        /// @param ref Ссылка на партию.
        /// @param qty Новое количество.
        void ChangeBatchQuantity(std::string_view ref, size_t qty);

        /// @brief Получает список партий продукта.
        /// @return Список партий продукта.
        [[nodiscard]] std::vector<Batch> GetBatches() const noexcept;

        /// @brief Получает номер версии продукта.
        /// @return Номер версии продукта.
        [[nodiscard]] size_t GetVersion() const noexcept;

        /// @brief Получает артикул продукта.
        /// @return Артикул продукта.
        [[nodiscard]] std::string GetSKU() const noexcept;

        /// @brief Получает сообщения продукта.
        /// @return Сообщения продукта.
        [[nodiscard]] const std::vector<Domain::IMessagePtr>& Messages() const noexcept;

        /// @brief Очищает сообщения продукта.
        void ClearMessages() noexcept;

    private:
        std::string _sku;
        std::unordered_map<std::string, Batch> _referenceByBatches;
        std::vector<Domain::IMessagePtr> _messages;
        size_t _versionNumber;
    };

    bool operator==(const Product& lhs, const Product& rhs) noexcept;

    using ProductPtr = std::shared_ptr<Product>;
}