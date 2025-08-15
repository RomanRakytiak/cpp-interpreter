#include <utility>

#include "lexer.h"


using namespace traf;


static bool is_newline(const char c) {
    return c == '\n';
}

static bool is_ignored(const char c) {
    return template_find_value<space>(c);
}

static bool is_digit(const char c) {
    return template_find_value<digits>(c);
}

static bool is_lowercase(const char c) {
    return template_find_value<lowercase>(c);
}

static bool is_uppercase(const char c) {
    return template_find_value<uppercase>(c);
}

static bool is_letter(const char c) {
    return template_find_value<letter>(c);
}

static character at(const std::string& data, size_t pos) {
    if (pos >= data.size())
        return '\0';
    return data[pos];
}

static bool parse_operator(token &result, const std::string &data) {
    if (template_find_value<template_join_t<suffix_operators>>(at(data, result.end.character))) {
        result.end.next_column();
        result.type = token::OPERATOR;
        return true;
    }
    if (template_find_value<template_join_t<prefix_operators, special>>(at(data, result.end.character)) == false)
        return false;
    result.end.next_column();
    while (character c = at(data, result.end.character)) {
        if (template_find_value<template_join_t<special>>(at(data, result.end.character)))
            result.end.next_column();
        else
            break;
    }
    if (template_find_value<template_join_t<suffix_operators>>(at(data, result.end.character)))
        result.end.next_column();
    result.type = token::OPERATOR;
    return true;
}

static bool parse_comment(token &result, const std::string &data) {
    if (at(data, result.end.character) != ';')
        return false;
    result.end.next_column();
    while (const character c = at(data, result.end.character)) {
        if (is_newline(c)) {
            result.end.next_line();
            break;
        }
        result.end.next_column();
    }
    result.type = token::COMMENT;
    return true;
}

static bool parse_name(token &result, const std::string &data) {
    if (is_letter(at(data, result.end.character)) == false)
        return false;
    result.end.next_column();
    while (character c = at(data, result.end.character)) {
        if (is_letter(at(data, result.end.character)) == false)
            break;
        result.end.next_column();
    }
    result.type = token::NAME;
    return true;
}

static bool parse_string(token &result, const std::string& data) {
    if (at(data, result.end.character) != '(')
        return false;
    result.end.next_column();
    if (at(data, result.end.character) == '"') {
        result.end.next_column();
        while(character c = at(data, result.end.character)) {
            if (c == '"' && at(data, result.end.character + 1) == ')')
                break;
            if (is_newline(c))
                result.end.next_line();
            else
                result.end.next_column();
        }
        if (at(data, result.end.character) != '"') {
            result.start = result.end;
            return false;
        }
        result.end.next_column();
        if (at(data, result.end.character) != '('){
            result.start = result.end;
            return false;
        }
        result.end.next_column();
    }
    else {
        result.start = result.end;
        return false;
    }
    result.type = token::STRING;
    return true;
}

static bool parse_number(token &result, const std::string& data) {
    if (is_digit(at(data, result.end.character)) == false)
        return false;
    result.end.next_column();
    while (character c = at(data, result.end.character)) {
        if (is_digit(c))
            result.end.next_column();
        else if (c == '_' || c == '\'')
            result.end.next_column();
        else
            break;
    }
    if (at(data, result.end.character) == '.' && is_digit(at(data, result.end.character + 1))) {
        result.end.next_column();
        while (character c = at(data, result.end.character)) {
            if (is_digit(c))
                result.end.next_column();
            else if (c == '_' || c == '\'')
                result.end.next_column();
            else
                break;
        }
    }
    result.type = token::NUMBER;
    return true;
}

void location::next_line()  {
    line++;
    character++;
    column = 1;
}

void location::next_column()  {
    column++;
    character++;
}

token token::parse(const std::string& data, const location &start) {
    token result;
    result.start = start;
    result.end = start;
    while (const char c = at(data, result.start.character)) {
        if (is_newline(c)) {
            result.start.next_line();
            result.end.next_line();
        }
        else if (is_ignored(c) == false)
            break;
        result.start.next_column();
        result.end.next_column();
    }
    if (parse_number(result, data)
        || parse_string(result, data)
        || parse_name(result, data)
        || parse_comment(result, data)
        || parse_operator(result, data)) {
        result.value = std::string_view(
            data.begin() + result.start.character,
            data.begin() + result.end.character
        );
        return result;
    }
    result.type = NONE;
    return result;
}

bool token::operator==(const token &other) const {
    return value == other.value;
}

bool token::operator==(const char *other) const {
    for (int i = 0; i < value.length(); i++) {
        if (value[i] != other[i])
            return false;
        if (other[i] == '\0')
            return false;
    }
    return true;
}

token_group::token_group()
    : type(NONE) {
}

token_group::token_group(const token &token)
    : single_token(token), type(SINGLE) {

}

token_group::token_group(std::vector<token_group> tokens)
    : tokens(std::move(tokens)), type(STREAM) {
}

token_group::~token_group() {
    if (type == STREAM)
        tokens.~vector();
}

token & token_group::operator*() {
    if (type == SINGLE)
        return single_token;
    if (tokens.size() == 1)
        return *tokens[0];
    throw std::runtime_error("token_group::operator*()");
}

const token & token_group::operator*() const {
    if (type == SINGLE)
        return single_token;
    if (tokens.size() == 1)
        return *tokens[0];
    throw std::runtime_error("token_group::operator*()");
}

token_group & token_group::operator[](const int index) {
    if (type == SINGLE) {
        if (index == 0 || index == -1)
            return *this;
        throw std::out_of_range("token_group::operator[]");
    }
    if (0 <= index) {
        if (index < tokens.size())
            return tokens[index];
    }
    else if (-index <= tokens.size())
        return tokens[tokens.size() - index];
    throw std::out_of_range("token_group::operator[]");
}

const token_group & token_group::operator[](const int index) const {
    if (type == SINGLE)
        throw std::runtime_error("token_group::operator[]()");
    return tokens[index];
}

bool token_group::operator==(const token_group &other) const {
    if (type != other.type)
        return false;
    switch (type) {
        case SINGLE:
            return single_token == other.single_token;
        case STREAM:
            return tokens == other.tokens;
        case NONE:
            return true;
    }
    return false;
}

static bool group_starter(const token &token) {
    const auto starters = {
        "begin", "let", "theorem", "assume", "theory", "implementation"
    };
    if (token.is_name()) {
        for (const auto starter: starters)
            if (token == starter)
                return true;
    }
    return false;
}

static bool group_terminator(const token &token) {
    const auto terminators = {
        "end", "in"
    };
    if (token.is_name()) {
        for (const auto terminator: terminators)
            if (token == terminator)
                return true;
    }
    return false;
}

static void parse_group(std::vector<token_group> &result, lexer &lexer) {
    while (true) {
        auto token = lexer.next_token();
        if (token.is_valid() == false)
            return;
        result.emplace_back(token);
        if (group_terminator(token))
            return;
    }
}

token_group token_group::parse(lexer &lexer) {
    auto token = lexer.next_token();
    if (token.is_valid() == false)
        return {};
    if (group_terminator(token))
        return token_group(token);
    if (lexer.peek_token().is_valid()) {
        std::vector<token_group> result;
        result.emplace_back(token);
        parse_group(result, lexer);
        return token_group(result);
    }
    return token_group(token);
}

lexer::lexer(std::string data, const std::string& source)
    : data(std::move(data)) {
    current.character = 0;
    current.line = 1;
    current.column = 1;
    current.source = source;
}

token lexer::next_token() {
    token token = peek_token();
    current = token.end;
    return token;
}

token lexer::peek_token() const {
    return token::parse(data, current);
}

void lexer::skip_token() {
    next_token();
}

void lexer::push_back(const token &token) {
    current = token.start;
}

token_group lexer::next_group() {
    return token_group::parse(*this);
}

token_group lexer::peek_group() {
    auto result = token_group::parse(*this);
    push_back(result);
    return result;
}

void lexer::skip_group() {
    next_group();
}


std::ostream & traf::operator<<(std::ostream &os, const token &token) {
    os << "Token";
    switch (token.type) {
        case token::STRING:
            os << "String";
            break;
        case token::NUMBER:
            os << "Number";
            break;
        case token::NAME:
            os << "Name";
            break;
        case token::COMMENT:
            os << "Comment";
            break;
        case token::OPERATOR:
            os << "Operator";
            break;
    }
    os << "("<< token.value << ")";
    return os;
}


