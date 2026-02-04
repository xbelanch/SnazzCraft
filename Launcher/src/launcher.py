import tkinter
import subprocess

Root = tkinter.Tk()
Root.title("Launcher")
Root.geometry("300x200")

subprocess.run(["./bin/executable"], cwd="../../SnazzCraft", check=True)






