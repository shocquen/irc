#include "Cmd.hpp"
#include "Colors.hpp"
#include "NumReply.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "irc.hpp"

#include <cstddef>
#include <iostream>
#include <ostream>
#define BUFFER_SIZE 1024

void Server::_acceptNewClient() {
    int                newSocket;
    struct sockaddr_in clientAddress;
    socklen_t          addrlen = sizeof(clientAddress);

    if ((newSocket = accept(_fd, (struct sockaddr *)&clientAddress, &addrlen)) <
        0) {
        throw ServerException("accept", errno);
    } else {
        _clients.push_back(Client(newSocket));
        std::cout << "New client" << std::endl;
    }
}

void Server::_disconnectClient(Client &client, std::string ctx) {
    _ClientIt target =
        std::find(_clients.begin(), _clients.end(), client.getPfd().fd);
    if (target != _clients.end()) {
        std::vector<int> chanIdToDel;
        for (_ChannelIt chanIt = _channels.begin(); chanIt != _channels.end();
             chanIt++) {
            chanIt->rmOperator(client);
            chanIt->rmMember(client);
            chanIt->unInviteMember(client);
            if (chanIt->getAuthor() == *target) {
                chanIdToDel.push_back(chanIt->getId());
            }
        }
        for (std::vector<int>::iterator it = chanIdToDel.begin();
             it != chanIdToDel.end(); it++) {
            _ChannelIt chanIt = _getChannel(*it);
            if (chanIt != _channels.end()) {
                chanIt->kickAll();
                _channels.erase(chanIt);
            }
        }
        client.sendMsg("ERROR :" + ctx);
        client.disconnect(ctx);
        _clients.erase(target);
    }
}

int Server::_readFromClient(const _ClientIt &client) {
    ssize_t s;
    char    buf[BUFFER_SIZE];

    bzero(buf, BUFFER_SIZE);
    s = recv(client->getPfd().fd, buf, BUFFER_SIZE - 1, MSG_DONTWAIT);
    if (s == -1) {
        throw ServerException("recv", errno);
    }
    if (s == 0) {
        client->disconnect("");
        _clients.erase(client);
    } else {
        if (client->appendBuffer(std::string(buf)))
            return (1);
    }
    return (0);
}

bool Server::_isNickUsed(std::string nick) const {
    for (_ClientConstIt it = _clients.begin(); it != _clients.end(); it++) {
        if (it->getNick() == nick)
            return (true);
    }
    return (false);
}

Server::_ClientConstIt Server::_getConstClient(std::string nick) const {
    _ClientConstIt it = std::find(_clients.begin(), _clients.end(), nick);
    return it;
}

Server::_ClientIt Server::_getClient(std::string nick) {
    _ClientIt it = std::find(_clients.begin(), _clients.end(), nick);
    return it;
}

void Server::_addChannel(Client &client, const std::string name,
                         const std::string key) {

    Channel newChan(client, name, key);
    newChan.addMember(&client);
    _channels.push_back(newChan);
}

const Server::_ChannelConstIt Server::_getConstChannel(std::string name) const {
    for (_ChannelConstIt it = _channels.begin(); it != _channels.end(); it++) {
        if (*it == name)
            return it;
    }
    return _channels.end();
}

const Server::_ChannelIt Server::_getChannel(std::string name) {
    _ChannelIt it = std::find(_channels.begin(), _channels.end(), name);
    return it;
}

const Server::_ChannelIt Server::_getChannel(unsigned long id) {
    _ChannelIt it = std::find(_channels.begin(), _channels.end(), id);
    return it;
}

const Server::_ChannelIt Server::_getChannel(const Channel &rhs) {
    for (_ChannelIt it = _channels.begin(); it != _channels.end(); it++) {
        if (*it == rhs)
            return it;
    }
    return _channels.end();
}
