#include <msgpack.hpp>
#include <iostream>
#include <string>

int main()
{
  msgpack::sbuffer b;
  msgpack::pack(b, "hi there");

  std::string s(b.data(), b.size());
  std::cout << s << std::endl;
  return 0;
}
