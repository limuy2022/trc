import os

os.system("git checkout master")
os.system("git merge dev")
os.system("git push")
os.system("git checkout dev")
input("ok")
