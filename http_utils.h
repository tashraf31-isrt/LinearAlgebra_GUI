// Simple HTTP server header - cpp-httplib alternative
// We'll create a minimal solution without external dependencies

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <map>
#include <iostream>

class SimpleResponse {
public:
    int status_code = 200;
    std::string body;
    std::map<std::string, std::string> headers;
};

std::string htmlEncode(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '"') result += "&quot;";
        else if (c == '<') result += "&lt;";
        else if (c == '>') result += "&gt;";
        else if (c == '&') result += "&amp;";
        else if (c == '\n') result += "\\n";
        else result += c;
    }
    return result;
}

#endif
