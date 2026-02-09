#include "Server.hpp"



// AF_INTE = ipv4  SOCK_STREAM =  protocolo TCP

// setsockopt() para cuando reinicias que no se bloquee el puerto

//fcntl para que no se congele cuando no hay datos
Server::Server(int port, std::string password) : _port(port), _password(password)
{
    struct sockaddr_in address;

    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int en = 1;
    setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en));
    fcntl(this->_server_fd, F_SETFL, O_NONBLOCK);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);
    bind(this->_server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(this->_server_fd, 10);
}