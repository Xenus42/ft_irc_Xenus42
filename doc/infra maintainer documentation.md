# GitHub Repository Setup Reference

Everything the infra person needs to configure: file hierarchy with descriptions, input reliability analysis, logging conventions, scaling strategy, and repository settings.

---

## File Hierarchy (PSEUDO)

```
.github/
├── ISSUE_TEMPLATE/
├── workflows/
│   ├── ci-lint-eslint.yaml
│   ├── ci-lint-prettier.yaml
│   ├── ci-lint-prisma.yaml
│   ├── ci-lint-pr-title.yaml
│   ├── ci-typecheck-frontend.yaml
│   ├── ci-typecheck-game-server.yaml
│   ├── ci-test-unit.yaml
│   ├── ci-test-e2e.yaml
│   ├── ci-test-integration.yaml
│   ├── ci-docker-build.yaml
│   ├── ci-docker-push.yaml
│   ├── automation-label-issue.yaml
│   ├── automation-label-pr.yaml
│   ├── automation-state-branch.yaml
│   └── automation-state-pr.yaml
└── scripts/
    ├── utils/
    │   ├── log.sh
    │   └── log.js
    ├── ci/
    │   ├── ci-lint-eslint.sh
    │   ├── ci-lint-prettier.sh
    │   ├── ci-lint-prisma.sh
    │   ├── ci-typecheck-frontend.sh
    │   ├── ci-typecheck-game-server.sh
    │   ├── ci-test-unit.sh
    │   ├── ci-test-e2e.sh
    │   └── ci-test-integration.sh
    └── automation/
        ├── automation-utils.js
        ├── automation-label-issue.js
        ├── automation-label-pr.js
        ├── automation-state-branch.js
        ├── automation-state-pr-open.js
        └── automation-state-pr-merge.js
.github/
├── pull_request_template.md
└── CODEOWNERS
```

---

## File Descriptions

### `ISSUE_TEMPLATE/`

Each template auto-applies its `type:` label and `status: todo` on creation. Scope labeling is handled separately by `automation-label-issue.yaml` since GitHub cannot natively map a dropdown value to a label.

---

### `workflows/`

Every workflow declares a `concurrency` group scoped to its trigger context (branch name or PR number) with `cancel-in-progress: true`. This cancels an in-progress run when a new commit is pushed to the same branch or PR.

Each workflow file contains **one job doing one thing.** This matches how professional repositories like Vite, SvelteKit, and ESLint structure their CI: every check is independently visible, independently re-runnable, and independently required as a status check.

#### CI Workflows

Read-only permissions. Each workflow sets up Node 24.15.0 and pnpm 10.x via `pnpm/action-setup`, then runs `pnpm install --frozen-lockfile` before invoking its script.

| File                           | Trigger                                                          | Script called                 | Purpose                                                                                                            |
| ------------------------------ | ---------------------------------------------------------------- | ----------------------------- | ------------------------------------------------------------------------------------------------------------------ |
| `ci-lint-eslint.yaml`          | push `feat/**` `fix/**` `chore/**` `ci/**` `infra/**`, PR to `main` | `ci-lint-eslint.sh`           | ESLint check only                                                                                                  |
| `ci-lint-prettier.yaml`        | push `feat/**` `fix/**` `chore/**` `ci/**` `infra/**`, PR to `main` | `ci-lint-prettier.sh`         | Prettier format check only                                                                                         |
| `ci-lint-prisma.yaml`          | push `feat/**` `fix/**` `chore/**` `ci/**` `infra/**`, PR to `main` | `ci-lint-prisma.sh`           | Prisma schema format check only (`prisma format --check`)                                                          |
| `ci-lint-pr-title.yaml`        | PR opened/edited targeting `main`                                | inline regex                  | Validates PR title against conventional commit format. **Required status check.**                                  |
| `ci-typecheck-frontend.yaml`   | push `feat/**` `fix/**` `chore/**` `ci/**` `infra/**`, PR to `main` | `ci-typecheck-frontend.sh`    | `pnpm typecheck` (svelte-check) on SvelteKit source                                                                |
| `ci-typecheck-game-server.yaml`| push `feat/**` `fix/**` `chore/**` `ci/**` `infra/**`, PR to `main` | `ci-typecheck-game-server.sh` | `tsc --noEmit` on `game-server/`                                                                                   |
| `ci-test-unit.yaml`            | push `feat/**` `fix/**` `chore/**`, PR to `main`                 | `ci-test-unit.sh`             | Vitest unit tests                                                                                                  |
| `ci-test-e2e.yaml`             | PR opened/synchronized targeting `main`                          | `ci-test-e2e.sh`              | Playwright E2E against the running stack. PR to `main` only: too slow for branch push. **Required status check.**  |
| `ci-test-integration.yaml`     | PR opened/synchronized targeting `main`                          | `ci-test-integration.sh`      | `compose.yaml up --build`, wait for healthchecks, run integration assertions. **Required status check.**           |
| `ci-docker-build.yaml`         | push `ci/**` `infra/**`, PR to `main`                            | inline                        | Builds both Dockerfiles via `compose.yaml` to verify they compile. No push.                                        |
| `ci-docker-push.yaml`          | push to `main` only                                              | inline                        | Builds and pushes Docker images to registry after merge. Runs post-merge, not a gate.                              |

#### Automation Workflows

Require `issues: write` and `pull-requests: write` permissions. Run concurrently with CI on PR events without conflict since they only modify labels and metadata, not shared compute resources.

| File | Trigger | Scripts called | Purpose |
|------|---------|---------------|---------| 
| `automation-label-issue.yaml` | `issues: opened` | `automation-label-issue.js` | Reads scope dropdown from issue form body, applies `scope:` label |
| `automation-label-pr.yaml` | PR opened/edited | `automation-label-pr.js` | Reads PR title, extracts type and scope, applies matching `type:` and `scope:` labels |
| `automation-state-branch.yaml` | `create` (branch) | `automation-state-branch.js` | Parses branch name `type/NN-slug`, sets `status: in progress` on issue #NN |
| `automation-state-pr.yaml` | PR `opened`, `ready_for_review`, `closed` | `automation-state-pr-open.js` or `automation-state-pr-merge.js` | Updates linked issue status label based on PR event |

`automation-state-pr.yaml` dispatches to one of two scripts based on the event:

| PR event | Script | Status result on linked issue |
|----------|--------|-------------------------------|
| `opened` / `ready_for_review` | `automation-state-pr-open.js` | `status: in review` |
| `closed` (merged) | `automation-state-pr-merge.js` | `status: done` |

---

### Automation Input Reliability

A valid concern: how reliable is it to read issue bodies and PR titles programmatically?

**`automation-label-issue.js` — reliable.**
GitHub's YAML form templates generate a consistent markdown structure in the issue body. The scope dropdown always renders as `### Scope\n\n<value>`. This is produced by GitHub's own renderer, not hand-written by the author. The script parses a known, stable format.

**`automation-label-pr.js` — reliable.**
Reads only the PR title, not the body. The title format `type(scope): description` is enforced by `ci-lint-pr-title.yaml` which runs concurrently. A badly-formatted title will have labels assigned incorrectly for the brief window before the lint check flags it, but it will not merge. By the time the PR is approved, the title is guaranteed well-formed.

**`automation-state-branch.js` — reliable.**
Reads the branch name, which is machine-created following a strict convention. No free-form text parsing.

**`automation-state-pr-open.js` and `automation-state-pr-merge.js` — reliable.**
Linked issues are resolved via the GitHub GraphQL `closingIssuesReferences` query on the PR object. This uses GitHub's own keyword detection engine internally — no regex parsing of the PR body. It handles all closing keyword variants (`Closes`, `Fixes`, `Resolves`, case-insensitive) automatically.

```
gh api graphql -f query='
  query($owner: String!, $repo: String!, $pr: Int!) {
    repository(owner: $owner, name: $repo) {
      pullRequest(number: $pr) {
        closingIssuesReferences(first: 10) {
          nodes { number }
        }
      }
    }
  }'
```

---

### `scripts/`

#### `scripts/utils/`

Cross-cutting logging utilities. Every script sources or imports its respective log utility at the top. This ensures all scripts produce output in the same format and use the same GitHub Actions annotation commands.

**`log.sh`** — sourced by all bash CI scripts. Wraps GitHub Actions workflow commands:

```
log_info    -> ::notice::  [script-name] message
log_warn    -> ::warning:: [script-name] message
log_error   -> ::error::   [script-name] message
log_group   -> ::group::   [script-name] message
log_end     -> ::endgroup::
```

Each script sets `SCRIPT_NAME` at its top before sourcing `log.sh`. This name appears in every log line and in the GitHub Actions annotation panel.

**`log.js`** — imported by all automation JS scripts. Wraps `@actions/core`:

```
log.info(msg)    -> core.info(`[${SCRIPT_NAME}] ${msg}`)
log.warn(msg)    -> core.warning(`[${SCRIPT_NAME}] ${msg}`)
log.error(msg)   -> core.error(`[${SCRIPT_NAME}] ${msg}`)
log.group(msg)   -> core.startGroup(`[${SCRIPT_NAME}] ${msg}`)
log.end()        -> core.endGroup()
```

Same naming contract: each JS script exports or sets `SCRIPT_NAME` before importing `log.js`.

#### `scripts/ci/`

One script per check. Each script: sources `utils/log.sh`, runs its single check, exits 0 on success or 1 on failure with a descriptive `log_error` message before exiting.

| File | Command it wraps |
|------|-----------------|
| `ci-lint-eslint.sh` | `pnpm lint:eslint` |
| `ci-lint-prettier.sh` | `pnpm lint:prettier` |
| `ci-lint-prisma.sh` | `pnpm lint:prisma` (`prisma format --check`) |
| `ci-typecheck-frontend.sh` | `pnpm typecheck` (svelte-check) |
| `ci-typecheck-game-server.sh` | `pnpm --filter game-server check` (tsc) |
| `ci-test-unit.sh` | `pnpm test:unit` (Vitest) |
| `ci-test-e2e.sh` | `pnpm test:e2e` (Playwright) |
| `ci-test-integration.sh` | `docker compose up --build`, healthcheck poll, assertions |

#### `scripts/automation/`

JavaScript files using `@actions/core` and `@actions/github`. Each imports `log.js` and `automation-utils.js`.

**`automation-utils.js`** — shared GitHub API helpers used by all automation scripts: resolve linked issues via GraphQL `closingIssuesReferences`, apply a label, remove a label, check if a label exists.

| File | Responsibility |
|------|---------------|
| `automation-label-issue.js` | Parse scope field from form body, map to `scope:` label |
| `automation-label-pr.js` | Parse PR title with conventional commit regex, apply `type:` and `scope:` labels |
| `automation-state-branch.js` | Extract issue number from branch name, apply `status: in progress` |
| `automation-state-pr-open.js` | Query `closingIssuesReferences`, apply `status: in review` |
| `automation-state-pr-merge.js` | Query `closingIssuesReferences`, apply `status: done` |

---

### Root files

| File | Purpose |
|------|---------| 
| `pull_request_template.md` | Pre-fills PR body with a task checklist and `Closes #` placeholder |
| `CODEOWNERS` | Maps directory paths to GitHub handles for automatic review requests |

---

## Scaling CI

**New check on an existing trigger:**
Add a script in `scripts/ci/`, add a workflow file in `workflows/`. Two files, no changes to anything else. The new workflow appears as an independent check in the GitHub UI immediately.

**New service added to the stack:**
Add its Dockerfile, update `compose.yaml`, add a `ci-typecheck-<service>.yaml` if it has TypeScript, update `ci-test-integration.sh` to assert its healthcheck, update `CODEOWNERS`.

**Path filtering (optional optimization):**
`ci-typecheck-game-server.yaml` only needs to run if `game-server/**` changed. Adding `paths:` filters to individual workflows reduces unnecessary runs as the project grows. Not essential now, useful at scale.

**Benchmark against professional repos:**
Vite, SvelteKit, and ESLint all use the same pattern: one workflow file per concern, each independently visible and re-runnable in the GitHub Actions UI. The "one job per workflow" structure here matches that approach. The separation of `ci-lint-eslint.yaml` from `ci-lint-prettier.yaml` follows ESLint's own CI where linting and formatting are separate status checks.

---

## Repository Settings

### Settings > General

| Setting | Path inside section | Value |
|---------|-------------------|-------|
| Default branch | Default branch | `main` |
| Allow merge commits | Pull Requests | Disabled |
| Allow squash merging | Pull Requests | Enabled |
| Default commit message for squash | Pull Requests > squash | Pull request title |
| Allow rebase merging | Pull Requests | Disabled |
| Always suggest updating pull request branches | Pull Requests | Enabled |
| Automatically delete head branches | Pull Requests | Enabled |

### Settings > Branches > Branch protection rule for `main`

| Setting | Value |
|---------|-------|
| Require a pull request before merging | Enabled |
| Required number of approvals | 1 or 2 (team decision) |
| Dismiss stale reviews when new commits are pushed | Enabled |
| Require review from Code Owners | Enabled |
| Require status checks to pass before merging | Enabled |
| Require branches to be up to date before merging | Enabled |
| Required status checks | `ci-lint-eslint`, `ci-lint-prettier`, `ci-lint-prisma`, `ci-lint-pr-title`, `ci-typecheck-frontend`, `ci-typecheck-game-server`, `ci-test-unit`, `ci-test-e2e`, `ci-test-integration`, `ci-docker-build` |
| Require conversation resolution before merging | Enabled |
| Do not allow bypassing the above settings | Enabled |
| Block force pushes | Enabled |
| Allow deletions | Disabled |

Status check names must match the `name:` field declared at the top of each workflow YAML file. They only appear as selectable in the GitHub UI after the workflows have run at least once on a real PR.

### Settings > Actions > General

| Setting | Path inside section | Value |
|---------|-------------------|-------|
| Actions permissions | Actions permissions | Allow all actions and reusable workflows |
| Workflow permissions | Workflow permissions | Read and write permissions |
| Allow GitHub Actions to create and approve pull requests | Workflow permissions | Disabled |

`ci-*.yaml` workflows override the default at the job level with `permissions: contents: read`. Only `automation-*.yaml` workflows need write access and use the elevated default.

### Settings > Labels

Create all 29 labels before pushing the first branches. The automation workflows depend on these labels existing — applying a label that does not exist is a no-op and will silently fail.

Labels are split into four namespaces: `type:`, `scope:`, `status:`, and `priority:`.

#### `type:` — What kind of work

| Label            | Description                                                     | Suggested color |
| ---------------- | --------------------------------------------------------------- | --------------- |
| `type: feat`     | New functionality visible in the product                        | `#0075ca`       |
| `type: fix`      | Bug correction                                                  | `#d73a4a`       |
| `type: refactor` | Code restructured with no behavior change                       | `#e4e669`       |
| `type: style`    | Formatting, linting, Prettier/ESLint passes — zero logic change | `#fef2c0`       |
| `type: test`     | Adding or updating tests                                        | `#0e8a16`       |
| `type: ci`       | CI pipelines, GitHub Actions, workflow changes                  | `#1d76db`       |
| `type: chore`    | Dependency updates, config files, tooling maintenance           | `#c5def5`       |
| `type: docs`     | Documentation only                                              | `#0075ca`       |
| `type: perf`     | Performance improvement, particularly game loop and rendering   | `#e99695`       |
| `type: security` | Auth, JWT, OAuth, 2FA, session, input validation                | `#b60205`       |

`type: style` vs `type: chore`: style is a bulk formatting pass (run Prettier across `game-server/`, apply ESLint auto-fixes); chore is a tooling or dependency change (bump Prisma, update tsconfig). A reviewer can skim a `style` PR — nothing in it affects logic.

#### `scope:` — Where the work lives

One scope per issue or PR. If a change genuinely spans two scopes, pick the primary one and note the secondary in the description.

| Label             | Covers                                                               | Suggested color |
| ----------------- | -------------------------------------------------------------------- | --------------- |
| `scope: frontend` | SvelteKit routes, Svelte components, shadcn-svelte, Vite config      | `#bfd4f2`       |
| `scope: backend`  | Server-side services, API routes, hooks.server.ts                    | `#bfd4f2`       |
| `scope: auth`     | 42 OAuth flow, JWT, TOTP, session injection                          | `#bfd4f2`       |
| `scope: game`     | Phaser engine, Colyseus rooms, game-server/, game logic              | `#bfd4f2`       |
| `scope: db`       | Prisma schema, migrations, database client, queries                  | `#bfd4f2`       |
| `scope: shared`   | `packages/game-shared/` — code used by both game-server and frontend | `#bfd4f2`       |
| `scope: infra`    | Docker, compose.yaml, deployment, .github/                           | `#bfd4f2`       |
| `scope: i18n`     | Paraglide translations, `messages/*.json`                            | `#bfd4f2`       |

`scope: auth` is kept separate from `scope: backend` because auth issues are security-sensitive, span the frontend/backend boundary, and have a distinct resolution path. Issues in `src/lib/server/auth/` almost always need both the auth owner and a security review.

#### `status:` — Where in the workflow

All status labels except `status: blocked` are set automatically by the automation workflows. Do not set them by hand except for blocked.

| Label                 | Set by                                                    | Meaning                                            | Suggested color |
| --------------------- | --------------------------------------------------------- | -------------------------------------------------- | --------------- |
| `status: todo`        | Issue template on creation                                | Created, not yet started                           | `#ededed`       |
| `status: in progress` | `automation-state-branch` on branch creation              | Branch exists, actively being worked on            | `#fbca04`       |
| `status: in review`   | `automation-state-pr-open` on PR open or ready for review | PR open, waiting for approval                      | `#0075ca`       |
| `status: done`        | `automation-state-pr-merge` on merge                      | Merged and closed                                  | `#0e8a16`       |
| `status: blocked`     | Manual                                                    | Waiting on an external dependency or another issue | `#d93f0b`       |

#### `priority:` — How urgent

Medium priority is the implicit default and gets no label. Labeling everything medium creates noise with no signal.

| Label                | Meaning                                                   | Suggested color |
| -------------------- | --------------------------------------------------------- | --------------- |
| `priority: critical` | Blocks the team or breaks the build — address immediately | `#b60205`       |
| `priority: high`     | Should be resolved within the current work cycle          | `#e99695`       |
| `priority: low`      | Nice to have, no time pressure                            | `#c5def5`       |

#### Full label list (29 total)

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

### Settings > Collaborators and teams

Add each team member with **Write** role. Assign **Admin** to the infra person only.

---

## Setup Order

- [x] Create all 29 labels (see Settings > Labels above) — automation workflows depend on them existing before any issues or branches are created
- [x] Create and push all `.github/` files to `main`
- [x] Push a test branch and open a PR to trigger all CI workflows at least once, so their names register in GitHub
- [x] Configure the branch protection rule for `main` — required status checks are only selectable after step 3
- [x] Set Actions > General permissions
- [x] Add collaborators
