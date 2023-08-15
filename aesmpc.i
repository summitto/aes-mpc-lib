%module aesmpc

%include "std_string.i"

%{
#include <stdexcept>
#include "src/mpc.h"
%}

%include "exception.i"

// The %exception directive will catch any exception thrown by the C++ library and
// panic() with the same message.
%exception {
  try {
    $action;
  } catch (std::exception &e) {
    _swig_gopanic(e.what());
  }
}

namespace mpc {
  std::string run_gcm_encrypted_iv_server(int port, const std::string& circuit_dir, const std::string& server_key_share, const std::string& iv);
  std::string run_gcm_powers_of_h_server(int port, const std::string& circuit_dir,  const std::string& server_key_share);
}

%rename(run_gcm_encrypted_iv_server) Run_gcm_encrypted_iv_server;
%rename(run_gcm_powers_of_h_server) Run_gcm_powers_of_h_server;

%go_import("fmt")
%insert(go_wrapper) %{
func catch(err *error) {
  if r := recover(); r != nil {
    *err = fmt.Errorf("aesmpc native: %v", r)
  }
}

func RunGcmEncryptedIvServer(port int, circuitDir string, serverKeyShare string, iv string) (tagMask string, err error) {
  defer catch(&err)
  tagMask = Run_gcm_encrypted_iv_server(port, circuitDir, serverKeyShare, iv)
  return
}

func RunGcmPowersOfHServer(port int, circuitDir string, serverKeyShare string) (maskedPowersOfH string, err error) {
  defer catch(&err)
  maskedPowersOfH = Run_gcm_powers_of_h_server(port, circuitDir, serverKeyShare)
  return
}
%}
