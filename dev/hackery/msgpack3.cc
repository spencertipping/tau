#include <iostream>
#include <map>
#include <sstream>
#include <msgpack.hpp>

int main() {
    // create a map with some data
    std::map<std::string, msgpack::object> data;
    data["foo"] = msgpack::object(std::string("bar"));
    data["bif"] = msgpack::object(3);

    // serialize the map to a buffer
    std::stringstream buffer;
    msgpack::pack(buffer, data);

    // deserialize the buffer back into a map
    msgpack::unpacked result;
    msgpack::unpack(result, buffer.str().data(), buffer.str().size());
    std::map<std::string, msgpack::object> result_data;
    result.get().convert(result_data);

    // print the deserialized map
    for (const auto& [key, val] : result_data) {
        std::cout << key << " => ";
        switch (val.type) {
            case msgpack::type::STR:
                std::cout << val.as<std::string>();
                break;
            case msgpack::type::POSITIVE_INTEGER:
            case msgpack::type::NEGATIVE_INTEGER:
                std::cout << val.as<int>();
                break;
            default:
                std::cout << "unknown type";
                break;
        }
        std::cout << std::endl;
    }

    return 0;
}
