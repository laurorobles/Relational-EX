# Relational EX — GitHub Repository Setup

Recommended GitHub repository name:

`Relational-EX`

Recommended description:

`Experimental relational synthesizer for frequency, rhythm, spectrum and motion.`

## First push

From the project root:

```bash
git init
git branch -M main
git add .
git commit -m "Initial Relational EX development repository"
git remote add origin git@github.com:YOUR_ACCOUNT/Relational-EX.git
git push -u origin main
```

Do not commit local `build/` output or downloaded scale archives that have not been license-reviewed.

## Recommended repository settings

- default branch: `main`
- enable Issues
- enable Discussions when the project becomes public
- require pull-request review for protected main branch
- enable GitHub Actions
- add a release tag for stable versions only

## Versioning

Use semantic versioning while the instrument remains compatible enough to support it:

`MAJOR.MINOR.PATCH`

Development snapshots use a `-dev` suffix until the state format and host compatibility are declared stable.

## Synchronization rule

A feature change is not considered complete until these are updated when relevant:

- source code
- manual
- technical specification
- implementation status
- decision log
- changelog
- third-party notices
