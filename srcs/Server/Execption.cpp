#include "Server.hpp"

#include <cstring>

Server::ServerException::~ServerException() throw() {
    _msg.clear();
};
Server::ServerException::ServerException(std::string const msg) : _msg(msg) {}
Server::ServerException::ServerException(std::string const msg, int errnoValue)
    : _msg(msg) {
    _msg += ": " + std::string(strerror(errnoValue));
}

const char *Server::ServerException::what() const throw() {
    return _msg.c_str();
}
