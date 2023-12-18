# Utils
*Connect to ircserv*
```bash
nc -vC <ip> <port>
nc -vC 127.0.0.1 8080 // At the moment
```

# Documentations
[Sockets](https://www.geeksforgeeks.org/socket-programming-cc)
[The Bible](https://modern.ircdocs.horse/)
[Workflow Exemple](http://chi.cs.uchicago.edu/chirc/irc_examples.html)
[RPL ERR list](https://www.alien.net.au/irc/irc2numerics.html) 

# TODO
## main
- [ ] args parser                                                          *Sou*
  - port
    - is the arg only digit?
  - password
    - I think there is restrictions?


## Cmds
- [x] PASS
- [x] NICK
- [x] USER
- [x] PING
- [x] PRIVMSG
- [x] JOIN
*Channel*
- [ ] KICK
- [ ] INVITE
- [x] TOPIC
- [ ] MODE
  - i: Set/remove Invite-only channel
  - t: Set/remove the restrictions of the TOPIC command to channel operators
  - k: Set/remove the channel key (password)
  - o: Give/take channel operator privilege
  - l: Set/remove the user limit to channel

## Bot
```bash
  ./bot <host> <port> <psw>
```
- [ ] Register to server
- [ ] Respond pong on PRIVMSG
