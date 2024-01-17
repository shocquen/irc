#include "Server.hpp"

#include "Colors.hpp"
#include "Ip.hpp"
#include "NumReply.hpp"
#include "Utils.hpp"
#include "irc.hpp"

#include <iostream>

Server::Server(std::string pwd, unsigned short port) {
    _pwd  = pwd;
    _port = port;

    _fd = socket(AF_INET, SOCK_STREAM, 0);

    if (_fd < 0) {
        throw ServerException("socket failed", errno);
    }

    int                opt = 1;
    struct sockaddr_in serverAddress;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        close(_fd);
        throw ServerException("setsocketopt", errno);
    }
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port        = htons(_port);

    if (bind(_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) <
        0) {
        close(_fd);
        throw ServerException("bind failed", errno);
    }
    if (listen(_fd, 3) < 0) {
        close(_fd);
        throw ServerException("listen", errno);
    }
    const Host host;
    std::cout << "Server running\n"
              << "\thostname: " << KYEL << host.name << RST "\n\tip: " << KYEL
              << host.IP << RST "\n\tport: " << KYEL << port << RST
              << std::endl;
}

Server::Server(const Server &copy) {
    *this = copy;
}

Server &Server::operator=(const Server &rhs) {
    _fd       = rhs._fd;
    _pwd      = rhs._pwd;
    _port     = rhs._port;
    _clients  = rhs._clients;
    _channels = rhs._channels;
    return (*this);
}

Server::~Server(){};
/* ========================================================================= */
void Server::run() {
    while (status_g == 0) {
        nfds_t        nfds = _clients.size() + 1;
        struct pollfd pfds[nfds];

        pfds[0].fd     = _fd;
        pfds[0].events = POLLIN;
        nfds_t i       = 1;
        for (_ClientConstIt it = _clients.begin(); it != _clients.end(); it++) {
            pfds[i++] = it->getPfd();
        }

        if (poll(pfds, nfds, -1) == -1) {
            throw ServerException("poll", errno);
        }
        for (nfds_t i = 0; i < nfds; i++) {
            if (!(pfds[i].revents & POLLIN))
                continue;
            if (pfds[i].fd == _fd) {
                _acceptNewClient();
                continue;
            }

            _ClientIt client =
                std::find(_clients.begin(), _clients.end(), pfds[i].fd);
            if (client == _clients.end()) {
                throw ServerException("No client found with fd: " +
                                      to_string(pfds[i].fd));
            }

            if (_readFromClient(client) == 0) // If now complete msg
                continue;

            std::vector<std::string> msgs = client->bufferToMsgs();
            for (size_t i = 0; i < msgs.size(); i++) {
                // LOG IN CMD
                std::cout << FMAG("<- ") << *client << ": " << msgs[i]
                          << std::endl;

                Cmd cmd(*client, msgs[i]);
                if (_cmdHandlers.find(cmd.getName()) == _cmdHandlers.end())
                    continue;

                CmdMiddleWare f = _cmdHandlers.at(cmd.getName());
                if (f.mustRegistered) {
                    if (client->checkRegistered() == false) {
                        client->sendMsg(NumReply::notRegistered(*client));
                        continue;
                    }
                }
                (this->*(f.func))(cmd);
            }
        }
    }
}

void Server::stop() {
    while (_clients.size() > 0) {
        _disconnectClient(_clients.front(), "server closing");
    }
    close(_fd);
}
