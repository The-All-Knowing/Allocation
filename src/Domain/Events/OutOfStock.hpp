#pragma once

#include "AbstractEvent.hpp"


namespace Allocation::Domain::Events
{
    /// @brief Событие "Нет в наличии товара".
    struct OutOfStock final : public AbstractEvent
    {
        /// @brief Конструктор.
        /// @param sku Артикул товара которого нет в наличии.
        OutOfStock(std::string sku) : sku(std::move(sku)){};

        /// @brief Возвращает имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override { return "OutOfStock"; };

        std::string sku;
    };

    /// @brief Читает аттрибуты события.
    /// @param event Событие.
    /// @return Атрибуты события: название - значение.
    template <>
    inline std::vector<EventAttribute> GetAttributes<OutOfStock>(EventPtr event)
    {
        auto p = std::static_pointer_cast<OutOfStock>(event);
        return {{"sku", p->sku}};
    };
}