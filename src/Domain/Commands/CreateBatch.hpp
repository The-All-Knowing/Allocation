#pragma once

#include "AbstractCommand.hpp"


namespace Allocation::Domain::Commands
{
    /// @brief Команда "CreateBatch".
    struct CreateBatch final : public AbstractCommand
    {
        /// @brief Конструктор команды "CreateBatch".
        /// @param ref Ссылка на партию.
        /// @param sku Артикул товара.
        /// @param qty Количество.
        /// @param eta Ожидаемая дата поступления.
        CreateBatch(std::string ref, std::string sku, size_t qty,
            std::optional<std::chrono::year_month_day> eta = std::nullopt)
            : ref(std::move(ref)), sku(std::move(sku)), qty(qty), eta(std::move(eta)) {};

        /// @brief Получить имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override { return "CreateBatch"; };

        std::string ref;
        std::string sku;
        size_t qty;
        std::optional<std::chrono::year_month_day> eta;
    };
}