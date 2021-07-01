#include "server/WSServer.h"

WSServer::WSServer(DataProcessor *processor) : _processor(processor) {}

void WSServer::on_message(const websocketpp::connection_hdl &hdl,
                          const websocketpp::server<websocketpp::config::asio>::message_ptr &msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
//    if (msg->get_payload() == "stop-listening") {
//        _server.stop_listening();
//        return;
//    }

    // TODO: Interpret message with protobuf

    try {
        auto payload = msg->get_payload();
        if(payload == "a") {
            // This will spin off a thread
            _processor->Start({
                                      .start_level = 2,
                                      .stop_level = 1,
                                      .auto_start = false,
                                      .auto_stop = false,
                                      .label = "test",
                                      .load_cell_sample_rate = 80,
                                      .decimation = 1,
//                                  .decimation = 80,
                              });
        } else if (payload == "b") {
            // This will spin off a thread
            _processor->Stop();
        } else if (payload == "k") {
            _server.stop_listening();
//            _server.stop();
        } else if (payload == "t") {
            _processor->Tare();
        }

    } catch (const std::exception &e) {
        std::cout << "Error while parsing message: " << e.what() << "\n";
    }

//    try {
//        _server.send(hdl, msg->get_payload(), msg->get_opcode());
//    } catch (websocketpp::exception const &e) {
//        std::cout << "Echo failed because: "
//                  << "(" << e.what() << ")" << std::endl;
//    }
}

void WSServer::run(uint16_t port) {
    try {
        _server.init_asio();

        _server.set_message_handler(
                [this](auto &&PH1, auto &&PH2) {
                    on_message(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
                });

        _server.set_reuse_addr(true);

        _server.listen(port);

        // Start the server accept loop
        _server.start_accept();

        // Start the ASIO io_service run loop
        _server.run();
    } catch (websocketpp::exception const &e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}
