import os
import sys

ANS_PATH = "examples/example_ans/"
ANS_TMP = "ans.tmp"

os.chdir("..")
os.system("cargo build")
for root, dirs, files in os.walk("examples/"):
    for file in files:
        if file.endswith(".trc"):
            if os.system(f"cargo run run {file} > ans.tmp"):
                print("error file on running", file)
                sys.exit(0)
            ans_file = os.path.join(
                ANS_PATH, (os.path.splitext(os.path.split(file)[1]))[0] + ".ans"
            )
            if not os.path.exists(ans_file):
                print("no ans file", ans_file)
                sys.exit(0)
            with open(ans_file, "r", encoding="utf-8") as f:
                if os.system(f"diff {ANS_TMP} {ans_file}"):
                    print("error file on diff {}", file)
                    sys.exit(0)
os.remove(ANS_TMP)
