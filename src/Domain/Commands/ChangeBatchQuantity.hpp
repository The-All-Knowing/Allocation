#pragma once

#include "AbstractCommand.hpp"


namespace Allocation::Domain::Commands
{
    /// @brief Команда "Изменить количество товара в партии заказа".
    struct ChangeBatchQuantity final : public AbstractCommand
    {
        /// @brief Конструктор.
        /// @param ref Ссылка на партию заказа.
        /// @param qty Новое количество товара.
        ChangeBatchQuantity(std::string ref, size_t qty) : ref(std::move(ref)), qty(qty) {}

        /// @brief Возвращает имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override { return "ChangeBatchQuantity"; };

        std::string ref;
        size_t qty;
    };
}