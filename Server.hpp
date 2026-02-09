#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include "Client.hpp"
#include <netinet/in.h>
#include <fcntl.h>



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
};

struct sockaddr_in address; // Declaración

address.sin_family = AF_INET;         // IPv4
address.sin_addr.s_addr = INADDR_ANY; // Aceptar cualquier IP del PC
address.sin_port = htons(port);       // El puerto convertido a formato de red

#endif