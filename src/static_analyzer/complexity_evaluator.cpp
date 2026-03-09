#include "static_analyzer/complexity_evaluator.h"
#include <string>

using namespace std;

namespace algochecker::static_analyzer {

    string evaluate_complexity(const ParseResult& parse_result)
    {
        int operations = parse_result.estimated_operations;
        int depth = parse_result.max_nesting_depth;

        string t_n;
        string big_o;

        if (depth == 0)
        {
            t_n = "T(n) = " + to_string(operations);
            big_o = "O(1)";
        }
        else if (depth == 1)
        {
            t_n = "T(n) = " + to_string(operations) + "n";
            big_o = "O(n)";
        }
        else
        {
            t_n = "T(n) = " + to_string(operations) + "n^" + to_string(depth);
            big_o = "O(n^" + to_string(depth) + ")";
        }

        return t_n + ", " + big_o;
    }

} // namespace algochecker::static_analyzer