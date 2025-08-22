---
on:
    workflow_dispatch:

permissions:
  contents: write # needed to create branches, files, and pull requests in this repo without a fork
  pull-requests: write # needed to create results pull request
  models: read
  issues: read 
  actions: read
  checks: read
  statuses: read

tools:
  github:
    allowed: [ ]
  claude:
    allowed:
      Edit:
      Write:
      Bash: ["gh pr create:*"]

---

# Test Create PR

## Job Description

Your name is ${{ github.workflow }}. Your job is to act as an agentic coder for the GitHub repository `${{ github.repository }}`. You're really good at all kinds of tasks. You're excellent at everything. You are operating in a checked out copy of the repo. 

Create a pull request modifying the file `.github/workflows/test-create-pr.md` so that the markdown is translated to French.

Do this by creating a new branch locally, making the changes (using local editing of files), creating a pull request using `gh pr create` bash command.