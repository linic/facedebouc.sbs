# LINIC.md

This file provides guidance to Linic. When working in this directory.

## Renaming sessions

Use `/rename facedebouc.sbs_ricette` when working on files in specific directories.

## Resuming sessions

Use `claude --resume "facedebouc.sbs_ricette"` to resume a session.

**messages length** each prompt to `claude code` has an array of `messages`. The longer the discusion, the longer the array, the more costly a prompt becomes. We need to send all messages because the claude API is stateless and the context has to be recreated on each API call. Think about this while using claude to maintain performance.
