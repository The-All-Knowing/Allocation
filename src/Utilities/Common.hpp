#pragma once

#include "Precompile.hpp"


namespace Allocation
{
    /// @brief Преобразует дату из формата Poco в формат std::chrono.
    /// @param date Дата в формате Poco.
    /// @return Дата в формате std::chrono.
    [[nodiscard]] std::optional<std::chrono::year_month_day> Convert(
        Poco::Nullable<Poco::DateTime> date) noexcept;

    /// @brief Преобразует дату из формата std::chrono в формат Poco.
    /// @param date Дата в формате std::chrono.
    /// @return Дата в формате Poco.
    [[nodiscard]] Poco::Nullable<Poco::DateTime> Convert(
        std::optional<std::chrono::year_month_day> date) noexcept;

    /// @brief Перегружает оператор + для добавления дней к дате.
    /// @param ymd Дата в формате std::chrono.
    /// @param days Количество дней для добавления.
    /// @return Новая дата с добавленными днями.
    [[nodiscard]] std::chrono::year_month_day operator+(
        const std::chrono::year_month_day& ymd, const std::chrono::days& days) noexcept;

    /// @brief Перегружает оператор + для добавления дней к дате.
    /// @param days Количество дней для добавления.
    /// @param ymd Дата в формате std::chrono.
    /// @return Новая дата с добавленными днями.
    [[nodiscard]] std::chrono::year_month_day operator+(
        const std::chrono::days& days, const std::chrono::year_month_day& ymd) noexcept;
}