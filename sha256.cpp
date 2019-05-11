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

vector<WORD> padMessage(string msg) {

	// String to byte vector
	vector<WORD> byteStr(msg.begin(), msg.end());

	size_t msgLen = byteStr.size() * 8;
	size_t padLen = msgLen;

	// Determine num bits to pad
	while(padLen > 512) {
		padLen = padLen - 512;
	}

	// Padding
	size_t k;
	
	// Figure out how many bytes of 0 to add 
	if(padLen % 512 != 0) {

		// If not enough space for 1-bit + 64-bit msg size, add another block
		if((512 - padLen) < 65) {
			k = 960 - (padLen + 8);
			cout << "hi" << endl;
		} else {
			k = 448 - (padLen + 8);
		}
	} else {
		k = 440;
	}

	// Push 1 followed by 0s
	byteStr.push_back(0x80);
	for(int i=0; i<k/8; i++) {
		byteStr.push_back(0x0);
	}

	// Pad with msg size in 64 bits
	for (int i=1; i<9; ++i) {
		byteStr.push_back(msgLen >> (64 - i * 8));
	}
	
	return byteStr;
}

// Break message into 512-bit blocks
vector<vector<WORD>> parseMessage(vector<WORD> paddedMsg) {
	size_t paddedLen = paddedMsg.size() * 8;
	size_t numBlocks = paddedLen/512;

	vector<vector<WORD>> N;
	vector<WORD> blocks;

	for(int i=0; i<numBlocks; i++) {
		for(int j=0; j<64; j++) {
			blocks.push_back(paddedMsg[j]);
		}
		N.push_back(blocks);
		blocks.clear();
	}
	
	return N;
}


int main() {

	string str = "abc";

	vector<WORD> paddedMsg = padMessage(str);

	cout << paddedMsg.size() << endl;
	cout << "Pad ok" << endl;

	vector<vector<WORD>> N = parseMessage(paddedMsg);

	cout << N.size() << endl;
	cout << "Split to block ok" << endl;
}