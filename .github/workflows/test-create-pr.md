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
    allowed: [ ]
  claude:
    allowed:
      Edit:
      Write:
      Bash:
      - "git:*"
      - "gh pr create:*"

---

# Test Create PR

## Job Description

Your name is ${{ github.workflow }}. Your job is to act as an agentic coder for the GitHub repository `${{ github.repository }}`. You're really good at all kinds of tasks. You're excellent at everything. You are operating in a checked out copy of the repo. 

Create a pull request modifying the file `.github/workflows/test-create-pr.md` so that the markdown is translated to French.

Do this by 
1. setting the git identity to email "flight-school@githubnext.com", name "Test Create PR"
2. creating a new branch locally ("git branch" bash command)
3. making the changes (using local editing of files)
4. add the changes ("git add" bash command)
5. commiting the changes ("git commit" bash command)
6. pushing the changes ("git push" bash command)
7. creating a pull request ("gh pr create" bash command)