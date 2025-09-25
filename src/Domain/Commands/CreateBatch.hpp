#pragma once

#include "AbstractCommand.hpp"


namespace Allocation::Domain::Commands
{
    /// @brief Команда "Создать партию поставки".
    struct CreateBatch final : public AbstractCommand
    {
        /// @brief Конструктор.
        /// @param ref Ссылка новой партии заказа.
        /// @param sku Артикул товара новой партии.
        /// @param qty Количество товара в партии.
        /// @param eta Ожидаемая дата поступления партии.
        CreateBatch(std::string ref, std::string sku, size_t qty,
            std::optional<std::chrono::year_month_day> eta = std::nullopt)
            : ref(std::move(ref)), sku(std::move(sku)), qty(qty), eta(std::move(eta)){};

        /// @brief Возвращает имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override { return "CreateBatch"; };

        std::string ref;
        std::string sku;
        size_t qty;
        std::optional<std::chrono::year_month_day> eta;
    };
}