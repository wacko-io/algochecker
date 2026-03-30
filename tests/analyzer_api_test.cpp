#include <cstdlib>
#include <iostream>
#include <string>

#include "algochecker/analyzer.h"

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << std::endl;
        std::exit(1);
    }
}

void test_only_for_loops_are_counted() {
    const std::string source = R"(
        int main() {
            int n = 10;
            while (n > 0) {
                for (int i = 0; i < n; ++i) {
                    n -= i;
                }
                --n;
            }
            do {
                --n;
            } while (n > 0);
            return n;
        }
    )";

    const auto report = algochecker::analyze_source(source);
    expect(report.parse_result.loop_count == 1, "Only for-loops must be counted.");
    expect(report.parse_result.variable_loop_count == 1, "Variable for-loop was not detected.");
    expect(report.parse_result.dominant_degree == 1, "Unsupported loops must not affect complexity.");
}

void test_function_calls_are_resolved_by_arity() {
    const std::string source = R"(
        int work(int n) {
            int sum = 0;
            for (int i = 0; i < n; ++i) {
                sum += i;
            }
            return sum;
        }

        int work(int a, int b) {
            return a + b;
        }

        int main() {
            return work(10);
        }
    )";

    const auto report = algochecker::analyze_source(source);
    expect(report.parse_result.function_call_count >= 1, "Function call was not detected.");
    expect(report.parse_result.resolved_function_call_count >= 1, "Function call overload was not resolved.");
    expect(report.parse_result.dominant_degree == 1, "Resolved helper function complexity was not propagated.");
}

void test_entry_point_can_be_selected_by_name_and_arity() {
    const std::string source = R"(
        int helper(int n) {
            for (int i = 0; i < n; ++i) {
                n -= i;
            }
            return n;
        }

        int helper(int a, int b) {
            return a + b;
        }
    )";

    algochecker::AnalysisOptions options;
    options.entry_function_name = "helper";
    options.entry_function_arity = 2;

    const auto report = algochecker::analyze_source(source, options);
    expect(report.parse_result.analyzed_function_name == "helper", "Entry function name is incorrect.");
    expect(report.parse_result.analyzed_function_arity == 2, "Entry function arity is incorrect.");
    expect(report.parse_result.dominant_degree == 0, "Wrong overload was analyzed.");
}

void test_recursion_is_ignored() {
    const std::string source = R"(
        int fact(int n) {
            if (n <= 1) {
                return 1;
            }
            return n * fact(n - 1);
        }

        int main() {
            return fact(5);
        }
    )";

    const auto report = algochecker::analyze_source(source);
    expect(report.parse_result.recursion_skipped_count >= 1, "Recursive call must be skipped.");
    expect(report.parse_result.dominant_degree == 0, "Recursion should not be unfolded into extra complexity.");
}

}

int main() {
    test_only_for_loops_are_counted();
    test_function_calls_are_resolved_by_arity();
    test_entry_point_can_be_selected_by_name_and_arity();
    test_recursion_is_ignored();
    return 0;
}
