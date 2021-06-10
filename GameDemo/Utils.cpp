#include "Utils.h"

BOOL SetConsoleColor(WORD wAttributes)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
		return FALSE;

	return SetConsoleTextAttribute(hConsole, wAttributes);
}


void Logger::debug(std::string str) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN); //green
	std::cout << "DEBUG\t" << str << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void Logger::info(std::string str) {
	std::cout << "INFO\t" << str << "\n";
}
void Logger::warn(std::string str) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // yellow
	std::cout << "WARN\t" << str << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void Logger::error(std::string str) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED); // red
	std::cout << "ERROR\t" << str << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}




//计算文件夹的个数
int FileUtils::getFilesCount(std::string path)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	int result = 0;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
				if ((fileinfo.attrib &  _A_SUBDIR)) {
				}
				else {
					result++;
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return result;
}

/*得到文件夹内的文件*/
void FileUtils::getFiles(std::string path, std::vector<std::string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				files.push_back(fileinfo.name);
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


/*
删除文件夹里面的文件
*/
void FileUtils::removeFileInDir(std::string path) {
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				remove(fileinfo.name);
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


bool FileUtils::isFileExist(std::string fileName) {
	std::ifstream inFile;
	inFile.open(fileName, std::ios::in);
	if (inFile) {  //条件成立，则说明文件打开成功
		inFile.close();
		return true;
	}
	return false;
}


void StringUtils::split(std::string& s, std::string c, std::vector<std::string>& v) {
	std::vector<std::string> v;
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
	return v;
}




















