/**
	@file 		parser.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		November 7, 2017
	@version 	1.0

	@brief		CMPT-764: Project, Header file for parser.
*/

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include <part_base.h>
#include <part.h>
#include <group.h>

using std::cout;
using std::endl;

using std::string;
using std::vector;

using namespace boost::filesystem;

bool is3DObjectFile (path filePath) {
	return filePath.extension() == ".obj" || filePath.extension() == ".smf";
}

PartBase *loadFileOrFolder (path fileOrFolderPath) {
	PartBase *newPart = NULL;

	if (is_regular_file(fileOrFolderPath) && is3DObjectFile(fileOrFolderPath)) {
		cout << "		Loading File: " << fileOrFolderPath << endl;
		newPart = Part::initPart(fileOrFolderPath.string());
	}

	if (is_directory(fileOrFolderPath)) {
		Group *group = new Group();

		cout << "	Loading Folder: " << fileOrFolderPath << endl;
		for (directory_entry& subDirectory: directory_iterator(fileOrFolderPath)) {
			PartBase *newSubPart = loadFileOrFolder(subDirectory.path());

			if (newSubPart != NULL) {
				group->addMember(newSubPart);
			}
		}

		newPart = group;
	}

	return newPart;
}

vector<PartBase*> loadFiles (string folderPathString) {
	vector<PartBase*> chairs;

	cout << "Loading Folder: " << folderPathString << endl << endl;
	path folderPath(folderPathString);

	try {
		// Check if the path exists
		if (!exists(folderPath)) {
			cout << "Path Does Not Exist!" << endl;
		}

		if (!is_directory(folderPath)) {
			cout << "Path Is Not a Directory!" << endl;
		}

		for (directory_entry& subDirectory : directory_iterator(folderPath)) {
			if (!is_directory(subDirectory)) {
				continue;
			}

			cout << "Loading Chair: " << subDirectory.path().filename() << endl;

			PartBase *newChair = loadFileOrFolder(subDirectory.path());
			chairs.push_back(newChair);

			cout << endl;
		}
	} catch (const filesystem_error& ex) {
		cout << "Filesystem Error! " << ex.what() << endl;
	}

	return chairs;
}

#endif
