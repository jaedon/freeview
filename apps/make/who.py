#!/usr/bin/env python

'''
@file : who.py
@brief : Search the config file which is now loaded.
@Date : 2014. 1. 22.
@author: jykim2
'''

import os

'''
Parsing configfile and making sorted list
'''
def ParseConfig(filename):
	clist = []	# list
	
	f = open(filename, 'r')
	lines = f.readlines()

	for line in lines: 
		if line[0] != "#" and len(line) > 1:
			item = line[:len(line)-1]	# remove "\n"
			clist.append(item)
			
	f.close()
	clist.sort()	# alphabet sorting

	return clist


'''
Comparing two config lists
'''
def Compare(a, b):
	if len(a) != len(b):
		return 0
	
	if a == b:
	#	print("Same!!")
		return 1
	else:
	#	print("Different!!")
		return 0
	
	return 1


'''
Main function
'''	
if __name__ == '__main__':

	count = 0
	target = ParseConfig(".config");
	
	for (path, dir, files) in os.walk("./product_configs"):
		for filename in files:
			cur = ParseConfig("%s/%s" %(path, filename))
# 			print("Open %s/%s" %(path, filename))
			res = Compare(target, cur)
			
			if res == 1:
				print("\n  Current your config is \"%s\"\n" %(filename))
				count = count + 1
				

	if count == 0:
		print("\n  ERROR: Could not find the config that same with \".config\"!!\n")
	elif count > 1:
		print("\n  WARNNING: The number of searched files are more than 1. Please check your config...\n")
