#!/bin/bash

source "$(dirname "$0")/common.sh"

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <section_id>"
  exit 1
fi

section_id="$1"
index_file="index.html"

if [[ ! -f "$index_file" ]]; then
  echo "Error: $index_file not found"
  exit 1
fi

sort_section_links "$section_id" "$index_file"

if [[ $? -eq 0 ]]; then
  echo "Sorted section '$section_id' in $index_file"
else
  exit 1
fi
