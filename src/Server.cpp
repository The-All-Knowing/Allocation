#include "Server.hpp"

#include "Adapters/Database/Session/DatabaseSessionPool.hpp"
#include "Entrypoints/REST/HandlerFactory.hpp"
#include "Entrypoints/Redis/Handlers.hpp"
#include "ServiceLayer/MessageBus/Handlers/Handlers.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "ServiceLayer/PocoLogger.hpp"
#include "Utilities/ConfigReaders.hpp"


namespace Allocation
{
    void ServerApp::initialize(Application& self)
    {
        if (!Allocation::Loggers::IsInitialize())
            Allocation::Loggers::InitializeLogger(
                std::make_shared<ServiceLayer::Loggers::PocoLogger>());

        if (_configFile.isFile())
            Allocation::Loggers::GetLogger()->Information(
                "Loaded configuration from: " + _configFile.toString());
        else
            Allocation::Loggers::GetLogger()->Information(
                "No configuration file loaded. Using environment variables.");

        InitRedis();
        InitDatabase();
        InitMessageBus();
        InitServer();
        ServerApplication::initialize(self);
    }

    void ServerApp::uninitialize()
    {
        _redisListener->Stop();
        Adapters::Database::DatabaseSessionPool::Instance().Shutdown();
    }

    void ServerApp::defineOptions(Poco::Util::OptionSet& options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
            Poco::Util::Option("help", "h", "Show help")
                .required(false)
                .repeatable(false)
                .callback(Poco::Util::OptionCallback<ServerApp>(this, &ServerApp::HandleHelp)));

        options.addOption(Poco::Util::Option("config", "c", "Set path to config file")
                              .argument("file")
                              .required(false)
                              .repeatable(false)
                              .callback(Poco::Util::OptionCallback<ServerApp>(
                                  this, &ServerApp::HandlePathToConfig)));
    }

    int ServerApp::main(const std::vector<std::string>&)
    {
        if (_helpRequested)
            return Application::EXIT_OK;
        StartMainCycle();
        return Application::EXIT_OK;
    };

    void ServerApp::HandleHelp(const std::string& name, const std::string&)
    {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("Order Management Service.");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
    }

    void ServerApp::HandlePathToConfig(const std::string& name, const std::string& path)
    {
        try
        {
            Poco::Path configPath(path);
            if (configPath.isRelative())
                configPath = Poco::Path(Poco::Path::current()).append(configPath);

            std::string absPath = configPath.toString();
            std::cout << "Config path argument: " << absPath << std::endl;

            Poco::File configFile(configPath);
            if (!configFile.exists() || !configFile.isFile())
            {
                std::cerr << "Configuration file not found or not a regular file: " << absPath
                          << std::endl
                          << "Loading environment variables." << std::endl;
                return;
            }
            _configFile = configPath;

            loadConfiguration(absPath);
        }
        catch (const Poco::Exception& ex)
        {
            std::cerr << "Failed to load configuration file '" << path << "': " << ex.displayText()
                      << std::endl
                      << "Loading environment variables." << std::endl;
        }
    }

    void ServerApp::StartMainCycle()
    {
        Poco::Net::ServerSocket serverSocket(_port);
        Poco::Net::HTTPServer server(
            new Entrypoints::Rest::HandlerFactory, serverSocket, _serverParameters);

        Allocation::Loggers::GetLogger()->Information(
            "Server listening on port " + std::to_string(_port));
        server.start();
        waitForTerminationRequest();
        server.stop();
        Allocation::Loggers::GetLogger()->Information("The server is stopped");
    }

    void ServerApp::InitServer()
    {
        if (_configFile.isFile())
        {
            auto [serverParameters, port] = LoadServerConfigFromFile(config());
            _serverParameters = serverParameters;
            _port = port;
        }
        else
        {
            auto [serverParameters, port] = ReadServerConfigurations();
            _serverParameters = serverParameters;
            _port = port;
        }
    }

    void ServerApp::InitMessageBus()
    {
        auto& messagebus = ServiceLayer::MessageBus::Instance();
        messagebus.SubscribeToEvent<Domain::Events::Allocated>(
            ServiceLayer::Handlers::PublishAllocatedEvent());
        messagebus.SubscribeToEvent<Domain::Events::Allocated>(
            ServiceLayer::Handlers::AddAllocationToReadModel);
        messagebus.SubscribeToEvent<Domain::Events::Deallocated>(
            ServiceLayer::Handlers::RemoveAllocationFromReadModel);
        messagebus.SubscribeToEvent<Domain::Events::Deallocated>(
            ServiceLayer::Handlers::Reallocate);
        messagebus.SubscribeToEvent<Domain::Events::OutOfStock>(
            ServiceLayer::Handlers::SendOutOfStockNotification());

        messagebus.SetCommandHandler<Domain::Commands::Allocate>(ServiceLayer::Handlers::Allocate);
        messagebus.SetCommandHandler<Domain::Commands::CreateBatch>(
            ServiceLayer::Handlers::AddBatch);
        messagebus.SetCommandHandler<Domain::Commands::ChangeBatchQuantity>(
            ServiceLayer::Handlers::ChangeBatchQuantity);
    }

    void ServerApp::InitDatabase()
    {
        Adapters::Database::DatabaseConfig databaseConfig;
        if (_configFile.isFile())
            databaseConfig = LoadDatabaseConfigFromFile(config());
        else
            databaseConfig = ReadDatabaseConfigurations();

        Adapters::Database::DatabaseSessionPool::Instance().Configure(databaseConfig);
    }

    void ServerApp::InitRedis()
    {
        Adapters::Redis::RedisConfig redisConfig;
        if (_configFile.isFile())
            redisConfig = LoadRedisConfigFromFile(config());
        else
            redisConfig = ReadRedisConfigurations();

        Adapters::Redis::RedisConnectionPool::Instance().Configure(redisConfig);
        _redisListener = std::make_unique<Entrypoints::Redis::RedisListener>();
        _redisListener->Subscribe(
            "change_batch_quantity", Entrypoints::Redis::Handlers::HandleChangeBatchQuantity);
        _redisListener->Start();
    }
}