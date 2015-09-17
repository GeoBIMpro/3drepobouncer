import os

srcDir='bouncerlib/src'
testDir='test/src'
clientDir='repobouncerclient/src'


def printHeaderForCMakeFiles(file):
	
	file.write('#THIS IS AN AUTOMATICALLY GENERATED FILE - DO NOT OVERWRITE THE CONTENT!\n')
	file.write('#If you need to update the sources/headers/sub directory information, run updateSources.py at project root level\n')
	file.write('#If you need to import an extra library or something clever, do it on the CMakeLists.txt at the root level\n')
	file.write('#If you really need to overwrite this file, be aware that it will be overwritten if updateSources.py is executed.\n\n\n')
	return

def createCMakeList(dirName, files, subDirList, sourceName, headerName):
	cppInd = 0
	hInd = 0
	sources = {}
	headers = {}
	for fname in files:
		if fname.lower().endswith('.cpp'):
			sources[cppInd] = fname
			cppInd+=1
		if fname.lower().endswith('.h'):
			headers[hInd] = fname
			hInd+=1
		
	#populate a CMake file with this info
	
	cmakeFile = open(dirName+"/CMakeLists.txt", 'w')
	cmakeFile.truncate()

	printHeaderForCMakeFiles(cmakeFile)

	#include sub directories
	for subDir in subDirList:
		cmakeFile.write('add_subdirectory(' + subDir + ")\n")

	#write sources
	if len(sources.values()) > 0 :
		cmakeFile.write('set('+sourceName+'\n')
		cmakeFile.write('\t${'+sourceName+'}\n')

		for fname in sources.values():
			cmakeFile.write('\t${CMAKE_CURRENT_SOURCE_DIR}/' + fname + '\n')
		
		cmakeFile.write('\tCACHE STRING "'+sourceName+'" FORCE)\n\n')

	#write headers
	if len(headers.values()) > 0 :
		cmakeFile.write('set('+headerName+'\n')
		cmakeFile.write('\t${'+headerName+'}\n')
	
		for fname in headers.values():
			cmakeFile.write('\t${CMAKE_CURRENT_SOURCE_DIR}/' + fname + '\n')
		
		cmakeFile.write('\tCACHE STRING "'+headerName+'" FORCE)\n\n')

	cmakeFile.close()


	print('Created CMakeLists.txt in %s' %dirName)
	return



for dir, subDirList, fl in os.walk(srcDir):
	createCMakeList(dir, fl, subDirList, "SOURCES", "HEADERS")


for dir, subDirList, fl in os.walk(testDir):
	createCMakeList(dir, fl, subDirList, "TEST_SOURCES", "TEST_HEADERS")

for dir, subDirList, fl in os.walk(clientDir):
	createCMakeList(dir, fl, subDirList, "CLIENT_SOURCES", "CLIENT_HEADERS")
