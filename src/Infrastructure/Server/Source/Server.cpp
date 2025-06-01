#include "Server.h"

#include "HandlerFactory.h"
#include "Adapters/Database/Include/Session/SessionPool.h"
#include "Adapters/Database/Include/DbTables.h"


namespace Allocation::Infrastructure::Server
{
    int ServerApp::main(const std::vector<std::string>&)
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Adapters::Database::SessionPool::Instance().Configure("SQLite", "TestBd");
        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        Adapters::Database::InitDatabase(session);

        Poco::UInt16 port = 9999;
        Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
        pParams->setMaxQueued(100);
        pParams->setMaxThreads(16);
        Poco::Net::ServerSocket svs(port);
        Poco::Net::HTTPServer server(new HandlerFactory, svs, pParams);

        server.start();
        waitForTerminationRequest();
        server.stop();

        return Application::EXIT_OK;
    };
}