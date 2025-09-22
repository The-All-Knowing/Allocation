#pragma once

#include "AbstractUnitOfWork.hpp"
#include "Adapters/Database/Session/DatabaseSessionPool.hpp"
#include "Adapters/Repository/SqlRepository.hpp"


namespace Allocation::ServiceLayer::UoW
{
    /// @brief Реализация единицы работы для SQL хранилища.
    class SqlUnitOfWork final : public AbstractUnitOfWork
    {
    public:
        /// @brief Конструктор.
        /// @details При создании объекта открывает сессию к БД и начинает транзакцию.
        SqlUnitOfWork()
            : _session(Adapters::Database::DatabaseSessionPool::Instance().GetSession()),
              _repository(_session),
              AbstractUnitOfWork(_repository)
        {
            _session.setTransactionIsolation(Poco::Data::Session::TRANSACTION_REPEATABLE_READ);
            _session.begin();
        }

        /// @brief Деструктор.
        /// @details Откатывает незафиксированные изменения.
        ~SqlUnitOfWork()
        {
            _session.rollback();
        }

        /// @brief Возвращает сессию подключения к базе данных.
        /// @return Сессия подключения к базе данных.
        [[nodiscard]] Poco::Data::Session GetSession() noexcept override
        {
            return _session;
        }

        /// @brief Подтверждает внесённые изменения.
        /// @throw std::runtime_error Если не удалось обновить агрегат из-за конфликта версий.
        /// @throw Poco::Data::DataException Если возникает ошибка при выполнении запроса.
        /// @details После фиксаций изменений запускает новую транзакцию.
        void Commit() override
        {
            AbstractUnitOfWork::Commit();
            _session.commit();
            _session.begin();
        }

        /// @brief Откатывает внесённые изменения.
        /// @details После отката изменений запускает новую транзакцию.
        void RollBack() override
        {
            _session.rollback();
            AbstractUnitOfWork::RollBack();
            _session.begin();
        }

    private:
        Poco::Data::Session _session;
        Adapters::Repository::SqlRepository _repository;
    };
}