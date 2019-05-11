#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef uint32_t WORD;

int regularRS() {
	return 0;
}

int circularRS() {
	return 0;
}

int shaFunc() {
	return 0;
}

int schedMessage() {
	return 0;
}

int padMessage() {
	return 0;
}

//break message into 
int parseMessage() {
	return 0;
}


int main() {

	string str = "abc";

	// String to byte vector
	vector<WORD> byteStr(str.begin(), str.end());

	// Padding 
	size_t msgLen = byteStr.size() * 8;

	// Determine num bits to pad
	while(msgLen > 512) {
		msgLen = msgLen - 512;
	}

	// Pad with 1 followed with zeroes to get 448 bits
	if(msgLen % 512 != 0) {
		size_t k = 448 - (msgLen + 8);

		byteStr.push_back(0x80);
		for(int i=0; i<k/8; i++) {
			byteStr.push_back(0x0);
		}
		
	}

	// Pad with msg size in 64 bits
	for (int i=1; i<9; ++i) {
        byteStr.push_back(msgLen >> (64 - i * 8));
    }

	cout << byteStr.size() << endl;
	cout << "Pad ok" << endl;
}