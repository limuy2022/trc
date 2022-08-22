import os,shutil

os.chdir("..")
shutil.rmtree("doc/html")
os.mkdir("doc/html")
os.system("doxygen")
input("ok")
