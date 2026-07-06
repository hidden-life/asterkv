# Git Workflow
AsterKV uses small, reviewable commits and dedicated branches for every development step.

## Rule
A branch must be created before implementation starts.

Do not implement directly on `main`.

## Branch naming
Use one of the following prefixes:
```text
feature/
fix/
docs/
refactor/
test/
build/
ci/
```

Examples:
```text
feature/project-conventions
feature/protocol-parser
feature/in-memory-storage
docs/storage-format
```

## Step workflow
Every development step follows the same sequence:
```bash
git checkout main
git pull --ff-only origin main
git checkout -b feature/<scope>
```

Then:
```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
git status
git diff
```

After verification:
```bash
git add .
git commit -m "<type>(<scope>): <description>"
```

Push the branch:
```bash
git push -u origin feature/<scope>
```

## Commit format
AsterKV uses conventional commits:
```text
<type>(<scope>): <description>
```

Examples:
```text
feat(project): bootstrap repository structure
refactor(core): align naming conventions
docs(development): add coding style guide
test(storage): cover missing key lookup
```

## Main branch
The `main` branch should remain buildable.

Feature branches are merged only after:
- successful build;
- successful tests;
- documentation update;
- reviewed diff.
