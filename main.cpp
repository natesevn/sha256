#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <sha256.h>

using namespace std;

int main() {

	string str = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

	
	string res = SHA256::sha_hash(str);

	

	cout << "Hash is: " << res << endl;
	
}