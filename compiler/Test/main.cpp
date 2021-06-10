#include<iostream>
#include<vector>

using namespace std;


void split(std::string& s, std::string c, std::vector<std::string>& v) {
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
}

int main() {
	string s = "22/33/44";
	std::vector<std::string> v;
	split(s, "/", v);
	for (int i = 0; i < v.size(); i++)
	{
		string e = v.at(i);
		cout << e.c_str() << endl;
	}
}