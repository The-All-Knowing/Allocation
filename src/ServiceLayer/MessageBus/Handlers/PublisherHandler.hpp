#pragma once

#include "Precompile.hpp"

#include "Domain/Events/AbstractEvent.hpp"


namespace Allocation::ServiceLayer::Handlers
{
    template <typename T, typename Message>
    concept PublisherSender = requires(T t, std::string channel, std::shared_ptr<Message> event) {
        { t(channel, event) } -> std::same_as<void>;
    };

    template <typename Message, PublisherSender<Message> Publisher>
        requires std::derived_from<Message, Domain::Events::AbstractEvent>
    class PublisherHandler
    {
    public:
        PublisherHandler(Publisher publisher = {}) : _publisher(std::move(publisher)) {}

        void operator()(std::shared_ptr<Message> event) const
        {
            Allocation::Loggers::GetLogger()->Debug(
                std::format("publishing: channel={}, event={}", "line_allocated", event->Name()));
            _publisher("line_allocated", event);
        }

    private:
        Publisher _publisher;
    };
}