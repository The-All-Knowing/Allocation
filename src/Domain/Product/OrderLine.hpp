#pragma once

#include "Precompile.hpp"


namespace Allocation::Domain
{
    /// @brief Представляет строку заказа для распределения.
    struct OrderLine
    {
        /// @brief Ссылка на заказ.
        std::string reference;
        /// @brief Артикул продукта.
        std::string SKU;
        /// @brief Количество продукта в заказе.
        size_t quantity;

        /// @brief Оператор сравнения для строк заказа.
        /// @param other Другая строка заказа.
        /// @return Результат сравнения.
        auto operator<=>(const OrderLine&) const = default;
    };
}

template <>
struct std::hash<Allocation::Domain::OrderLine>
{
    std::size_t operator()(const Allocation::Domain::OrderLine& line) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(line.reference);
        std::size_t h2 = std::hash<std::string>{}(line.SKU);
        std::size_t h3 = std::hash<size_t>{}(line.quantity);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};