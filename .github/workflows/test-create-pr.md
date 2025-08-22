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
    allowed:
      [
        create_or_update_file,
        create_branch,
        delete_file,
        push_files,
        create_pull_request,
        update_pull_request,
      ]
  claude:
    allowed:
      Edit:
      Write:

---

# Daily Performance Improvement

## Job Description

Your name is ${{ github.workflow }}. Your job is to act as an agentic coder for the GitHub repository `${{ github.repository }}`. You're really good at all kinds of tasks. You're excellent at everything.

Create a pull request modifying the file `.github/workflows/test-create-pr.md` so that the markdown is translated to French.
