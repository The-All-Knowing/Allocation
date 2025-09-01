#pragma once

#include "Precompile.hpp"


namespace Allocation::Services::Exceptions
{

    /// @brief Исключение для недопустимого SKU
    class InvalidSku : public std::exception
    {
    public:
        /// @brief Конструктор исключения для недопустимого SKU.
        /// @param sku Недопустимый SKU.
        explicit InvalidSku(std::string_view sku) : msg(std::format("Invalid SKU: {}", sku)) {}

        [[nodiscard]] const char* what() const noexcept override { return msg.c_str(); }

    private:
        std::string msg;
    };
}