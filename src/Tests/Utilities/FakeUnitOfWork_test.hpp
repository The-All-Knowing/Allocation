#pragma once

#include "Precompile.hpp"

#include "FakeRepository_test.hpp"
#include "Services/UoW/AbstractUnitOfWork.hpp"


namespace Allocation::Tests
{
    /// @brief Фейковая реализация Unit of Work для тестирования.
    class FakeUnitOfWork final : public Services::UoW::AbstractUnitOfWork
    {
    public:
        /// @brief Конструктор.
        FakeUnitOfWork() : AbstractUnitOfWork(_repo) {}

        /// @brief Получение сессии базы данных.
        /// @return Всегда возвращает std::nullopt.
        std::optional<Poco::Data::Session> GetSession() noexcept { return std::nullopt; }

    private:
        FakeRepository _repo;
    };
}
