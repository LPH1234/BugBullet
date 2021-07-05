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
#include <unordered_map>

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

unordered_map<int, mc*> map1;
unordered_map<int, std::string> map2;

int main() {
	std::string dir = "images/dir1/dir2/";
	replace(dir, '/', '\\');
	std::cout << dir << "\n";

	mc* c1 = map1[0];
	cout << (c1 == nullptr) << "\n";


	//std::string s1 = ;
	cout << map2[111].empty() << " :ewae\n";
	system("pause");
}