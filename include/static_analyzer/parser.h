#pragma once

#include <string>

namespace algochecker::static_analyzer {

struct ParseOptions {
    std::string entry_function_name;
    int entry_function_arity = -1;
};

struct ParseResult {
    int loop_count = 0;
    int variable_loop_count = 0;
    int constant_loop_count = 0;
    int max_nesting_depth = 0;
    int loop_body_operations = 0;
    int estimated_operations = 0;
    int function_call_count = 0;
    int resolved_function_call_count = 0;
    int recursion_skipped_count = 0;
    int dominant_degree = 0;
    std::string analyzed_function_name;
    int analyzed_function_arity = -1;
};

ParseResult parse_control_structures(
    const std::string& source,
    const ParseOptions& options = {});

}
