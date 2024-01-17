#pragma once

#include "Channel.hpp"
#include "Client.hpp"
#include "Cmd.hpp"

#include <list>
#include <map>
#include <string>
#include <vector>

class Server {
public:
    Server(std::string pwd, unsigned short port);
    Server(const Server &copy);
    ~Server();
    Server &operator=(const Server &rhs);

    // === Methods
    void run();
    void stop();
    // === Exeption
    class ServerException : public std::exception {
    private:
        std::string _msg;

    public:
        virtual ~ServerException() throw();
        ServerException(std::string const msg);
        ServerException(std::string const msg, int errnoValue);
        virtual const char *what() const throw();
    };
    typedef void (Server::*CmdFuncPtr)(const Cmd &);
    typedef struct {
        bool mustRegistered;
        // bool mustOnChannel;
        // bool mustPrivileges;
        CmdFuncPtr func;
    } CmdMiddleWare;

private:
    Server();
    int                  _fd;
    std::string          _pwd;
    unsigned short       _port;
    std::list<Client>    _clients;
    std::vector<Channel> _channels;
    // === Types
    typedef std::list<Client>::iterator          _ClientIt;
    typedef std::list<Client>::const_iterator    _ClientConstIt;
    typedef std::vector<Channel>::iterator       _ChannelIt;
    typedef std::vector<Channel>::const_iterator _ChannelConstIt;
    // === Methods
    void _acceptNewClient();
    void _disconnectClient(Client &client, std::string ctx);
    void _addChannel(Client &client, const std::string name,
                     const std::string key);
    // Return 1 if there is one or more complete msgs to treat.
    // Else return 0.
    int _readFromClient(const _ClientIt &client);
    // === Client Getters
    _ClientConstIt _getConstClient(std::string nick) const;
    _ClientIt      _getClient(std::string nick);

    // === Channel Getters
    const _ChannelConstIt _getConstChannel(std::string name) const;
    const _ChannelIt      _getChannel(std::string name);
    const _ChannelIt      _getChannel(unsigned long int);
    const _ChannelIt      _getChannel(const Channel &rhs);
    // === Getter checker
    bool _isNickUsed(std::string nick) const;
    // === Handlers
    void _handlePASS(const Cmd &cmd);
    void _handleNICK(const Cmd &cmd);
    void _handleUSER(const Cmd &cmd);
    void _handlePING(const Cmd &cmd);
    void _handlePRIVMSG(const Cmd &cmd);
    void _handleJOIN(const Cmd &cmd);
    void _handleTOPIC(const Cmd &cmd);
    void _handleKICK(const Cmd &cmd);
    void _handleNAMES(const Cmd &cmd);
    void _handleMODE(const Cmd &cmd);
    void _handleINVITE(const Cmd &cmd);
    void _handleQUIT(const Cmd &cmd);
    // void _handleCAP(const Cmd &cmd);

    static std::map<std::string, Server::CmdMiddleWare> initCmdHandlers();
    const static std::map<std::string, CmdMiddleWare>   _cmdHandlers;
};
