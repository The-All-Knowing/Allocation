#pragma once

#include "Precompile.hpp"


namespace Allocation::ServiceLayer::Exceptions
{
    /// @brief Исключение для недопустимого SKU
    class InvalidSku : public std::exception
    {
    public:
        /// @brief Конструктор.
        /// @param sku Недопустимый SKU.
        explicit InvalidSku(std::string_view sku) : _msg(std::format("Invalid SKU: {}", sku)) {}

        /// @brief Возвращает текст сообщения.
        /// @return Текст сообщения.
        [[nodiscard]] const char* what() const noexcept override { return _msg.c_str(); }

    private:
        std::string _msg;
    };
}