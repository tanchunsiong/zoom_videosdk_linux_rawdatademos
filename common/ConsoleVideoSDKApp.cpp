#include "ConsoleVideoSDKApp.h"

#include "zoom_video_sdk_api.h"

#include <curl/curl.h>
#include <glib.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>

#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace ZOOMVIDEOSDK;

namespace {

volatile sig_atomic_t stop_requested = 0;
GMainLoop* main_loop = nullptr;
IZoomVideoSDK* active_sdk = nullptr;
ConsoleVideoSDKFeature* active_feature = nullptr;
bool leave_requested = false;

void SignalHandler(int)
{
    stop_requested = 1;
}

void SkipWhitespace(const std::string& input, std::size_t& position)
{
    while (position < input.size() && std::isspace(static_cast<unsigned char>(input[position])))
    {
        ++position;
    }
}

bool ParseQuotedString(
    const std::string& input,
    std::size_t& position,
    std::string& output,
    std::string& error)
{
    if (position >= input.size() || input[position] != '"')
    {
        error = "Expected a quoted JSON string.";
        return false;
    }

    ++position;
    output.clear();
    while (position < input.size())
    {
        const char character = input[position++];
        if (character == '"')
        {
            return true;
        }
        if (character != '\\')
        {
            output.push_back(character);
            continue;
        }
        if (position >= input.size())
        {
            break;
        }
        const char escaped = input[position++];
        switch (escaped)
        {
            case '"': output.push_back('"'); break;
            case '\\': output.push_back('\\'); break;
            case '/': output.push_back('/'); break;
            case 'b': output.push_back('\b'); break;
            case 'f': output.push_back('\f'); break;
            case 'n': output.push_back('\n'); break;
            case 'r': output.push_back('\r'); break;
            case 't': output.push_back('\t'); break;
            default:
                error = "Unsupported escape sequence in config.json.";
                return false;
        }
    }

    error = "Unterminated JSON string.";
    return false;
}

std::string EscapeJsonString(const std::string& value)
{
    std::string escaped;
    escaped.reserve(value.size());
    for (const char character : value)
    {
        switch (character)
        {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped.push_back(character); break;
        }
    }
    return escaped;
}

size_t WriteResponse(char* contents, size_t size, size_t count, std::string* response)
{
    const size_t length = size * count;
    response->append(contents, length);
    return length;
}

std::string FetchSignature(
    const std::string& url,
    const std::string& session_name,
    std::string& error)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        error = "Failed to initialize cURL.";
        return {};
    }

    const std::string payload =
        "{\"sessionName\":\"" + EscapeJsonString(session_name) +
        "\",\"role\":\"1\",\"user_identity\":\"sample-generated\"}";
    std::string response;
    curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(payload.size()));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    const CURLcode result = curl_easy_perform(curl);
    long status_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK)
    {
        error = std::string("Signature request failed: ") + curl_easy_strerror(result);
        return {};
    }
    if (status_code < 200 || status_code >= 300)
    {
        error = "Signature service returned HTTP " + std::to_string(status_code) + ".";
        return {};
    }

    FlatJsonConfig response_json;
    if (!response_json.Parse(response, error))
    {
        error = "Invalid signature response: " + error;
        return {};
    }
    const std::string signature = response_json.GetString("signature");
    if (signature.empty())
    {
        error = "Signature response does not contain a string signature.";
    }
    return signature;
}

gboolean PollForStop(gpointer)
{
    if (!stop_requested || leave_requested)
    {
        return G_SOURCE_CONTINUE;
    }

    leave_requested = true;
    if (active_feature)
    {
        active_feature->OnSessionLeaving();
    }
    if (active_sdk)
    {
        const ZoomVideoSDKErrors error = active_sdk->leaveSession(false);
        if (error != ZoomVideoSDKErrors_Success && main_loop)
        {
            std::cerr << "leaveSession failed with error " << error << std::endl;
            g_main_loop_quit(main_loop);
        }
    }
    else if (main_loop)
    {
        g_main_loop_quit(main_loop);
    }
    return G_SOURCE_CONTINUE;
}

} // namespace

bool FlatJsonConfig::Load(const std::string& path, std::string& error)
{
    std::ifstream input(path);
    if (!input)
    {
        error = "Unable to open " + path + ".";
        return false;
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return Parse(buffer.str(), error);
}

bool FlatJsonConfig::Parse(const std::string& content, std::string& error)
{
    values_.clear();
    std::size_t position = 0;
    SkipWhitespace(content, position);
    if (position >= content.size() || content[position++] != '{')
    {
        error = "Expected a JSON object.";
        return false;
    }

    while (true)
    {
        SkipWhitespace(content, position);
        if (position < content.size() && content[position] == '}')
        {
            ++position;
            break;
        }

        std::string key;
        if (!ParseQuotedString(content, position, key, error))
        {
            return false;
        }
        SkipWhitespace(content, position);
        if (position >= content.size() || content[position++] != ':')
        {
            error = "Expected ':' after config key.";
            return false;
        }
        SkipWhitespace(content, position);

        std::string value;
        if (position < content.size() && content[position] == '"')
        {
            if (!ParseQuotedString(content, position, value, error))
            {
                return false;
            }
        }
        else
        {
            const std::size_t value_start = position;
            while (position < content.size() && content[position] != ',' && content[position] != '}')
            {
                ++position;
            }
            value = content.substr(value_start, position - value_start);
            while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back())))
            {
                value.pop_back();
            }
        }
        values_[key] = value;

        SkipWhitespace(content, position);
        if (position < content.size() && content[position] == ',')
        {
            ++position;
            continue;
        }
        if (position < content.size() && content[position] == '}')
        {
            ++position;
            break;
        }
        error = "Expected ',' or '}' in config.json.";
        return false;
    }

    SkipWhitespace(content, position);
    if (position != content.size())
    {
        error = "Unexpected content after the JSON object.";
        return false;
    }
    return true;
}

std::string FlatJsonConfig::GetString(const std::string& key, const std::string& fallback) const
{
    const auto value = values_.find(key);
    return value == values_.end() ? fallback : value->second;
}

bool FlatJsonConfig::GetBool(const std::string& key, bool fallback) const
{
    const auto value = values_.find(key);
    if (value == values_.end())
    {
        return fallback;
    }
    return value->second == "true" ? true : value->second == "false" ? false : fallback;
}

int FlatJsonConfig::GetInt(const std::string& key, int fallback) const
{
    const auto value = values_.find(key);
    if (value == values_.end())
    {
        return fallback;
    }
    char* end = nullptr;
    errno = 0;
    const long parsed = std::strtol(value->second.c_str(), &end, 10);
    return errno == 0 && end && *end == '\0' ? static_cast<int>(parsed) : fallback;
}

void ConsoleVideoSDKFeature::Attach(IZoomVideoSDK* sdk)
{
    sdk_ = sdk;
}

void ConsoleVideoSDKFeature::onSessionJoin()
{
    std::cout << "Joined session successfully" << std::endl;
    OnSessionJoined();
}

void ConsoleVideoSDKFeature::onSessionLeave()
{
    HandleSessionLeave();
}

void ConsoleVideoSDKFeature::onSessionLeave(ZoomVideoSDKSessionLeaveReason)
{
    HandleSessionLeave();
}

void ConsoleVideoSDKFeature::onError(ZoomVideoSDKErrors error, int detail_error)
{
    std::cerr << "Session error " << error << " (detail " << detail_error << ")" << std::endl;
    OnSessionError(error, detail_error);
    if (main_loop)
    {
        g_main_loop_quit(main_loop);
    }
}

void ConsoleVideoSDKFeature::ConfigureSession(ZoomVideoSDKSessionContext&)
{
}

void ConsoleVideoSDKFeature::OnSessionLeaving()
{
}

void ConsoleVideoSDKFeature::OnSessionError(ZoomVideoSDKErrors, int)
{
}

IZoomVideoSDK* ConsoleVideoSDKFeature::sdk() const
{
    return sdk_;
}

void ConsoleVideoSDKFeature::HandleSessionLeave()
{
    if (leave_handled_)
    {
        return;
    }
    leave_handled_ = true;
    std::cout << "Left session" << std::endl;
    if (main_loop)
    {
        g_main_loop_quit(main_loop);
    }
}

std::string GetExecutableDirectory()
{
    char path[PATH_MAX] = {};
    const ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (length <= 0)
    {
        return {};
    }
    path[length] = '\0';
    std::string executable(path);
    const std::size_t separator = executable.find_last_of('/');
    return separator == std::string::npos ? std::string() : executable.substr(0, separator);
}

bool LoadRuntimeConfig(FlatJsonConfig& config, std::string& error)
{
    const std::string directory = GetExecutableDirectory();
    if (directory.empty())
    {
        error = "Unable to determine executable directory.";
        return false;
    }
    return config.Load(directory + "/config.json", error);
}

int RunConsoleVideoSDKSample(const FlatJsonConfig& config, ConsoleVideoSDKFeature& feature)
{
    const std::string session_name = config.GetString("session_name");
    const std::string session_password = config.GetString("session_psw");
    const std::string user_name = config.GetString("user_name", "Linux Bot");
    std::string token = config.GetString("token");
    if (session_name.empty())
    {
        std::cerr << "session_name is required in config.json." << std::endl;
        return 1;
    }
    if (config.GetBool("getSignatureFromWebService"))
    {
        const std::string signature_url = config.GetString("signatureUrl");
        if (signature_url.empty())
        {
            std::cerr << "signatureUrl is required when getSignatureFromWebService is true." << std::endl;
            return 1;
        }
        std::string error;
        token = FetchSignature(signature_url, session_name, error);
        if (token.empty())
        {
            std::cerr << error << std::endl;
            return 1;
        }
    }
    if (token.empty())
    {
        std::cerr << "token is required in config.json." << std::endl;
        return 1;
    }

    stop_requested = 0;
    leave_requested = false;
    active_feature = &feature;
    active_sdk = CreateZoomVideoSDKObj();
    if (!active_sdk)
    {
        std::cerr << "Failed to create Video SDK object." << std::endl;
        return 1;
    }

    ZoomVideoSDKInitParams init_params;
    init_params.domain = "https://zoom.us";
    init_params.enableLog = true;
    init_params.logFilePrefix = "zoom_videosdk_demo";
    init_params.videoRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
    init_params.shareRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
    init_params.audioRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
    init_params.enableIndirectRawdata = false;

    ZoomVideoSDKErrors error = active_sdk->initialize(init_params);
    if (error != ZoomVideoSDKErrors_Success)
    {
        std::cerr << "SDK initialization failed with error " << error << std::endl;
        DestroyZoomVideoSDKObj();
        active_sdk = nullptr;
        active_feature = nullptr;
        return 1;
    }

    feature.Attach(active_sdk);
    active_sdk->addListener(&feature);

    ZoomVideoSDKSessionContext context;
    context.sessionName = session_name.c_str();
    context.sessionPassword = session_password.c_str();
    context.userName = user_name.c_str();
    context.token = token.c_str();
    context.videoOption.localVideoOn = false;
    context.audioOption.connect = false;
    context.audioOption.mute = true;
    feature.ConfigureSession(context);

    main_loop = g_main_loop_new(nullptr, FALSE);
    struct sigaction action = {};
    action.sa_handler = SignalHandler;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);
    g_timeout_add(100, PollForStop, nullptr);

    if (!active_sdk->joinSession(context))
    {
        std::cerr << "joinSession returned null." << std::endl;
        g_main_loop_unref(main_loop);
        main_loop = nullptr;
        active_sdk->removeListener(&feature);
        active_sdk->cleanup();
        DestroyZoomVideoSDKObj();
        active_sdk = nullptr;
        active_feature = nullptr;
        return 1;
    }

    std::cout << "Joining session '" << session_name << "'. Press Ctrl+C to stop." << std::endl;
    g_main_loop_run(main_loop);

    feature.OnSessionLeaving();
    active_sdk->removeListener(&feature);
    active_sdk->cleanup();
    DestroyZoomVideoSDKObj();
    active_sdk = nullptr;
    active_feature = nullptr;
    g_main_loop_unref(main_loop);
    main_loop = nullptr;
    return 0;
}
