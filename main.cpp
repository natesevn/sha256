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

/*
 * OpenSSL HMAC function
 * Returns length of hmac on success, 0 on failure
 * @msg: message to hash
 * @key: key to use
 * @keyLen: length of key
 * @msgLen: length of message
 * @hmac: buffer to store returned mac
 */
int getHmac(unsigned char* msg, unsigned char* key, int keyLen, int msgLen, unsigned char* hmac) {

  	unsigned char* result = new unsigned char[32];
	unsigned int resultLen = 0;
	
	// Get HMAC of msg
  	HMAC(EVP_sha256(), key, keyLen, msg, msgLen, result, &resultLen);

	// Copy result to passed in buffer
	memcpy(hmac, result, resultLen);

	delete[] result;

	return resultLen;
}

/*
 * Verifies passed in HMAC using OpenSSL's implementation
 * Returns true if passed in HMAC is valid for given message and key, false otherwise
 * @msg: message to test
 * @key: key to use
 * @keyLen: length of key
 * @msgLen: length of message
 * @hmac: hmac to verify
 */
bool verifyHmac(unsigned char* msg, unsigned char* key, int keyLen, int msgLen, unsigned char* hmac) {

	unsigned char* testhmac = new unsigned char[32];
	int result = getHmac(msg, key, keyLen, msgLen, testhmac);

	bool isEqual = (memcmp(testhmac, hmac, result) == 0);
	delete[] testhmac;

	return isEqual;
}

/*
 * Encodes string in hex
 * Returns hex-encoded string
 * @msg: string to encode
 */
string textToHex(const string& msg) {
	ostringstream result;
	result << setw(2) << setfill('0') << hex;
	copy(msg.begin(), msg.end(), ostream_iterator<unsigned int>(result));
	
	return result.str();
}

/*
 * HMAC function
 * Returns HMAC 
 * @keyHash: hex-encoded SHA256 hash of key
 * @hexMsg: hex-encoded message
 */
string hmac(const string keyHash, const string hexMsg) {
	vector<WORD> k_ipad(64);
	WORD ipad = 0x36;
	vector<WORD> k_opad(64);
	WORD opad = 0x5c;
	
	// Get byte arrays of password and message
	vector<WORD> passBytes = SHA256::hexToBytes(keyHash);
	// Password is SHA256 hash, so its size is 32 bytes
	// Pad it with 0s to get 64 bytes
	for(int i=32; i<64; i++){
		passBytes.push_back(0);
	}
	vector<WORD> msgBytes = SHA256::hexToBytes(hexMsg);

	// K xor opad
	// K xor ipad
	for(int i=0; i<64; i++) {
		k_ipad[i] = passBytes[i] ^ ipad;
		k_opad[i] = passBytes[i] ^ opad;
	}

	// Inner hash = H((Key^ipad)||text)
	// Concatenates message bytes to key^ipad
	k_ipad.insert(k_ipad.end(), msgBytes.begin(), msgBytes.end());
	// Constructs hexstring 
	stringstream istream;
	for(auto it : k_ipad) {
		istream << setfill ('0') << setw(2) << hex << it;
	}
	string inner_hash = SHA256::sha_hash(istream.str());

	vector<WORD> innerHashByte = SHA256::hexToBytes(inner_hash);

	// Outer hash = H((Key^opad)||inner hash)
	// Concatenates inner hash bytes to key^opad
	k_opad.insert(k_opad.end(), innerHashByte.begin(), innerHashByte.end());
	// Constructs hexstring 
	stringstream ostream;
	for(auto it : k_opad) {
		ostream << setfill ('0') << setw(2) << hex << it;
	}
	string mac = SHA256::sha_hash(ostream.str());

	return mac;
}

int main() {

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
	
	// Get hex values of password hash and message
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