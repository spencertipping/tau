#include "../tau.hh"
#include "begin.hh"

namespace τ
{


// TODO: make a general interface for these types of counters,
// allow optional elision with preprocessor options

static uN ηo_iptrs_ = 0;
static uN ηo_commits_ = 0;
static uN ηo_side_allocs_ = 0;

uN ηo_iptrs()       { return ηo_iptrs_; }
uN ηo_commits()     { return ηo_commits_; }
uN ηo_side_allocs() { return ηo_side_allocs_; }

void ηo_track_iptr()       { ++ηo_iptrs_; }
void ηo_track_commit()     { ++ηo_commits_; }
void ηo_track_side_alloc() { ++ηo_side_allocs_; }


}

#include "end.hh"
