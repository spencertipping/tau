#include <iostream>
#include <curl/curl.h>
#include <cstring>

using namespace std;

// This function will be called by libcurl when a WebSocket message is received
static size_t message_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    // Print the received message
    cout << "Received message: " << string(ptr, size * nmemb) << endl;
    return size * nmemb;
}

int main() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        cout << "Failed to initialize libcurl" << endl;
        return 1;
    }

    // Set the WebSocket URL
    curl_easy_setopt(curl, CURLOPT_URL, "ws://ws.postman-echo.com/raw");

    // Set the WebSocket protocol
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Sec-WebSocket-Protocol: chat");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the WebSocket callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, message_callback);

    // Open the WebSocket connection
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cout << "Failed to open WebSocket connection: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Send a message to the server
    const char *message = "Hello, server!";
    curl_easy_send(curl, message, strlen(message), 0);

    // Wait for the WebSocket connection to close
    curl_easy_cleanup(curl);
    return 0;
}
