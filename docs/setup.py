import os, shutil, sys

path = "../builddir/docs/"
pages = "./pages/"

for file in os.listdir(path):
    print(file)
    if os.fsdecode(file).endswith(".rst"):
        if os.fsdecode(file).endswith("index.rst"):
            shutil.copyfile(path + file, "./index.rst")
        elif os.fsdecode(file).endswith("howtouse.rst"):
            shutil.copyfile(path + file, pages + "howtouse.rst")
        else:
            subproj = file.split('_')
            shutil.copyfile(path + file, pages + subproj[0] + "/" + subproj[1])