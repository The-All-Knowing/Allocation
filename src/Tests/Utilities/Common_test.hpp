#pragma once

#include "Precompile.hpp"

#include "Domain/Product/Batch.hpp"
#include "gtest/gtest-assertion-result.h"


// Привет!

namespace Allocation::Tests
{
    /// @brief Создаёт партию и строку заказа.
    /// @param SKU Артикул.
    /// @param batchQty Количество в партии.
    /// @param lineQty Количество в строке заказа.
    /// @return Пара (партия, строка заказа).
    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& SKU, size_t batchQty, size_t lineQty);

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

    /// @brief Удаляет продукт из базы данных.
    /// @param session Сессия базы данных.
    /// @param sku Артикул продукта.
    void DeleteProduct(Poco::Data::Session session, const std::string& sku);

    /// @brief Записывает партию в базу данных.
    /// @param session Сессия базы данных.
    /// @param batchRef Ссылка на партию.
    /// @param sku Артикул продукта.
    /// @param qty Количество в партии.
    /// @param version Номер версии продукта.
    /// @return
    int InsertBatch(Poco::Data::Session session, std::string batchRef,
        std::string sku = "GENERIC-SOFA", int qty = 100, int version = 0);

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

    /// @brief Получает ссылку на выделенную партию для строки заказа.
    /// @param session Сессия базы данных.
    /// @param orderid Идентификатор заказа.
    /// @param sku Артикул продукта.
    /// @return Ссылка на выделенную партию.
    std::string GetAllocatedBatchRef(
        Poco::Data::Session session, std::string orderid, std::string sku);
}