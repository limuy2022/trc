import os

os.chdir("..")
l = ["src", "tests", "language"]
for path_ in l:
    for root, dirs, files in os.walk(path_):
        for i in files:
            tmp = os.path.splitext(i)[1]
            if tmp == ".hpp" or tmp == ".cpp" or tmp == ".h" or tmp == ".cppm":
                path = os.path.join(root, i)
                os.system("clang-format -i --style=file %s" % path)

input("ok")
