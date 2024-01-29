import os
import sys

print("Finding xmake")
if os.system("xmake --version"):
    print("Cannot find xmake.Installing xmake")
    if sys.platform == "Linux":
        print("Trying wget")
        if not os.system("wget --version"):
            print("Using wget to install xmake")
            if os.system("wget https://xmake.io/shget.text -O - | bash"):
                print("Error!Installing failed!Stop")
                exit(1)
        elif not os.system("curl --version"):
            print("Using curl to install xmake")
            if os.system("curl -fsSL https://xmake.io/shget.text | bash"):
                print("Error!Installing failed!Stop")
                exit(1)
    elif sys.platform == "Windows":
        print("Trying winget")
        if os.system("winget install xmake"):
            print("Error!Installing failed!Stop")
            exit(1)

os.chdir("..")
if os.system("xmake") or os.system("xmake install --admin"):
    print("Compiling Error!")
    exit(0)
elif os.system("xmake build unittest") or os.system("xmake run unittest"):
    print("Unittest Error!")
    exit(0)
print("Compile Successfully and pass all unittest cases.You can start to use Trc.")
