#pragma once

#include "OrderLine.hpp"


namespace Allocation::Domain
{
    /// @brief Представляет партию поставки продукции для распределения.
    class Batch
    {
    public:
        /// @brief Конструктор.
        /// @param reference Ссылка партии заказа.
        /// @param sku Артикул продукции.
        /// @param quantity Количество продукции в партии.
        /// @param eta Ожидаемая дата поставки.
        Batch(const std::string& reference, const std::string& sku, size_t quantity,
            std::optional<std::chrono::year_month_day> eta = std::nullopt);

        /// @brief Копирующий оператор присваивания.
        /// @param other Другой объект.
        /// @return Текущий объект.
        Batch& operator=(const Batch& other) = default;

        /// @brief Устанавливает количество продукции в партии.
        /// @param newQty Новое количество.
        void SetPurchasedQuantity(size_t newQty) noexcept;

        /// @brief Проверяет, можно ли распределить позицию заказа в партии.
        /// @param line Позиция заказа для проверки.
        /// @return true, если распределение возможно; в противном случае - false.
        [[nodiscard]] bool CanAllocate(const OrderLine& line) const noexcept;

        /// @brief Распределяет позицию заказа в партии.
        /// @param line Позиция заказа для распределения.
        void Allocate(const OrderLine& line) noexcept;

        /// @brief Отменяет распределение одной позиции заказа в партии.
        /// @return Отменённая позиция заказа.
        /// @throw std::runtime_error Выбрасывается, если отменены все позиции заказов.
        [[nodiscard]] OrderLine DeallocateOne();

        /// @brief Возвращает количество распределённых товаров в партии.
        /// @return Количество распределённой продукции в партии.
        [[nodiscard]] int GetAllocatedQuantity() const noexcept;

        /// @brief Возвращает доступное количество продукции в партии.
        /// @return Доступное количество продукции в партии.
        [[nodiscard]] int GetAvailableQuantity() const noexcept;

        /// @brief Возвращает количество продукции в партии заказа.
        /// @return Количество продукции в партии.
        [[nodiscard]] int GetPurchasedQuantity() const noexcept;

        /// @brief Возвращает ссылку партии заказа.
        /// @return Ссылка партии заказа.
        [[nodiscard]] std::string GetReference() const noexcept;

        /// @brief Возвращает ожидаемую дату поставки.
        /// @return Ожидаемая дата поставки.
        [[nodiscard]] std::optional<std::chrono::year_month_day> GetETA() const noexcept;

        /// @brief Возвращает артикул продукции.
        /// @return Артикул продукции.
        [[nodiscard]] std::string GetSKU() const noexcept;

        /// @brief Возвращает все распределённые позиции заказа для данной партии.
        /// @return Распределённых позиций заказа в партии.
        [[nodiscard]] std::vector<OrderLine> GetAllocations() const noexcept;

    private:
        std::string _reference;
        std::string _sku;
        size_t _purchasedQuantity;
        std::optional<std::chrono::year_month_day> _eta;
        std::unordered_set<OrderLine> _allocations;
    };

    /// @brief Сравнивает партии по ссылкам партий заказов.
    /// @param lhs Левая партия.
    /// @param rhs Правая партия.
    /// @return true, если ссылки партий равны; в противном случае - false.
    bool operator==(const Batch& lhs, const Batch& rhs) noexcept;

    /// @brief Сравнивает партии по ожидаемой дате поставки.
    /// @param lhs Левая партия.
    /// @param rhs Правая партия.
    /// @return true, если левая партия имеет более раннюю дату поставки, чем правая.
    bool operator<(const Batch& lhs, const Batch& rhs) noexcept;
}