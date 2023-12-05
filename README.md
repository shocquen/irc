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


# ATM
This is a very basic broadcast system


# TODO
## main
- [ ] args parser                                                          *Sou*
  - port
    - Is the port is a `short` ?
  - password
    - I think there is restrictions ?

## Message
- [ ] finish constructor                                                   *Sou*
  - parse `content` into list of `params` and the `cmd`

## Cmds
- [ ] NICK                                                                 *Sou*
- [ ] USER                                                                 *Sou*
- [ ] PRIVMSG                                                              *Sou*
*First do the cmds to logging into the ircserv*
- [ ] ...
