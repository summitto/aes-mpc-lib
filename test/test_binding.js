const assert = require("assert");

const AesMpcLib = require("../lib/binding.js");

assert(AesMpcLib.runMpcEncryptedIv, "The expected function is undefined");
assert(AesMpcLib.runMpcPowersOfH, "The expected function is undefined");

assert.throws(AesMpcLib.runMpcPowersOfH, undefined, "runMpcPowersOfH should throw an expection");
assert.throws(AesMpcLib.runMpcEncryptedIv, undefined, "runMpcEncryptedIv should throw an expection");

console.log("Tests passed- everything looks OK!");