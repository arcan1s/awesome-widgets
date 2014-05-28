#!/bin/bash

# create diff
diff -ruN README.md.orig README.md > readme_diff.patch

# update english readme
cp README.md md_readme/en.md

# create html pages
for MARKDOWN in $(find md_readme -name '*.md'); do
  FILENAME=$(basename -- "${MARKDOWN}")
  FILENAME="${FILENAME%.*}"
  markdown "${MARKDOWN}" > "sources/readme/${FILENAME}.html"
done
