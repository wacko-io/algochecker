#pragma once

#include <string>

namespace algochecker::static_analyzer {

struct ParseResult {
    int loop_count = 0;
    int variable_loop_count = 0;
    int constant_loop_count = 0;
    int max_nesting_depth = 0;
    int loop_body_operations = 0;
    int estimated_operations = 0;
};

ParseResult parse_control_structures(const std::string& source);

}
