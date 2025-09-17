#!/bin/bash
# filepath: run-wony-lit.sh

set -e

# List of test directories
TEST_DIRS=(
  "../llvm/test/CodeGen/Wony"
  "../llvm/test/MC/Wony"
  "../llvm/test/Transforms/LoadStoreVectorizer/Wony"
  "../llvm/test/Analysis/CostModel/Wony"
  "../clang/test/CodeGen/Wony/"
  "../clang/test/CodeGen/target-data.c"
  "../clang/test/Preprocessor/predefined-arch-macros.c"
)

for dir in "${TEST_DIRS[@]}"; do
  echo "Running llvm-lit -v $dir"
  llvm-lit -v "$dir"
done

