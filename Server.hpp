#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include "Client.hpp"
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <sstream>




struct Command
{
    std::string name; //PASS, NICK ...
    std::vector<std::string> params; //args
    std::string trailing; //"holamundo" lo q va despues de ":"
}
class Client;
class Server
{
    public:
        Server(int port, std::string password);
        ~Server();

        void start();
        void stop(); //Cierra sockets limpiamente.
        
    
    
    private:
        int _server_fd;
        int _port;
        std::string _password;

        //panel de contrl para poll()
        std::vector<struct pollfd> _fds;


        //Agenda para encontrar clientes por su numero de socket (FD)
        std::map<int, Client*> _clients;


        void _acceptNewConnection();
        void _receiveData(int fd);
        void _handleDisconnection(int fd);
        bool _parseCommand(const std::string &line, Command &cmd);
};




#endif