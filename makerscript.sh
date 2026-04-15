ScriptDirectory="$(cd "$(dirname "$0")" && pwd)"

cd "$ScriptDirectory/SnazzCraft"

mkdir -p "worlds"

make all
./bin/executable
