# ft_irc

*This project has been created as part of the 42 curriculum by dabuchhe, lebroue, and willda-s.*

## Description

**ft_irc** is a non-blocking IRC (Internet Relay Chat) server implemented in C++98. It conforms to the core IRC protocol (RFC 1459) and demonstrates advanced networking concepts including asynchronous I/O multiplexing, client-server state management, and real-time message routing across multiple concurrent connections.

### Project Goal

The project teaches fundamental concepts in systems programming:
- **Asynchronous I/O multiplexing** using `poll()` for handling multiple clients without threading
- **RFC protocols compliance** for commands, numeric replies, and message formatting
- **Non-blocking socket programming** with proper error handling and state transitions
- **Data structure design** (maps, vectors, sets) for efficient lookups and client/channel management
- **Signal handling** and graceful server shutdown

### What It Does

The server allows multiple IRC clients to:
- Authenticate using a shared password (`PASS` command)
- Register with a nickname and username (`NICK`, `USER` commands)
- Create and join channels (`JOIN`, `PART` commands)
- Exchange messages in channels or privately (`PRIVMSG`, `NOTICE` commands)
- Manage channel modes and permissions (`MODE`, `KICK`, `INVITE`, `TOPIC` commands)
- Query user and channel information (`WHO`, `WHOIS`, `LIST` commands)

## Instructions

### Requirements

- **Operating System**: Linux
- **Compiler**: C++ supporting `-std=c++98`
- **Dependencies**: None (POSIX system calls and standard C++ library only)

### Compilation

```bash
make
```

This creates an executable named `ircserv`.

To clean build artifacts:
```bash
make clean        # Remove object files
make fclean       # Remove everything including executable
make re           # Clean and rebuild
```

### Execution

```bash
./ircserv <port> <password>
```

**Parameters:**
- `<port>`: TCP listening port (typically 6667 for IRC)
- `<password>`: Server password (clients must authenticate with this via `PASS` command)

**Example:**
```bash
./ircserv 6667 secretpassword
```

The server will output:
```
[INFO  ] Listening on port 6667
```

### Basic Client Connection Example

Connect using a standard IRC client (e.g., `irssi`, `HexChat`) or use `nc`:

```bash
nc localhost 6667
```

Then send commands:
```
PASS secretpassword
NICK alice
USER alice 0 * :Alice Smith
JOIN #general
PRIVMSG #general :Hello everyone!
QUIT
```

## Resources

### RFC & Standards Documentation

- **RFC 1459** — Internet Relay Chat Protocol (foundational)  
  [https://www.rfc-editor.org/rfc/rfc1459](https://www.rfc-editor.org/rfc/rfc1459)  
  Defines core IRC commands, numeric replies, message format, and protocol semantics.

- **RFC 2812** — Internet Relay Chat: Client Protocol (modern revision)  
  [https://www.rfc-editor.org/rfc/rfc2812](https://www.rfc-editor.org/rfc/rfc2812)  
  Clarifications and updates to RFC 1459.

- **Modern IRC Documentation** — Best practices and real-world server behavior  
  [https://modern.ircdocs.horse/](https://modern.ircdocs.horse/)  
  Edge cases, extended modes, and practical implementation notes.

### System Programming Resources

- **POSIX Socket API** — Low-level networking  
  Manual pages: `man 2 socket`, `man 2 bind`, `man 2 listen`, `man 2 accept`, `man 2 recv`, `man 2 send`

- **Poll Multiplexing** — Event-driven I/O  
  Manual pages: `man 2 poll`, `man 2 fcntl` (for O_NONBLOCK)

- **Signal Handling** — Clean server shutdown  
  Manual pages: `man 2 signal`, `man 7 signal-safety`

### C++98 Standard Library

Core containers and utilities used:
- `<map>` / `<set>` — Client and channel indexing by nickname/name
- `<vector>` — Dynamic arrays for poll descriptors and command parameters
- `<string>` — IRC message parsing and formatting
- `<stdexcept>` — Exception handling for protocol errors

## AI Usage Declaration

AI was used to assist with:

- **Code Review & Quality Assurance** — Static analysis of includes, dead code detection, consistency verification, and resource cleanup validation
- **Parsing Refinement** — Protocol message parsing and validation logic
- **Channel Logic** — Channel state management and user privilege handling

---

## Project Highlights

### 1. Core Commands Implemented

**Authentication:** `PASS`, `NICK`, `USER`, `QUIT`  
**Channels:** `JOIN`, `PART`, `TOPIC`, `MODE`, `INVITE`, `KICK`, `LIST`  
**Messaging:** `PRIVMSG`, `NOTICE`  
**Information:** `WHO`, `WHOIS`, `PING`

### 2. Channel Modes

- `+i` — Invite-only (only invited users can join)
- `+t` — Topic restricted (only operators can set topic)
- `+k` — Key-protected (requires password to join)
- `+l` — User limit (maximum members allowed)

### 3. Technical Highlights

✓ **Non-blocking I/O** — Server never blocks on socket operations  
✓ **Asynchronous Multiplexing** — Handles 100+ concurrent clients efficiently via `poll()`  
✓ **RFC Compliance** — Numeric replies (401, 403, 433, etc.) per RFC 1459  
✓ **Signal Safety** — Graceful SIGINT/SIGQUIT handling  
✓ **Buffer Management** — Automatic line buffering with 512-byte limit  
✓ **Case-Insensitive Matching** — IRC standard scrambling (`[]\` ↔ `{}|`) for nicks/channels

## Project Structure

```
ft_irc/
├── Makefile              # Build rules
├── README.md             # This file
├── ircserv              # Compiled server executable
├── include/             # Header files
│   ├── Server.hpp       # Main server class
│   ├── Client.hpp       # Client state & buffers
│   ├── Channel.hpp      # Channel & member management
│   ├── Commands.hpp     # Command handler declarations
│   ├── Parsing.hpp      # IRC protocol parser
│   └── Utils.hpp        # Utility functions
└── src/                 # Implementation
    ├── main.cpp
    ├── server/          # Server networking (poll, accept, send/recv)
    ├── client/          # Client state machine
    ├── channel/         # Channel logic
    ├── commands/        # Individual command implementations
    ├── parsing/         # IRC message parsing
    └── utils/           # Helper functions
```

## Known Limitations

- Broadcast of `NICK` changes not yet implemented
- No server-initiated `PING` keepalive
- `USER` re-registration not blocked (should return 462)
- No MOTD/server banner

Detailed issues and future improvements are documented in `doc/BUGS-TODO.md`.

---

## Testing & Debugging

### Quick Test

```bash
make && ./ircserv 6667 pass &
nc localhost 6667
# Then type IRC commands as shown above
```

### Memory Leak Detection

```bash
valgrind --leak-check=full ./ircserv 6667 pass
```

### With Existing IRC Client

```bash
./ircserv 6667 mypass
# In another terminal:
irssi -c localhost -p 6667 -w mypass
```

---

## References

RFC 1459: Internet Relay Chat Protocol  
RFC 2812: Internet Relay Chat: Client Protocol  
Modern IRC Documentation: https://modern.ircdocs.horse/
Internet Relay Chat with Hexchat : https://youtu.be/G9_vvWTb8sI?si=BdKlBLoU5yM52ju1