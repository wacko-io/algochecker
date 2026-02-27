#include "static_analyzer/complexity_evaluator.h"

namespace algochecker::static_analyzer {

std::string evaluate_complexity(const ParseResult& parse_result) {
    (void)parse_result;
    return "O(1)";
}

} // namespace algochecker::static_analyzer
