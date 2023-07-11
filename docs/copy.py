import os, shutil, sys


print(sys.argv[1])
print(sys.argv[2])

shutil.copyfile(sys.argv[1], sys.argv[2])