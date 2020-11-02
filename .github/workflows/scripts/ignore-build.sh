#!/bin/bash

delimiter='\|'
ignore_patterns=(
	'^\.github\/workflows\/scripts\/ignore-build\.sh$' \
	'^\.github\/workflows\/rebase\.yml$' \
	'^\.github\/workflows\/sync-labels\.yml$' \
	'^\.github\/CODEOWNERS$' \
	'^\.github\/dependabot\.yml$' \
	'^\.github\/labels\.yml$' \
	'^\.gitattributes$' \
	'^\.gitignore$' \
	'^\.clang-format$' \
	'^LICENSE$' \
	'\.md$'
	'\.asciidoc$'
)
printf -v ignore_patterns "%s$delimiter" "${ignore_patterns[@]}"
ignore_patterns=${ignore_patterns::-2}
result=$(
  git diff --name-only master..HEAD |
  sed -e "/${ignore_patterns}/d" |
  wc -l
)

if [[ $result -ne 0 ]]; then
  exit 1
fi
