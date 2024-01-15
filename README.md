# Utils
*Connect to ircserv*
```bash
nc -vC <ip> <port>
irssi -c <server> -p <port> -w <pwd> -n <name>

# At the moment
nc -vC 127.0.0.1 8080
irssi -c 127.0.0.1 -p 8080 -w LLD -n sou
```

# Documentations
[Sockets](https://www.geeksforgeeks.org/socket-programming-cc)
[The Bible](https://modern.ircdocs.horse/)
[Workflow Exemple](http://chi.cs.uchicago.edu/chirc/irc_examples.html)
[RPL ERR list](https://www.alien.net.au/irc/irc2numerics.html) 

# TODO

## main
- [x] args parser                                                          *Sou*
  - port
    - is the arg only digit?
  - password
    - I think there is restrictions?

Must finish MODE and check JOIN implement with MODE

## Cmds
- [x] PASS
- [x] NICK
- [x] USER
- [x] PING
- [x] PRIVMSG
- [ ] JOIN
  - create only one chan without any pwd
  - check if you're invited before letting u join the chan
*Channel*
- [ ] KICK
- [ ] INVITE
- [x] NAMES
- [x] TOPIC
- [ ] MODE (channel only)
  - i: Set/remove Invite-only channel
  - t: Set/remove the restrictions of the TOPIC command to channel operators
  - k: Set/remove the channel key (password)
  - o: Give/take channel operator privilege
  - l: Set/remove the user limit to channel
  - handle if only target is sent in the params

## Bot
```bash
  ./bot <host> <port> <psw>
```
- [ ] Register to server
- [ ] Respond pong on PRIVMSG

## Mon 15 Jan 24
- [ ] MODE (channel only)
```
  if chan doesn't exist: ERR_NOSUCHCHANNEL
  if no modestring: RPL_CHANNELMODEIS
    if client is not a chan's member dont send chan's key
  if modestring:
    if client has no priv: ERR_CHANOPRIVSNEEDED

    Invite Only: +i
      no arg
    Topic Protected: +t
      no arg
    Key: +k
      arg string
    Operator Priv: +o
      arg string
    Client Limit: +l
      arg int

  At the end: send MODE cmd to all members
```
