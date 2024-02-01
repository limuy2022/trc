"""
A easy script for auto generate the locales files
"""

import os
import sys

os.chdir("../locales")
tt = os.listdir(".")
for i in tt:
    os.chdir(i)
    os.chdir("LC_MESSAGES")
    subdirs = os.listdir()
    for j in subdirs:
        tmp = os.path.splitext(j)
        if tmp[1] != ".po":
            continue
        if os.system(f"msgfmt -o {tmp[0]}.mo {j}"):
            print("Cannot generate your locales files.Stop.")
            sys.exit(1)
    os.chdir("../..")
