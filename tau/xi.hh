#ifndef τξ_h
#define τξ_h


#include "arch.hh"


#include "xi/basics.hh"
#include "xi/asm.hh"
#include "xi/theta.hh"
#include "xi/digamma.hh"
#include "xi/debug.hh"
#include "xi/textio.hh"
#include "xi/pi0.hh"


#if τplatform == τplatform_linux
# include "xi/linux/io.hh"
# include "xi/linux/tcp.hh"
# include "xi/linux/xframe.hh"
#elif τplatform == τplatform_wasm
# warning TODO: wasm ξ includes
#elif
# warning no native ξ definitions for this platform
#endif


#endif
