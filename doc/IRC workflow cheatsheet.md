# Git & Workflow Cheatsheet — ft_irc

## Workflow

1. **Create an Issue** → `[TYPE] Short description`
2. **Create a Branch** from that issue → `<type>/<issue>-<short-description>`
3. **Commit** on that branch → `type(scope): description`
4. **Push** and open a **Pull Request**→ `type/<issue>-<short-description>`

---

# Main naming convention

## Canonical Types

These 8 types are used identically in issues, branches, commits, PRs, and labels.

| Type       | Use it for                           |
| ---------- | ------------------------------------ |
| `task`     | New IRC command or capability        |
| `fix`      | Something broken or wrong            |
| `refactor` | Internal cleanup, no behavior change |
| `style`    | Formatting/norm only                 |
| `docs`     | Documentation                        |
| `test`     | Test scripts or coverage             |
| `chore`    | Makefile, CI, tooling, environment   |
| `perf`     | Performance improvement              |

## Scopes

| Scope | Covers |
|---|---|
| `auth` | PASS, NICK, USER, registration state machine |
| `channel` | JOIN, PART, channel creation, membership |
| `messaging` | PRIVMSG, NOTICE |
| `operator` | KICK, INVITE, TOPIC, operator permission checks |
| `mode` | Channel modes i, t, k, o, l |
| `lifecycle` | QUIT, client cleanup, disconnect handling |
| `keepalive` | PING, PONG |
| `parser` | IRC message parsing, partial recv reassembly |
| `network` | poll loop, socket setup, accept, non-blocking I/O |
| `server` | Server class, ClientManager, ChannelManager, dispatch |
| `makefile` | Compilation rules, flags, relinking |
| `ci` | GitHub Actions workflows |
| `infra` | Environment setup, tooling, file structure |
| `docs` | README, ADRs, runbook, architecture |
| `global` | Cross-cutting changes |


## Labels

### Type labels — mirror the canonical types above

| Label | Meaning |
|---|---|
| `type:task` | New IRC command or capability |
| `type:fix` | Something broken or wrong |
| `type:refactor` | Internal cleanup, no behavior change |
| `type:style` | Formatting/norm only |
| `type:docs` | Documentation |
| `type:test` | Test scripts or coverage |
| `type:chore` | Makefile, CI, tooling, environment |
| `type:perf` | Performance improvement |

### Priority

| Label | Meaning |
|---|---|
| `priority:high` | Blocking or critical path |
| `priority:medium` | Normal sprint work |
| `priority:low` | Nice to have |

### State

| Label | Meaning |
|---|---|
| `state:blocked` | Waiting on another issue or PR |
| `state:in-progress` | Actively being worked on |
| `state:needs-review` | Ready for code review |


---

## Issues

Format: `[type] Short description of the goal`
```text
[task] Implement PASS + NICK + USER registration flow  
[task] Implement JOIN with error replies  
[task] Implement PART with member broadcast  
[task] Implement PRIVMSG user-to-user and user-to-channel  
[task] Implement KICK with operator permission check  
[task] Implement INVITE with mode i enforcement  
[task] Implement TOPIC with mode t enforcement  
[task] Implement channel modes i, t, k, o, l  
[task] Implement QUIT with full client cleanup  
[task] Implement PING/PONG keepalive handling  
[fix] Fix partial message reassembly across recv calls  
[fix] Fix +k mode removal when no key is provided  
[fix] Fix fd leak on brutal client disconnect  
[refactor] Simplify mode flag storage in Channel  
[style] Apply clang-format rules across all source files  
[docs] Document compilation and usage in README  
[chore] Set up GitHub Actions CI build check  
[test] Add nc-based fragmentation test script
```

---

## Branches

Format: `<type>/<issue>-<short-description>`

| Example |
|---|
| `task/12-join-command` |
| `fix/25-part-segfault` |
| `refactor/31-parser-cleanup` |
| `docs/40-readme-setup` |
| `chore/07-add-ci-workflow` |
| `style/18-apply-clang-format` |
| `test/22-nc-fragmentation-script` |
| `perf/44-poll-loop-optimization` |

---

## Commit Messages

Format: `type(scope): short verb-first description`

### Scopes

| Scope | Covers |
|---|---|
| `auth` | PASS, NICK, USER, registration state machine |
| `channel` | JOIN, PART, channel creation, membership |
| `messaging` | PRIVMSG, NOTICE |
| `operator` | KICK, INVITE, TOPIC, operator permission checks |
| `mode` | Channel modes i, t, k, o, l |
| `lifecycle` | QUIT, client cleanup, disconnect handling |
| `keepalive` | PING, PONG |
| `parser` | IRC message parsing, partial recv reassembly |
| `network` | poll loop, socket setup, accept, non-blocking I/O |
| `server` | Server class, ClientManager, ChannelManager, dispatch |
| `makefile` | Compilation rules, flags, relinking |
| `ci` | GitHub Actions workflows |
| `infra` | Environment setup, tooling, file structure |
| `docs` | README, ADRs, runbook, architecture |
| `global` | Cross-cutting changes |

### Examples
```text
task(auth): implement PASS and USER registration flow  
fix(parser): handle partial message split across recv calls  
refactor(channel): simplify mode flag storage  
style(global): apply clang-format across all source files  
chore(makefile): add fclean and re rules  
chore(ci): add build check on pull_request event  
docs(docs): document single-poll event loop decision (ADR-0001)  
test(network): add nc-based fragmentation test script
```

---

## Pull Requests

Format: `type/issue Short description of what was done`
```text
task/12 Join command with error replies  
task/08 PASS NICK USER registration flow  
fix/25 Part segfault on empty channel  
fix/33 Partial message reassembly in parser  
refactor/31 Simplify mode flag storage  
style/18 Apply clang-format across all files  
chore/05 Add CI build check on pull request  
docs/40 Update README with setup instructions  
test/22 Add nc fragmentation test script
```


---

## Labels

### Type labels — mirror the canonical types above

| Label | Meaning |
|---|---|
| `type:task` | New IRC command or capability |
| `type:fix` | Something broken or wrong |
| `type:refactor` | Internal cleanup, no behavior change |
| `type:style` | Formatting/norm only |
| `type:docs` | Documentation |
| `type:test` | Test scripts or coverage |
| `type:chore` | Makefile, CI, tooling, environment |
| `type:perf` | Performance improvement |

### Priority

| Label | Meaning |
|---|---|
| `priority:high` | Blocking or critical path |
| `priority:medium` | Normal sprint work |
| `priority:low` | Nice to have |

### State

| Label | Meaning |
|---|---|
| `state:blocked` | Waiting on another issue or PR |
| `state:in-progress` | Actively being worked on |
| `state:needs-review` | Ready for code review |
