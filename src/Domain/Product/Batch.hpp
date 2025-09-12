#pragma once

#include "Precompile.hpp"

#include "OrderLine.hpp"


namespace Allocation::Domain
{
    /// @brief Представляет партию продуктов для распределения.
    class Batch
    {
    public:
        /// @brief Конструктор для создания партии продуктов.
        /// @param reference Ссылка на партию.
        /// @param SKU Артикул продукта.
        /// @param quantity Количество продукта в партии.
        /// @param ETA Ожидаемая дата поставки.
        Batch(const std::string& reference, const std::string& SKU, size_t quantity,
            std::optional<std::chrono::year_month_day> ETA = std::nullopt);

        /// @brief Присваивает значение объекту.
        /// @param other Другой объект.
        /// @return Текущий объект.
        Batch& operator=(const Batch& other) = default;

        /// @brief Устанавливает количество приобретенных товаров в партии.
        /// @param newQty Новое количество.
        void SetPurchasedQuantity(size_t newQty) noexcept;

        /// @brief Проверяет, можно ли выделить линию заказа из партии.
        /// @param line Линия заказа для проверки.
        /// @return true, если выделение возможно; в противном случае - false.
        [[nodiscard]] bool CanAllocate(const OrderLine& line) const noexcept;

        /// @brief Выделяет линию заказа из партии.
        /// @param line Линия заказа для выделения.
        void Allocate(const OrderLine& line) noexcept;

        /// @brief Освобождает линию заказа из партии.
        /// @return Освобожденная линия заказа.
        /// @throw std::runtime_error Выбрасывается, если изъяты все строки заказов.
        [[nodiscard]] OrderLine DeallocateOne();

        /// @brief Возвращает количество выделенных товаров в партии.
        /// @return Количество выделенных товаров.
        [[nodiscard]] int GetAllocatedQuantity() const noexcept;

        /// @brief Возвращает доступное количество продуктов в партии.
        /// @return Доступное количество продуктов.
        [[nodiscard]] int GetAvailableQuantity() const noexcept;

        /// @brief Возвращает количество продуктов в партии.
        /// @return Количество продуктов в партии.
        [[nodiscard]] int GetPurchasedQuantity() const noexcept;

        /// @brief Возвращает ссылку на партию.
        /// @return Ссылка на партию.
        [[nodiscard]] std::string GetReference() const noexcept;

        /// @brief Возвращает ожидаемую дату поставки.
        /// @return Ожидаемая дата поставки.
        [[nodiscard]] std::optional<std::chrono::year_month_day> GetETA() const noexcept;

        /// @brief Возвращает артикул продукта.
        /// @return Артикул продукта.
        [[nodiscard]] std::string GetSKU() const noexcept;

        /// @brief Возвращает все выделенные линии заказа для данной партии.
        /// @return Вектор выделенных линий заказа.
        [[nodiscard]] std::vector<OrderLine> GetAllocations() const noexcept;

        /// @brief Сравнивает партии по всем атрибутам.
        /// @param other Правая партия.
        /// @return true, если партии равны; в противном случае - false.
        bool operator==(const Batch& other) const = default;

    private:
        std::string _reference;
        std::string _SKU;
        size_t _purchasedQuantity;
        std::optional<std::chrono::year_month_day> _ETA;
        std::set<OrderLine> _allocations;
    };

    /// @brief Сравнивает партии по ожидаемой дате поставки.
    /// @param lhs Левая партия.
    /// @param rhs Правая партия.
    /// @return true, если левая партия имеет более раннюю дату поставки, чем правая.
    bool operator<(const Batch& lhs, const Batch& rhs) noexcept;
}