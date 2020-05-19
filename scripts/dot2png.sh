#!/usr/bin/env bash
# Usage: bash ./dot2png <pathToFolderWithDotFiles>

# Captures the directory where the dotfiles are located
TARGET_DIR=${1}

# Glob on all .dot files
EXT=dot
for i in ${TARGET_DIR}/*.${EXT}; do
    INPUT="${i}"
#    Convert the extension of the dot file to a png
    BASE=${i//\.dot/\.png}
    CMD="dot -Tpng ${INPUT} -o ${BASE}"
    echo ${CMD}
    ${CMD}
done