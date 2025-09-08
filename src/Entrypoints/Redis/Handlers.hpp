#pragma once

#include "Precompile.hpp"


namespace Allocation::Entrypoints::Redis::Handlers
{
    /// @brief Обрабатывает сообщение Redis "изменить количество партии".
    /// @param payload Тело сообщения.
    void HandleChangeBatchQuantity(const std::string& payload);
}