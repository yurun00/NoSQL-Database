#include "Common.h"
#include "SSTable.h"
#include "Controller.h"
#include "BloomFilter.h"
#include "Query.h"

using namespace std;

int main() {
	srand(time(NULL));
	cout << "Enter the compression method, 'b' for block compression, 'd' for dictionary compression, 'u' for no compression: ";
	while (cin >> compressMethod) {
		if (compressMethod == "b" || compressMethod == "d" || compressMethod == "u")
			break;
		else
			cout << "Invalid method. Please enter again." << endl;
	}

	std::vector<std::string> names{ "Binny", "Bird", "Birdie", "Birgit", "Birgitta",
		"Blair", "Blaire", "Blake", "Blakelee", "Blakeley",
		"Blanca", "Blanch", "Blancha", "Blanche", "Blinni",
		"Blinnie", "Blinny", "Bliss", "Blisse", "Blithe" };

	std::vector<std::string> ids(20);
	for (int i = 0; i < 20; i++) {
		ids[i] = std::to_string(10000 + rand() % 90000);
	}
	
	std::vector<std::string> ages(20);
	for (int i = 0; i < 20; i++) {
		ages[i] = std::to_string(10 + rand() % 5);
	}

	Query qr;

	qr.createTable("student");
	for(int i = 0;i < 19;i++) {
		qr.insert(std::vector<std::string>{"name", "id", "age"},
			std::vector<std::string>{names[i], ids[i], ages[i]}).into("student");
	}

	qr.insert(std::vector<std::string>{"name", "age"},
		std::vector<std::string>{names[19], ages[19]}).into("student");

	qr.select({ "name", "id"}).from("student").where("age", "10").printAsTable();

	cout << "Overall false poisitive error rate of bloom filters: " << float(bfFp) / bfNg << endl;
	cout << bfNg << endl;
	return 0;
}

