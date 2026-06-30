# ft_IRC — Issue Generation Prompt  
  
## Context  
  
This prompt is used to generate the full GitHub artifact set for a single issue in the `ft_IRC` project (a C++98 IRC server, 42 school project). For each issue described in the **Issue List** section below, produce the complete set of artifacts defined in the **Output Format** section, strictly following the **Conventions** section.  
  
---  
  
## Project Overview  
  
The codebase being merged introduces a full command parsing architecture onto `main` from a feature branch (`test/command-parsing`). The new files are:  
  
**Headers (`include/commands/`)**  
include/commands/  
├── ACommand.hpp  
├── CommandDispatcher.hpp  
├── CommandParser.hpp  
├── auth/  
│ ├── NickCommand.hpp  
│ ├── PassCommand.hpp  
│ └── UserCommand.hpp  
├── channel/  
│ ├── JoinCommand.hpp  
│ └── PrivmsgCommand.hpp  
└── operator/  
├── InviteCommand.hpp  
├── KickCommand.hpp  
├── ModeCommand.hpp  
└── TopicCommand.hpp  

  
**Sources (`src/commands/`)**  
src/commands/  
├── ACommand.cpp  
├── CommandDispatcher.cpp  
├── CommandParser.cpp  
├── auth/  
│ ├── auth.mk  
│ ├── NickCommand.cpp  
│ ├── PassCommand.cpp  
│ └── UserCommand.cpp  
├── channel/  
│ ├── channel.mk  
│ ├── JoinCommand.cpp  
│ └── PrivmsgCommand.cpp  
├── commands.mk  
└── operator/  
├── operator.mk  
├── InviteCommand.cpp  
├── KickCommand.cpp  
├── ModeCommand.cpp  
└── TopicCommand.cpp  
  
The root `Makefile` also requires changes to include the new `.mk` sub-makefiles (`commands.mk`, `auth.mk`, `channel.mk`, `operator.mk`).  
  
---  
  
## Conventions  
  
All artifacts must strictly follow these rules.  
  
### Canonical Types  
  
Used identically in issue titles, branch names, commit messages, PR titles, and labels.  
  
| Type | Use it for |  
|---|---|  
| `task` | New IRC command or capability |  
| `fix` | Something broken or wrong |  
| `refactor` | Internal cleanup, no behavior change |  
| `style` | Formatting/norm only |  
| `docs` | Documentation |  
| `test` | Test scripts or coverage |  
| `chore` | Makefile, CI, tooling, environment |  
| `perf` | Performance improvement |  
  
### Scopes  
  
| Scope | Covers |  
|---|---|  
| `auth` | PASS, NICK, USER, registration state machine |  
| `channel` | JOIN, PART, channel creation, membership |  
| `messaging` | PRIVMSG, NOTICE |  
| `operator` | KICK, INVITE, TOPIC, operator permission checks |  
| `mode` | Channel modes i, t, k, o, l |  
| `parser` | IRC message parsing, partial recv reassembly |  
| `server` | Server class, ClientManager, ChannelManager, dispatch |  
| `makefile` | Compilation rules, flags, relinking |  
| `infra` | Environment setup, tooling, file structure |  
| `docs` | README, ADRs, runbook, architecture |  
| `global` | Cross-cutting changes |  
  
### Labels  
  
type:task | type:fix | type:refactor | type:style | type:docs | type:test | type:chore | type:perf  
priority:high | priority:medium | priority:low  
  
  
### Naming Formats  
  
**Issue title:** `[type] Short description of the goal`  
[task] Implement JOIN with error replies  
[chore] Integrate commands.mk into root Makefile  
  
  
**Branch name:** `<type>/<issue-number>-<short-description>`  
task/12-join-command  
chore/03-commands-mk-integration  

> The issue number is a placeholder (e.g. `XX`) if not yet known — leave it as a variable.  
  
**Commit messages:** `type(scope): short verb-first description`  
task(auth): implement PASS command with pre-registration check  
chore(makefile): include auth.mk and channel.mk sub-makefiles  

> One commit message = one atomic change. Provide 1–3 representative commit messages per issue, not an exhaustive list.  
  
**PR title:** `type/XX Short description of what was done`  
task/12 PASS command with pre-registration check  
chore/03 Integrate sub-makefiles into root Makefile  

  
### Issue Body Template  
  
```markdown  
## Context  
Why does this task exist? Which part of the protocol / architecture does it touch?  
  
## Objective  
What is the expected outcome?  
  
## Acceptation  
- [ ] acceptance criterion 1  
- [ ] acceptance criterion 2  
- [ ] acceptance criterion 3  
  
## Dependencies  
- related issue (if any)  
- required structure (if any)  
  
## Technical notes  
Relevant technical elements: IRC reply numbers, edge cases, affected files, etc.  
```  
  
### PR Body Template  
  
```markdown  
## Description  
What does this PR do?  
  
## Why  
Why is this change necessary?  
  
## Changes  
- change 1  
- change 2  
- change 3  
  
## Tests  
- [ ] Manual test with nc / reference IRC client  
- [ ] Edge case test (fragmented message, brutal disconnect)  
- [ ] Test with bad parameters (malformed command)  
  
## Risks  
What are the possible impacts on the rest of the server?  
  
## Linked issue  
Closes #XX  
```  
  
---  
  
## Output Format  
  
For each issue in the list below, produce the following sections in order:  
  
[type] Short description of the goal  
Issue title: [type] Short description of the goal  
Labels: type:X, priority:X  
Branch: type/XX-short-description  
Issue body:  
(filled template)  
Commit messages:  
type(scope): description  
type(scope): description  
PR title: type/XX Short description of what was done  
PR body:  
(filled template)  
  
---  
  
## Issue List  
  
Each entry below is an issue to generate. The short description after the title is non-exhaustive context — use it to infer scope, acceptance criteria, affected files, and technical notes. Do not treat it as an exhaustive specification.  
  
---  
  
### 1. `[chore]` Integrate command sub-makefiles into root Makefile  
  
The new parsing architecture introduces partial Makefiles (`commands.mk`, `auth.mk`, `channel.mk`, `operator.mk`). The root `Makefile` must be updated to include them so all new `.cpp` files are compiled. No new feature — purely build system wiring.  
  
---  
  
### 2. `[task]` Add ACommand abstract base class  
  
Introduces `ACommand.hpp` and `ACommand.cpp`. This is the abstract interface all command handlers derive from. Defines the pure virtual `execute()` method and any shared utilities (e.g. reply formatting helpers, sender reference).  
  
---  
  
### 3. `[task]` Add CommandParser IRC message tokenizer  
  
Introduces `CommandParser.hpp` and `CommandParser.cpp`. Responsible for parsing a raw IRC line (prefix, command name, parameters including trailing) into a structured representation. Must handle edge cases: missing prefix, trailing parameter with `:`, malformed input.  
  
---  
  
### 4. `[task]` Add CommandDispatcher with command registry  
  
Introduces `CommandDispatcher.hpp` and `CommandDispatcher.cpp`. Owns the map from command name string to command handler instance. Receives a parsed message and routes it to the correct handler. Must gracefully handle unknown commands with `ERR_UNKNOWNCOMMAND`.  
  
---  
  
### 5. `[task]` Implement PASS command with pre-registration check  
  
Introduces `PassCommand.hpp` and `PassCommand.cpp` under `auth/`. Validates the server password before NICK/USER. Must reject with `ERR_NEEDMOREPARAMS` or `ERR_ALREADYREGISTERED` where appropriate.  
  
---  
  
### 6. `[task]` Implement NICK command with uniqueness and validity check  
  
Introduces `NickCommand.hpp` and `NickCommand.cpp` under `auth/`. Sets or changes a client's nickname. Must validate character set, check for collisions, and broadcast change to relevant channels if already registered.  
  
---  
  
### 7. `[task]` Implement USER command and registration state finalization  
  
Introduces `UserCommand.hpp` and `UserCommand.cpp` under `auth/`. Finalizes client registration when combined with NICK and PASS. Must send `RPL_WELCOME` (001), `RPL_YOURHOST` (002), `RPL_CREATED` (003), `RPL_MYINFO` (004) on success.  
  
---  
  
### 8. `[task]` Implement JOIN command with channel creation and error replies  
  
Introduces `JoinCommand.hpp` and `JoinCommand.cpp` under `channel/`. Creates the channel if it doesn't exist and adds the client as member (and operator if first to join). Must handle `ERR_INVITEONLYCHAN`, `ERR_BADCHANNELKEY`, `ERR_CHANNELISFULL`. Sends `RPL_TOPIC` / `RPL_NOTOPIC` and `RPL_NAMREPLY` on success.  
  
---  
  
### 9. `[task]` Implement PRIVMSG for user-to-user and user-to-channel  
  
Introduces `PrivmsgCommand.hpp` and `PrivmsgCommand.cpp` under `channel/`. Delivers a message to a target nick or channel. Must handle `ERR_NOSUCHNICK`, `ERR_CANNOTSENDTOCHAN`, `ERR_NORECIPIENT`, `ERR_NOTEXTTOSEND`.  
  
---  
  
### 10. `[task]` Implement KICK command with operator permission check  
  
Introduces `KickCommand.hpp` and `KickCommand.cpp` under `operator/`. Removes a client from a channel. Requires the sender to be a channel operator. Must handle `ERR_NOTONCHANNEL`, `ERR_CHANOPRIVSNEEDED`, `ERR_USERNOTINCHANNEL`.  
  
---  
  
### 11. `[task]` Implement INVITE command with mode i enforcement  
  
Introduces `InviteCommand.hpp` and `InviteCommand.cpp` under `operator/`. Invites a client to a channel. On invite-only channels (`+i`), only operators may invite. Must handle `ERR_NOTONCHANNEL`, `ERR_CHANOPRIVSNEEDED`, `ERR_USERONCHANNEL`. Sends `RPL_INVITING`.  
  
---  
  
### 12. `[task]` Implement TOPIC command with mode t enforcement  
  
Introduces `TopicCommand.hpp` and `TopicCommand.cpp` under `operator/`. Gets or sets the channel topic. When mode `+t` is set, only operators may change it. Must handle `ERR_NOTONCHANNEL`, `ERR_CHANOPRIVSNEEDED`. Sends `RPL_TOPIC` or `RPL_NOTOPIC`.  
  
---  
  
### 13. `[task]` Implement MODE command for channel flags i, t, k, o, l  
  
Introduces `ModeCommand.hpp` and `ModeCommand.cpp` under `operator/`. Sets or unsets channel modes: `i` (invite-only), `t` (topic restricted), `k` (key), `o` (operator grant/revoke), `l` (user limit). Must handle `ERR_CHANOPRIVSNEEDED`, `ERR_UNKNOWNMODE`, `ERR_NEEDMOREPARAMS`. Sends `RPL_CHANNELMODEIS` on query.  
  
---  
  
### 14. `[test]` Add nc-based integration test script for command parsing  
  
A shell script (e.g. `tests/test_commands.sh`) that connects via `nc -C` and exercises the full registration flow (PASS/NICK/USER), then each implemented command in sequence. Covers happy path and at least one error case per command group (auth, channel, operator).  
  
---  
  
### 15. `[style]` Apply clang-format across all new commands source files  
  
Applies the project `.clang-format` rules uniformly to all files introduced by this milestone: `ACommand`, `CommandParser`, `CommandDispatcher`, and all files under `auth/`, `channel/`, `operator/`. No logic changes.  
  
---  
  
### 16. `[docs]` Document command parsing architecture in docs/architecture.md  
  
Updates `docs/architecture.md` to describe the parsing pipeline: how a raw IRC line flows from `recv` buffer → `CommandParser` → `CommandDispatcher` → concrete command handler. Includes a note on the `ACommand` interface contract and the dispatch map design.