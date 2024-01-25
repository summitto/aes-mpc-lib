const addon = require('bindings')('aes-mpc-lib-native.node');

/**
 * @brief Run the client part of GCM powers of H MPC
 *
 * Will try to load xor128.txt, gcm_shares_100.txt, and xor12800.txt from circuitDir
 *
 * @param {string} serverAddress IP address of the MPC server
 * @param {number} port MPC port to connect to on the server
 * @param {string} circuitDir Path to the directory with circuits
 * @param {string} clientKeyShare Client write key share as hex string
 * @param {string} powersOfHMask Powers of H mask as hex string
 * @returns {void}
 */
const runMpcPowersOfH = (serverAddress, port, circuitDir, clientKeyShare, powersOfHMask) => addon.RunMpcPowersOfH(serverAddress, port, circuitDir, clientKeyShare, powersOfHMask);

/**
 * @brief Run the client part of GCM encrypted IV MPC
 *
 * Will try to load xor128.txt and aes128_full.txt from circuitDir
 *
 * @param {string} serverAddress IP address of the MPC server
 * @param {number} port MPC port to connect to on the server
 * @param {string} circuitDir Path to the directory with circuits
 * @param {string} clientKeyShare Client write key share as hex string
 * @param {string} iv Tag initialization vector as hex string (shared for client and server)
 * @param {string} tagMaskMask 128 random bits as hex string
 * @returns {void}
 */
const runMpcEncryptedIv = (serverAddress, port, circuitDir, clientKeyShare, iv, tagMaskMask) => addon.RunMpcEncryptedIv(serverAddress, port, circuitDir, clientKeyShare, iv, tagMaskMask);

module.exports = {
  runMpcPowersOfH,
  runMpcEncryptedIv,
}

