#pragma once

#include "Precompile.h"


namespace Allocation::Tests
{
    class RedisClient
    {
    public:
        RedisClient(const std::string& host, int port, const std::string& channel);

        ~RedisClient();

        void Stop();
        std::optional<std::string> GetMessage() const;
        bool WaitForMessage(int timeoutMs);

    private:
        void Listen();

        mutable std::mutex _mutex;
        std::condition_variable _cv;
        std::optional<std::string> _message;

        std::atomic_bool _running;
        Poco::Redis::Client _client;
        std::string _channel;
        std::thread _thread;
    };
}