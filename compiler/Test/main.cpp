#include <vector>
#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>

#include <math.h>

class A {
public:
	int a;
	float b;
	static std::vector<int> vv;
};
std::vector<int> A::vv;

using namespace std;
int main() {

	string str = "111/1009/1 222/1010/1 333/1011/1";
	istringstream s(str);
	string tmpdata[4];
	// f 25/25/25     28/28/28    26/26/26
	s >> tmpdata[0] >> tmpdata[1] >> tmpdata[2] >> tmpdata[3];
	for (size_t i = 0; i < 4; i++)
	{
		cout << "!" << tmpdata[i] << "!  ";
	}

	//  return  hash<int>(classA.getvalue());

	A ca;
	char* tmp = new char[32];
	sprintf_s(tmp, 32, "%ld", &ca);
	cout << "====" << endl;
	cout << "==" << tmp << "==" << endl;
	cout << "==" << atoll(tmp) << "==" << endl;

	A::vv.push_back(1);
}