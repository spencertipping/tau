#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <msgpack.hpp>

int main() {
    // create a map with some data
    std::map<std::string, msgpack::object> data;
    data["array"] = msgpack::object(std::vector<int>{1, 2, 3});
    data["boolean"] = msgpack::object(true);
    data["null"] = msgpack::object();
    data["string"] = msgpack::object("hello world");

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
            case msgpack::type::ARRAY:
                // access the array as a vector
                std::cout << "[";
                for (const auto& element : val.as<std::vector<msgpack::object>>()) {
                    std::cout << element << ", ";
                }
                std::cout << "]";
                break;
            case msgpack::type::BOOLEAN:
                std::cout << (val.as<bool>() ? "true" : "false");
                break;
            case msgpack::type::NIL:
                std::cout << "null";
                break;
            case msgpack::type::STR:
                std::cout << val.as<std::string>();
                break;
            default:
                std::cout << "unknown type";
                break;
        }
        std::cout << std::endl;
    }

    return 0;
}
