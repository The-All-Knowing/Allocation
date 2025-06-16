#include "RedisClient.h"


namespace Allocation::Tests
{
    RedisClient::RedisClient(const std::string& host, int port, const std::string& channel)
        : _client(host, port), _channel(channel), _running(true)
    {
        Poco::Redis::Command subscribe("SUBSCRIBE");
        subscribe.add(channel);
        _client.execute<void>(subscribe);

        _thread = std::thread([this]() { this->Listen(); });
    }

    RedisClient::~RedisClient()
    {
        Stop();
    }

    void RedisClient::Stop()
    {
        if (_running.exchange(false))
        {
            if (_thread.joinable())
                _thread.join();
        } 
    }

    std::optional<std::string> RedisClient::GetMessage() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _message;
    }

    bool RedisClient::WaitForMessage(int timeoutMs)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _cv.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this]() {
            return _message.has_value();
        });
    }

    void RedisClient::Listen()
    {
        while (_running)
        {
            Poco::Redis::Array reply;
            _client.setReceiveTimeout(500);
            try
            {
                _client.readReply(reply);
                if (!reply.isNull() && reply.size() >= 3)
                {
                    std::string type = reply.get<Poco::Redis::BulkString>(0).value();
                    std::string channel = reply.get<Poco::Redis::BulkString>(1).value();
                    std::string payload = reply.get<Poco::Redis::BulkString>(2).value();
                    if (type == "message" && channel == _channel)
                    {
                        {
                            std::lock_guard<std::mutex> lock(_mutex);
                            _message = payload;
                        }
                        _cv.notify_one();
                        break;
                    }
                }
            }
            catch (const Poco::Exception&)
            {}
        }
    }
}