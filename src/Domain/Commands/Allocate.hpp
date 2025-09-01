#pragma once

#include "AbstractCommand.hpp"


namespace Allocation::Domain::Commands
{
    /// @brief Команда "Allocate".
    struct Allocate final : public AbstractCommand
    {
        /// @brief Конструктор команды "Allocate".
        /// @param orderid Идентификатор заказа.
        /// @param sku Артикул товара.
        /// @param qty Количество.
        Allocate(std::string orderid, std::string sku, size_t qty)
            : orderid(std::move(orderid)), sku(std::move(sku)), qty(qty)
        {
        }

        /// @brief Получить имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override { return "Allocate"; };

        std::string orderid;
        std::string sku;
        size_t qty;
    };
}