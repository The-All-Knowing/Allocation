#include "Infrastructure/Server/Server.h"


int main(int argc, char** argv)
{
    Allocation::Infrastructure::Server::ServerApp app;
    return app.run(argc, argv);
}