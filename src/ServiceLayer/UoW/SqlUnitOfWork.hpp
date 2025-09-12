#pragma once

#include "Precompile.hpp"

#include "AbstractUnitOfWork.hpp"
#include "Adapters/Database/Session/SessionPool.hpp"
#include "Adapters/Repository/SqlRepository.hpp"


namespace Allocation::ServiceLayer::UoW
{
    /// @brief SQL реализация единицы работы.
    class SqlUnitOfWork final : public AbstractUnitOfWork
    {
    public:
        /// @brief Конструктор.
        /// @details При создании объекта открывает сессию и начинает транзакцию с уровнем
        ///          изоляции REPEATABLE READ. Используется для работы с репозиторием внутри
        ///          единицы работы.
        SqlUnitOfWork()
            : _session(Adapters::Database::SessionPool::Instance().GetSession()),
              _repository(_session),
              AbstractUnitOfWork(_repository)
        {
            _session.setTransactionIsolation(Poco::Data::Session::TRANSACTION_REPEATABLE_READ);
            _session.begin();
        }

        /// @brief Возвращает сессию базы данных.
        /// @return Сессия базы данных.
        [[nodiscard]] std::optional<Poco::Data::Session> GetSession() noexcept override
        {
            return _session;
        }

        /// @brief Подтверждает изменения.
        /// @throw std::runtime_error Выбрасывается, если не удалось обновить продукт из-за
        /// конфликта версий.
        /// @throw Poco::Data::DataException Выбрасывается, если возникают ошибки при выполнении
        /// запроса.
        void Commit() override
        {
            AbstractUnitOfWork::Commit();
            _session.commit();
            _session.begin();
        }

        /// @brief Откатывает изменения.
        void RollBack() override
        {
            _session.rollback();
            AbstractUnitOfWork::RollBack();
        }

    private:
        Poco::Data::Session _session;
        Adapters::Repository::SqlRepository _repository;
    };
}