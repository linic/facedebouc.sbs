# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this directory.
Continue reading /home/code/mes-repertoires-git/collaboration/COLLAB.md for context about how to work with linic (Nic).

## About this repo

Static HTML website published via GitHub Pages at https://facedebouc.sbs.

- No build system or framework — plain HTML files
- Multilingual content: French, Italian, and English
- `index.html` is maintained manually: links to new pages must be added by hand
- Each topic is a subdirectory (e.g. `debian/`, `rust/`, `ricette/`) containing `.html` files

## Workflow: from raw notes to published page

When a `.md` file exists in a topic subdirectory with raw notes or a conversation transcript:

1. **Rework** the `.md` into a structured format: keep only key elements, organized as *experience → result → explanation*.
2. **Validate** — linic reviews the reworked `.md` and confirms or adjusts.
3. **Convert** the `.md` to an equivalent `.html` page (same subdirectory, same filename).
4. **Link** — add an entry for the new page in `index.html`.
5. **Commit and push** to the branch currently checked out locally once linic agrees with the content.

## Content conventions

**Quantities:** Use digits and fractions, not words (e.g. `1½` not `1 tazza e mezzo`, `½` not `mezza`).
- Use `½` and `¼` unicode characters.
- For all other fractions, use digits and `/` (e.g. `3/4`, `1/8`, `2/3`).
