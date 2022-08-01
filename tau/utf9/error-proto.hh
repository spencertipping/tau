#ifndef tau_utf9_error_proto_h
#define tau_utf9_error_proto_h


#include <string>


#include "../module/begin.hh"

namespace tau::utf9
{


struct ibuf;
struct tval;
struct val;


template<class T = void> T throw_internal_error(std::string const &);
template<class T = void> T throw_badbyte_error (std::string const &, uint8_t);
template<class T = void> T throw_decoding_error(std::string const &, ibuf const &, uint64_t);
template<class T = void> T throw_top_error     (std::string const &, tval const &);
template<class T = void> T throw_vop_error     (std::string const &, val const &);
template<class T = void> T throw_binop_error   (std::string const &, val const &, val const &);
template<class T = void> T throw_encoding_error(std::string const &, tval const &, val const &);


}

#include "../module/end.hh"


#endif
