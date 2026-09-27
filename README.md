# ft_irc — IRC Server Implementation

A lightweight IRC (Internet Relay Chat) server written in C++98, implementing the core IRC protocol to enable real-time text communication between clients.

## Overview

This project implements a multi-client IRC server using **non-blocking I/O** with `poll()`. The server manages concurrent client connections, channel operations, and command dispatch according to RFC 1459 specifications.

**Language:** C++98  
**Compilation:** `-Wall -Wextra -Werror -std=c++98`

---

## Building & Running

### Build
```bash
make          # Compile the project
make clean    # Remove object files and dependencies
make fclean   # Remove everything (including executable)
make re       # Clean rebuild
```

### Run
```bash
./ircserv <port> <password>
```

**Example:**
```bash
./ircserv 6667 mypassword
```

---

## Architecture

### 1. Server & Client Management

**Key Files:** `src/server/Server.cpp`, `src/client/Client.cpp`

The server operates as a **singleton event loop** using `poll()` to monitor multiple file descriptors simultaneously:

- **Socket Creation:** Listening socket bound to the specified port in non-blocking mode.
- **Poll Loop:** Continuously monitors incoming connections (`POLLIN`), client data, and outgoing buffers (`POLLOUT`).
- **Client Lifecycle:** 
  - `acceptClient()` — Accepts new connections and registers them in the poll set.
  - `receiveClient()` — Reads incoming data into a **512-byte line buffer** (RFC 1459 limit).
  - `sendClient()` — Flushes buffered messages to clients.
  - `disconnectClient()` — Gracefully removes clients and notifies channels.

**Client State:**
- `_nickname` — User's display name (case-insensitive with IRC scrambling).
- `_username` / `_hostname` / `_realname` — Identity information.
- `_inBuffer` / `_outBuffer` — Persistent input and output buffers for partial messages.
- `_authenticated` — Password validation status.
- `_registered` — Full registration status (requires password, nick, and user).

**Key Design Decisions:**
- Non-blocking I/O prevents server stalls on slow clients.
- `std::map<int, Client*>` indexes clients by socket FD for O(log n) lookup.
- `std::map<std::string, Client*>` maintains nick uniqueness (case-insensitive).
- Signal handlers (`SIGINT`, `SIGQUIT`, `SIGPIPE`) enable graceful shutdown.

---

### 2. Parsing & Command Dispatch

**Key Files:** `src/parsing/Parsing.cpp`, `src/server/ServerCommands.cpp`

IRC commands follow a line-based protocol with optional prefix, command name, and up to 15 parameters:

```
[:<prefix>] <command> [<param1> ... <param15>] <trailing>
```

**Parser Responsibilities:**
- Extract and validate the RFC 1459 **message format** (max 512 bytes per line including `\r\n`).
- Normalize command names to uppercase.
- Split parameters while preserving the trailing argument (after the last space or `:` marker).
- Handle malformed messages gracefully (ignore or disconnect on critical errors).

**Command Dispatch:**
- `dispatchCommand()` routes parsed commands to appropriate handlers (e.g., `handleJoin()`, `handleNick()`).
- Access control: Only unauthenticated clients can send `PASS`, `NICK`, `USER`, `CAP`, or `QUIT`.
- Unknown commands trigger `ERR_UNKNOWNCOMMAND (421)`.

**Implemented Commands:**
- **Connection:** `PASS`, `NICK`, `USER`, `QUIT`, `PING`
- **Channel:** `JOIN`, `PART`, `MODE`, `TOPIC`, `INVITE`, `KICK`, `LIST`
- **Messaging:** `PRIVMSG`, `NOTICE`
- **Information:** `WHO`, `WHOIS`
- **Negotiation:** `CAP` (basic support)

---

### 3. Channels

**Key Files:** `src/channel/Channel.cpp`, `src/server/ServerChannels.cpp`

Channels are persistent namespaces where clients can join, leave, and exchange messages.

**Channel Features:**
- **Name:** Starts with `#`, case-insensitive with IRC scrambling (`[]\` ↔ `{}|`).
- **Membership:** Tracks members and operators separately via `std::map<Client*, int>`.
- **Modes:**
  - `+i` — Invite-only (controlled via `INVITE` command).
  - `+t` — Topic restricted (only operators can change topic).
  - `+k` — Key-protected (password required to join).
  - `+l` — User limit (maximum members).
- **Broadcast:** `broadcast()` sends a message to all members, optionally excluding one client.
- **Lifecycle:** Created on first `JOIN`, destroyed when empty.

**Topic & Key Management:**
- `setTopic()` / `getTopic()` — Channel message of the day (editable).
- `setKey()` / `getKey()` — Optional password (required for join if `+k` set).

---

## Data Flow

### Client Registration
1. Client connects → added to `_clientsByFd`.
2. Sends `PASS <password>` → validated, `_authenticated = true`.
3. Sends `NICK <name>` → normalized, added to `_clientsByNick`.
4. Sends `USER <user> 0 * :<realname>` → user info stored, `_registered = true`.
5. Server sends welcome messages (`001 RPL_WELCOME`, `002 RPL_YOURHOST`, etc.).

### Channel Join
1. `JOIN #channelname [key]` arrives.
2. Parser extracts channel name(s) and key(s).
3. Server creates channel if needed.
4. Client added as member; mode checks apply (invite-only, key validation).
5. `JOIN` message broadcast to all channel members.

### Message Flow
1. Client sends `PRIVMSG #channel :message`.
2. Parser extracts target and text.
3. Server finds channel and broadcasts to all members (except sender).
4. Or, if target is a nick, message sent to that specific client.

---

## Error Handling

The server implements RFC 1459 numeric error codes:

| Code | Condition |
|------|-----------|
| `401` | `ERR_NOSUCHNICK` — Target nick not found |
| `403` | `ERR_NOSUCHCHANNEL` — Channel does not exist |
| `404` | `ERR_CANNOTSENDTOCHAN` — Missing channel membership |
| `421` | `ERR_UNKNOWNCOMMAND` — Unrecognized command |
| `431` | `ERR_NONICKNAMEGIVEN` — No nick provided |
| `432` | `ERR_ERRONEUSNICKNAME` — Invalid nick format |
| `433` | `ERR_NICKNAMEINUSE` — Nick already taken |
| `451` | `ERR_NOTREGISTERED` — Must register first |
| `461` | `ERR_NEEDMOREPARAMS` — Missing required parameters |
| `464` | `ERR_PASSWDMISMATCH` — Wrong password |
| `482` | `ERR_CHANOPRIVSNEEDED` — Operator status required |

Malformed messages or protocol violations result in:
- Line too long (>512 bytes) → truncation + client disconnect.
- Missing newline → buffered until complete.
- Exception during parsing → message ignored, client remains connected.

---

## Compilation & Standards

- **C++ Version:** C++98 (strict compliance, no C++11+ features).
- **Compiler Flags:** `-Wall -Wextra -Werror -std=c++98`.
- **System Dependencies:** POSIX (Linux/macOS) — uses `poll()`, `select()`, and standard sockets.

### File Organization
```
include/         — Header files (Server.hpp, Client.hpp, Channel.hpp, etc.)
src/
  ├── main.cpp            — Entry point
  ├── client/Client.cpp   — Client implementation
  ├── channel/Channel.cpp — Channel implementation
  ├── server/Server*.cpp  — Server core, channels, clients, commands, network, communication
  ├── commands/*.cpp      — Individual command handlers
  └── parsing/Parsing*.cpp — Message parsing utilities
Makefile         — Build automation
```

---

## Notable Implementation Details

1. **Case-Insensitive Nicks & Channels:** Keys stored normalized; display names preserved separately.
2. **Circular Buffer Avoidance:** Partial messages buffered in `_inBuffer` until a complete line (`\r\n`) arrives.
3. **Non-Blocking I/O:** `fcntl(..., O_NONBLOCK)` on all sockets prevents blocking reads/writes.
4. **Signal Safety:** `SIGPIPE` ignored; `SIGINT`/`SIGQUIT` trigger graceful shutdown.
5. **Memory Management:** Destructors clean up client maps and channel pointers; no persistent leaks on normal exit.

---

## Sources & References

- **RFC 1459:** Internet Relay Chat Protocol  
  https://tools.ietf.org/html/rfc1459
- **RFC 2812:** Internet Relay Chat: Client Protocol  
  https://tools.ietf.org/html/rfc2812
- **POSIX Socket Programming:** `man 7 socket`, `man 2 poll`, `man 2 recv`, `man 2 send`
- **C++ Standard (ISO/IEC 14882:1998):** Templates, STL containers (map, vector, set)

---

## Known Limitations

- No TLS/SSL encryption.
- Simplified operator/mode system (no full IRC operator hierarchy).
- No persistent state or logging.
- Limited to RFC 1459 core features (no SASL, CAP negotiation incomplete).

---

**Project Status:** Functional IRC server suitable for educational purposes and small-scale testing. Production use requires additional hardening and feature completeness.
