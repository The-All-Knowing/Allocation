#pragma once

#include "AbstractEvent.hpp"


namespace Allocation::Domain::Events
{
    /// @brief Событие "Отменено распределение позиции заказа".
    struct Deallocated final : public AbstractEvent
    {
        /// @brief Конструктор.
        /// @param orderid Идентификатор заказа клиента.
        /// @param sku Артикул отменённого товара.
        /// @param qty Количество отменённого товара.
        Deallocated(std::string orderid, std::string sku, size_t qty)
            : orderid(std::move(orderid)), sku(std::move(sku)), qty(qty){};

        /// @brief Возвращает имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override { return "Deallocated"; };

        std::string orderid;
        std::string sku;
        size_t qty;
    };

    /// @brief Читает аттрибуты события.
    /// @param event Событие.
    /// @return Атрибуты события: название - значение.
    template <>
    inline std::vector<EventAttribute> GetAttributes<Deallocated>(EventPtr event)
    {
        auto p = std::static_pointer_cast<Deallocated>(event);
        return {{"orderid", p->orderid}, {"sku", p->sku}, {"qty", std::to_string(p->qty)}};
    };
}