#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "json.hh"
#include "begin.hh"

namespace σ
{

using namespace rapidjson;


static void json_decode(ηm &w, Value const &v)
{
  switch (v.GetType())
  {
  case kNullType:   w << ηatom::ηnull; break;
  case kFalseType:  w << false; break;
  case kTrueType:   w << true; break;
  case kStringType: w << v.GetString(); break;
  case kNumberType: if (v.IsInt()) w << v.GetInt(); else w << v.GetDouble(); break;
  case kArrayType:
  {
    ηm r;
    for (let &x : v.GetArray()) json_decode(r, x);
    w << r;
    break;
  }
  case kObjectType:
  {
    ηm r;
    for (let &x : v.GetObject())
    {
      r << ηname{x.name.GetString()};
      json_decode(r, x.value);
    }
    w << r;
    break;
  }
  }
}


static void json_encode(Document::AllocatorType &a, Value &v, ηic &x)
{
  switch (x.t())
  {
  case ηtype::atom:
    switch (x.a())
    {
    case ηatom::ηnull:  v.SetNull(); break;
    case ηatom::ηtrue:  v.SetBool(true); break;
    case ηatom::ηfalse: v.SetBool(false); break;
    }
    break;

  case ηtype::n_int:   v.SetInt(x.i()); break;
  case ηtype::n_float: v.SetFloat(x.f()); break;
  case ηtype::string:
  case ηtype::name:    v.SetString(St{x.cs()}.c_str(), a); break;
  case ηtype::η:
  {
    ηi y = x.η();
    if (y && y.is_n())
    {
      v.SetObject();
      while (y)
      {
        Value k(St{y.n()}.c_str(), a);
        Value z;
        json_encode(a, z, y.next());
        v.AddMember(k, z, a);
        y = y.next().next();
      }
    }
    else
    {
      v.SetArray();
      while (y)
      {
        Value z;
        json_encode(a, z, y);
        v.PushBack(z, a);
        y = y.next();
      }
    }
    break;
  }

  case ηtype::sig:
    v.SetString((Ss{} << x.sig()).str().c_str(), a);
    break;

  case ηtype::int8s:
    v.SetArray();
    for (auto y : x.i8s()) v.PushBack(y, a);
    break;

  case ηtype::int16s:
    v.SetArray();
    for (auto y : x.i16s()) v.PushBack(y, a);
    break;

  case ηtype::int32s:
    v.SetArray();
    for (auto y : x.i32s()) v.PushBack(y, a);
    break;

  case ηtype::int64s:
    v.SetArray();
    for (auto y : x.i64s()) v.PushBack(y, a);
    break;

  case ηtype::float32s:
    v.SetArray();
    for (auto y : x.f32s()) v.PushBack(y, a);
    break;

  case ηtype::float64s:
    v.SetArray();
    for (auto y : x.f64s()) v.PushBack(y, a);
    break;

  default:
    A(0, "json_encode: unknown type " << x.t());
  }
}


St η_to_json(ηic &x)
{
  Document d;
  Value v;
  auto &a = d.GetAllocator();

  json_encode(a, v, x);

  StringBuffer s;
  Writer<StringBuffer> w(s);
  v.Accept(w);
  return s.GetString();
}


ηm json_to_η(St const &s)
{
  Document d;
  d.Parse(s.c_str());
  ηm r;
  json_decode(r, d);
  return r;
}


void πjson(πφ &p)
{
  p .def_spre(">J", [](ηic &x) { return η_to_json(x); })
    .def_spre("<J", [](πi &i, Stc &s)
      { return i << json_to_η(s).all(); });
}


}

#include "end.hh"
