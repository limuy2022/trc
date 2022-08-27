import os
import subprocess

os.chdir("../tests/black_test")
testlog = open("testlog.log", "w", encoding="utf-8")

ans_root = "answer"
pro_root = "program"

os.chdir(pro_root)

print("this is the programs files:")
tests = []
for root, dirs, files in os.walk("."):
	for i in files:
		tmp = os.path.join(root, os.path.splitext(i)[0])[2:]
		tests.append(tmp)
		print(tmp)

os.chdir("..")
print("\n\nStarting tests...\n")
for i in tests:
	ans_path = os.path.join(ans_root, i) + ".txt"
	pro_path = f"trc run {os.path.abspath(os.path.join(pro_root, i)) + '.tree'}"
	pipe = subprocess.Popen(pro_path, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	run_res = (pipe.stderr.read() + pipe.stdout.read()).decode("utf-8").replace("\r", '').strip('\n')
	with open(ans_path, "r", encoding="utf-8") as file:
		tmp = file.read().strip('\n')
		if run_res != tmp:
			out_msg = f"""
-------------------------------------------------------------
the result file [{ans_path}] is 
{tmp}
------------------------------
the program out [{pro_path}] is
{run_res}
-------------------------------------------------------------
"""
			testlog.write(out_msg)
			print(out_msg)
testlog.close()
input("Result was written to testlog.txt.")
