#include <DataProcessor.h>
#include <server/WSServer.h>

int main() {
    DataProcessor processor;
    WSServer server(&processor);

    std::cout << "Listening on port 9002";
    server.run(9002);


//    processor.AutoStart(1, 1);
//    usleep(1000000);



    return 0;
}
