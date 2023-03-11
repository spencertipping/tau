#include <curl/curl.h>
#include <iostream>
#include <string>

// Define a callback function to handle the response body
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::string *response_body = reinterpret_cast<std::string *>(userdata);
    response_body->append(ptr, size * nmemb);
    return size * nmemb;
}

int main(int argc, char **argv)
{
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a curl handle
    CURL *curl = curl_easy_init();

    if (curl) {
        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, argv[1]);

        // Specify that we want to use SSL/TLS
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        std::string response_body;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

        // Send the HTTP request and fetch the response
        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            // Fetch the response code and print it
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            std::cout << "Response code: " << response_code << std::endl;

            // Fetch the response body and print it
            std::cout << "Response body: " << response_body << std::endl;
        } else {
            // An error occurred
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Clean up libcurl
    curl_global_cleanup();

    return 0;
}
