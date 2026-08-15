*This project has been created as part of the 42 curriculum by kmitsuki, yuknakas.*

# ft_irc

## Description

`ft_irc` is an IRC server written from scratch in **C++98**, following RFC 1459 / 2812.
It accepts connections from real IRC clients over TCP/IP, authenticates them with a
password, and relays messages between users and channels.

The whole server runs in **a single process, a single thread and a single `poll()` call**.
Every socket is non-blocking and no I/O is ever performed outside the event loop, so one
slow or frozen client can never stall the others. `errno` is never inspected after
`recv`/`send`: the return value alone drives the state machine, and real errors are
detected through `POLLERR` / `POLLHUP`.

The event loop is four steps, repeated forever:

| Step | What it does |
| :--- | :--- |
| `reap()` | closes and deletes the clients that finished disconnecting |
| `build()` | rebuilds the `pollfd` array; asks for `POLLOUT` only when data is queued |
| `poll()` | sleeps until something happens (0 % CPU while idle) |
| dispatch | `accept` new clients, `recv` from ready ones, `send` queued replies |

Two problems shape the whole design:

- **TCP has no message boundaries.** Incoming bytes are appended to a per-client buffer
  and only split into commands when a newline arrives; the remainder is kept for later.
- **`send()` may accept only part of the data.** Replies are appended to a per-client
  queue and flushed byte by byte whenever `poll()` reports the socket as writable.

## Features

### Client messages

| Command | Parameters | Description |
| :--- | :--- | :--- |
| `CAP` | `LS` / `LIST` / `REQ` / `END` | Capability negotiation. No capability is supported, so `LS` returns an empty list and `REQ` is answered with `NAK`. Required by irssi. |
| `PASS` | `<password>` | Connection password. Must be sent **before** `NICK`/`USER`. |
| `NICK` | `<nickname>` | Sets or changes the nickname. Duplicates (case-insensitive) are refused with `433`. |
| `USER` | `<username> 0 * :<realname>` | Sets username and realname. Registration completes once `PASS`, `NICK` and `USER` are all valid. |
| `PING` | `<token>` | Answered with `PONG <token>`. Clients use it as a keep-alive. |
| `PONG` | `[<server>] <token>` | Reply to a `PING`. Accepted and ignored. |
| `QUIT` | `[<reason>]` | Leaves every channel, notifies the other members, then closes the connection. |

### Channel operations

| Command | Parameters | Description |
| :--- | :--- | :--- |
| `JOIN` | `<channel>{,<channel>} [<key>{,<key>}]` | Joins channels, creating them if needed. The creator becomes operator. `JOIN 0` leaves every channel. |
| `PART` | `<channel>{,<channel>} [<reason>]` | Leaves channels. An empty channel is destroyed. |
| `TOPIC` | `<channel> [<topic>]` | Views the topic, or changes it. Restricted to operators when `+t` is set. |
| `INVITE` | `<nickname> <channel>` | Invites a user. Required to enter an `+i` channel. |
| `KICK` | `<channel> <user>{,<user>} [<comment>]` | Removes users from a channel. Operators only. |
| `MODE` | `<channel> [<modestring> [<arguments>]]` | Views or changes channel modes. Operators only for changes. |
| `PRIVMSG` | `<target>{,<target>} :<text>` | Sends a message to channels and/or users. |
| `NOTICE` | `<target>{,<target>} :<text>` | Same as `PRIVMSG`, except **no error reply is ever returned**, which prevents loops between bots. |

### Channel modes

| Mode | Argument | Effect |
| :--- | :--- | :--- |
| `+i` / `-i` | none | Invite-only. `JOIN` without an invite is refused with `473`. |
| `+t` / `-t` | none | Only operators may change the topic. |
| `+k` / `-k` | `<key>` on `+k` | Channel password. Wrong key is refused with `475`. |
| `+o` / `-o` | `<nickname>` | Grants or removes operator status. |
| `+l` / `-l` | `<limit>` on `+l` | Maximum number of members. A full channel is refused with `471`. |

## Technical choices

| Choice | Reason |
| :--- | :--- |
| One `poll()`, non-blocking sockets | Required by the subject. Also the only way one thread can serve many clients without ever blocking on a slow peer. |
| Per-client receive buffer + line splitting | TCP is a byte stream. A command may arrive split across several `recv()` calls, or several commands may arrive in one. |
| Per-client send queue flushed on `POLLOUT` | `send()` may transmit only part of the buffer. Queueing lets a frozen client's messages pile up harmlessly instead of blocking the server. |
| `POLLOUT` requested only when data is queued | Sockets are almost always writable; requesting it unconditionally makes `poll()` return immediately and burns 100 % CPU. |
| Deferred close (`quit()` marks, `reap()` destroys) | Deleting a client in the middle of the event loop would invalidate iterators and pointers still in use. |
| Case-insensitive lookups, original spelling kept | The protocol treats `#Chan` and `#chan` as the same channel, but clients expect their own spelling back. |
| A source sent by a client is skipped, never trusted | Otherwise anyone could impersonate another user by prefixing their commands. |
| No length limit on nicknames or lines | Not required by the subject. An input line without a newline is capped at 64 KB to prevent memory exhaustion. |
| Signal handler only sets a flag | Nothing else is async-signal-safe. The real cleanup runs in the normal context, so destructors free everything. |

## Instructions

### Build

```sh
make            # builds ./ircserv
make clean      # removes object and dependency files
make fclean     # also removes the binary
make re         # fclean + all
```

Compiled with `c++ -Wall -Wextra -Werror -std=c++98 -pedantic -Wshadow`.
No external library is used. Header dependencies are generated by the compiler
(`-MMD -MP`), so editing a header triggers the right rebuild.

### Run

```sh
./ircserv <port> <password>
./ircserv 6667 hunter2
```

`<port>` must be an integer between 1024 and 65535, `<password>` must not be empty.
The server listens on **every** network interface (`0.0.0.0`) and prints nothing while
idle. Stop it with `Ctrl-C`.

### Connecting with `nc`

`nc` speaks raw IRC, which makes it the easiest way to see exactly what the server sends.

```sh
nc -C localhost 6667
```

`-C` makes Enter send `CRLF`, which is the protocol's line ending. The server also
accepts a bare `LF`, so `-C` is optional.

Type these lines one by one. The server replies `001` to `005` once the third one is sent:

```
PASS hunter2
NICK alice
USER alice 0 * :Alice Liddell
```

From there, every command in the tables above can be typed directly:

```
JOIN #42                        join (and create) a channel
PRIVMSG #42 :hello everyone     talk to the channel
PRIVMSG bob :hello bob          private message
NOTICE #42 :heads up            notice (never produces an error reply)
TOPIC #42 :daily standup        set the topic
TOPIC #42                       show the topic
MODE #42                        show the current modes
MODE #42 +t                     only operators may change the topic
MODE #42 +k s3cret              set a channel key
MODE #42 +l 10                  limit the channel to 10 users
MODE #42 +o bob                 make bob an operator
MODE #42 +i                     make the channel invite-only
INVITE bob #42                  invite bob
KICK #42 bob :see you           remove bob
PART #42 :bye                   leave the channel
QUIT :goodbye                   disconnect
```

Joining several channels at once, with keys, works as well:

```
JOIN #dev,#ops secret1,secret2
PRIVMSG #dev,#ops :message to both
```

To check that a command split across several packets is reassembled, type `NI`, wait a
few seconds without pressing Enter, then type `CK bob` and press Enter. The server still
sees a single `NICK bob`.

### Connecting with `irssi`

`irssi` is our reference client.

Inside irssi:

```
/connect localhost 6667 hunter2     connect and authenticate
/join #42                           join a channel
/msg #42 hello                      talk to the channel
/msg bob hello                      private message
/notice bob heads up                notice
/topic #42 daily standup            set the topic
/mode #42 +t                        channel mode
/mode #42 +o bob                    give operator status
/invite bob #42                     invite
/kick #42 bob bye                   kick
/names                              list the members of the channel
/part #42                           leave
/quit                               disconnect
```

Useful keys: `Ctrl-N` / `Ctrl-P` switch windows, `Alt-1`...`Alt-9` jump to a window,
`/window close` closes one.

Starting irssi already connected is also possible:

```sh
irssi --connect=localhost --port=6667 --password=hunter2 --nick=alice
```

### Testing both at once

Run the server in one terminal, `irssi` in a second and `nc` in a third, join the same
channel from both clients and talk in either direction. Every message sent by one client
is delivered to the other, and a client's own message is never echoed back to itself.

## Resources

- [RFC 1459: Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812: IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/): the clearest description
  of numeric replies and of the nickname/channel naming rules we follow
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/): sockets, `poll()`
  and non-blocking I/O
- `man 2 poll`, `man 2 recv`, `man 2 send`, `man 2 fcntl`, `man 7 signal`
- `bircd.tar.gz`, the skeleton attached to the subject, for the shape of the event loop

### Use of AI

AI tools were used for the following tasks:

- **Translating and organising this README**: the structure and the English wording of
  this document.
- **Searching**: locating the relevant sections of RFC 1459 / 2812 and of the modern IRC
  documentation, and looking up the exact format of numeric replies.
- **Formatting the code**: making naming, indentation and comment style consistent
  across files written by two people.
- **Finding edge cases**: enumerating situations to test (partial commands, killed
  clients, frozen clients being flooded, oversized input, many simultaneous connections)
  and checking the behaviour against the evaluation sheet.

The protocol logic, the event loop and the class design were written and are understood
by us.
