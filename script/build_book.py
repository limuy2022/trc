import os

os.chdir("../docs")
for i in os.listdir("."):
    if i.startswith("book"):
        os.chdir(i)
        os.system("mdbook build")
        os.chdir("..")
