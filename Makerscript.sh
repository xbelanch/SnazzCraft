
cd "SnazzCraft"

mkdir -p "build"
mkdir -p "bin"
mkdir -p "worlds"

make all
./bin/executable

#cd "../Launcher"
#source pyinstaller-env/bin/activate
#pyinstaller --hidden-import=tkinter --onefile src/launcher.py

#./dist/launcher



