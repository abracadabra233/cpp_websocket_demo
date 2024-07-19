#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>
#include <set>
#include <functional>
#include <nlohmann/json.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::connection_hdl;
using json = nlohmann::json;

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;

class broadcast_server
{
public:
    broadcast_server()
    {
        m_server.init_asio();
        m_server.set_open_handler(std::bind(&broadcast_server::on_open, this, std::placeholders::_1));
        m_server.set_close_handler(std::bind(&broadcast_server::on_close, this, std::placeholders::_1));
        m_server.set_message_handler(std::bind(&broadcast_server::on_message, this, std::placeholders::_1, std::placeholders::_2));
    }

    void on_open(connection_hdl hdl)
    {
        m_connections.insert(hdl);
        std::cout << "Client connected" << std::endl; // Print message on client connect
    }

    void on_close(connection_hdl hdl)
    {
        m_connections.erase(hdl);
        std::cout << "Client disconnected" << std::endl; // Print message on client disconnect
    }

    void on_message(connection_hdl hdl, server::message_ptr msg)
    {
        std::string received_message = msg->get_payload();
        std::cout << "Received message: " << received_message << std::endl;

        // Parse the received message as JSON
        json received_json = json::parse(received_message);
        std::string reply_message = "Server received: " + received_json["message"].get<std::string>();

        // Create a JSON response
        json response_json;
        response_json["reply"] = reply_message;

        // Send the JSON response
        m_server.send(hdl, response_json.dump(), msg->get_opcode());

        // Broadcast the original message to all connected clients
        for (auto it : m_connections)
        {
            m_server.send(it, received_message, msg->get_opcode());
        }
    }
    void run(uint16_t port)
    {
        m_server.listen(port);
        m_server.start_accept();
        m_server.run();
    }

private:
    typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

    server m_server;
    con_list m_connections;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));
    broadcast_server server_instance;
    server_instance.run(port);
}
