#pragma once

namespace Allocation::Domain
{
    /// @brief Представляет позицию заказа для распределения.
    struct OrderLine
    {
        /// @brief Ссылка на заказ клиента.
        std::string reference;
        /// @brief Артикул продукции.
        std::string sku;
        /// @brief Количество продукции в позиции заказа.
        size_t quantity;

        /// @brief Сравнивает позиции заказа.
        /// @param other Другая позиция заказа.
        /// @return Результат сравнения.
        bool operator==(const OrderLine&) const = default;
    };
}

template <>
struct std::hash<Allocation::Domain::OrderLine>
{
    std::size_t operator()(const Allocation::Domain::OrderLine& line) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(line.reference);
        std::size_t h2 = std::hash<std::string>{}(line.sku);
        std::size_t h3 = std::hash<size_t>{}(line.quantity);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};