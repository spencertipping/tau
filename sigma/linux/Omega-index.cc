#include <unistd.h>

#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ωp Ωi::pack(τ::u64 offset, τ::u32 size)
{
  A(offset == (offset & 0x0003'ffff'ffff'ffffull), "Ωi::pack: offset overflow: " << offset);
  A(size   == (size   & 0x0000'0000'ffff'ffffull), "Ωi::pack: size overflow: "   << size);

  let b = ubits(size) - 16;
  return size < 65536
    ? u64(size) << 44 | offset
    : u64(b) << 60 | (size - u64(65536ull << b) >> b & 0xffffull) << 44 | offset;
}


Ωi::val Ωi::unpack(Ωp p)
{
  let e = p >> 60;
  return {p & 0x0003'ffff'ffff'ffffull,
          e ? 65536ull + (p >> 44 & 0xffffull) << e : p >> 44};
}


Ωi::Ωi(Stc &path, bool rw, u64 mss)
  : f_(path, rw ? 0644 : 0444),
    mss_(mss),
    rw_(rw),
    prof_add_   (measurement_for(ηm{} << "Ωi" << path << "add")),
    prof_get_   (measurement_for(ηm{} << "Ωi" << path << "get")),
    prof_repack_(measurement_for(ηm{} << "Ωi" << path << "repack")),
    prof_commit_(measurement_for(ηm{} << "Ωi" << path << "commit"))
{
  if (rw)
  {
    auto &f = this->rw();
    if (f.size() < 4096)
    {
      TODO("Ωi write initial header");
    }

    read_fragment_table_();

    hkey_ = u64(getpid()) << 32 ^ intptr_t(this);
    hrev_ = real_hrev();
    write_hkey_();
  }
  else
  {
    u8 magic[8];
    A(ro().read(magic, 8, 0), "Ωi " << path << ": missing magic");
    A(!memcmp(magic, "Ωidx\0\0\1", 8),
      "Ωi " << path << ": illegal magic "
            << magic[0] << magic[1] << magic[2] << magic[3] << magic[4]
            << " " << int(magic[5]) << int(magic[6]) << int(magic[7]));

    read_fragment_table_();

    hkey_ = real_hkey();
    hrev_ = real_hrev();
  }
}


void Ωi::add(key k, u64 offset, u32 size)
{
  let     t(prof_add_->start());
  Ul<Smu> l(stage_mu_);
  stage_[k.high64] = pack(offset, size);
  if (stage_.size() > mss_) commit_();
}


V<Ωi::val> Ωi::get(key k) const
{
  let    t(prof_get_->start());
  V<val> r;

  {
    Sl<Smu> l(stage_mu_);
    if (let i = stage_.find(k.high64); i != stage_.end())
      r.push_back(unpack(i->second));
  }

  {
    Sl<Smu> l(fs_mu_);

  }

  TODO("Ωi::get: search all fragments");
}


V<u64> Ωi::ks() const
{
  V<u64> r;
  r.reserve(fs_.size());
  for (let &[o, s] : fs_) r.push_back(s);
  return r;
}


void Ωi::repack(u64 max_bytes)
{
  let t(prof_repack_->start());
  TODO("Ωi::repack");
}


void Ωi::commit()
{
  let t(prof_commit_->start());
  // append new fragment
  TODO("Ωi::commit");
}


}

#include "../end.hh"
