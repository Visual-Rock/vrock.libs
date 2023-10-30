#!/usr/bin/env python3 
import os, sys, shutil

input_path = sys.argv[1]
output_path = sys.argv[2]

print(input_path)
print(output_path)

try:
    shutil.copytree(input_path, output_path, dirs_exist_ok = True)
except OSError as exc:
    print(exc.strerror)

if not os.path.exists(sys.argv[3]):
    os.makedirs(sys.argv[3])