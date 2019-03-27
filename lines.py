# -*- coding: utf-8 -*-

import os
import sys
import re

root = os.getcwd()
shields = ["CMakeFiles", "cmake-build-debug", "jsoncpp-src-0.5.0", ".idea", ".git", "system_libs"]
lines = 0
sizes = 0
same = root

def sizeStr(size):
	t = "B"
	if size >= pow(1024, 4):# TB
		size /= pow(1024, 4)
		t = "TB"
	elif size >= pow(1024, 3):# GB
		size /= pow(1024, 3)
		t = "GB"
	elif size >= pow(1024, 2):# MB
		size /= pow(1024, 2)
		t = "MB"
	elif size >= 1024:# KB
		size /= 1024
		t = "KB"
	return str(size) + t
def toSmaller(path):
	global root

def each(files, root):
	global shields
	global lines
	global sizes
	global same
	for i in files:
		e = True
		path = os.path.join(root, i)
		for j in shields:
			if path.find(j) > -1:
				e = False
				break
		if e:
			if os.path.isdir(path):
				each(os.listdir(path), path)
			else:
				if path.find(".cpp") > -1 or path.find(".h") > -1:
					file = open(path)
					content = file.read()
					line = len(content.split("\n"))
					size = os.path.getsize(path)
					file.close()
					path = re.sub("^" + same, "..", path)
					print("\033[33m" + path + "\033[0m have \033[34m" + str(line) + "\033[0m lines, \033[34m" + sizeStr(size) + "\033[0m.")
					lines += line
					sizes += size

def main():
	print("\033[36mKeep '..'='" + same + "'\033[0m")
	each(os.listdir(root), root)
	print("\033[32mTotal number of lines: \033[35m" + str(lines) + "\033[32m, Total number of size: \033[35m" + sizeStr(sizes) + "\033[0m")
def help():
	print("\033[1m\033[37mBerryMath Data Statistics Tool\033[0m")
	print("\033[1m\033[37mCopyright BerryMathDevelopmentTeam (c) 2019\033[0m")
	print("\033[1m\033[35mUsage:\033[0m \033[34mpython \033[36mlines.py \033[33m[options]\033[0m")
	print("\t\033[1m\033[34mpython \033[36mlines.py\033[0m ================================\033[35mShow datas\033[0m")
	print("\t\033[1m\033[34mpython \033[36mlines.py \033[33m--help\033[0m or \033[1m\033[34mpython \033[36mlines.py \033[33m-h\033[0m ===\033[35mHelp\033[0m")
	print("\t\033[1m\033[34mpython \033[36mlines.py \033[33m--version\033[0m or \033[1m\033[34mpython \033[36mlines.py \033[33m-v\033[0m =======\033[35mShow version\033[0m")
def version():
	print("\033[1m\033[37mBerryMath Data Statistics Tool \033[35mv-1.0.0\033[0m")
	print("\033[1m\033[37mCopyright BerryMathDevelopmentTeam (c) 2019\033[0m")

if len(sys.argv) == 1:
	main()
elif sys.argv[1] == "--help" or sys.argv[1] == "-h":
	help()
elif sys.argv[1] == "--version" or sys.argv[1] == "-v":
	version()
else:
	print("\033[31mWrong flag: '" + sys.argv[1] + "'\033[0m")
	help()
