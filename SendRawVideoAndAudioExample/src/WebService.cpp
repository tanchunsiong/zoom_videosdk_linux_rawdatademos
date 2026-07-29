#include <curl/curl.h>
#include <iostream>
#include <string>

#include "json.hpp"

using Json = nlohmann::json;

namespace {

size_t WriteCallback(char* contents, size_t size, size_t nmemb, std::string* response)
{
	size_t totalSize = size * nmemb;
	response->append(contents, totalSize);
	return totalSize;
}

} // namespace

std::string GetSignatureFromWebService(std::string remote_url, std::string session_name, std::string role)
{
	CURL* curl = curl_easy_init();
	if (!curl)
	{
		std::cerr << "Failed to initialize cURL." << std::endl;
		return {};
	}

	std::string response_body;
	const std::string payload = Json{
		{"sessionName", session_name},
		{"role", role},
		{"user_identity", "sample-generated"}
	}.dump();
	curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(curl, CURLOPT_URL, remote_url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(payload.size()));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

	const CURLcode result = curl_easy_perform(curl);
	long status_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (result != CURLE_OK)
	{
		std::cerr << "Signature service request failed: " << curl_easy_strerror(result) << std::endl;
		return {};
	}
	if (status_code < 200 || status_code >= 300)
	{
		std::cerr << "Signature service returned HTTP " << status_code << "." << std::endl;
		return {};
	}

	try
	{
		Json response_data = Json::parse(response_body);
		Json::iterator signature = response_data.find("signature");
		if (signature != response_data.end() && signature->is_string())
		{
			return signature->get<std::string>();
		}
		std::cerr << "Signature service response does not contain a string signature." << std::endl;
	}
	catch (const Json::exception& error)
	{
		std::cerr << "Invalid signature service response: " << error.what() << std::endl;
	}

	return {};
}
