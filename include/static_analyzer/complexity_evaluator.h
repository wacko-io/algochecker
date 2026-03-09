#pragma once

#include <string>

#include "static_analyzer/parser.h"

namespace algochecker::static_analyzer {

std::string evaluate_complexity(const ParseResult& parse_result);

}
