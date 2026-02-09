#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>

class Client
{
    public:
        Client(int fd);
        ~Client();

        const std::string &getNick(); //recibimos por referencia constante para evitar que el ordenador haga una copia del
        const std::string &getUsername();
        int getFd();


    private:

    //identificacion de red
        int _fd;
        std::string _hostname;

    //identificacion de IRC
        std::string _nickname;
        std::string _username;
        bool _isRegistered;
        bool _isAdmin;

    //Gestion de datos
        std::string _inputBuffer;  //aqui guardamos buffer de mensaje para que llegue todo del tiron.

};

#endif