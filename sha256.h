#ifndef SHA256_H
#define SHA256_H

#include <vector>
#include <array>

using namespace std;

typedef unsigned int WORD;

class SHA256 {
	public:
		static string sha_hash(string str);
		static vector<WORD> hexToBytes(const string hex);

	private:

		// Helper functions
		static constexpr WORD ShR(const WORD A, const WORD n);
		static constexpr WORD RotR(const WORD A, const WORD n);

		static constexpr WORD Ch(const WORD X, const WORD Y, const WORD Z);
		static constexpr WORD Maj(const WORD X, const WORD Y, const WORD Z);

		static constexpr WORD capsigma0(const WORD X);
		static constexpr WORD capsigma1(const WORD X);
		static constexpr WORD sigma0(const WORD X);
		static constexpr WORD sigma1(const WORD X);

		static vector<WORD> schedMessage(vector<WORD> block);

		static vector<WORD> padMessage(string msg);
		static vector<vector<WORD>> parseMessage(vector<WORD> paddedMsg);
};	

#endif /* SHA256_H */