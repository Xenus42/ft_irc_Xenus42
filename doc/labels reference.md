# GitHub Labels Reference

Labels are split into four namespaces: `type:`, `scope:`, `status:`, and `priority:`. 
Every issue and PR must have at least one label from each of `type:`, `scope:`, and `status:`. 
Priority is optional when urgency is neutral.

---

## `type:` — What kind of work

| Label | Use when |
|-------|----------|
| `type: feat` | New functionality visible in the product |
| `type: fix` | Bug correction |
| `type: refactor` | Code restructured with no behavior change |
| `type: style` | Formatting, linting, Prettier/ESLint passes — zero logic change |
| `type: test` | Adding or updating tests |
| `type: ci` | CI pipelines, GitHub Actions, workflow changes |
| `type: chore` | Dependency updates, config files, tooling maintenance |
| `type: docs` | Documentation only |
| `type: perf` | Performance improvement, particularly game loop and rendering |
| `type: security` | Auth, JWT, OAuth, 2FA, session, input validation |

### Style vs Chore:
- `type:style` is a bulk formatting pass (run Prettier across `game-server/`, apply ESLint auto-fixes).
- `type:chore` is a tooling or dependency change (bump Prisma, update tsconfig). 
A reviewer can skim a `style` PR, nothing in it affects logic.

---

## `scope:` — Where the work lives

Mapped to the team split and workspace structure. One scope per issue or PR; if a change genuinely spans two scopes, pick the primary one and note the secondary in the description.

| Label | Covers |
|-------|--------|
| `scope: frontend` | SvelteKit routes, Svelte components, shadcn-svelte, Vite config |
| `scope: backend` | Server-side services, API routes, hooks.server.ts |
| `scope: auth` | 42 OAuth flow, JWT, TOTP, session injection — complex enough to own its own scope |
| `scope: game` | Phaser engine, Colyseus rooms, game-server/, game logic |
| `scope: db` | Prisma schema, migrations, database client, queries |
| `scope: shared` | `packages/game-shared/` — code used by both game-server and frontend |
| `scope: infra` | Docker, compose.yaml, deployment, .github/ |
| `scope: i18n` | Paraglide translations, `messages/*.json` |

`scope: auth` is kept separate from `scope: backend` because auth issues are security-sensitive, span the frontend/backend boundary, and have a distinct resolution path. Issues in `src/lib/server/auth/` almost always need both the auth owner and a security review.

---

## `status:` — Where in the workflow

All status labels except `status: blocked` are managed automatically by the automation workflows. Do not set them by hand except for blocked.

| Label | Set by | Meaning |
|-------|--------|---------|
| `status: todo` | Issue template on creation | Created, not yet started |
| `status: in progress` | `automation-state-branch` on branch creation | Branch exists, actively being worked on |
| `status: in review` | `automation-state-pr-open` on PR open or ready for review | PR open, waiting for approval |
| `status: done` | `automation-state-pr-merge` on merge | Merged and closed |
| `status: blocked` | Manual | Waiting on an external dependency or another issue to resolve first |

---

## `priority:` — How urgent

Medium priority is the implicit default and gets no label. Labeling everything medium creates noise with no signal.

| Label | Meaning |
|-------|---------|
| `priority: critical` | Blocks the team or breaks the build — address immediately |
| `priority: high` | Should be resolved within the current work cycle |
| `priority: low` | Nice to have, no time pressure |

---

## Full List

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

**Total: 29 labels.**
