#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdarg>

static std::vector<std::string> splitPath(const std::string &path)
{
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string token;

    while (std::getline(ss, token, '/'))
        if (!token.empty())
            parts.push_back(token);

    return parts;
}

static std::string resolveParts(const std::vector<std::string> &segments, bool isAbsolute)
{
    std::vector<std::string> stack;
    int pendingUps = 0;

    for (std::vector<std::string>::const_iterator it = segments.begin(); it != segments.end(); ++it)
    {
        if (*it == ".")
            continue;
        else if (*it == "..")
        {
            if (!stack.empty())
                stack.pop_back();
            else if (!isAbsolute)
                pendingUps++;
        }
        else
            stack.push_back(*it);
    }

    std::string result;

    for (int i = 0; i < pendingUps; ++i)
        result += "../";
    if (!result.empty())
        result.erase(result.size() - 1, 1);

    for (std::vector<std::string>::const_iterator it = stack.begin(); it != stack.end(); ++it)
    {
        if (!result.empty())
            result += "/";
        result += *it;
    }

    if (isAbsolute)
        return result.empty() ? "/" : "/" + result;

    return result.empty() ? "." : result;
}

std::string resolvePath(int count, ...)
{
    va_list args;
    va_start(args, count);

    std::vector<std::string> allSegments;
    bool isAbsolute = false;

    for (int i = 0; i < count; i++)
    {
        const char *raw = va_arg(args, const char *);
        if (!raw)
            continue;

        std::string path(raw);

        if (!path.empty() && path[0] == '/')
        {
            allSegments.clear();
            isAbsolute = true;
        }

        std::vector<std::string> parts = splitPath(path);
        for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
            allSegments.push_back(*it);
    }

    va_end(args);

    return resolveParts(allSegments, isAbsolute);
}
