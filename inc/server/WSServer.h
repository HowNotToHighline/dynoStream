#ifndef DYNOSTREAM_WSSERVER_H
#define DYNOSTREAM_WSSERVER_H


#include <DataProcessor.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

class WSServer {
public:
    explicit WSServer(DataProcessor *processor);

    void on_message(const websocketpp::connection_hdl& hdl, const websocketpp::server<websocketpp::config::asio>::message_ptr& msg);

    void run(uint16_t port);

private:
    DataProcessor *_processor;
    Calibrator *_calibrator;
    websocketpp::server<websocketpp::config::asio> _server;
};


#endif //DYNOSTREAM_WSSERVER_H
