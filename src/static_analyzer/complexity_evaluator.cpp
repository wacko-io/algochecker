#include "static_analyzer/complexity_evaluator.h"

#include <algorithm>
#include <string>

namespace algochecker::static_analyzer {

std::string evaluate_complexity(const ParseResult& parse_result) {
    const int operations = std::max(1, parse_result.estimated_operations);
    const int degree = std::max(parse_result.dominant_degree, parse_result.max_nesting_depth);

    std::string t_n;
    std::string big_o;

    if (degree <= 0) {
        t_n = "T(n) = " + std::to_string(operations);
        big_o = "O(1)";
    } else if (degree == 1) {
        t_n = "T(n) = " + std::to_string(operations) + " * n";
        big_o = "O(n)";
    } else {
        t_n = "T(n) = " + std::to_string(operations) + " * n^" + std::to_string(degree);
        big_o = "O(n^" + std::to_string(degree) + ")";
    }

    return t_n + ", " + big_o;
}

}
