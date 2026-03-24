#pragma once

#include <cstdlib>
#include <fstream>
#include <string>
#include <unistd.h>

namespace sample_env {

inline std::string TrimWhitespace(const std::string& value) {
    const std::string whitespace = " \t\r\n";
    const std::size_t first = value.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return "";
    }

    const std::size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
}

inline std::string StripQuotes(const std::string& value) {
    if (value.size() >= 2) {
        const char first = value.front();
        const char last = value.back();
        if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
            return value.substr(1, value.size() - 2);
        }
    }

    return value;
}

inline std::string ReadEnvValueFromFile(const std::string& env_file, const std::string& key) {
    std::ifstream input(env_file.c_str());
    if (!input.is_open()) {
        return "";
    }

    std::string line;
    while (std::getline(input, line)) {
        const std::string trimmed = TrimWhitespace(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        std::string content = trimmed;
        if (content.compare(0, 7, "export ") == 0) {
            content = TrimWhitespace(content.substr(7));
        }

        const std::size_t equals = content.find('=');
        if (equals == std::string::npos) {
            continue;
        }

        const std::string name = TrimWhitespace(content.substr(0, equals));
        if (name != key) {
            continue;
        }

        std::string value = TrimWhitespace(content.substr(equals + 1));
        const std::size_t comment = value.find(" #");
        if (comment != std::string::npos) {
            value = TrimWhitespace(value.substr(0, comment));
        }

        return StripQuotes(value);
    }

    return "";
}

inline std::string FindEnvValue(const std::string& key) {
    const char* process_value = std::getenv(key.c_str());
    if (process_value != NULL && process_value[0] != '\0') {
        return process_value;
    }

    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return "";
    }

    std::string current_dir = cwd;
    while (!current_dir.empty()) {
        const std::string env_file = current_dir + "/.env";
        const std::string value = ReadEnvValueFromFile(env_file, key);
        if (!value.empty()) {
            return value;
        }

        const std::size_t slash = current_dir.find_last_of('/');
        if (slash == std::string::npos) {
            break;
        }
        if (slash == 0) {
            current_dir = "/";
        } else {
            current_dir = current_dir.substr(0, slash);
        }

        if (current_dir == "/") {
            const std::string root_value = ReadEnvValueFromFile("/.env", key);
            if (!root_value.empty()) {
                return root_value;
            }
            break;
        }
    }

    return "";
}

}  // namespace sample_env
