# Git & GitHub Workflow Guide

---

## Workflow Checklist

A chronological reference from opening an issue to merging into `main`.

1. **Issue** — open an issue in full english, set priority label, assign yourself, write a task checklist
2. **Branch** — create a branch from `main` (or branch from a dependent branch)  with `<type>/<issue#>-<slug>`
3. **Commits** — commit incrementally using `type(scope): description`
4. **Living issue** — tick checkboxes as tasks are completed, edit if scope shifts
5. **Integration** — merge `integration` into your branch if your work depends on other ongoing branches
6. **Pull Request** — open a PR with a conventional commit title `<type>(<scope>): <description>` and `Closes #<issue>` in the body
7. **Living PR** — keep the PR description accurate as the work evolves
8. **Review** — CI passes, required approvals received
9. **Merge** — squash merge into `main`

---

## Quick Reference

### Patterns

| Artifact       | Pattern                          |
| -------------- | -------------------------------- |
| Issue title    | Imperative plain English         |
| Branch name    | `<type>/<issue#>-<slug>`         |
| Commit message | `<type>(<scope>): <description>` |
| PR title       | `<type>(<scope>): <description>` |
| PR body        | contains `Closes #<issue>`       |
|                |                                  |

A **slug** is a short, lowercase, hyphen-separated label derived from the issue title, three to four words maximum.

### Examples

| Artifact | Example |
|----------|---------|
| Issue title | `Add JWT authentication to backend` |
| Branch name | `feat/42-jwt-auth` |
| Commit message | `feat(backend): add token generation` |
| PR title | `feat(backend): add JWT authentication` |
| PR body | `Closes #42` |

### Commit Types

| `type`     | Description                                                     |
| ---------- | --------------------------------------------------------------- |
| `feat`     | New functionality visible in the product                        |
| `fix`      | Bug correction                                                  |
| `refactor` | Code restructured with no behavior change                       |
| `style`    | Formatting, linting, Prettier/ESLint passes — zero logic change |
| `test`     | Adding or updating tests                                        |
| `ci`       | CI pipelines, GitHub Actions, workflow changes                  |
| `chore`    | Dependency updates, config files, tooling maintenance           |
| `docs`     | Documentation only                                              |
| `perf`     | Performance improvement, particularly game loop and rendering   |
| `security` | Auth, JWT, OAuth, 2FA, session, input validation                |

### Scopes

| `scope`     |  description |
|---|---|
| `frontend`  |  SvelteKit routes, Svelte components, shadcn-svelte, Vite config |
| `backend`  |   Server-side services, API routes, hooks.server.ts |
| `auth`    |    42 OAuth flow, JWT, TOTP, session injection |
| `game`   |     Phaser engine, Colyseus rooms, game-server/, game logic |
| `db`     |     Prisma schema, migrations, database client, queries |
| `shared` |     packages/game-shared/ — code used by both game-server and frontend |
| `infra` |      Docker, compose.yaml, deployment, .github/ |
| `i18n` |       Paraglide translations, messages/*.json |

`auth` is kept separate from `backend` because auth issues are security-sensitive, span the frontend/backend boundary, and have a distinct resolution path. Commits touching `src/lib/server/auth/` almost always need both the auth owner and a security review.

### Labels

```
type: feat          type: fix           type: refactor      type: style
type: test          type: ci            type: chore         type: docs
type: perf          type: security

scope: frontend     scope: backend      scope: auth         scope: game
scope: db           scope: shared       scope: infra        scope: i18n

status: todo        status: in progress     status: in review
status: done        status: blocked

priority: critical  priority: high      priority: low
```

Every issue and PR must have at least one label from each of `type:`, `scope:`, and `status:`. `priority:` is optional when urgency is neutral — medium priority is the implicit default and gets no label.

All `status:` labels except `status: blocked` are managed automatically by the automation workflows. Do not set them by hand except for blocked.

---

## Language

Everything in this repository — issue titles, descriptions, commit messages, PR titles, comments — must be written in **English**.

---

## Workflow Overview

`main` is always in a deployable state. All work happens on short-lived branches that merge back through a reviewed pull request. The integration CI runs on every PR to `main` and must pass before merging.

---

## Issues

### Creating an Issue

An issue represents deliberate, planned work. Choose the template that matches the type of work: it automatically applies the correct `type:` label and `status: todo`. **Before submitting** set the assignees, and priority directly in the creation panel — GitHub exposes all of this without needing to edit afterward.

Issue titles are plain imperative English. The type belongs in the labels, not the title.

**Pattern:**
```
Imperative plain English sentence, no prefix
```

**Examples:**
```
Add POST /login endpoint
Fix redirect loop on authentication failure
Update Nginx configuration for WebSocket support
```

The body is free-form Markdown. Write it by hand or adapt AI-generated content — the format is yours. What matters is that the intent is clear.

Every issue **must** include a task checklist. It keeps work visible, makes progress trackable, and gives teammates a real-time picture of where things stand without needing to ask. An issue without a checklist is either too vague or not yet ready to work on.

```markdown
- [ ] Implement the endpoint
- [ ] Add unit tests
- [ ] Update the documentation
```

GitHub renders these as interactive checkboxes and shows a progress counter on the issue card.

> If you are unsure what the checklist should contain, the issue scope is not defined clearly enough yet.

Some features are large enough to be split into smaller, tracked sub-issues. Those are covered in the Sub-Issues section. When opening an issue that turns out to be a dependency of another, link or convert it rather than leaving it disconnected.

### Living Issues

An issue is not a static document. As work progresses:

- Task completed -> tick the checkbox
- Something in the checklist is no longer relevant -> remove it
- Something new comes up during implementation -> add it to the checklist
- The scope has shifted -> update the description

Keeping the issue accurate is part of the work, not optional housekeeping.

### Sub-Issues

When a feature is too large for a single branch and PR, create a **parent issue** and attach sub-issues to it. Each sub-issue gets its own branch and PR. GitHub tracks their completion from the parent automatically. The **parent issue** should not have a branch and should close itself once all sub-issues branches are merged.

```
Issue #10: Implement user authentication         [parent]
  Sub-issue #11: Add POST /register endpoint
  Sub-issue #12: Add POST /login endpoint
  Sub-issue #13: Add JWT refresh token logic
```

Use sub-issues when the work is a distinct, mergeable deliverable of a larger goal.

[Not a sub-issue] Small incidental changes discovered during implementation:
- editing a Makefile
- adjusting a config file
->belong as commits on the current branch. They do not need their own issue. This is covered in the Commits section.

---

## Branches

### Naming

**Pattern:**
```
<type>/<issue#>-<slug>
```

**Examples:**
```
feat/42-jwt-auth
fix/67-login-redirect
ci/15-docker-healthcheck
docs/8-api-reference
```

The issue number is mandatory: it links a branch to its intent without needing to trace through commit messages.

### Lifecycle

Always create a branch from an up-to-date `main`.

```bash
git checkout main
git pull origin main
git checkout -b feat/42-jwt-auth
```

Keep branches short-lived. A branch open for weeks without merging is a sign the scope needs to be split into sub-issues.

### Personal Sandbox: `dev/`

A `dev/<yourname>` branch is a personal sandbox: experiment freely, prototype ideas, push broken code. It is *never* merged into `main` or `integration`. Delete it by hand once it is no longer useful — there is no automation for this.

`test/` is a branch prefix reserved for *actual test code* (unit tests, integration tests, test fixtures).

**Personal sandbox** -> use `dev/`
**Test code changes** -> use `test/`

---

## Working on Dependent Issues

### The Situation

`feat/42-jwt-auth` is open for review. Your next issue depends on that work. Branching from `main` is not an option because the code you need is not there yet.

### The Solution

Branch from the open branch, not from `main`:

```
main
 |
 +-- feat/42-jwt-auth           [open PR, under review]
      |
      +-- feat/55-login-form    [your next branch, based here]
```

Open a PR for `feat/55` targeting **`feat/42-jwt-auth` as the base branch**. This way the diff only shows your new work, not the entirety of `feat/42`.

### After `feat/42` Merges

`feat/42` is squash-merged into `main`. Your `feat/55` still carries the old commits from `feat/42`. Update it:

```bash
git fetch origin
git merge origin/main
git push origin feat/55-login-form
```

Then update the PR base branch in GitHub from `feat/42-jwt-auth` to `main`. The PR now shows only your work, correctly based on the latest `main`.

### Full Flow: Developer and Reviewer for branching from another branch

**Developer:**

1. `feat/42` is open for review — branch `feat/55` from it, open a PR targeting `feat/42`
2. Work on `feat/55` independently
3. `feat/42` merges into `main` -> merge `origin/main` into `feat/55` -> update the PR base to `main`
4. `feat/55` is now a standard PR ready for review

**Reviewer:**

1. Receive the review request
2. Read the linked issue first — understand the intent before reading the code
3. Review the diff with that intent in mind
4. Leave inline comments on specific lines, or a general comment for broader concerns
5. **Something unclear** -> request changes and explain what needs addressing, do not approve
6. Changes addressed -> re-review -> approve if satisfied
7. Required approvals received and CI passing -> author merges

> Approving code you do not understand is not a review.

---

## Integration Branch

The `integration` branch is a **shared staging area** where work from multiple open branches gets wired together before any of it hits `main`. It is useful when parallel workstreams have mutual dependencies.

```
main
 |
 +-- feat/42-jwt-auth        \
 +-- feat/55-login-form   --> integration   [testing inter-service work]
 +-- feat/60-auth-middleware /
```

`integration` is not permanent and can be freely updated. It is never the source of truth — `main` is. Work that goes through `integration` still goes through its own individual PR to `main`. Integration is a coordination tool, not a shortcut around review.

---

## Commits

### Format

**Pattern:**
```
<type>(<scope>): <short description>

[optional body: why, not what]
```

**Examples:**
```
feat(backend): add JWT token generation
fix(frontend): correct auth redirect on logout
chore(makefile): add backend compilation flags
```

The description is lowercase, imperative mood ("add" not "added"), and under 72 characters.

### Scope

Scopes are flat and coarse-grained: they name the domain, not the file or subcomponent. If a commit touches two scopes, pick the primary one.

```
Good: feat(backend): add token validation middleware
Bad:  feat(backend/auth/jwt): add token validation middleware
```

### Atomicity and Incidental Commits

Each commit should represent one logical change. Discovering mid-implementation that you need to update a Makefile or adjust a config file is normal. Write a `chore` commit for it on the same branch — this is an **incidental commit**, a small unplanned-but-necessary change that does not need its own issue or branch.

The branch type does not constrain what commit types it contains. A `feat` branch naturally includes `chore`, `test`, and `docs` commits. That is expected.

### Commit Body

Use the body when the reasoning is non-obvious or when future contributors need context. Skip it for straightforward changes.

**Example:**
```
fix(backend): clamp token expiry to 24 hours

Tokens were issued without an upper bound on expiry duration.
A client-supplied value could produce indefinitely valid tokens.
```

---

## Pull Requests

### Title

The PR title becomes the squash commit message on `main`: it is the *permanent record* of what landed. Follow the conventional commit format exactly.

**Pattern:**
```
<type>(<scope>): <description>
```

**Examples:**
```
feat(backend): add JWT authentication
fix(frontend): correct redirect on failed login
ci(infra): add healthcheck to all services
```

Do not include the issue number in the PR title. It belongs in the body.

### Body

`Closes #<issue>` in the body -> the issue closes automatically on merge.

Beyond that, explain the approach taken and flag anything the reviewer should pay particular attention to.

**Example:**
```markdown
Implements HS256 token-based authentication.

Token payload is limited to user ID and role.
Refresh tokens are stored server-side and invalidated on logout.

Closes #42
```

### Living PRs

A PR description should stay accurate for the duration of the review.

- Approach changed -> update the description
- A task in the checklist is done -> tick it
- Something no longer applies -> remove it

A reviewer reading an outdated PR description wastes time and misses context.

### Review Requirements

Every PR to `main` requires the configured number of required approvals (one or two, set in branch protection settings) and all CI checks passing. `main` is protected: no direct pushes, including from maintainers.

### CI

Full CI runs on every PR to `main`. A subset also runs on push for faster feedback while you work:

| Branch prefix | CI on push |
|---------------|-----------|
| `feat/**`, `fix/**`, `chore/**` | Lint, typecheck, unit tests |
| `ci/**`, `infra/**` | Docker build |
| All other prefixes | — (full CI runs on PR to `main`) |

---

## Merging

Merge via **squash merge**. Branch deletion is automatic. The result on `main` is one clean, traceable commit per feature:

```
feat(backend): add JWT authentication (#42)
fix(frontend): correct redirect on failed login (#67)
ci(infra): add healthcheck to all services (#15)
```

Every line maps to a closed issue and links back to a full PR for context.
