#!/bin/bash

date >> line_count_history.txt;
find ./src -type f \( -iname \*.h -o -iname \*.c \) |
  xargs sed '/^\s*#/d;/^\s*$/d' |
  grep -v "^\s*//" |
  sed -r ':a; s%(.*)/\*.*\*/%\1%; ta; /\/\*/ !b; N; ba' |
  wc -l >> line_count_history.txt;
tail -n 2 line_count_history.txt;
