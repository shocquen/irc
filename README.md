# Utils
*Connect to ircserv*
```bash
nc -vC <ip> <port>
nc -vC 127.0.0.1 8080 // At the moment
```

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
