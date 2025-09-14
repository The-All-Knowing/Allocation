#include "Server.hpp"

#include "Adapters/Database/Session/SessionPool.hpp"
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

        if (_configFile.exists())
            Allocation::Loggers::GetLogger()->Information(
                "Loaded configuration from: " + _configFile.path());
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
        Adapters::Database::SessionPool::Instance().Shutdown();
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
        StartServer();
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
            _configFile = configFile;

            loadConfiguration(absPath);
        }
        catch (const Poco::Exception& ex)
        {
            std::cerr << "Failed to load configuration file '" << path << "': " << ex.displayText()
                      << std::endl
                      << "Loading environment variables." << std::endl;
        }
    }

    void ServerApp::StartServer()
    {
        Poco::Net::ServerSocket serverSocket(_port);
        Poco::Net::HTTPServer server(
            new Entrypoints::Rest::HandlerFactory, serverSocket, _serverParameters.get());

        Allocation::Loggers::GetLogger()->Information(
            "Server listening on port " + std::to_string(_port));
        server.start();
        waitForTerminationRequest();
        server.stop();
        Allocation::Loggers::GetLogger()->Information("The server is stopped");
    }

    void ServerApp::InitServer()
    {
        if (_configFile.exists())
        {
            auto [serverParameters, port] = LoadServerConfigFromFile();
            _serverParameters = std::move(serverParameters);
            _port = port;
        }
        else
        {
            auto [serverParameters, port] = ReadServerConfigurations();
            _serverParameters = std::move(serverParameters);
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
        Adapters::Database::DatabaseConfig config;
        if (_configFile.exists())
            config = LoadDatabaseConfigFromFile();
        else
            config = ReadDatabaseConfigurations();

        Adapters::Database::SessionPool::Instance().Configure(config);
        Poco::Data::PostgreSQL::Connector::registerConnector();
    }

    void ServerApp::InitRedis()
    {
        Adapters::Redis::RedisConfig config;
        if (_configFile.exists())
            config = LoadRedisConfigFromFile();
        else
            config = ReadRedisConfigurations();

        Adapters::Redis::ClientFactory::Instance().Configure(config);
        _redisListener = std::make_shared<Entrypoints::Redis::RedisListener>();
        _redisListener->Subscribe(
            "change_batch_quantity", Entrypoints::Redis::Handlers::HandleChangeBatchQuantity);
        _redisListener->Start();
    }

    Adapters::Database::DatabaseConfig ServerApp::LoadDatabaseConfigFromFile()
    {
        const auto& cfg = config();
        std::string dbHost = cfg.getString("database.host", "localhost");
        int dbPort = cfg.getInt("database.port", 5432);
        std::string dbname = cfg.getString("database.name", "allocation");
        std::string user = cfg.getString("database.username", "user");
        std::string password = cfg.getString("database.password", "password");

        std::ostringstream oss;
        oss << "host=" << dbHost << " port=" << dbPort << " dbname=" << dbname << " user=" << user
            << " password=" << password;

        Adapters::Database::DatabaseConfig config;
        config.connTimeout = cfg.getInt("database.connection_timeout", 60);
        config.connector = Poco::Data::PostgreSQL::Connector::KEY;
        config.connectionString = oss.str();
        return config;
    }

    Adapters::Redis::RedisConfig ServerApp::LoadRedisConfigFromFile()
    {
        const auto& cfg = config();
        Adapters::Redis::RedisConfig result;
        result.path = cfg.getString("redis.host", "localhost");
        result.port = cfg.getInt("redis.port", 6379);
        return result;
    }

    std::pair<std::unique_ptr<Poco::Net::HTTPServerParams>, Poco::UInt16>
    ServerApp::LoadServerConfigFromFile()
    {
        const auto& cfg = config();
        Poco::UInt16 port = cfg.getInt("server.port", 8080);
        auto pParams = std::make_unique<Poco::Net::HTTPServerParams>();
        pParams->setMaxQueued(cfg.getInt("server.max_queued", 100));
        pParams->setMaxThreads(cfg.getInt("server.max_threads", 16));
        pParams->setMaxKeepAliveRequests(cfg.getInt("server.max_connections", 100));
        return {std::move(pParams), port};
    }
}