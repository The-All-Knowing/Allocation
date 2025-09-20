#pragma once

namespace Allocation::Entrypoints::Redis::Handlers
{
    /// @brief Обрабатывает сообщение Redis "изменить количество продукции в партии заказа".
    /// @param payload Тело сообщения.
    void HandleChangeBatchQuantity(const std::string& payload);
}