#pragma once

#include <string>

#include "static_analyzer/parser.h"

namespace algochecker {

struct AnalysisOptions {
    std::string entry_function_name;
    int entry_function_arity = -1;
};

struct AnalysisReport {
    std::string source;
    std::string normalized_source;
    static_analyzer::ParseResult parse_result;
    std::string complexity;
};

AnalysisReport analyze_source(
    const std::string& source,
    const AnalysisOptions& options = {});

AnalysisReport analyze_file(
    const std::string& path,
    const AnalysisOptions& options = {});

}
