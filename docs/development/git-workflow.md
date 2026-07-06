# Git Workflow
AsterKV uses small, reviewable commits.

## Branch naming
Recommended branch format:
```text
feature/<scope>
fix/<scope>
docs/<scope>
refactor/<scope>
test/<scope>
```

Examples:
```text
feature/bootstrap
feature/protocol-parser
feature/in-memory-storage
```

## Commit format
AsterKV uses conventional commits.

Examples:
```text
feat(project): bootstrap repository structure
feat(storage): add in-memory key-value engine
fix(protocol): reject malformed set command
docs(architecture): describe project layout
test(storage): cover missing key lookup
```

## Step workflow
For every development step:
1. Create or switch to a feature branch.
2. Implement the smallest useful slice.
3. Update documentation.
4. Build.
5. Run tests.
6. Review git diff.
7. Commit with a conventional commit message.

## Bootstrap commit
```bash
git checkout -b feature/bootstrap

git status
git add .
git commit -m "feat(project): bootstrap AsterKV repository structure"
```

## Push
```bash
git push -u origin feature/bootstrap
```

After review or local verification, merge into `main`.
