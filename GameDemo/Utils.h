#pragma once
#include<string>
#include<iostream>
#include <fstream>
#include<stdio.h>
#include<Windows.h>
#include<io.h>
#include<string>
#include<vector>

class Logger
{
public:
	static void debug(std::string);
	static void info(std::string);
	static void warn(std::string);
	static void error(std::string);

};



class FileUtils
{
public:
	static int getFilesCount(std::string path);
	static void getFiles(std::string path, std::vector<std::string>& files);
	static void removeFileInDir(std::string dir);
	static bool isFileExist(std::string filePath);
};

