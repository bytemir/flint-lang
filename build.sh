clear
set -e
CC="gcc"
CFLAGS="-std=c11 -Wall -Wextra -O2"
TARGET_DIR="build"
OUTPUT="$TARGET_DIR/flint"

mkdir -p "$TARGET_DIR"

echo "Compiling Flint components..."
$CC $CFLAGS src/Codegen.c src/Lexxer.c src/Main.c  -o "$OUTPUT"

echo "Build successful! Executable created at: $OUTPUT"
"$OUTPUT" "$@"