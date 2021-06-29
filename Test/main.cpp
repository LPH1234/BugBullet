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


using namespace std;
int main() {
	mc * ptr = new mc(1);
	char* str = new char[12];
	sprintf_s(str, 12, "%d", ptr);
	std::cout << str << "  int value:---" << atoi(str) << "---\n";

	srand(12312);
	for (size_t i = 0; i < 5; i++)
	{
		cout << rand() << " ";
	}
	cout << "\n";
	srand(12312);
	for (size_t i = 0; i < 5; i++)
	{
		cout << rand() << " ";
	}
}