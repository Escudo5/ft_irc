#include "Server.hpp"
#include "Client.hpp"



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
    server_pfd.revents = 0;
    _fds.push_back(server_pfd);

    _initCommands();
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

        while (_clients[fd]->getBuffer().find("\n") != std::string::npos)
        {
            std::string buffer = _clients[fd]->getBuffer();
            size_t pos = buffer.find("\n");
            
            // Extract the line (command)
            std::string line = buffer.substr(0, pos);
            
            // Remove the processed line from the buffer
            // pos + 1 to include the '\n' character
            std::string remaining = buffer.substr(pos + 1);
            _clients[fd]->setBuffer(remaining);

            // Handle carriage return if present (e.g., from telnet)
            if (!line.empty() && line[line.length() - 1] == '\r')
                line.erase(line.length() - 1);

            if (!line.empty())
            {
                Command cmd;
                if (_parseCommand(line, cmd))
                {
                   std::cout << "CMD: " << cmd.name << " ARGS: " << cmd.params.size() << std::endl;
                    _executeCommand(cmd, _clients[fd]);
                }
            }
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


bool Server::_parseCommand(const std::string &line, Command &cmd)
{
    if (line.empty())
        return false;
    if (!line.find_first_not_of(" \t"))
        return false;

    std::string head;
    size_t pos = line.find(" :");
    if (pos != std::string::npos)
    {
        head = line.substr(0, pos);
        cmd.trailing = line.substr(pos + 2);
    }
    else
    {
        head = line;
        cmd.trailing = "";
    }

    std::stringstream ss(head);
    std::string token;
    
    // First token logic (prefix vs command)
    if (ss >> token)
    {
        if (token[0] == ':')
        {
            cmd.prefix = token.substr(1); // Remove ':'
            if (ss >> token)
                cmd.name = token;
            else
                return false; // Prefix but no command?
        }
        else
        {
            cmd.prefix = "";
            cmd.name = token;
        }
    }
    else
        return false;

    // Uppercase command name (NICK == nick)
    for (size_t i = 0; i < cmd.name.length(); i++)
        cmd.name[i] = std::toupper(cmd.name[i]);

    cmd.params.clear();
    while (ss >> token)
    {
        cmd.params.push_back(token);
    }
    return true;
}  








void Server::_initCommands()
{
    _commands["PASS"] = &Server::_handlePass;
    _commands["NICK"] = &Server::_handleNick;
    _commands["USER"] = &Server::_handleUser;
}

void Server::_executeCommand(const Command &cmd, Client *client)
{
    if (_commands.count(cmd.name))
    {
        (this->*_commands[cmd.name])(client, cmd);
    }
    else
    {
        std::cout << "Comando no encontrado: " << cmd.name << std::endl;
    }
}

void Server::_handlePass(Client *client, const Command &cmd)
{
    if (cmd.params.empty())
    {
        std::cout << "Error: PASS command requires a password" << std::endl;
        return;
    }
    else if (cmd.params[0] == _password)
    {
        client->setAuthenticated(true);
        std::cout << "Usuario autenticado" << std::endl;
    }
    else
    {
        std::cout << "Error: Invalid password" << std::endl;
    }
    std::cout << "Manejando PASS para FD " << client->getFd() << std::endl;
}

void Server::_handleNick(Client *client, const Command &cmd)
{
    std::cout << "Manejando NICK para FD " << client->getFd() << std::endl;
}

void Server::_handleUser(Client *client, const Command &cmd)
{
    std::cout << "Manejando USER para FD " << client->getFd() << std::endl;
}



























