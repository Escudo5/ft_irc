#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _isRegistered(false), _isAdmin(false) {}

Client::~Client() {}

int Client::getFd() { return _fd; }

const std::string &Client::getNick() { return _nickname; }

Client* Client::setBuffer(std::string raw_data)
{
    _buffer = raw_data;
    return this;
}


void Client::setAuthenticated(bool status)
{
    _isRegistered = status;
}
