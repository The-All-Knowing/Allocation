#pragma once

#include "AbstractCommand.hpp"


namespace Allocation::Domain::Commands
{
    /// @brief Команда "Распределить позицию заказа".
    struct Allocate final : public AbstractCommand
    {
        /// @brief Конструктор.
        /// @param orderid Идентификатор заказа клиента.
        /// @param sku Артикул товара для распределения.
        /// @param qty Количество распределяемого товара.
        Allocate(std::string orderid, std::string sku, size_t qty)
            : orderid(std::move(orderid)), sku(std::move(sku)), qty(qty)
        {
        }

        /// @brief Возвращает имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override { return "Allocate"; };

        std::string orderid;
        std::string sku;
        size_t qty;
    };
}