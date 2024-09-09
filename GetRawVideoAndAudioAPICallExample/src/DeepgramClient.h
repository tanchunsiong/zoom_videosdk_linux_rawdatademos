#ifndef DEEPGRAMCLIENT_H
#define DEEPGRAMCLIENT_H

#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <string>
#include "json.hpp" // Include the nlohmann JSON library

using json = nlohmann::json; // Define a convenient alias

// Callback function to capture the response data from CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize); // Append the response data
    return totalSize;
}

class DeepgramClient {
public:
    // Constructor to initialize with API key
    DeepgramClient(const std::string& apiKey) : apiKey(apiKey) {
        // Initialize libcurl globally
        curl_global_init(CURL_GLOBAL_ALL);
    }

    // Destructor to cleanup libcurl
    ~DeepgramClient() {
        curl_global_cleanup();
    }

    // Function to send audio file to Deepgram API and capture response
    bool sendAudioFile(const std::string& audioFilePath, const std::string& audioType = "audio/wav") {

        // Print message to say it is here
        std::cout << "DeepgramClient::sendAudioFile() called" << std::endl;

        // Create a CURL handle
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to initialize libcurl" << std::endl;
            return false;
        }

        // Set verbose output for debugging
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

        // Set the request URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.deepgram.com/v1/listen");

        // Set the request headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Token " + apiKey).c_str());
        headers = curl_slist_append(headers, ("Content-Type: " + audioType).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Print message to say it is reading binary data
        // std::cout << "Reading binary data" << std::endl;

        // Read the audio file as binary data
        std::ifstream file(audioFilePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Failed to open audio file: " << audioFilePath << std::endl;
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            return false;
        }

        // Get the file size and read the contents into a string
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string audioData(fileSize, '\0');
        if (!file.read(&audioData[0], fileSize)) {
            std::cerr << "Failed to read audio file" << std::endl;
            file.close();
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            return false;
        }
        file.close();

        // Set the request data as binary
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audioData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, audioData.size());

        // Prepare to capture the response from the API
        std::string responseString;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

        // Perform the request
        // std::cout << "Performing the request" << std::endl;
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            return false;
        }

        // Parse the JSON response
        // std::cout << "Parsing API response..." << std::endl;
        json jsonResponse;
        try {
            jsonResponse = json::parse(responseString);
        }
        catch (const json::parse_error& e) {
            std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
            return false;
        }

        // Print the transcript and confidence for each result
        if (jsonResponse.contains("results")) {
            for (const auto& channel : jsonResponse["results"]["channels"]) {
                for (const auto& alternative : channel["alternatives"]) {
                    std::string transcript = alternative["transcript"];
                    double confidence = alternative["confidence"];

                    std::cout << "Transcript: " << transcript << std::endl;
                    std::cout << "Confidence: " << confidence << std::endl;

                    // Print individual words with timings
 /*                   for (const auto& word : alternative["words"]) {
                        std::string wordText = word["word"];
                        double start = word["start"];
                        double end = word["end"];
                        double wordConfidence = word["confidence"];

                        std::cout << "Word: " << wordText << " (Start: " << start << ", End: " << end
                            << ", Confidence: " << wordConfidence << ")" << std::endl;
                    }*/
                }
            }
        }
        else {
            std::cerr << "No transcription results found in response." << std::endl;
        }

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        return true;
    }

private:
    std::string apiKey;
};

#endif // DEEPGRAMCLIENT_H
