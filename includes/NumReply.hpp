#pragma once

#include "Channel.hpp"
#include "Client.hpp"
#include "Cmd.hpp"

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
class NumReply {
public:
    static std::string wolcome(const Client &client) {
        return _Builder("001", client.getNick() + " :Welcome to ft_irc " +
                                   client.getNick() + "!" +
                                   client.getUsername() + "@localhost");
    }

    static std::string noTopic(const Client &client, const Channel &chan) {
        return _Builder("331", client.getNick() + " " + chan.getName() +
                                   " :No topic is set");
    }

    static std::string topic(const Client &client, const Channel &chan) {
        return _Builder("332", client.getNick() + " " + chan.getName() + " :" +
                                   chan.getTopic());
    }

    static std::string channelModIs(const Client &client, const Channel &chan) {
        std::ostringstream modes;
        std::ostringstream args;
        if (chan.isOnInvite())
            modes << "i";
        if (chan.isTopicProtected())
            modes << "t";
        if (chan.getKey().empty() == false) {
            modes << "k";
            if (chan.isOperator(client))
                args << chan.getKey();
        }
        if (chan.getMemberLimit() != 0) {
            modes << "l";
            if (args.rdbuf()->in_avail())
                args << ",";
            args << chan.getMemberLimit();
        }

        return _Builder("324", client.getNick() + " " + chan.getName() + " " +
                                   modes.str() + " " + args.str());
    }

    static std::string inviting(const Client &client, const std::string nick,
                                const Channel &chan) {
        return _Builder("341",
                        client.getNick() + " " + nick + " " + chan.getName());
    }

    static std::string nameReply(const Client &client, const Channel &chan) {
        std::ostringstream oss;
        oss << client.getNick() << " = ";
        oss << chan.getName();
        oss << " :" << chan.listMembers();
        return _Builder("353", oss.str());
    }

    static std::string endOfNames(const Client &client, const Channel &chan) {
        return _Builder("366", client.getNick() + " " + chan.getName() +
                                   " :End of NAMES list");
    }

    // === Errors
    static std::string noSushChannel(const Client     &client,
                                     const std::string chanName) {
        return _Builder("403", client.getNick() + " " + chanName +
                                   " :No sush channel");
    }

    static std::string noSuchNick(const Cmd &cmd) {
        return _Builder("401", cmd.getAuthor().getNick() + " " +
                                   cmd.getParams().front() +
                                   " :No such nick/channel");
    }

    static std::string userNotInChannel(const Client &client, std::string nick,
                                        const Channel &chan) {
        return _Builder("442", client.getNick() + " " + nick + " " +
                                   chan.getName() +
                                   " :You're not on that channel");
    }

    static std::string notOnChannel(const Client &client, const Channel &chan) {
        return _Builder("442", client.getNick() + " " + chan.getName() +
                                   " :You're not on that channel");
    }

    static std::string userOnChannel(const Client     &client,
                                     const std::string nick,
                                     const Channel    &chan) {
        return _Builder("443", client.getNick() + " " + nick + " " +
                                   chan.getName() + " :is already on channel");
    }

    static std::string needMoreParams(const Cmd &cmd) {
        std::ostringstream oss;
        oss << cmd.getAuthor().getNick() << " ";
        oss << cmd.getName() << " :Not enough parameters";
        return _Builder("461", oss.str());
    }

    static std::string passwdMismatch() {
        return _Builder("464", ":Password incorrect");
    }

    static std::string nicknameInUse(const Cmd &cmd) {
        return _Builder("433", cmd.getAuthor().getNick() + " " +
                                   cmd.getParams().front() +
                                   " :Nickname is already in use");
    }

    static std::string noNickNameGiven(const Client &client) {
        return _Builder("431", client.getNick() + " :No nickname given");
    }

    static std::string erroneusNickname(const Cmd &cmd) {
        return _Builder("432", cmd.getAuthor().getNick() + " " +
                                   cmd.getParams().front() +
                                   " :Erroneus nickname");
    }

    static std::string alreadyRegistered(const Client &client) {
        return _Builder("462", client.getNick() + " :You may not reregister");
    }

    static std::string notRegistered(const Client &client) {
        return _Builder("451", client.getNick() + " :You have not registered");
    }

    static std::string channelIsFull(const Client  &client,
                                     const Channel &chan) {
        return _Builder("471", client.getNick() + " " + chan.getName() +
                                   " :Cannot join channel (+l)");
    }

    static std::string inviteOnlyChan(const Client     &client,
                                      const std::string chanName) {
        return _Builder("473", client.getNick() + " " + chanName +
                                   " :Cannot join channel (+i)");
    }

    static std::string badChannelKey(const Client &client,
                                     std::string   chanName) {
        return _Builder("475", client.getNick() + " " + chanName +
                                   " :Cannot join channel (+k)");
    }

    static std::string badChanMask(std::string chanName) {
        return _Builder("476", chanName + " :Bad channel Mask");
    }

    static std::string chanOPrivsNeeded(const Client  &client,
                                        const Channel &chan) {
        return _Builder("482", client.getNick() + " " + chan.getName() +
                                   " :You're not channel operator");
    }

    // ===

private:
    static std::string _Builder(const std::string code,
                                const std::string content) {
        std::ostringstream oss;
        oss << ":localhost " << code << " " << content;
        return (oss.str());
    }
};
