#include "algochecker/analyzer.h"

#include "static_analyzer/complexity_evaluator.h"
#include "static_analyzer/file_loader.h"
#include "static_analyzer/lexer.h"

namespace algochecker {
namespace {

static_analyzer::ParseOptions to_parse_options(const AnalysisOptions& options) {
    return {
        options.entry_function_name,
        options.entry_function_arity
    };
}

}

AnalysisReport analyze_source(const std::string& source, const AnalysisOptions& options) {
    AnalysisReport report;
    report.source = source;
    report.normalized_source = static_analyzer::strip_comments_and_strings(source);
    report.parse_result = static_analyzer::parse_control_structures(
        report.normalized_source,
        to_parse_options(options));
    report.complexity = static_analyzer::evaluate_complexity(report.parse_result);
    return report;
}

AnalysisReport analyze_file(const std::string& path, const AnalysisOptions& options) {
    return analyze_source(static_analyzer::load_source_file(path), options);
}

}
