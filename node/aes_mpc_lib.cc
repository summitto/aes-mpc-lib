#include <napi.h>

#include "../src/mpc.h"

using namespace Napi;

Napi::Value RunMpcPowersOfH(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 5) {
    Napi::TypeError::New(env, "Expected 5 arguments - (serverAddress: string, port: number, circuitDir: string, clientKeyShare: string, powersOfHMask: string)").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Expected 1st argument (serverAddress) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[1].IsNumber()) {
    Napi::TypeError::New(env, "Expected 1st argument (port) to be a number").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[2].IsString()) {
    Napi::TypeError::New(env, "Expected 2nd argument (circuitDir) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[3].IsString()) {
    Napi::TypeError::New(env, "Expected 3rd argument (clientKeyShare) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[4].IsString()) {
    Napi::TypeError::New(env, "Expected 4th argument (powersOfHMask) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string serverAddress = info[0].As<Napi::String>().Utf8Value();
  int port = static_cast<int>(info[1].As<Napi::Number>().Int32Value());
  std::string circuitDir = info[2].As<Napi::String>().Utf8Value();
  std::string clientKeyShare = info[3].As<Napi::String>().Utf8Value();
  std::string power_of_h_mask = info[4].As<Napi::String>().Utf8Value();

  mpc::run_gcm_powers_of_h_client(serverAddress, port, circuitDir, clientKeyShare, power_of_h_mask);
  return env.Null();
}

Napi::Value RunMpcEncryptedIv(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 6) {
    Napi::TypeError::New(env, "Expected 6 arguments - (serverAddress: string, port: number, circuitDir: string, clientKeyShare: string, iv: string, tagMaskMask: string)").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Expected 1nd argument (serverAddress) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[1].IsNumber()) {
    Napi::TypeError::New(env, "Expected 1st argument (port) to be a number").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[2].IsString()) {
    Napi::TypeError::New(env, "Expected 2nd argument (circuitDir) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[3].IsString()) {
    Napi::TypeError::New(env, "Expected 3rd argument (clientKeyShare) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[4].IsString()) {
    Napi::TypeError::New(env, "Expected 4th argument (iv) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[5].IsString()) {
    Napi::TypeError::New(env, "Expected 5th argument (tagMaskMask) to be a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string serverAddress = info[0].As<Napi::String>().Utf8Value();
  int port = static_cast<int>(info[1].As<Napi::Number>().Int32Value());
  std::string circuitDir = info[2].As<Napi::String>().Utf8Value();
  std::string clientKeyShare = info[3].As<Napi::String>().Utf8Value();
  std::string iv = info[4].As<Napi::String>().Utf8Value();
  std::string tagMaskMask = info[5].As<Napi::String>().Utf8Value();

  mpc::run_gcm_encrypted_iv_client(serverAddress, port, circuitDir, clientKeyShare, iv, tagMaskMask);
  return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "RunMpcPowersOfH"),
              Napi::Function::New(env, RunMpcPowersOfH));

  exports.Set(Napi::String::New(env, "RunMpcEncryptedIv"),
              Napi::Function::New(env, RunMpcEncryptedIv));
  return exports;
}

NODE_API_MODULE(addon, Init)
