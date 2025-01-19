#!/bin/bash

# Corey Knutson, 1/19/2025


echo "Running integration tests."

# Count number of tests in bash-groups directory

NUM_FILES=$(ls test/integration/bash-groups -1 | wc -l)
FILE_COUNT=1

for t in test/integration/bash-groups/*.sh; do
  echo "Test $FILE_COUNT / $NUM_FILES: $t"
  bash "$t"
  FILE_COUNT=$((FILE_COUNT + 1))
done
