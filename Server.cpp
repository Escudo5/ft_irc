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


    struct pollfd server_pfd;
    server_pfd.fd = _server_fd;
    server_pfd.events = POLLIN;
    server_pfd.revents = 0;
    _fds.push_back(server_pfd);
}


void Server::start()
{
    while (1)
    {
        if (poll(&_fds[0], _fds.size(), -1) == -1)
            break;
        for (int i = _fds.size() -1; i >0 ; i--)
        {
            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _server_fd) //hay actividad
                    _acceptNewConnection(); //socket maestro
                else
                    _receiveData(_fds[i].fd); //cliente que ya existia
            }
            
        }
    }
}


void Server::_acceptNewConnection()
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);


    //aceptar conexion y obtener nuevo FD
    int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd == -1)
        return;
    //hacer que el socket del cliente no sea bloqueante
    fcntl(client_fd, F_SETFL, O_NONBLOCK);


    struct pollfd client_pfd;
    client_pfd.fd = client_fd;
    client_pfd.events = POLLIN;
    client_pfd.revents = 0;

    _clients[client_fd] = new Client(client_fd);
    std::cout << "Nuevo cliente conectado en FD" << client_fd << std::endl;
}

void Server::_receiveData(int fd)
{
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int  bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) // se ha desconectado o error
    {
        _handleDisconnection(fd);
    }
    else
    {
        buffer[bytes_read] = '\0';
        _clients[fd]->appendBuffer(buffer);

        if (_clients[fd]->getBuffer().find("\n") != std::string::npos)
        {
            _processCommand(_clients[fd]);
        }
    }
}

void Server::_handleDisconnection(int fd)
{
    std::cout << "EL usuario con el FD" << fd << "se ha desconectado\n";
    for (size_t i= 0; i < _fds.size(); i++)
    {
        if (_fds[i].fd == fd)
        {
            _fds.erase(_fds.begin() + i);
            break;
        }
    }
    if (_clients.count(fd))
    {
        delete _clients[fd];
        _clients.erase(fd);
    }
    close(fd);
}


//Mientras haya \n tenemos comando completo
// Limoiamos comandos de chars invisibles
//actualizamos el buffer con lo que sobra


void Server::_processCommand(Client *client)
{
    std::string raw_data = client->getBuffer();
    size_t pos;

    while ((pos = raw_data.find("\n")) != std::string::npos)
    {
        std::string command = raw_data.substr(0, pos);
        if (!command.empty() && command[command.size() -1] == '\r')
            command.erase(command.size() - 1);
        std::cout << "Ejecutando: " << command << std::endl;


        //meter logica de comandos
        std::stringstream ss(command);
        std::string tokens;
        std::string args;
        
        ss >> tokens;
        if (tokens == "PASS")
            //seguir extrayendo contraseña
        if (tokens == "NICK")
            //sacar apodo


        
        raw_data.erase(0, pos + 1);
        //añadir funcion setBuffer.
        client->setBuffer(raw_data);
    }
}


//gestionar comandos que usen ":"
//comandos cona rgumentos vacios
// guardar argumentos en estructura de cliente