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
	arr.push_back(&mc(1));
	arr.push_back(&mc(2));
	arr.push_back(&mc(3));
	arr.push_back(&mc(4));
	arr.erase(arr.begin() + 2);
	for (size_t i = 0; i < arr.size(); i++)
	{
		std::cout << arr[i]->id << "\n";
	}
}