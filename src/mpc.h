#ifndef MPCLIB_H
#define MPCLIB_H

#include <string>

using namespace std;

namespace mpc {

namespace util {

/**
 * @brief Converts a hex string into a byte array
 *
 * @param hex hex string
 * @param output output pointer
 */
void hex_to_byte_array(const string& hex, unsigned char* output);

void bytes_to_bits(bool* customInputParsed,  unsigned char* customInput, int customInputSize);

};

static const int NUM_PARTIES = 2;

/**
 * @brief Run the client part of GCM encrypted IV MPC
 *
 * Will try to load xor128.txt and aes128_full.txt from circuit_dir
 *
 * @param serverAddress IP address of the MPC server
 * @param port MPC port to connect to on the server
 * @param circuit_dir Path to the directory with circuits
 * @param client_key_share Client write key share as hex string
 * @param iv Tag initialization vector as hex string (shared for client and server)
 * @param tag_mask_mask 128 random bits as hex string
 */
void run_gcm_encrypted_iv_client(const string& serverAddress, int port, const string& circuit_dir, const string& client_key_share, const string& iv, const string& tag_mask_mask);

/**
 * @brief Run the server part of GCM encrypted IV MPC
 *
 * Will try to load xor128.txt and aes128_full.txt from circuit_dir
 *
 * @param port MPC port to listen to. The next 3 ports must also be free to listen on
 * @param circuit_dir Path to the directory with circuits
 * @param server_key_share Server write key share as hex string
 * @param iv Tag initialization vector as hex string (shared for client and server)
 * @return Tag mask as a string of bits
 */
string run_gcm_encrypted_iv_server(int port, const string& circuit_dir, const string& server_key_share, const string& iv);

/**
 * @brief Run the client part of GCM powers of H MPC
 *
 * Will try to load xor128.txt, gcm_shares_100.txt, and xor12800.txt from circuit_dir
 *
 * @param serverAddress IP address of the MPC server
 * @param port MPC port to connect to on the server
 * @param circuit_dir Path to the directory with circuits
 * @param client_key_share Client write key share as hex string
 * @param powers_of_h_mask Powers of H mask as hex string
 */
void run_gcm_powers_of_h_client(const string& serverAddress, int port, const string& circuit_dir, const string& client_key_share, const string& powers_of_h_mask);

/**
 * @brief Run the server part of GCM powers of H MPC
 *
 * Will try to load xor128.txt, gcm_shares_100.txt, and xor12800.txt from circuit_dir
 *
 * @param port MPC port to listen to. The next 3 ports must also be free to listen on
 * @param circuit_dir Path to the directory with circuits
 * @param server_key_share Server write key share as hex string
 * @return Masked powers of H
 */
string run_gcm_powers_of_h_server(int port, const string& circuit_dir,  const string& server_key_share);

};

#endif // MPCLIB_H
