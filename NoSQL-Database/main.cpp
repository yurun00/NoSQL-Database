#include "Common.h"
#include "SSTable.h"
#include "Controller.h"
#include "BloomFilter.h"
#include "Query.h"


using namespace std;

int main() {
	Query qr;
	qr.createTable("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"aa", "a01", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ab", "a02", "14"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ac", "a03", "12"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ad", "a04", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ae", "a05", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"af", "a06", "12"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ag", "a07", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ah", "a08", "15"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ai", "a09", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"aj", "a10", "11"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ak", "a11", "12"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"al", "a12", "12"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"am", "a13", "12"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"an", "a14", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ao", "a15", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ap", "a16", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"aq", "a17", "14"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"ar", "a18", "13"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"as", "a19", "14"}).into("student");
	qr.insert(std::vector<std::string>{"name", "id", "age"},
		std::vector<std::string>{"at", "a20", "13"}).into("student");

	qr.select({ "name" }).from("student").print();

	cout << "Overall false poisitive error rate of bloom filters: " << float(bfFp) / bfNg << endl;

	return 0;
}

