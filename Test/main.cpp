#include <vector>
#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <windows.h>
#include<vector>
#include <math.h>
#include <thread>


class mc {
public:
	int *a;
	int id;
	mc(int id) {
		this->id = id;
	}
};

std::vector<mc*> arr;

void replace(std::string& str, char oldChar, char newChar) {
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == oldChar)
			str[i] = newChar;
	}
}

using namespace std;
int main() {
	std::string dir = "images/dir1/dir2/";
	replace(dir, '/', '\\');
	std::cout << dir << "\n";
}