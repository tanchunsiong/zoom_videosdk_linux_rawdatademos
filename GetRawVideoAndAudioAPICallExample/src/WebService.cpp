#include <iostream>
#include <locale>
#include <string>
#include <cstdlib>
#include <cstring>
#include "json.hpp"
#include <curl/curl.h>
using Json = nlohmann::json;

size_t WriteCallback(char* contents, size_t size, size_t nmemb, std::string* response) {
	size_t totalSize = size * nmemb;
	response->append(contents, totalSize);
	return totalSize;
}

std::string GetSignatureFromWebService(std::string remote_url, std::string session_name, std::string role) {

	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	char* json = NULL;
	struct curl_slist* headers = NULL;
	printf("initing curl");
	curl = curl_easy_init();
	if (curl) {


		printf("setting remote url");
		curl_easy_setopt(curl, CURLOPT_URL, remote_url.c_str());

		//buffer size
		printf("setting buffer");
		curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 120000L);
		//region for callback
		// curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		// curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		// res = curl_easy_perform(curl);
		// curl_easy_cleanup(curl);
		//std::cout << readBuffer << std::endl;
		//region for call back

	   //temp workaround to enable SSL / HTTPS
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0);
		curl_easy_setopt(curl, CURLOPT_CAINFO, "/root/zoom_v-sdk_linux_bot/bin/cacert.pem");
		curl_easy_setopt(curl, CURLOPT_CAPATH, "/root/zoom_v-sdk_linux_bot/bin/cacert.pem");
		//headers
		printf("setting headers");
		headers = curl_slist_append(headers, "Expect:");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		printf("setting payload");

		// Concatenate the variables with the JSON data
		std::string json = R"({
    "sessionName": ")" + session_name + R"(",
    "role": ")" + role + R"(",
    "user_identity": "sample-generated"
})";

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
		//callback
		printf("preparing callback");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		//perform
		printf("calling remote URL");
		res = curl_easy_perform(curl);



		// Parse the JSON response
		try {
			Json responseData = Json::parse(readBuffer);

			// Check if the "signature" key exists
			if (responseData.find("signature") != responseData.end()) {
				std::string signature = responseData["signature"];
				std::cout << "Signature: " << signature << std::endl;
				return signature;
			}
			else {
				std::cerr << "Key 'signature' not found in JSON response." << std::endl;

				return nullptr;
			}

		}
		catch (const Json::parse_error& e) {
			std::cerr << "JSON parse error: " << e.what() << std::endl;
			return nullptr;
		}

		std::cout << readBuffer << std::endl;
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

	}
	return nullptr;

}
