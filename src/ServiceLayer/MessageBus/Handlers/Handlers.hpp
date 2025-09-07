#pragma once

#include "Adapters/Notification/EmailNotifications.hpp"
#include "Adapters/Redis/RedisEventPublisher.hpp"
#include "Domain/Commands/Allocate.hpp"
#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "Domain/Commands/CreateBatch.hpp"
#include "Domain/Events/Allocated.hpp"
#include "Domain/Events/Deallocated.hpp"
#include "Domain/Events/OutOfStock.hpp"
#include "Domain/Ports/IUnitOfWork.hpp"
#include "NotificationHandler.hpp"
#include "PublisherHandler.hpp"


namespace Allocation::ServiceLayer::Handlers
{
    /// @brief Добавляет новую партию товара.
    /// @param uow Единица работы.
    /// @param command Команда "создать партию".
    void AddBatch(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Commands::CreateBatch> command);

    /// @brief Выделяет товар из партии.
    /// @param uow Единица работы.
    /// @param command Событие "выделить".
    void Allocate(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Commands::Allocate> command);

    /// @brief Изменяет количество товара в партии.
    /// @param uow Единица работы.
    /// @param command Команда "изменить количество партии".
    void ChangeBatchQuantity(
        Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Commands::ChangeBatchQuantity> command);

    /// @brief Перераспределяет товар из одной партии в другую.
    /// @param uow Единица работы.
    /// @param event Событие "товар деаллоцирован".
    void Reallocate(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::Deallocated> event);

    /// @brief Отправляет уведомление по электронной почте, когда товара нет на складе.
    using SendOutOfStockNotification = NotificationHandler<Domain::Events::OutOfStock,
        Allocation::Adapters::Notification::EmailSender>;

    /// @brief Публикует событие "товар выделен".
    using PublishAllocatedEvent = PublisherHandler<Domain::Events::Allocated,
        Allocation::Adapters::Redis::RedisEventPublisher<Domain::Events::Allocated>>;

    /// @brief Добавляет выделение в модель чтения.
    /// @param uow Единица работы.
    /// @param event Событие "товар выделен".
    void AddAllocationToReadModel(
        Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::Allocated> event);

    /// @brief Удаляет выделение из модели чтения.
    /// @param uow Единица работы.
    /// @param event Событие "товар деаллоцирован".
    void RemoveAllocationFromReadModel(
        Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::Deallocated> event);
}