#include "static_analyzer/parser.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace algochecker::static_analyzer {
namespace {

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

std::size_t find_matching(const std::string& source, std::size_t open_pos, char open_char, char close_char) {
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

        if (text[pos] == '+' || text[pos] == '-' || text[pos] == '*' || text[pos] == '/') {
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

class AnalyzerParser {
public:
    explicit AnalyzerParser(const std::string& source) : source_(source) {
    }

    ParseResult parse() {
        std::size_t pos = 0;

        while (pos < source_.size()) {
            pos = parse_statement(pos, 0, 0, true);
        }

        result_.estimated_operations = result_.loop_body_operations;
        return result_;
    }

private:
    std::size_t parse_statement(
        std::size_t pos,
        int loop_depth,
        int variable_depth,
        bool measure) {
        skip_whitespace(source_, pos);

        if (pos >= source_.size()) {
            return pos;
        }

        if (source_[pos] == '{') {
            return parse_block(pos, loop_depth, variable_depth, measure);
        }

        if (is_keyword_at(source_, pos, "for")) {
            return parse_for_statement(pos, loop_depth, variable_depth, measure);
        }

        if (is_keyword_at(source_, pos, "while")) {
            return parse_while_statement(pos, loop_depth, variable_depth, measure);
        }

        if (is_keyword_at(source_, pos, "do")) {
            return parse_do_while_statement(pos, loop_depth, variable_depth, measure);
        }

        if (is_keyword_at(source_, pos, "if")) {
            return parse_if_statement(pos, loop_depth, variable_depth, measure);
        }

        if (is_keyword_at(source_, pos, "else")) {
            return parse_else_statement(pos, loop_depth, variable_depth, measure);
        }

        return parse_simple_statement(pos, loop_depth, measure);
    }

    std::size_t parse_block(
        std::size_t pos,
        int loop_depth,
        int variable_depth,
        bool measure) {
        ++pos;

        while (pos < source_.size()) {
            skip_whitespace(source_, pos);

            if (pos >= source_.size()) {
                break;
            }

            if (source_[pos] == '}') {
                return pos + 1;
            }

            pos = parse_statement(pos, loop_depth, variable_depth, measure);
        }

        return pos;
    }

    std::size_t parse_simple_statement(std::size_t pos, int loop_depth, bool measure) {
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

        if (measure && loop_depth > 0 && pos > start) {
            result_.loop_body_operations += count_elementary_operations(source_.substr(start, pos - start));
        }

        return pos;
    }

    std::size_t parse_if_statement(
        std::size_t pos,
        int loop_depth,
        int variable_depth,
        bool measure) {
        pos += 2;
        skip_whitespace(source_, pos);

        if (pos < source_.size() && source_[pos] == '(') {
            const std::size_t condition_end = find_matching(source_, pos, '(', ')');
            pos = condition_end == std::string::npos ? source_.size() : condition_end + 1;
        }

        pos = parse_statement(pos, loop_depth, variable_depth, measure);
        skip_whitespace(source_, pos);

        if (is_keyword_at(source_, pos, "else")) {
            pos = parse_else_statement(pos, loop_depth, variable_depth, measure);
        }

        return pos;
    }

    std::size_t parse_else_statement(
        std::size_t pos,
        int loop_depth,
        int variable_depth,
        bool measure) {
        pos += 4;
        return parse_statement(pos, loop_depth, variable_depth, measure);
    }

    std::size_t parse_for_statement(
        std::size_t pos,
        int loop_depth,
        int variable_depth,
        bool measure) {
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

        if (measure) {
            register_loop(is_variable_loop, variable_depth);
        }

        return parse_statement(
            pos,
            loop_depth + 1,
            variable_depth + (is_variable_loop ? 1 : 0),
            measure);
    }

    std::size_t parse_while_statement(
        std::size_t pos,
        int loop_depth,
        int variable_depth,
        bool measure) {
        pos += 5;
        skip_whitespace(source_, pos);

        bool is_variable_loop = false;

        if (pos < source_.size() && source_[pos] == '(') {
            const std::size_t condition_end = find_matching(source_, pos, '(', ')');

            if (condition_end != std::string::npos) {
                const std::string condition = source_.substr(pos + 1, condition_end - pos - 1);
                is_variable_loop = contains_variable_n(condition);
                pos = condition_end + 1;
            } else {
                pos = source_.size();
            }
        }

        if (measure) {
            register_loop(is_variable_loop, variable_depth);
        }

        return parse_statement(
            pos,
            loop_depth + 1,
            variable_depth + (is_variable_loop ? 1 : 0),
            measure);
    }

    std::size_t parse_do_while_statement(
        std::size_t pos,
        int loop_depth,
        int variable_depth,
        bool measure) {
        pos += 2;

        std::size_t body_start = pos;
        body_start = parse_statement(body_start, loop_depth + 1, variable_depth, false);

        std::size_t cursor = body_start;
        skip_whitespace(source_, cursor);

        bool is_variable_loop = false;

        if (is_keyword_at(source_, cursor, "while")) {
            cursor += 5;
            skip_whitespace(source_, cursor);

            if (cursor < source_.size() && source_[cursor] == '(') {
                const std::size_t condition_end = find_matching(source_, cursor, '(', ')');

                if (condition_end != std::string::npos) {
                    const std::string condition = source_.substr(cursor + 1, condition_end - cursor - 1);
                    is_variable_loop = contains_variable_n(condition);
                    cursor = condition_end + 1;
                } else {
                    cursor = source_.size();
                }
            }
        }

        skip_whitespace(source_, cursor);

        if (cursor < source_.size() && source_[cursor] == ';') {
            ++cursor;
        }

        if (measure) {
            register_loop(is_variable_loop, variable_depth);
            parse_statement(
                pos,
                loop_depth + 1,
                variable_depth + (is_variable_loop ? 1 : 0),
                true);
        }

        return cursor;
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

    const std::string& source_;
    ParseResult result_;
};

}

ParseResult parse_control_structures(const std::string& source) {
    return AnalyzerParser(source).parse();
}

}
