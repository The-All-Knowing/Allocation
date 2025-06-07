#include "Infrastructure/Server/Server.h"


int main(int argc, char** argv)
{
    setlocale(LC_ALL, "RUS");

    Allocation::Infrastructure::Server::ServerApp app;
    return app.run(argc, argv);
}