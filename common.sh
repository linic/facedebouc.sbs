#!/bin/bash

# Extract title text from an HTML link, removing all tags
extract_title_from_link() {
  local link="$1"
  # Remove <a href="..."> at start, </a><br> at end, and any remaining HTML tags
  echo "$link" | sed 's/<a[^>]*>//; s/<\/a><br>.*//' | sed 's/<[^>]*>//g'
}

# Sort links in a section alphabetically by title
sort_section_links() {
  local section_id="$1"
  local index_file="$2"

  if [[ ! -f "$index_file" ]]; then
    echo "Error: $index_file not found" >&2
    return 1
  fi

  # Create temp file
  local temp_file=$(mktemp)
  local in_section=false
  local section_found=false
  declare -a all_links

  while IFS= read -r line; do
    # Check if entering target section
    if [[ $line =~ \<h1\ id=\"$section_id\" ]]; then
      in_section=true
      section_found=true
      echo "$line" >> "$temp_file"
    # Check if leaving target section
    elif $in_section && [[ $line == "</div>" ]]; then
      in_section=false

      # Sort collected links alphabetically by title
      if [[ ${#all_links[@]} -gt 0 ]]; then
        # Create array of "title|link" pairs, sort, extract links
        IFS=$'\n' sorted_links=($(
          for link in "${all_links[@]}"; do
            title=$(extract_title_from_link "$link")
            echo "$title|$link"
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
    elif $in_section && [[ $line =~ \<a\ href ]]; then
      all_links+=("$line")
    # Output non-link content in section (whitespace, etc.)
    elif $in_section; then
      if [[ -n "$line" ]]; then
        echo "$line" >> "$temp_file"
      fi
    # Output all lines outside target section
    else
      echo "$line" >> "$temp_file"
    fi
  done < "$index_file"

  if ! $section_found; then
    echo "Error: section '$section_id' not found in $index_file" >&2
    rm "$temp_file"
    return 1
  fi

  # Replace original file
  mv "$temp_file" "$index_file"
  return 0
}
