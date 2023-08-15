#include "mpc.h"

#include <sstream>

#include <emp-tool/emp-tool.h>
#include <emp-agmpc/emp-agmpc.h>
#include <emp-agmpc/flexible_input_output.h>

using namespace std;
using namespace emp;
using namespace mpc;

#define POH_SIZE 200
#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 128

void util::hex_to_byte_array(const string& hex, unsigned char* output)
{
	assert(hex.length() % 2 == 0);

	for (int i = 0; i < hex.length(); i = i + 2) {
		// string hexa =  +  + ;
		unsigned short temp;
		stringstream ss;
		ss << "0x" << hex.at(i) << hex.at(i + 1);
		ss >> std::hex >> temp;
		output[i / 2] = temp;
	}
}

// NOTE: also reverses endianness
void util::bytes_to_bits(bool* customInputParsed,  unsigned char* customInput, int customInputSize) {
	for (int i = 0; i < customInputSize/8; i++) {
		int w = customInput[customInputSize/8 - 1 - i];
		for (int j = 0; j < 8; j++) {
			customInputParsed[i * 8 + j] = w & 1 ? true : false;
			w >>= 1;
		}
	}
}

string run_gcm_powers_of_h(int party, const char* serverAddress, int port, const string& circuit_dir, unsigned char *aes_key_share, unsigned char *powers_of_h_200_mask) {
	string xor128circuit = circuit_dir + "/xor128.txt";
	string gcmcircuit = circuit_dir + "/gcm_shares_200.txt";
	string xor12800circuit = circuit_dir + "/xor25600.txt";

	PRG prg;
	bool delta[AES_KEY_SIZE];
	prg.random_bool(delta, AES_KEY_SIZE);
	auto io = NetIOMP<NUM_PARTIES>(serverAddress, party, port);
	auto io2 = NetIOMP<NUM_PARTIES>(serverAddress, party, port+2);

	NetIOMP<NUM_PARTIES> *ios[2] = {&io, &io2};
	ThreadPool pool(2*(NUM_PARTIES-1)+2);

	// Cannot be BristolFashion because CMPC only accepts BristolFormat
	BristolFormat cf(xor128circuit.c_str());

	CMPC<NUM_PARTIES>* mpc = new CMPC<NUM_PARTIES>(ios, &pool, party, &cf, delta);
	ios[0]->flush();
	ios[1]->flush();

	mpc->function_independent();
	ios[0]->flush();
	ios[1]->flush();

	mpc->function_dependent();
	ios[0]->flush();
	ios[1]->flush();

	FlexIn<NUM_PARTIES> in(cf.n1 + cf.n2, party);

	// When computing the GCM shares, Alice and Bob will each have part of the AES key, and therefore also both have part of h
	// For a general overview of the code which enables flexible in/output, see: https://github.com/n-for-1-auth/emp-agmpc-flex-in-out/commit/c27a62db81428e16ca09f2f39019da935cb0d110#diff-9b54ab53bb856c16948f54588c715dc8a53fd3cf0fc2b4fc00fd697a92782f34R102
	// In the following, we use two circuits. First, Alice and Bob XOR their secret shares in 2PC to get secret shares of the AES key which EMP can work with

	for (int i = 0; i < cf.n1; i++) {
		in.assign_party(i, ALICE); // only Alice assigns
	}
	for (int i = 0; i < cf.n2; i++) {
		in.assign_party(cf.n1 + i, BOB); // only Bob assigns
	}

	FlexOut<NUM_PARTIES> out(cf.n3, party);
	for (int i = 0; i < cf.n3; i++) {
		out.assign_party(i, -1); // both parties can access only secret shares of this intermediary output
	}

	bool aesKeyBits[AES_KEY_SIZE];
	util::bytes_to_bits(aesKeyBits, aes_key_share, AES_KEY_SIZE);
	if (party == ALICE) {
		for (auto i = 0; i < AES_KEY_SIZE; i++) {
			in.assign_plaintext_bit(AES_KEY_SIZE - 1 - i, aesKeyBits[i]);
		}
	} else if (party == BOB) {
		for (auto i = 0; i < AES_KEY_SIZE; i++) {
			in.assign_plaintext_bit(AES_KEY_SIZE + AES_KEY_SIZE - 1 - i, aesKeyBits[i]);
		}
	}

	mpc->online(&in, &out);
	ios[0]->flush();
	ios[1]->flush();

	BristolFormat cf2(gcmcircuit.c_str());

	CMPC<NUM_PARTIES>* mpc2 = new CMPC<NUM_PARTIES>(ios, &pool, party, &cf2, delta);
	ios[0]->flush();
	ios[1]->flush();

	mpc2->function_independent();
	ios[0]->flush();
	ios[1]->flush();

	mpc2->function_dependent();
	ios[0]->flush();
	ios[1]->flush();

	FlexIn<NUM_PARTIES> in2(cf2.n1 + cf2.n2, party);
	for (int i = 0; i < cf2.n1+cf2.n2; i++) {
		in2.assign_party(i, -1);
	}

	FlexOut<NUM_PARTIES> out2(cf2.n3, party);
	for (int i = 0; i < cf2.n3; i++) {
		out2.assign_party(i, -1);
	}

	for (int i = 0; i < cf2.n1+cf2.n2; i++) {
		AuthBitShare<NUM_PARTIES> mabit = out.get_authenticated_bitshare(i);
		in2.assign_authenticated_bitshare(cf2.n1+cf2.n2 - 1 - i, &mabit);
	}

	mpc2->online(&in2, &out2);
	ios[0]->flush();
	ios[1]->flush();

	BristolFormat cf3(xor12800circuit.c_str());
	CMPC<NUM_PARTIES>* mpc3 = new CMPC<NUM_PARTIES>(ios, &pool, party, &cf3, delta);
	ios[0]->flush();
	ios[1]->flush();

	mpc3->function_independent();
	ios[0]->flush();
	ios[1]->flush();

	mpc3->function_dependent();
	ios[0]->flush();
	ios[1]->flush();

	FlexIn<NUM_PARTIES> in3(cf3.n1 + cf3.n2, party);
	for (int i = 0; i < cf3.n1; i++) {
		in3.assign_party(i, -1);
	}
	for (int i = 0; i < cf3.n2; i++) {
		in3.assign_party(cf3.n1 + i, ALICE);
	}

	FlexOut<NUM_PARTIES> out3(cf3.n3, party);
	for (int i = 0; i < cf3.n3; i++) {
		out3.assign_party(i, BOB);
	}

	for (int i = 0; i < cf3.n1; i++) {
		AuthBitShare<NUM_PARTIES> mabit = out2.get_authenticated_bitshare(i);
		in3.assign_authenticated_bitshare(i, &mabit);
	}

	if (party == ALICE) {
		bool maskBits[AES_KEY_SIZE*POH_SIZE];
		util::bytes_to_bits(maskBits, powers_of_h_200_mask, AES_KEY_SIZE*POH_SIZE);
		for (auto i = 0; i < AES_KEY_SIZE*POH_SIZE; i++) {
			in3.assign_plaintext_bit(cf3.n1 + AES_KEY_SIZE*POH_SIZE - 1 - i, maskBits[i]);
		}
	}

	mpc3->online(&in3, &out3);
	ios[0]->flush();
	ios[1]->flush();


	stringstream ss;
	if (party == BOB) {
		ofstream o;

		string outbitstr;
		for (auto i = 0; i < cf3.n3; i++) {
			outbitstr += out3.get_plaintext_bit(i) ? "1" : "0";
		}
		for (auto i = 0; i < outbitstr.length() / AES_KEY_SIZE; i++) {
			auto outbitstr2 = outbitstr.substr(AES_KEY_SIZE*i, AES_KEY_SIZE);
			std::string outbitstr3{""};
			for (auto j = 0; j < AES_KEY_SIZE / 8; j++) {
				outbitstr3 += outbitstr2.substr(AES_KEY_SIZE - 8 - 8*j, 8);
			}
			ss << outbitstr3 << std::endl;
		}
	}

	delete mpc;
	delete mpc2;
	delete mpc3;

	return string(istreambuf_iterator<char>(ss), {});
}

void mpc::run_gcm_powers_of_h_client(const string& serverAddress, int port, const string& circuit_dir, const string& client_key_share, const string& powers_of_h_mask)
{
	unsigned char raw_aes_key_share[AES_BLOCK_SIZE] = {};
	unsigned char powers_of_h_200_mask[AES_BLOCK_SIZE * POH_SIZE] = {};

	util::hex_to_byte_array(std::move(client_key_share), raw_aes_key_share);
	util::hex_to_byte_array(std::move(powers_of_h_mask), powers_of_h_200_mask);

	run_gcm_powers_of_h(emp::ALICE, serverAddress.c_str(), port, std::move(circuit_dir), raw_aes_key_share, powers_of_h_200_mask);
}

string mpc::run_gcm_powers_of_h_server(int port, const string& circuit_dir, const string& server_key_share)
{
	unsigned char raw_aes_key_share[AES_BLOCK_SIZE] = {};

	util::hex_to_byte_array(std::move(server_key_share), raw_aes_key_share);

	return run_gcm_powers_of_h(emp::BOB, nullptr, port, std::move(circuit_dir), raw_aes_key_share, nullptr);
}

string run_gcm_encrypted_iv(int party, const char* serverAddress, int port, const string& circuit_dir, unsigned char *aes_key_share, unsigned char *IV, unsigned char *tag_mask) {
	string xor128circuit = circuit_dir + "/xor128.txt";
	string aescircuit = circuit_dir + "/aes128_full.txt";

	PRG prg;
	bool delta[AES_KEY_SIZE];
	prg.random_bool(delta, AES_KEY_SIZE);
	auto io = NetIOMP<NUM_PARTIES>(serverAddress, party, port);
	auto io2 = NetIOMP<NUM_PARTIES>(serverAddress, party, port+2);

	NetIOMP<NUM_PARTIES> *ios[2] = {&io, &io2};
	ThreadPool pool(2*(NUM_PARTIES-1)+2);

	// Cannot be BristolFashion because CMPC only accepts BristolFormat
	BristolFormat cf_xor(xor128circuit.c_str());

	CMPC<NUM_PARTIES>* mpc = new CMPC<NUM_PARTIES>(ios, &pool, party, &cf_xor, delta);
	ios[0]->flush();
	ios[1]->flush();

	mpc->function_independent();
	ios[0]->flush();
	ios[1]->flush();

	mpc->function_dependent();
	ios[0]->flush();
	ios[1]->flush();

	FlexIn<NUM_PARTIES> in(cf_xor.n1 + cf_xor.n2, party);

	// When computing the AES shares, Alice and Bob will each have part of the AES key, and therefore also both have part of h
	// For a general overview of the code which enables flexible in/output, see: https://github.com/n-for-1-auth/emp-agmpc-flex-in-out/commit/c27a62db81428e16ca09f2f39019da935cb0d110#diff-9b54ab53bb856c16948f54588c715dc8a53fd3cf0fc2b4fc00fd697a92782f34R102
	// In the following, we use two circuits. First, Alice and Bob XOR their secret shares in 2PC to get secret shares of the AES key which EMP can work with

	for (int i = 0; i < cf_xor.n1; i++) {
		in.assign_party(i, ALICE); // only Alice assigns
	}
	for (int i = 0; i < cf_xor.n2; i++) {
		in.assign_party(cf_xor.n1 + i, BOB); // only Bob assigns
	}

	FlexOut<NUM_PARTIES> out(cf_xor.n3, party);
	for(int i = 0; i < cf_xor.n3; i++) {
		out.assign_party(i, -1); // both parties can access only secret shares of this intermediary output
	}

	bool aesKeyBits[AES_KEY_SIZE];
	util::bytes_to_bits(aesKeyBits, aes_key_share, AES_KEY_SIZE);
	if (party == ALICE) {
		for (auto i = 0; i < AES_KEY_SIZE; i++) {
			in.assign_plaintext_bit(AES_KEY_SIZE - 1 - i, aesKeyBits[i]);
		}
	} else if (party == BOB) {
		for (auto i = 0; i < AES_KEY_SIZE; i++) {
			in.assign_plaintext_bit(AES_KEY_SIZE + AES_KEY_SIZE - 1 - i, aesKeyBits[i]);
		}
	}

	mpc->online(&in, &out);
	ios[0]->flush();
	ios[1]->flush();

	BristolFormat cf_aes(aescircuit.c_str());

	CMPC<NUM_PARTIES>* mpc2 = new CMPC<NUM_PARTIES>(ios, &pool, party, &cf_aes, delta);
	ios[0]->flush();
	ios[1]->flush();

	mpc2->function_independent();
	ios[0]->flush();
	ios[1]->flush();

	mpc2->function_dependent();
	ios[0]->flush();
	ios[1]->flush();

	FlexIn<NUM_PARTIES> in2(cf_aes.n1 + cf_aes.n2, party);
	for (int i = 0; i < AES_KEY_SIZE; i++) {
		in2.assign_party(i, -1); // the secret-shared AES value comes from the previous circuit evaluation above
	}
	for (int i = 0; i < AES_KEY_SIZE; i++) {
		in2.assign_party(AES_KEY_SIZE + i, 0); // both assign the IV
	}

	FlexOut<NUM_PARTIES> out2(cf_aes.n3, party);
	for (int i = 0; i < cf_aes.n3; i++) {
		out2.assign_party(i, -1);
	}

	for (int i = 0; i < AES_KEY_SIZE; i++) {
		AuthBitShare<NUM_PARTIES> mabit = out.get_authenticated_bitshare(i);
		in2.assign_authenticated_bitshare(AES_KEY_SIZE - 1 - i, &mabit);
	}
	bool ivBits[AES_KEY_SIZE];
	util::bytes_to_bits(ivBits, IV, AES_KEY_SIZE);
	for (auto i = 0; i < AES_KEY_SIZE; i++) {
		in2.assign_plaintext_bit(AES_KEY_SIZE + i, ivBits[i]);
	}

	mpc2->online(&in2, &out2);
	ios[0]->flush();
	ios[1]->flush();

	BristolFormat cf3(xor128circuit.c_str());
	CMPC<NUM_PARTIES>* mpc3 = new CMPC<NUM_PARTIES>(ios, &pool, party, &cf3, delta);
	ios[0]->flush();
	ios[1]->flush();

	mpc3->function_independent();
	ios[0]->flush();
	ios[1]->flush();

	mpc3->function_dependent();
	ios[0]->flush();
	ios[1]->flush();

	FlexIn<NUM_PARTIES> in3(cf3.n1 + cf3.n2, party);
	for (int i = 0; i < cf3.n1; i++) {
		in3.assign_party(i, -1);
	}
	for (int i = 0; i < cf3.n2; i++) {
		in3.assign_party(cf3.n1 + i, ALICE);
	}

	FlexOut<NUM_PARTIES> out3(cf3.n3, party);
	for (int i = 0; i < cf3.n3; i++) {
		out3.assign_party(i, BOB);
	}

	// note, this weird bit ordering perhaps comes from a symmetrically weird bit ordering earlier
	for (int i = 0; i < cf3.n1/8; i++) {
		for (auto j = 0; j < 8; j++) {
			AuthBitShare<NUM_PARTIES> mabit = out2.get_authenticated_bitshare(8*i + j);
			in3.assign_authenticated_bitshare(8*i + 7 - j, &mabit);
		}
	}

	if (party == ALICE) {
		bool maskBits[AES_KEY_SIZE];
		util::bytes_to_bits(maskBits, tag_mask, AES_KEY_SIZE);
		for (auto i = 0; i < AES_KEY_SIZE; i++) {
			in3.assign_plaintext_bit(cf3.n1 + AES_KEY_SIZE - 1 - i, maskBits[i]);
		}
	}

	mpc3->online(&in3, &out3);
	ios[0]->flush();
	ios[1]->flush();

	string output = "";

	if (party == BOB) {
		for (auto i = 0; i < cf3.n3; i++) {
			output += out3.get_plaintext_bit(i) ? "1" : "0";
		}
	}

	delete mpc;
	delete mpc2;
	delete mpc3;

	return output;
}

void mpc::run_gcm_encrypted_iv_client(const string& serverAddress, int port, const string& circuit_dir, const string& client_key_share, const string& iv, const string& tag_mask_mask)
{
	unsigned char aes_key_share[AES_BLOCK_SIZE] = {};
	unsigned char IV[AES_BLOCK_SIZE] = {};
	unsigned char tag_mask[AES_BLOCK_SIZE] = {};

	util::hex_to_byte_array(std::move(client_key_share), aes_key_share);
	util::hex_to_byte_array(std::move(iv), IV);
	util::hex_to_byte_array(std::move(tag_mask_mask), tag_mask);

	run_gcm_encrypted_iv(emp::ALICE, serverAddress.c_str(), port, std::move(circuit_dir), aes_key_share, IV, tag_mask);
}

string mpc::run_gcm_encrypted_iv_server(int port, const string& circuit_dir, const string& server_key_share, const string& iv)
{
	unsigned char aes_key_share[AES_BLOCK_SIZE] = {};
	unsigned char IV[AES_BLOCK_SIZE] = {};

	util::hex_to_byte_array(std::move(server_key_share), aes_key_share);
	util::hex_to_byte_array(std::move(iv), IV);

	return run_gcm_encrypted_iv(emp::BOB, nullptr, port, std::move(circuit_dir), aes_key_share, IV, nullptr);
}

