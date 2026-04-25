#!/bin/bash

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <markdown_file>"
  exit 1
fi

md_file="$1"
html_file="${md_file%.md}.html"

if [[ ! -f "$md_file" ]]; then
  echo "Error: $md_file not found"
  exit 1
fi

# Extract title from first # heading, removing markdown syntax
title=$(grep -m1 "^#" "$md_file" | \
  sed 's/^#\+[[:space:]]*//' | \
  sed 's/`//g; s/\*\*//g; s/\*//g; s/_//g; s/\[//g; s/\]//g')

if [[ -z "$title" ]]; then
  title="Untitled"
fi

# Convert markdown to HTML using pandoc
tmp_body=$(mktemp)
pandoc -f markdown -t html --no-highlight "$md_file" > "$tmp_body"

# Write HTML file with boilerplate
{
  printf '<!DOCTYPE html><html><head><title>%s</title><meta charset="UTF-8"></head>\n<body>\n' "$title"
  cat "$tmp_body"
  printf '</body></html>\n'
} > "$html_file"

rm "$tmp_body"

echo "Converted $md_file to $html_file"
