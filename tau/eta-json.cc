#include "../dep/rapidjson-h/rapidjson.h"
#include "../dep/rapidjson-h/document.h"
#include "../dep/rapidjson-h/stringbuffer.h"
#include "../dep/rapidjson-h/writer.h"

#include "eta-json.hh"
#include "begin.hh"

using namespace rapidjson;

namespace τ
{


static_assert(RAPIDJSON_HAS_CXX11_RVALUE_REFS);


static Value to_json(Document&, η);
static η0o from_json(Value const&);


St ηjson(η y)
{
  Document d;
  Value v = to_json(d, y);

  StringBuffer s;
  Writer<StringBuffer> w(s);
  v.Accept(w);
  return s.GetString();
}

static Value to_json(Document &d, η y)
{
  switch (y.t())
  {
  case η0t::tuple:
  case η0t::set:
  {
    Value v(kArrayType);
    for (let x : y.T()) v.PushBack(to_json(d, x), d.GetAllocator());
    return v;
  }

  case η0t::map:
  {
    Value v(kObjectType);
    for (auto [k, x] : y.M())
      v.AddMember(Value().SetString(Rc<chc*>(k.data()), k.size()),
                  to_json(d, x), d.GetAllocator());
    return v;
  }

  case η0t::boolean:  return Value(y.pb());
  case η0t::uint_be:  return Value(y.pu());
  case η0t::int_be:   return Value(y.pi());
  case η0t::float_be: return Value(y.pf());

  case η0t::bytes:
  case η0t::utf8:
  {
    Value v(kStringType);
    v.SetString(Rc<chc*>(y.data()), y.size());
    return v;
  }

  default:
    return Value(kNullType);
  }
}


η0o jsonη(Stc &j)
{
  Document d; d.Parse(j.c_str());
  return from_json(d);
}

static η0o from_json(Value const &v)
{
  if (v.IsBool())   return η1o(v.GetBool());
  if (v.IsInt64())  return η1o(v.GetInt64());
  if (v.IsDouble()) return η1o(v.GetDouble());
  if (v.IsString()) return η1o(v.GetString());
  if (v.IsNull())   return η1o(η1sig::κ);

  if (v.IsArray())
  {
    η0o x(η0t::tuple);
    for (let &y : v.GetArray()) x << from_json(y);
    return x;
  }

  if (v.IsObject())
  {
    η0o x(η0t::map);
    for (let &m : v.GetObject())
      x << η1o(m.name.GetString()) << from_json(m.value);
    return x;
  }

  A(0, "unknown JSON entity of type " << v.GetType());
  return η1o(η1sig::τ);
}


}

#include "end.hh"
