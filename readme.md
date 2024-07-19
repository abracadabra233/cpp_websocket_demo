```sh
sudo apt update
sudo apt install git build-essential libboost-system-dev libboost-thread-dev

git clone https://github.com/abracadabra233/cpp_websocket_demo
cd cpp_websocket_demo

git clone --depth 1 --branch main https://github.com/zaphoyd/websocketpp.git
git clone --depth 1 --branch develop https://github.com/nlohmann/json.git

g++ -std=c++11 -o server server.cpp -I./json/include -I./websocketpp -lboost_system -lpthread
g++ -std=c++11 -o client client.cpp -I./json/include -I./websocketpp -lboost_system -lpthread

./server 19988
./client 19988
```
