#include "static_analyzer/parser.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace algochecker::static_analyzer {
namespace {

struct FunctionDefinition {
    std::string name;
    int arity = 0;
    std::string body;
};

struct StatementSummary {
    int degree = 0;
    int operations = 0;
};

bool is_identifier_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool is_keyword_at(const std::string& source, std::size_t pos, const std::string& keyword) {
    if (pos + keyword.size() > source.size()) {
        return false;
    }

    if (source.compare(pos, keyword.size(), keyword) != 0) {
        return false;
    }

    const bool left_ok = pos == 0 || !is_identifier_char(source[pos - 1]);
    const bool right_ok = pos + keyword.size() == source.size() ||
        !is_identifier_char(source[pos + keyword.size()]);

    return left_ok && right_ok;
}

void skip_whitespace(const std::string& source, std::size_t& pos) {
    while (pos < source.size() &&
        std::isspace(static_cast<unsigned char>(source[pos])) != 0) {
        ++pos;
    }
}

std::size_t find_matching(
    const std::string& source,
    std::size_t open_pos,
    char open_char,
    char close_char) {
    if (open_pos >= source.size() || source[open_pos] != open_char) {
        return std::string::npos;
    }

    int balance = 1;

    for (std::size_t pos = open_pos + 1; pos < source.size(); ++pos) {
        if (source[pos] == open_char) {
            ++balance;
        } else if (source[pos] == close_char) {
            --balance;

            if (balance == 0) {
                return pos;
            }
        }
    }

    return std::string::npos;
}

std::string make_function_key(const std::string& name, int arity) {
    return name + "#" + std::to_string(arity);
}

bool contains_variable_n(const std::string& text) {
    for (std::size_t pos = 0; pos < text.size(); ++pos) {
        if (text[pos] != 'n') {
            continue;
        }

        const bool left_ok = pos == 0 || !is_identifier_char(text[pos - 1]);
        const bool right_ok = pos + 1 == text.size() || !is_identifier_char(text[pos + 1]);

        if (left_ok && right_ok) {
            return true;
        }
    }

    return false;
}

int count_elementary_operations(const std::string& text) {
    int operations = 0;

    for (std::size_t pos = 0; pos < text.size(); ++pos) {
        if (pos + 1 < text.size()) {
            const std::string two_chars = text.substr(pos, 2);

            if (two_chars == "++" || two_chars == "--" ||
                two_chars == "+=" || two_chars == "-=" ||
                two_chars == "*=" || two_chars == "/=" ||
                two_chars == "%=") {
                ++operations;
                ++pos;
                continue;
            }
        }

        if (text[pos] == '+' || text[pos] == '-' || text[pos] == '*' ||
            text[pos] == '/' || text[pos] == '%') {
            ++operations;
            continue;
        }

        if (text[pos] == '=' &&
            (pos == 0 || text[pos - 1] != '=') &&
            (pos + 1 == text.size() || text[pos + 1] != '=')) {
            ++operations;
        }
    }

    return operations;
}

int count_parameters(const std::string& parameters_text) {
    std::size_t pos = 0;
    skip_whitespace(parameters_text, pos);

    if (pos >= parameters_text.size()) {
        return 0;
    }

    if (parameters_text.compare(pos, 4, "void") == 0) {
        std::size_t after_void = pos + 4;
        if (after_void == parameters_text.size() ||
            !is_identifier_char(parameters_text[after_void])) {
            skip_whitespace(parameters_text, after_void);
            if (after_void == parameters_text.size()) {
                return 0;
            }
        }
    }

    int count = 1;
    int parentheses_balance = 0;
    int braces_balance = 0;
    int brackets_balance = 0;

    for (char c : parameters_text) {
        if (c == '(') {
            ++parentheses_balance;
        } else if (c == ')') {
            --parentheses_balance;
        } else if (c == '{') {
            ++braces_balance;
        } else if (c == '}') {
            --braces_balance;
        } else if (c == '[') {
            ++brackets_balance;
        } else if (c == ']') {
            --brackets_balance;
        } else if (c == ',' &&
            parentheses_balance == 0 &&
            braces_balance == 0 &&
            brackets_balance == 0) {
            ++count;
        }
    }

    return count;
}

bool is_function_keyword(const std::string& name) {
    static const std::unordered_set<std::string> keywords = {
        "if",
        "for",
        "while",
        "switch",
        "catch"
    };

    return keywords.find(name) != keywords.end();
}

std::unordered_map<std::string, FunctionDefinition> extract_function_definitions(const std::string& source) {
    std::unordered_map<std::string, FunctionDefinition> functions;
    int brace_depth = 0;

    for (std::size_t pos = 0; pos < source.size(); ++pos) {
        if (source[pos] == '{') {
            ++brace_depth;
            continue;
        }

        if (source[pos] == '}') {
            brace_depth = std::max(0, brace_depth - 1);
            continue;
        }

        if (brace_depth != 0 || source[pos] != '(') {
            continue;
        }

        std::size_t name_end = pos;
        while (name_end > 0 &&
            std::isspace(static_cast<unsigned char>(source[name_end - 1])) != 0) {
            --name_end;
        }

        std::size_t name_start = name_end;
        while (name_start > 0 && is_identifier_char(source[name_start - 1])) {
            --name_start;
        }

        if (name_start == name_end) {
            continue;
        }

        const std::string function_name = source.substr(name_start, name_end - name_start);
        if (is_function_keyword(function_name)) {
            continue;
        }

        const std::size_t params_end = find_matching(source, pos, '(', ')');
        if (params_end == std::string::npos) {
            continue;
        }

        std::size_t body_start = params_end + 1;
        skip_whitespace(source, body_start);
        if (body_start >= source.size() || source[body_start] != '{') {
            continue;
        }

        const std::size_t body_end = find_matching(source, body_start, '{', '}');
        if (body_end == std::string::npos) {
            continue;
        }

        const std::string params_text = source.substr(pos + 1, params_end - pos - 1);
        const int arity = count_parameters(params_text);
        const std::string key = make_function_key(function_name, arity);

        if (functions.find(key) == functions.end()) {
            functions.emplace(key, FunctionDefinition{
                function_name,
                arity,
                source.substr(body_start + 1, body_end - body_start - 1)
            });
        }

        pos = body_end;
    }

    return functions;
}

const FunctionDefinition* resolve_entry_function(
    const std::unordered_map<std::string, FunctionDefinition>& functions,
    const ParseOptions& options) {
    if (!options.entry_function_name.empty()) {
        if (options.entry_function_arity >= 0) {
            const auto exact_it = functions.find(
                make_function_key(options.entry_function_name, options.entry_function_arity));
            if (exact_it != functions.end()) {
                return &exact_it->second;
            }
        } else {
            for (const auto& [key, function] : functions) {
                (void)key;
                if (function.name == options.entry_function_name) {
                    return &function;
                }
            }
        }
    }

    const auto main_it = functions.find(make_function_key("main", 0));
    if (main_it != functions.end()) {
        return &main_it->second;
    }

    if (!functions.empty()) {
        return &functions.begin()->second;
    }

    return nullptr;
}

class AnalyzerParser {
public:
    AnalyzerParser(
        const std::string& source,
        ParseOptions options)
        : functions_(extract_function_definitions(source)),
          options_(std::move(options)) {
    }

    ParseResult parse() {
        const FunctionDefinition* entry = resolve_entry_function(functions_, options_);
        if (entry == nullptr) {
            return result_;
        }

        result_.analyzed_function_name = entry->name;
        result_.analyzed_function_arity = entry->arity;

        std::vector<std::string> call_stack = { make_function_key(entry->name, entry->arity) };
        const StatementSummary summary = analyze_function(*entry, call_stack);
        result_.dominant_degree = summary.degree;
        result_.estimated_operations = std::max(1, summary.operations);

        return result_;
    }

private:
    class BodyParser {
    public:
        BodyParser(
            AnalyzerParser& owner,
            const std::string& source,
            const std::string& current_function_key,
            std::vector<std::string>& call_stack)
            : owner_(owner),
              source_(source),
              current_function_key_(current_function_key),
              call_stack_(call_stack) {
        }

        StatementSummary parse_sequence() {
            std::size_t pos = 0;
            StatementSummary summary;

            while (pos < source_.size()) {
                skip_whitespace(source_, pos);

                if (pos >= source_.size()) {
                    break;
                }

                const StatementSummary statement = parse_statement(pos, 0, 0);
                summary.degree = std::max(summary.degree, statement.degree);
                summary.operations += statement.operations;
            }

            return summary;
        }

    private:
        StatementSummary parse_statement(
            std::size_t& pos,
            int loop_depth,
            int variable_depth) {
            skip_whitespace(source_, pos);

            if (pos >= source_.size()) {
                return {};
            }

            if (source_[pos] == '{') {
                return parse_block(pos, loop_depth, variable_depth);
            }

            if (is_keyword_at(source_, pos, "for")) {
                return parse_for_statement(pos, loop_depth, variable_depth);
            }

            if (is_keyword_at(source_, pos, "if")) {
                return parse_if_statement(pos, loop_depth, variable_depth);
            }

            if (is_keyword_at(source_, pos, "else")) {
                pos += 4;
                return parse_statement(pos, loop_depth, variable_depth);
            }

            if (is_keyword_at(source_, pos, "while")) {
                return parse_unsupported_condition_statement(pos, loop_depth, variable_depth, 5);
            }

            if (is_keyword_at(source_, pos, "switch")) {
                return parse_unsupported_condition_statement(pos, loop_depth, variable_depth, 6);
            }

            if (is_keyword_at(source_, pos, "do")) {
                return parse_do_statement(pos, loop_depth, variable_depth);
            }

            return parse_simple_statement(pos, loop_depth);
        }

        StatementSummary parse_block(
            std::size_t& pos,
            int loop_depth,
            int variable_depth) {
            StatementSummary summary;
            ++pos;

            while (pos < source_.size()) {
                skip_whitespace(source_, pos);

                if (pos >= source_.size()) {
                    break;
                }

                if (source_[pos] == '}') {
                    ++pos;
                    break;
                }

                const StatementSummary statement = parse_statement(pos, loop_depth, variable_depth);
                summary.degree = std::max(summary.degree, statement.degree);
                summary.operations += statement.operations;
            }

            return summary;
        }

        StatementSummary parse_if_statement(
            std::size_t& pos,
            int loop_depth,
            int variable_depth) {
            pos += 2;
            skip_whitespace(source_, pos);

            if (pos < source_.size() && source_[pos] == '(') {
                const std::size_t condition_end = find_matching(source_, pos, '(', ')');
                pos = condition_end == std::string::npos ? source_.size() : condition_end + 1;
            }

            const StatementSummary if_branch = parse_statement(pos, loop_depth, variable_depth);
            skip_whitespace(source_, pos);

            StatementSummary else_branch;
            if (is_keyword_at(source_, pos, "else")) {
                pos += 4;
                else_branch = parse_statement(pos, loop_depth, variable_depth);
            }

            return {
                std::max(if_branch.degree, else_branch.degree),
                std::max(if_branch.operations, else_branch.operations)
            };
        }

        StatementSummary parse_unsupported_condition_statement(
            std::size_t& pos,
            int loop_depth,
            int variable_depth,
            std::size_t keyword_length) {
            pos += keyword_length;
            skip_whitespace(source_, pos);

            if (pos < source_.size() && source_[pos] == '(') {
                const std::size_t condition_end = find_matching(source_, pos, '(', ')');
                pos = condition_end == std::string::npos ? source_.size() : condition_end + 1;
            }

            return parse_statement(pos, loop_depth, variable_depth);
        }

        StatementSummary parse_do_statement(
            std::size_t& pos,
            int loop_depth,
            int variable_depth) {
            pos += 2;
            StatementSummary body = parse_statement(pos, loop_depth, variable_depth);
            skip_whitespace(source_, pos);

            if (is_keyword_at(source_, pos, "while")) {
                pos += 5;
                skip_whitespace(source_, pos);

                if (pos < source_.size() && source_[pos] == '(') {
                    const std::size_t condition_end = find_matching(source_, pos, '(', ')');
                    pos = condition_end == std::string::npos ? source_.size() : condition_end + 1;
                }

                skip_whitespace(source_, pos);
                if (pos < source_.size() && source_[pos] == ';') {
                    ++pos;
                }
            }

            return body;
        }

        StatementSummary parse_for_statement(
            std::size_t& pos,
            int loop_depth,
            int variable_depth) {
            pos += 3;
            skip_whitespace(source_, pos);

            bool is_variable_loop = false;

            if (pos < source_.size() && source_[pos] == '(') {
                const std::size_t header_end = find_matching(source_, pos, '(', ')');

                if (header_end != std::string::npos) {
                    const std::string header = source_.substr(pos + 1, header_end - pos - 1);
                    is_variable_loop = contains_variable_n(header);
                    pos = header_end + 1;
                } else {
                    pos = source_.size();
                }
            }

            owner_.register_loop(is_variable_loop, variable_depth);

            const StatementSummary body = parse_statement(
                pos,
                loop_depth + 1,
                variable_depth + (is_variable_loop ? 1 : 0));

            return {
                body.degree + (is_variable_loop ? 1 : 0),
                std::max(1, body.operations)
            };
        }

        StatementSummary parse_simple_statement(std::size_t& pos, int loop_depth) {
            const std::size_t start = pos;
            int parentheses_balance = 0;

            while (pos < source_.size()) {
                if (source_[pos] == '(') {
                    ++parentheses_balance;
                } else if (source_[pos] == ')' && parentheses_balance > 0) {
                    --parentheses_balance;
                } else if (source_[pos] == ';' && parentheses_balance == 0) {
                    ++pos;
                    break;
                } else if ((source_[pos] == '{' || source_[pos] == '}') && parentheses_balance == 0) {
                    break;
                }

                ++pos;
            }

            const std::string statement_text = source_.substr(start, pos - start);
            StatementSummary summary = owner_.analyze_statement_calls(
                statement_text,
                current_function_key_,
                call_stack_);
            summary.operations += count_elementary_operations(statement_text);

            if (loop_depth > 0) {
                owner_.result_.loop_body_operations += summary.operations;
            }

            return summary;
        }

        AnalyzerParser& owner_;
        const std::string& source_;
        const std::string& current_function_key_;
        std::vector<std::string>& call_stack_;
    };

    StatementSummary analyze_function(
        const FunctionDefinition& function,
        std::vector<std::string>& call_stack) {
        const std::string key = make_function_key(function.name, function.arity);
        const auto cache_it = summary_cache_.find(key);
        if (cache_it != summary_cache_.end()) {
            return cache_it->second;
        }

        BodyParser parser(*this, function.body, key, call_stack);
        const StatementSummary summary = parser.parse_sequence();
        summary_cache_.emplace(key, summary);
        return summary;
    }

    StatementSummary analyze_statement_calls(
        const std::string& statement_text,
        const std::string& current_function_key,
        std::vector<std::string>& call_stack) {
        StatementSummary summary;

        for (std::size_t pos = 0; pos < statement_text.size(); ++pos) {
            if (!is_identifier_char(statement_text[pos]) ||
                std::isdigit(static_cast<unsigned char>(statement_text[pos])) != 0) {
                continue;
            }

            const std::size_t identifier_start = pos;
            while (pos < statement_text.size() && is_identifier_char(statement_text[pos])) {
                ++pos;
            }

            const std::string name = statement_text.substr(identifier_start, pos - identifier_start);
            if (is_function_keyword(name) || name == "return" || name == "sizeof") {
                --pos;
                continue;
            }

            std::size_t cursor = pos;
            skip_whitespace(statement_text, cursor);
            if (cursor >= statement_text.size() || statement_text[cursor] != '(') {
                --pos;
                continue;
            }

            const std::size_t args_end = find_matching(statement_text, cursor, '(', ')');
            if (args_end == std::string::npos) {
                --pos;
                continue;
            }

            ++result_.function_call_count;

            const std::string arguments_text =
                statement_text.substr(cursor + 1, args_end - cursor - 1);
            const int arity = count_parameters(arguments_text);
            const std::string key = make_function_key(name, arity);

            const auto function_it = functions_.find(key);
            if (function_it != functions_.end()) {
                if (key == current_function_key ||
                    std::find(call_stack.begin(), call_stack.end(), key) != call_stack.end()) {
                    ++result_.recursion_skipped_count;
                } else {
                    ++result_.resolved_function_call_count;
                    call_stack.push_back(key);
                    const StatementSummary callee = analyze_function(function_it->second, call_stack);
                    call_stack.pop_back();

                    summary.degree = std::max(summary.degree, callee.degree);
                    summary.operations += std::max(1, callee.operations);
                }
            }

            pos = args_end;
        }

        return summary;
    }

    void register_loop(bool is_variable_loop, int variable_depth) {
        ++result_.loop_count;

        if (is_variable_loop) {
            ++result_.variable_loop_count;
            result_.max_nesting_depth = std::max(result_.max_nesting_depth, variable_depth + 1);
        } else {
            ++result_.constant_loop_count;
        }
    }

    std::unordered_map<std::string, FunctionDefinition> functions_;
    ParseOptions options_;
    ParseResult result_;
    std::unordered_map<std::string, StatementSummary> summary_cache_;
};

}

ParseResult parse_control_structures(const std::string& source, const ParseOptions& options) {
    return AnalyzerParser(source, options).parse();
}

}
