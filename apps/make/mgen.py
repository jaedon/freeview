#!/usr/bin/env python
# -*- coding:utf-8 -*-

'''
@ File : mgen.py
@ Brife : Template Module Generator
@ Date : 2014/05/15
@ Author : jykim2
'''

import os
import sys
import json
#import pysvn		# You should install pysvn below if you want to use this module. 
'''
# Ubuntu
sudo apt-get install python-svn
sudo apt-get install svn-workbench
'''
from pprint import pprint
from subprocess import call

# location list of "module_info.json"
g_InfoLocation = {	"obama":"../octo/service/obama/convention", \
					"umma":"../octo/service/umma/convention"}
					# Add other process...

# location list of each process
g_RootList = {"obama" : "../octo/service/obama", \
			"umma" : "../octo/service/umma"}
				# Add other process...

def GetJsonData(filename, start, end):

	str_data = ''
	bFind = False

	# detect seperator and make json format 
	f = open(filename)
	for line in f.readlines():
		if end in line:
			print("Find end point!!")
			bFind = False
			break
		
		if bFind == True :
			str_data = str_data + line #.strip("\\r\\n")

		if start in line:
			print("Find start point!!")
			bFind = True

	f.close()
#	pprint(str_data)
	
	json_data = json.loads(str_data)
	return json_data


'''
Replace multiple chars
'''

def mReplace(s, chararray, newchararray):
	for a, b in zip(chararray, newchararray):
		s = s.replace(a, b)
	return s

'''
Make template file
'''
def filemaker(process, module, newpath, m_name, p_name):
	
	changeList = [m_name.lower(), m_name.upper(), m_name.capitalize(), p_name.lower(), p_name.upper(), p_name.capitalize()]
	targetList = ["xxx", "XXX", "Xxx", "yyy", "YYY", "Yyy"]

	# /xxx/module_xxx.c ----> /m_name/module_m_name.c
	# 오른쪽에서 /을 기준으로 문자열을 얻어온다음 xxx를 m_name으로 변환하고, 그 것을 다시 yyy인 것을 p_name으로 변환 
	filename = newpath.rsplit("/",1)[1].replace(m_name, "xxx").replace(p_name, "yyy")

	# template file name
	if module == 'null' :
		filepath = g_InfoLocation[process] + "/" + filename
	else :
		filepath = g_InfoLocation[process]+"/"+module+"/"+filename

	# Check 'Makefile' and replace name ('xxx_Makefile' -> 'Makefile')
	temp = newpath.rpartition('/')
	if temp[2].find('Makefile') > 0 :
		newpath = temp[0] + temp[1] + 'Makefile'

	if os.path.exists(newpath):	
		return [False, newpath]

	newfd = open(newpath,"w")	# Create new file 

	# check reference file 
	if os.path.exists(filepath):
		fd = open(filepath)

		for line in fd.readlines():
			line = mReplace(line, targetList, changeList)
			newfd.write(line)

		fd.close()
	else:
		newfd = open(newpath,"w")

	newfd.close()

	return [True, newpath]
'''
Generator
'''
def Generator(process, module, m_name, p_name, d_name):
	
#	print(module, m_name, process)
	addList = []
	#### Get header name from module_info.json and module ####
	location = g_InfoLocation[process]
	path = location+'/module_info.json'

	# Load module info JSON format
	f = open(path)
	
	mod_process = "["+ process +"]"
	json_data = f.read()
	data = json.loads(json_data)
	json_module = data[module]

	f.close()

	if d_name == "NULL" : 
		d_name = m_name

	#### Get module info from header file ####
	bSave = False

	if p_name != "NULL" :
		bPlugIn = True
	else : 
		bPlugIn = False
#	print(bPlugIn)	

	#### Generate dummy files ####

	# Get file list
	gen_list = []

	# Replace keywords
	for item in json_module["base"]["gen"]:
		item = mReplace(item, [mod_process,"[name]", "[d_name]"], [g_RootList[process], m_name, d_name])
		gen_list.append(item)
	
	if bPlugIn == True:
		for item in json_module["plug-in"]["gen"]:
			item = mReplace(item, [mod_process,"[name]","[plugin]","[d_name]"], [g_RootList[process], m_name, p_name, d_name])
			gen_list.append(item)


	# Make files and dirs 
	for item in gen_list:
		if item.endswith("/") == True:	# Directory
			if os.path.exists(item):
				print("\tSkip : %s" % item)
				pass
			else:
				print("\tAdd : %s" % item)
				os.makedirs(item)
				addList.append(item)
		
		else:	# File
			result = filemaker(process, module, item, m_name, p_name)
			if result[0] == True :
				# Add 
				print("\tAdd : %s" % result[1])
				addList.append(result[1])
			else : 
				# Skip
				print("\tSkip : %s" % result[1])
				pass
		
#	pprint(addList)

	# SVN Add
	if len(addList) > 0 :
		ans = raw_input("\nAdd to SVN(not commit)? (y/n) : ")
		print("")
		if ans == 'y' : 
			for item in addList :
				command = 'svn add -N '+ item
				call(command, shell=True)

	return 0

'''
Main
'''
# [0] : ./mnt.py (itself)
# [1] : process (e.g.: obama, umma, rdk...)
# [2] : layer (e.g.: svc, mgr, om, oapi...)
# [3] : name
# [4] : plug-in name (e.g. : ziggo, ntv...)
# [5] : option (-d...)
if __name__ == '__main__':

	if len(sys.argv) <= 3 or len(sys.argv) > 7: 
		print("\nUsage \t ./mgen.py process layer name [plugin_name] [-d directory_name]\n")
		print("\tprocess \t\t\t obama, umma, rdk...")
		print("\tlayer \t\t\t\t svc, mgr, om, oapi...(Non-layer : null)")
		print("\tname \t\t\t\t The name of module creating\n")
		print("\tplug-in_name(optional) \t\t base, ziggo, ntv...")
		print("\t-d directory_name(optional) \t Specify directory name which differ from module name.\n\n")

		print("EXAMPLES:")
		print("\t ./mgen.py obama svc newsi")
		print("\t ./mgen.py obama svc newsi base")
		print("\t ./mgen.py obama svc newsi ziggo")
		print("\t ./mgen.py obama svc pb -d playback")
		print("\t ./mgen.py obama svc pb ziggo -d playback")
		print("\t ./mgen.py obama oapi reclist")
		print("\t ./mgen.py umma null fs\n")
		

	elif len(sys.argv) <= 4 :	# base only
		Generator(sys.argv[1], sys.argv[2], sys.argv[3], "NULL", "NULL")
		print("End!! ")

	elif len(sys.argv) <= 5 :	# with plugin
		Generator(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], "NULL")
		print("End!! ")



	elif len(sys.argv) <= 7 : 
		if sys.argv.count('-d') > 0 :
			idx = sys.argv.index('-d')
			if idx == 5 : 
				Generator(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[idx+1])
				print("End!! ")
			elif idx == 4 : 
				Generator(sys.argv[1], sys.argv[2], sys.argv[3], "NULL", sys.argv[idx+1])
		else : 
			print("Unknown option : " + sys.argv[5])