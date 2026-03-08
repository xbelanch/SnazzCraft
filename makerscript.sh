ScriptDirectory="$(cd "$(dirname "$0")" && pwd)"

cd "$ScriptDirectory/SnazzCraft"

mkdir -p "build"
mkdir -p "bin"
mkdir -p "worlds"

make all

cd "$ScriptDirectory/Launcher"

#if [ ! -d "pyinstaller-env" ]; then
#    python3 -m venv pyinstaller-env
#fi

#source pyinstaller-env/bin/activate
#pip install --upgrade pip
#pip install pyinstaller

#pyinstaller --hidden-import=tkinter --onefile --name=launcher #src/launcher.py

cd $ScriptDirectory
./Launcher/dist/launcher
