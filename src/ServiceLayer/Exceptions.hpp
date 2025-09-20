#pragma once

namespace Allocation::ServiceLayer::Exceptions
{
    /// @brief Исключение для недопустимого артикула продукции.
    class InvalidSku : public std::exception
    {
    public:
        /// @brief Конструктор.
        /// @param sku Недопустимый артикул продукции.
        explicit InvalidSku(std::string_view sku) : _msg(std::format("Invalid sku: {}", sku)) {}

        /// @brief Возвращает текст исключения.
        /// @return Текст исключения.
        [[nodiscard]] const char* what() const noexcept override { return _msg.c_str(); }

    private:
        std::string _msg;
    };
}