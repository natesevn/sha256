#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <sha256.h>

using namespace std;

const WORD K[] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

constexpr WORD SHA256::ShR(const WORD A, const WORD n) {
	return A >> n;
}

constexpr WORD SHA256::RotR(const WORD A, const WORD n) {
	return (A >> n) | (A << (32 - n));
}

constexpr WORD SHA256::Ch(const WORD X, const WORD Y, const WORD Z) {
	return (X & Y) ^ (~X & Z);
}

constexpr WORD SHA256::Maj(const WORD X, const WORD Y, const WORD Z) {
	return (X & Y) ^ (X & Z) ^ (Y & Z);
}

constexpr WORD SHA256::capsigma0(const WORD X) {
	return RotR(X, 2) ^ RotR(X, 13) ^ RotR(X, 22);
}

constexpr WORD SHA256::capsigma1(const WORD X) {
	return RotR(X, 6) ^ RotR(X, 11) ^ RotR(X, 25);
}

constexpr WORD SHA256::sigma0(const WORD X) {
	return RotR(X, 7) ^ RotR(X, 18) ^ ShR(X, 3);
}

constexpr WORD SHA256::sigma1(const WORD X) {
	return RotR(X, 17) ^ RotR(X, 19) ^ ShR(X, 10);
}

vector<WORD> SHA256::schedMessage(vector<WORD> block) {
	vector<WORD> W(64);

	for(int i=0; i<16; i++) {
		W[i] = block[i];
	}

	for(int i=16; i<64; i++) {
		W[i] = sigma1(W[i-2]) + W[i-7] + sigma0(W[i-15]) + W[i-16];
	}

	return W;
}

vector<WORD> SHA256::padMessage(string msg) {

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
vector<vector<WORD>> SHA256::parseMessage(vector<WORD> paddedMsg) {
	size_t paddedLen = paddedMsg.size() * 8;
	size_t numBlocks = paddedLen/512;
	
	vector<vector<WORD>> N;
	vector<WORD> blocks;

	int msgIndex = 0;
	
	for(int i=0; i<numBlocks; i++) {

		for(int j=0; j<16; j++) {

			// Create 32 bit words from 8 bit parts of message
			WORD word = 0;
			for(int k=msgIndex; k<msgIndex+4; k++) {
				word <<= 8;
				word |= paddedMsg[k];
			}

			msgIndex += 4;
			blocks.push_back(word);
		}

		N.push_back(blocks);
		blocks.clear();
	}
	
	return N;
}

string SHA256::sha_hash(string str) {

	// Prepare string 
	vector<WORD> paddedMsg = padMessage(str);
	vector<vector<WORD>> N = parseMessage(paddedMsg);

	// Initialize H values
	// Updated H
	array<WORD, 8> H_UPD = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	// Intermediate H
	array<WORD, 8> H_INT = {0,0,0,0,0,0,0,0};

	WORD a,b,c,d,e,f,g,h;
	WORD T1, T2;

	for(int i=0; i<N.size(); i++) {

		// Get message schedule
		vector<WORD> W = schedMessage(N[i]);
		
		// Update intermediate H with updated H
		H_INT = H_UPD;

		// Initialize registers with intermediate hash value
		a = H_INT[0];
		b = H_INT[1]; 
		c = H_INT[2];
		d = H_INT[3];
		e = H_INT[4];
		f = H_INT[5];
		g = H_INT[6];
		h = H_INT[7];
		
		// Compression Function
		for(int j=0; j<64; j++) {
			T1 = h + capsigma1(e) + Ch(e,f,g) + K[j] + W[j];
			T2 = capsigma0(a) + Maj(a,b,c);
			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		// Compute the new H values
		H_UPD[0] = H_INT[0] + a;
		H_UPD[1] = H_INT[1] + b;		
		H_UPD[2] = H_INT[2] + c;
		H_UPD[3] = H_INT[3] + d;
		H_UPD[4] = H_INT[4] + e;
		H_UPD[5] = H_INT[5] + f;
		H_UPD[6] = H_INT[6] + g;
		H_UPD[7] = H_INT[7] + h;
	}

	stringstream stream;
	for (auto it : H_UPD) {
		stream << setfill ('0') << setw(8) << hex << it;
	}

	return stream.str();
}