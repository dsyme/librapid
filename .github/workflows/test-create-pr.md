---
on:
    workflow_dispatch:

permissions:
  contents: read
  actions: read
  models: read
  issues: write 
  checks: read
  statuses: read

tools:
  github:
    allowed: [ create_issue, assign_copilot_to_issue ]
  claude:
    allowed:
      Edit:
      Write:
      Bash:
      - "git:*"
      - "gh pr create:*"

---

# Test Create PR

1. Use tool `create_issue` to create an issue with body "translate the file `.github/workflows/test-create-pr.md` to French"

2. Use tool `assign_copilot_to_issue` to assign copilot to the issue created in step 1
