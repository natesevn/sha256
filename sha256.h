#ifndef SHA256_H
#define SHA256_H

#include <vector>
#include <array>

using namespace std;

typedef uint32_t WORD;

class SHA256 {

	public:
		/*
		 * Takes in a hex-encoded message and hashes it
		 * Returns SHA256 hash in hex
		 * @str: hex-encoded message to hash
		 */
		static string sha_hash(string str);

		/*
		 * Converts hex string to byte vector
		 * Returns byte vector of passed in string
		 * @hex: hex string to convert
		 */
		static vector<WORD> hexToBytes(const string hex);

	private:

		// SHA256 compression function helpers
		static constexpr WORD ShR(const WORD A, const WORD n);
		static constexpr WORD RotR(const WORD A, const WORD n);

		static constexpr WORD Ch(const WORD X, const WORD Y, const WORD Z);
		static constexpr WORD Maj(const WORD X, const WORD Y, const WORD Z);

		static constexpr WORD capsigma0(const WORD X);
		static constexpr WORD capsigma1(const WORD X);
		static constexpr WORD sigma0(const WORD X);
		static constexpr WORD sigma1(const WORD X);

		/*
		 * SHA256 message scheduler
		 * Returns a 64-word array 
		 * @block: block of message bytes to expand
		 */
		static vector<WORD> schedMessage(vector<WORD> block);

		/*
		 * Converts a hex-encoded message string to a vector of bytes and pads it
		 * Returns padded vector of message bytes
		 * @msg: hex-encoded message to convert and pad
		 */
		static vector<WORD> padMessage(string msg);

		/*
		 * Decomposes a vector of padded message bytes into 512-bit blocks
		 * Returns a vector of blocks of padded message bytes
		 * @paddedMsg: bytes of padded message to decompose
		 */
		static vector<vector<WORD>> decomposeMessage(vector<WORD> paddedMsg);
};	

#endif /* SHA256_H */