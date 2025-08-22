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

# Test de Création de PR

1. Utiliser l'outil `create_issue` pour créer un problème avec le corps "traduire le fichier `.github/workflows/test-create-pr.md` en français"

2. Utiliser l'outil `assign_copilot_to_issue` pour assigner copilot au problème créé à l'étape 1
