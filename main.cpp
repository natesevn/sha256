#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <iterator>
#include <sha256.h>

using namespace std;

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

bool verifyHmac(string mac, string keyHash, string hexMsg) {
	string testmac = hmac(keyHash, hexMsg);
	if (testmac.compare(mac) == 0)
		return true;

	return false;
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

	string hmac_val = hmac(hashPass, hexMsg);

	if(option_num == 1) {
		cout << "Hash is: " << hmac_val << endl;

	} else if(option_num == 2) {

		cout << "Enter HMAC: ";
		string mymac;
		getline(cin, mymac);

		if(mymac.compare(hmac_val) == 0) {
			cout << "HMAC matches" << endl;
		} else {
			cout << "Invalid HMAC" << endl;
		}

	} else {
		cout << "Invalid option" << endl;
		exit(0);
	}
}	