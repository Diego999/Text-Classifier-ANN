import os

def writeIndex(dir):
	indexFile = open(dir +"/files.index", "w")

	for file in os.listdir(dir):
		if file.endswith(".txt"):
			indexFile.write(file + "\n")

	indexFile.close()
	
writeIndex("./pos")
writeIndex("./neg")
