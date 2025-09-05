#pragma once

#include "Precompile.hpp"

#include "AbstractUnitOfWork.hpp"
#include "Adapters/Database/Session/SessionPool.hpp"
#include "Adapters/Repository/SqlRepository.hpp"


namespace Allocation::Services::UoW
{
    /// @brief SQL реализация единицы работы (Unit of Work).
    class SqlUnitOfWork final : public AbstractUnitOfWork
    {
    public:
        /// @brief Конструктор.
        SqlUnitOfWork() : AbstractUnitOfWork(_repository) {}

        /// @brief Получение сессии базы данных.
        /// @return Сессию базы данных.
        [[nodiscard]] std::optional<Poco::Data::Session> GetSession() noexcept override
        {
            return _session;
        }

        /// @brief Подтверждение изменений.
        /// @throw std::runtime_error Выбрасывается, если не удалось обновить продукт из-за
        /// конфликта версий.
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        void Commit() override
        {
            AbstractUnitOfWork::Commit();
            _session.commit();
        }

        /// @brief Откат изменений.
        void RollBack() override
        {
            _session.rollback();
            AbstractUnitOfWork::RollBack();
        }

    private:
        Poco::Data::Session _session{Adapters::Database::SessionPool::Instance().GetSession()};
        Adapters::Repository::SqlRepository _repository{_session};
    };
}