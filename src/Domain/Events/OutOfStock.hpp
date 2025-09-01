#pragma once

#include "AbstractEvent.hpp"


namespace Allocation::Domain::Events
{
    /// @brief Событие "OutOfStock".
    struct OutOfStock final : public AbstractEvent
    {
        /// @brief Конструктор события "OutOfStock".
        /// @param SKU Артикул товара.
        OutOfStock(std::string SKU) : SKU(std::move(SKU)){};

        /// @brief Получить имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override { return "OutOfStock"; };
        
        std::string SKU;
    };
}