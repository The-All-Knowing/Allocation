#pragma once

#include "Precompile.h"
#include "Events/IEvent.h"


namespace Allocation::Services
{

    class EventBus
    {
    public:
        static EventBus& Instance();

        template<typename T>
        void Subscribe(std::function<void(std::shared_ptr<T>)> handler)
        {
            auto& handlers = _subscribers[typeid(T)];
            handlers.push_back([handler](Domain::Events::IEventPtr event)
            {
                handler(std::static_pointer_cast<T>(event));
            });
        }

        void Publish(const Domain::Events::IEventPtr& event);

    private:
        EventBus();

        std::unordered_map<std::type_index, std::vector<std::function<void(Domain::Events::IEventPtr)>>> _subscribers;
    };
}