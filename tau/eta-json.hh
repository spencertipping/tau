#ifndef τηjson_h
#define τηjson_h


#include "eta-i.hh"
#include "eta0o.hh"
#include "eta1o.hh"

#include "begin.hh"

namespace τ
{


// Serialize η to JSON string
St ηjson(η);


// Convert JSON string to η
η0o jsonη(Stvc&);


}

#include "end.hh"


#endif
