---
on:
    workflow_dispatch:

permissions:
  contents: write # needed to create branches, files, and pull requests in this repo without a fork
  pull-requests: write # needed to create results pull request
  actions: write # allow changes to workflow files
  models: read
  issues: read 
  checks: read
  statuses: read

tools:
  github:
    allowed: [ create_pull_request_with_copilot ]
  claude:
    allowed:
      Edit:
      Write:
      Bash:
      - "git:*"
      - "gh pr create:*"

---

# Test Create PR

Use tool `create_pull_request_with_copilot ` to create a Copilot task with task "translate the file `.github/workflows/test-create-pr.md` to French"
