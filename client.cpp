#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

typedef websocketpp::client<websocketpp::config::asio_client> client;

void on_open(client *c, websocketpp::connection_hdl hdl)
{
    std::thread([c, hdl]()
                {
        std::string msg;
        while (true)
        {
            std::cout << "Enter message to send: ";
            std::getline(std::cin, msg);

            json j;
            j["message"] = msg;

            c->send(hdl, j.dump(), websocketpp::frame::opcode::text);
        } })
        .detach();
}

void on_message(client *c, websocketpp::connection_hdl hdl, client::message_ptr msg)
{
    std::cout << "Received message: " << msg->get_payload() << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));

    client c;
    std::string uri = "ws://localhost:" + std::to_string(port);

    try
    {
        c.init_asio();
        c.set_open_handler(bind(&on_open, &c, std::placeholders::_1));
        c.set_message_handler(bind(&on_message, &c, std::placeholders::_1, std::placeholders::_2));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec)
        {
            std::cout << "Could not create connection because: " << ec.message() << std::endl;
            return 1;
        }

        c.connect(con);
        c.run();
    }
    catch (websocketpp::exception const &e)
    {
        std::cout << e.what() << std::endl;
    }
}
