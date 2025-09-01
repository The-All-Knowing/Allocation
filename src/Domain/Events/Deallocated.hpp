#pragma once

#include "AbstractEvent.hpp"


namespace Allocation::Domain::Events
{
    /// @brief Событие "Deallocated".
    struct Deallocated final : public AbstractEvent
    {
        /// @brief Конструктор события "Deallocated".
        /// @param orderid Идентификатор заказа.
        /// @param SKU Артикул товара.
        /// @param qty Количество.
        Deallocated(std::string orderid, std::string SKU, size_t qty)
            : orderid(std::move(orderid)),
              SKU(std::move(SKU)),
              qty(qty) {};

        /// @brief Получить имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override { return "Deallocated"; };

        std::string orderid;
        std::string SKU;
        size_t qty;
    };
}