#include "static_analyzer/lexer.h"
#include <string>

using namespace std;

namespace algochecker::static_analyzer {

string strip_comments_and_strings(const string& source)
{
    string result;

    bool in_single_comment = false;
    bool in_multi_comment = false;
    bool in_string = false;
    bool in_char = false;
    bool escaped = false;

    for (size_t i = 0; i < source.size(); i++)
    {
        char c = source[i];
        char next = '\0';

        if (i + 1 < source.size())
        {
            next = source[i + 1];
        }

        if (in_single_comment)
        {
            if (c == '\n')
            {
                in_single_comment = false;
                result += '\n';
            }
            continue;
        }

        if (in_multi_comment)
        {
            if (c == '\n')
            {
                result += '\n';
            }

            if (c == '*' && next == '/')
            {
                in_multi_comment = false;
                i++;
            }
            continue;
        }

        if (in_string)
        {
            if (c == '\n')
            {
                result += '\n';
            }

            if (escaped)
            {
                escaped = false;
                continue;
            }

            if (c == '\\')
            {
                escaped = true;
                continue;
            }

            if (c == '"')
            {
                in_string = false;
            }

            continue;
        }

        if (in_char)
        {
            if (c == '\n')
            {
                result += '\n';
            }

            if (escaped)
            {
                escaped = false;
                continue;
            }

            if (c == '\\')
            {
                escaped = true;
                continue;
            }

            if (c == '\'')
            {
                in_char = false;
            }

            continue;
        }

        if (c == '/' && next == '/')
        {
            in_single_comment = true;
            i++;
            continue;
        }

        if (c == '/' && next == '*')
        {
            in_multi_comment = true;
            i++;
            continue;
        }

        if (c == '"')
        {
            in_string = true;
            escaped = false;
            continue;
        }

        if (c == '\'')
        {
            in_char = true;
            escaped = false;
            continue;
        }

        result += c;
    }

    return result;
}

}