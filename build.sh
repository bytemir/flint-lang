
set -e
echo "=== Cleaning previous build artifacts ==="
make clean
echo "=== Compiling Flint-Lang Pipeline ==="
make build/flint
echo "=== Build Successful! Running Executable ==="
echo "--------------------------------------------"
./build/flint