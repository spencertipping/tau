#ifndef σlinux_websocket_h
#define σlinux_websocket_h

#include "../../sigma.hh"
#include "../string.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


struct wsdecode
{
  wsdecode() : s(state::header) {}

  wsdecode &operator<<(Stc&);

  bool empty() const { return b.empty(); }


  // Emit the decoded frame
  void emit()
    {
      if (masked)
        for (uN i = 0; i < size; ++i) b[i] ^= maskingKey[i % 4];

      std::cout << "Decoded WebSocket frame: " << std::endl;
      std::cout << "   Fin: " << fin << std::endl;
      std::cout << "   RSV1: " << rsv1 << std::endl;
      std::cout << "   RSV2: " << rsv2 << std::endl;
      std::cout << "   RSV3: " << rsv3 << std::endl;
      std::cout << "   Opcode: " << opcode << std::endl;
      std::cout << "   Masked: " << masked << std::endl;
      std::cout << "   Payload length: " << size << std::endl;
      std::cout << "   Payload: " << b << std::endl;

      // Reset the state for the next frame
      reset();
    }

private:
  enum class state {
    header,
    elen,
    mask,
    payload
  };

  bool fin, rsv1, rsv2, rsv3, masked;

  u8        opcode;
  u64       size;
  u64       elsize;
  Ar<u8, 4> maskingKey;
  St        b;
  state     s;

  void reset()
    { fin = rsv1 = rsv2 = rsv3 = masked = false;
      opcode = 0;
      size = 0;
      s = state::header;
      b.clear(); }

  void parse_header()
    { fin    = b[0] & 0x80;
      rsv1   = b[0] & 0x40;
      rsv2   = b[0] & 0x20;
      rsv3   = b[0] & 0x10;
      opcode = b[0] & 0x0F;
      masked = b[1] & 0x80;
      size   = b[1] & 0x7F;

      if (size == 126)
      { if (b.size() < 4) return;
        elsize = 2;
        s = state::elen; }
      else if (size == 127)
      { if (b.size() < 10) return;
        elsize = 8;
        s = state::elen; }
      else
        s = masked ? state::mask : state::payload;
      b = b.substr(2); }

  // Parse the extended payload length
  void parse_elen()
    { size = 0;
      for (uN i = 0; i < elsize; ++i) size = (size << 8) | u8(b[i]);
      b = b.substr(elsize);
      s = masked ? state::mask : state::payload; }

  // Parse the masking key
  void parse_mask()
    { maskingKey = { u8(b[0]), u8(b[1]), u8(b[2]), u8(b[3]) };
      b = b.substr(4);
      s = state::payload; }
};


inline int main() {
  wsdecode d;
  d << hex_decode("810548656c6c6f");
  d << hex_decode("8103636174");
  return 0;
}


}

#include "../end.hh"

#endif
