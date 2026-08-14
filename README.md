*This project has been created as part of the 42 curriculum by kmitsuki.*

# ft_irc

## Description

## Features

List of implemented Client Messages
| Client Message | Parameters | Description |
| :------------: | :--------: | :---------- |
| `CAP` | `<subcommand>` `[:<capabilities>]` | Used for capability negotiation between a server and a client. |
| `PASS` | `<password>` | Used to set a "connection password" to connect to a server. If the server requires a password, clients must provide the password before attempts to register a connection (before `NICK`/`USER` combination). |
| `NICK` | `nickname` | Used to give or change the client's nickname. If duplicate nickname, server issues an `ERR_NICKNAMEINUSE` numeric and ignores the `NICK` command. |
| `USER` | `<username> 0 * <realname>` | Used at the start of a connection to specify the username and realname of a new user. The second and third parameter should be a `0` and `*` by the client, since the meaning of these parameters differ betweeen versions of the IRC protocol. |
| `PING` | `<token>` | Used by either clients or servers to check if the other side is still connected, and check for connection latency, at the application layer. The `<token>` may be any non-empty string. |
| `PONG` | `[<server>] <token>` | Used to reply to the `PING` command, by both server and client. The `<token>` should match the one in the `PING` message that triggerd this. The `<server>` parameter MUST be sent by servers, while clients SHOULD ignore it. |
| `QUIT` | `[<reason>]` | Used to terminate a clinet's connection to the server. The server awknowledges this by replying with an `ERROR` message and closing the connection to the client. |

List of implemented Channel Operations
| Channel Operation | Parameters | Description |
| :------------: | :--------: | :---------- |
| `JOIN` | `<channel>{,<channel>} [<key>{,<key>}]` | Indicatecs that the client wants to join the given channel(s), each channel using the given key for it. |
| `PART` | `<channel>{,<channel>} [<reason>]` | Removes the client from the given channels. |
| `TOPIC` | `<channel> [<topic>]` | Change or view the topic of the given channel. If `<topic>` is not given, either `RPL_TOPIC` (current topic) or `RPL_NOTOPIC` (no channel topic) is retuned. If `<topic>` is an empty string, the channel's topic will be cleared. |
| `INVITE` | `<nickname> <channel>` | Invites a user with nickname `<nickname>` to the channel `<channel>`. The server should reject the command under certain circumstances, including when the channel is *invite-only*, and the user is not a channel operator. |
| `KICK` | `<channel> <user> *( "," <user> ) [<comment>]` | Requests the forced removal of a user `<user>` from channel `<channel>`. This message may be sent by a server to a client to notify that someone has been removed from a channel, in which `<source>` will be the client who sent the kick, and `<channel>` be the channel said client got kicked from. If no comment is given, the server SHOULD use a default message instead. |
| `MODE` | `<target> [<modestring> [<mode arguments>...]]` | Sets or removes options (aka modes) from a given target. |
| `PRIVMSG`| `<target>{,<target>} <text to be sent>` | Sends a private message between users, as well as send messages to channels. `<target>` is the nickname or name of client/channel. |
| `NOTICE` | `<target>{,<target>} <text to be sent>` | Sends notices between users and channels. `<target>` is interpreted the same way as for the `PRIVMSG` command. Main difference is that automatic replies must never be sent in responce to a `NOTICE` message. Typically used by bots. |

## Technical choices
Nicknames se by `NICK` are limited by the following [rules](https://modern.ircdocs.horse/#clients)


## Instructions

### Build

```sh
make
```

### Run

```sh
./ircserv <port> <password>
```

### Connect

## Resources
 - https://medium.com/@mohcin.ghalmi/irc-server-internet-relay-chat-bd08e4f469f8
 - https://zenn.dev/rt3mis10/articles/84a45df8e898d9
 - https://modern.ircdocs.horse/#cap-message
 - https://modern.ircdocs.horse/#clients
## Use of AI
