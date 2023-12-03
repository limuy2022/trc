import os

if os.system("git --version"):
    print("Please install git first!")
    exit(1)


def mkdir_noexception(dirname):
    try:
        os.mkdir(dirname)
    except:
        pass


os.chdir("..")
mkdir_noexception("cmake_third_party")
os.chdir("cmake_third_party")
print("download googletest...")
if os.path.exists("googletest"):
    os.chdir("googletest")
    if os.system("git pull"):
        exit(1)
else:
    if os.system("git clone https://github.com/google/googletest --depth=1"):
        exit(1)
print("Requirements are ready!")
