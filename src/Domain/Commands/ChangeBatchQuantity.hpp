#pragma once

#include "AbstractCommand.hpp"


namespace Allocation::Domain::Commands
{
    /// @brief Команда "ChangeBatchQuantity".
    struct ChangeBatchQuantity final : public AbstractCommand
    {
        /// @brief Конструктор команды "ChangeBatchQuantity".
        /// @param ref Ссылка на партию.
        /// @param qty Новое количество.
        ChangeBatchQuantity(std::string ref, size_t qty) : ref(std::move(ref)), qty(qty) {}

        /// @brief Получить имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override { return "ChangeBatchQuantity"; };

        std::string ref;
        size_t qty;
    };
}