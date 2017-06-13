#include "Common.h"
#include "SSTable.h"
#include "BloomFilter.h"
#include "Query.h"

using namespace std;

int main() {
	map<string, unsigned int> index;
	index["name"] = 0;
	index["age"] = 1;
	index["gender"] = 2;

	vector<map<unsigned int, string>> data;
	map<unsigned int, string> mp;
	mp[0] = "James";
	mp[1] = "Kobe";
	mp[2] = "Durant";
	data.push_back(mp);
	mp[0] = "11";
	mp[1] = "22";
	mp[2] = "33";
	data.push_back(mp);
	mp[0] = "male";
	mp[1] = "male";
	mp[2] = "female";
	data.push_back(mp);
	SSTable sst1(string("myTable"), index, data);
	sst1.print();

	index = map<string, unsigned int>{};
	index["grade"] = 0;
	index["pid"] = 1;
	data = vector<map<unsigned int, string>>{};
	mp = map<unsigned int, string>{};
	mp[0] = "95";
	mp[1] = "93";
	mp[2] = "100";
	data.push_back(mp);
	mp[0] = "123123132";
	mp[1] = "454564564";
	mp[2] = "789789789";
	data.push_back(mp);
	SSTable sst2(string("studentTable"), index, data);
	sst2.printAsTable();

	cout << "-------------****************-------------" << endl;

	string fn = "data/sstTest.data";
	std::vector<SSTable> ssts1{ sst1, sst2 };
	writeSSTablesD(fn, ssts1);
	cout << "end" << endl;
	std::vector<SSTable> ssts2{};
	readSSTablesD(fn, ssts2);

	cout << ssts2.size() << endl;
	if (ssts2.size() >= 1)
		ssts2[0].printAsTable();
	return 0;
}
