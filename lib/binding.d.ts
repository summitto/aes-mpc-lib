
/**
 * @brief Run the client part of GCM powers of H MPC
 *
 * Will try to load xor128.txt, gcm_shares_100.txt, and xor12800.txt from circuitDir
 *
 * @param serverAddress IP address of the MPC server
 * @param port MPC port to connect to on the server
 * @param circuitDir Path to the directory with circuits
 * @param clientKeyShare Client write key share as hex string
 * @param powersOfHMask Powers of H mask as hex string
 */
export function runMpcPowersOfH(serverAddress: string, port: number, circuitDir: string, clientKeyShare: string, powersOfHMask: string): void;

/**
 * @brief Run the client part of GCM encrypted IV MPC
 *
 * Will try to load xor128.txt and aes128_full.txt from circuitDir
 *
 * @param serverAddress IP address of the MPC server
 * @param port MPC port to connect to on the server
 * @param circuitDir Path to the directory with circuits
 * @param clientKeyShare Client write key share as hex string
 * @param iv Tag initialization vector as hex string (shared for client and server)
 * @param tagMaskMask 128 random bits as hex string
 */
export function runMpcEncryptedIv(serverAddress: string, port: number, circuitDir: string, clientKeyShare: string, iv: string, tagMaskMask: string): void;
