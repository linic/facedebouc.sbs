#!/bin/bash

source "$(dirname "$0")/common.sh"

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <html_file>"
  exit 1
fi

html_file="$1"
index_file="index.html"

if [[ ! -f "$html_file" ]]; then
  echo "Error: $html_file not found"
  exit 1
fi

if [[ ! -f "$index_file" ]]; then
  echo "Error: $index_file not found"
  exit 1
fi

# Extract title from HTML file
title=$(grep -oP '(?<=<title>).*?(?=</title>)' "$html_file")

if [[ -z "$title" ]]; then
  echo "Error: Could not extract title from $html_file"
  exit 1
fi

# Get directory and filename
dir=$(basename "$(dirname "$html_file")")
filename=$(basename "$html_file")

# Check if file is already in index
if grep -q "href=\"\\./$dir/$filename\"" "$index_file"; then
  echo "Error: $filename already exists in index.html"
  exit 1
fi

# Create new link HTML
new_link="<a href=\"./$dir/$filename\">$title</a><br>"

# Create temp file
temp_file=$(mktemp)
in_target_section=false
section_found=false
declare -a all_links

while IFS= read -r line; do
  # Check if entering the target section
  if [[ $line =~ \<h1\ id=\"$dir\" ]]; then
    in_target_section=true
    section_found=true
    echo "$line" >> "$temp_file"
  # Check if leaving the target section
  elif $in_target_section && [[ $line == "</div>" ]]; then
    in_target_section=false

    # Add new link to collection
    all_links+=("$new_link")

    # Sort all links alphabetically by title
    if [[ ${#all_links[@]} -gt 0 ]]; then
      IFS=$'\n' sorted_links=($(
        for link in "${all_links[@]}"; do
          link_title=$(extract_title_from_link "$link")
          echo "$link_title|$link"
        done | sort -f | sed 's/.*|//'
      ))
      unset IFS

      # Output sorted links
      for link in "${sorted_links[@]}"; do
        echo "$link" >> "$temp_file"
      done
    fi

    echo "</div>" >> "$temp_file"
    all_links=()
  # Collect link lines while in section
  elif $in_target_section && [[ $line =~ \<a\ href ]]; then
    all_links+=("$line")
  # Output non-link content in section
  elif $in_target_section; then
    if [[ -n "$line" ]]; then
      echo "$line" >> "$temp_file"
    fi
  # Output all lines outside target section
  else
    echo "$line" >> "$temp_file"
  fi
done < "$index_file"

# If section wasn't found, create it
if ! $section_found; then
  # Find the closing </body></html> and insert new section before it
  sed -i "/<\/body><\/html>/i\\
<h1 id=\"$dir\">$dir</h1><div>\\
$new_link\\
</div>" "$temp_file"
fi

mv "$temp_file" "$index_file"

echo "Added $filename to index.html"
