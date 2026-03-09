#include "static_analyzer/parser.h"
#include <string>
#include <stack>
#include <cctype>

using namespace std;

namespace algochecker::static_analyzer {

bool is_identifier_char(char c)
{
    return isalnum(c) || c == '_';
}

bool contains_variable_n(const string& text)
{
    for (size_t i = 0; i < text.size(); i++)
    {
        if (text[i] == 'n')
        {
            bool left_ok = (i == 0 || !is_identifier_char(text[i - 1]));
            bool right_ok = (i + 1 == text.size() || !is_identifier_char(text[i + 1]));

            if (left_ok && right_ok)
            {
                return true;
            }
        }
    }

    return false;
}

string get_for_condition(const string& header)
{
    size_t first_semicolon = header.find(';');
    if (first_semicolon == string::npos)
    {
        return "";
    }

    size_t second_semicolon = header.find(';', first_semicolon + 1);
    if (second_semicolon == string::npos)
    {
        return "";
    }

    return header.substr(first_semicolon + 1, second_semicolon - first_semicolon - 1);
}

ParseResult parse_control_structures(const string& source)
{
    ParseResult result;

    int current_depth = 0;
    int max_depth = 0;
    int operations = 0;

    stack<char> loop_stack;

    for (size_t i = 0; i < source.size(); i++)
    {
        char c = source[i];

        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=')
        {
            operations++;
        }

        if (i + 2 < source.size() && source.substr(i, 3) == "for")
        {
            result.loop_count++;

            size_t start = source.find('(', i);
            size_t end = source.find(')', start);

            bool constant_loop = true;

            if (start != string::npos && end != string::npos)
            {
                string header = source.substr(start + 1, end - start - 1);
                string condition = get_for_condition(header);

                if (contains_variable_n(condition))
                {
                    constant_loop = false;
                }
            }

            if (!constant_loop)
            {
                current_depth++;

                if (current_depth > max_depth)
                {
                    max_depth = current_depth;
                }
            }

            loop_stack.push('L');
        }

        if (i + 4 < source.size() && source.substr(i, 5) == "while")
        {
            result.loop_count++;

            size_t start = source.find('(', i);
            size_t end = source.find(')', start);

            bool constant_loop = true;

            if (start != string::npos && end != string::npos)
            {
                string condition = source.substr(start + 1, end - start - 1);

                if (contains_variable_n(condition))
                {
                    constant_loop = false;
                }
            }

            if (!constant_loop)
            {
                current_depth++;

                if (current_depth > max_depth)
                {
                    max_depth = current_depth;
                }
            }

            loop_stack.push('L');
        }

        if (source[i] == '{')
        {
            loop_stack.push('{');
        }

        if (source[i] == '}')
        {
            if (!loop_stack.empty())
            {
                if (loop_stack.top() == '{')
                {
                    loop_stack.pop();
                }
                else if (loop_stack.top() == 'L')
                {
                    loop_stack.pop();

                    if (current_depth > 0)
                    {
                        current_depth--;
                    }
                }
            }
        }
    }

    result.max_nesting_depth = max_depth;
    result.estimated_operations = operations;

    return result;
}

} // namespace algochecker::static_analyzer