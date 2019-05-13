#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <array>
#include <iterator>
#include <sha256.h>

using namespace std;

/* Library HMAC function */
int getHmac(unsigned char* msg, unsigned char* key, int keyLen, int msgLen, unsigned char* hmac) {

  	unsigned char* result = new unsigned char[32];
	unsigned int resultLen = 0;
	
	// Get HMAC of data
  	HMAC(EVP_sha256(), key, keyLen, msg, msgLen, result, &resultLen);

	// Copy result to passed in buffer
	memcpy(hmac, result, resultLen);

	delete[] result;

	return resultLen;
}

bool verifyHmac(unsigned char* msg, unsigned char* key, int keyLen, int msgLen, unsigned char* hmac) {

	unsigned char* testhmac = new unsigned char[32];
	int result = getHmac(msg, key, keyLen, msgLen, testhmac);

	bool isEqual = (memcmp(testhmac, hmac, result) == 0);
	delete[] testhmac;

	return isEqual;
}




string textToHex(const string& msg) {
	ostringstream result;
	result << setw(2) << setfill('0') << hex;
	copy(msg.begin(), msg.end(), ostream_iterator<unsigned int>(result));
	
	return result.str();
}

string hmac(const string keyHash, const string hexMsg) {
	vector<unsigned int> k_ipad(64);
	unsigned int ipad = 0x36;
	vector<unsigned int> k_opad(64);
	unsigned int opad = 0x5c;
	
	// Get byte arrays of password and message
	vector<unsigned int> passBytes = SHA256::hexToBytes(keyHash);
	for(int i=32; i<64; i++){
		passBytes.push_back(0);
	}
	vector<unsigned int> msgBytes = SHA256::hexToBytes(hexMsg);

	// K xor opad
	// K xor ipad
	for(int i=0; i<64; i++) {
		k_ipad[i] = passBytes[i] ^ ipad;
		k_opad[i] = passBytes[i] ^ opad;
	}

	// Inner hash = H((Key^ipad)||text)
	k_ipad.insert(k_ipad.end(), msgBytes.begin(), msgBytes.end());
	stringstream istream;
	for(auto it : k_ipad) {
		istream << setfill ('0') << setw(2) << hex << it;
	}
	string inner_hash = SHA256::sha_hash(istream.str());

	vector<WORD> innerHashByte;
	int str_len = inner_hash.length();
	for(int i=0; i<str_len; i+=2) {
		unsigned char byte = stoi(inner_hash.substr(i, 2), nullptr, 16);
		innerHashByte.push_back(byte);
	}

	// Outer hash = 
	k_opad.insert(k_opad.end(), innerHashByte.begin(), innerHashByte.end());
	stringstream ostream;
	for(auto it : k_opad) {
		ostream << setfill ('0') << setw(2) << hex << it;
	}
	
	string mac = SHA256::sha_hash(ostream.str());

	return mac;
}

int main() {

	// test vectors 
	// string testkey = "554e344537a09659920c19b40f2850b07235c3c7209993a6de905c82db1e5faff148e16f2883ce087c6da219e0bb892d8272c591515b5163bdb0c4ecbd1c7730";
	// string testmsg = "44e9a1f1437791963c1a3e0aaaae24affc3b405844d16a5233b6e5a145c4358b390c305bc4bf585f864f68333dd12d4139a69789105a109e92cc0cf1ff8fe2527891dab4b4fa8731f457574e39f8687fb4969dee7e3af27889590cf8d74415c9e9c0c6867bf0c5146e7c32e306ec7c7055557a0ff738b7e700a70d3e33a975f7";

	cout << "1. Get HMAC" << endl;
	cout << "2. Verify HMAC" << endl;
	cout << "Choose: ";
	string option;
	getline(cin, option);
	int option_num = stoi(option);

	cout << "Enter msg: ";
	string mymsg;
	getline(cin, mymsg);

	cout << "Enter password: ";
	string mypass;
	getline(cin, mypass);

	string hexPass = textToHex(mypass);
	string hashPass = SHA256::sha_hash(hexPass);
	string hexMsg = textToHex(mymsg);

	if(option_num == 1) {
		string hmac_val = hmac(hashPass, hexMsg);
		cout << "Hash is: " << hmac_val << endl;

	} else if(option_num == 2) {

		cout << "Enter HMAC: ";
		string mymac;
		getline(cin, mymac);

		// Convert message string to unsigned char
		unsigned char *testMsg = new unsigned char[mymsg.length()];
		strcpy((char *)testMsg, mymsg.c_str());

		// Convert password hex string to unsigned char
		const char *tempPass = &hashPass[0];
		unsigned char testPass[32];
		for (size_t count = 0; count < sizeof testPass/sizeof *testPass; count++) {
			sscanf(tempPass, "%2hhx", &testPass[count]);
			tempPass += 2;
		}

		// Convert submitted mac hex string to unsigned char
		const char *tempMac = &mymac[0];
		unsigned char testHmac[32];
		for (size_t count = 0; count < sizeof testHmac/sizeof *testHmac; count++) {
			sscanf(tempMac, "%2hhx", &testHmac[count]);
			tempMac += 2;
		}
		
		if(verifyHmac(testMsg, testPass, 32, mymsg.length(), testHmac)) {
			cout << "HMAC verified" << endl;
		} else {
			cout << "Invalid HMAC" << endl;
		};

	} else {
		cout << "Invalid option" << endl;
		exit(0);
	}
}	