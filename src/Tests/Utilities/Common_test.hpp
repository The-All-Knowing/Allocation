#pragma once

#include "Domain/Product/Batch.hpp"
#include "gtest/gtest-assertion-result.h"


namespace Allocation::Tests
{
    /// @brief Создаёт партию заказа и позицию заказа.
    /// @param sku Артикул продукции.
    /// @param batchQty Количество продукции в партии заказа.
    /// @param lineQty Количество в продукции в позиции заказа.
    /// @return Пара: партия заказа - позиция заказа.
    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& sku, size_t batchQty, size_t lineQty);

    /// @brief Проверяет, выбрасывает ли функция ожидаемое исключение с заданным сообщением.
    /// @tparam Exception Тип ожидаемого исключения.
    /// @tparam Func Тип функции.
    /// @param func Функция для выполнения.
    /// @param expectedMsg Ожидаемое сообщение об ошибке.
    /// @return Результат проверки.
    template <class Exception, class Func>
    testing::AssertionResult ThrowsWithMessage(Func&& func, const std::string& expectedMsg)
    {
        try
        {
            func();
            return testing::AssertionFailure() << "No exception thrown";
        }
        catch (const Exception& e)
        {
            if (e.what() == expectedMsg)
                return testing::AssertionSuccess();

            return testing::AssertionFailure() << "Wrong message. Actual: " << e.what();
        }
        catch (...)
        {
            return testing::AssertionFailure() << "Unexpected exception type thrown";
        }
    }

    /// @brief Генерирует случайный артикул.
    /// @param name Имя для генерации.
    /// @return Случайный артикул.
    std::string RandomSku(const std::string& name = "");

    /// @brief Генерирует случайную ссылку на партию.
    /// @param name Имя для генерации.
    /// @return Случайная ссылка на партию.
    std::string RandomBatchRef(const std::string& name = "");

    /// @brief Генерирует случайный идентификатор заказа.
    /// @param name Имя для генерации.
    /// @return Случайный идентификатор заказа.
    std::string RandomOrderId(const std::string& name = "");
}