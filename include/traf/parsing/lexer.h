#ifndef LEXER_H
#define LEXER_H

#include "core.h"

namespace traf {

    using character = char;

    using digits = integer_range_t<character, '0', '9' + 1>;
    using hexadecimal = integer_range_t<character, 'a', 'f' + 1>;
    using octal = integer_range_t<character, '0', '8' + 1>;
    using decimal = integer_range_t<character, '0', '9' + 1>;
    using lowercase = integer_range_t<character, 'a', 'z' + 1>;
    using uppercase = integer_range_t<character, 'A', 'Z' + 1>;
    using letter = template_join_t<lowercase, uppercase>;
    using uppercase = integer_range_t<character, 'A', 'Z' + 1>;
    using special = std::integer_sequence<character,
        '+', '-', '*', '/', '%',
        ':', '@', '#', '^',
        '=', '<', '>', '&', '|',
        ',', '.'>;
    using prefix_operators = std::integer_sequence<character, '!', '\'', '(', '[', '{'>;
    using suffix_operators = std::integer_sequence<character, '?', ')', ']', '}'>;
    using operators = template_join_t<prefix_operators, suffix_operators, special>;
    using space = std::integer_sequence<character, ' ', '\t'>;



    struct location {
        size_t line = 1;
        size_t column = 1;
        size_t character = 0;
        std::string_view source;

        void next_line();
        void next_column();
    };

    struct token {
        enum type {
            NONE,
            NAME,
            NUMBER,
            OPERATOR,
            STRING,
            COMMENT,
        } type = NONE;
        std::string_view value;
        location start;
        location end;

        static token parse(const std::string &data, const location &start);
        bool operator==(const token &other) const;
        bool operator==(const char *other) const;
        bool operator==(const std::string &value) const { return *this == value.c_str(); }
        [[nodiscard]] bool is_number() const { return type == NUMBER; }
        [[nodiscard]] bool is_string() const { return type == STRING; }
        [[nodiscard]] bool is_operator() const { return type == OPERATOR; }
        [[nodiscard]] bool is_comment() const { return type == COMMENT; }
        [[nodiscard]] bool is_name() const { return type == NAME; }
        [[nodiscard]] bool is_valid() const { return type != NONE; }
    };

    struct token_group {
        union {
            token single_token;
            std::vector<token_group> tokens;
        };
        enum type {
            NONE,
            SINGLE,
            STREAM,
        } type = NONE;

        token_group();
        explicit token_group(const token &token);
        explicit token_group(std::vector<token_group> tokens);
        ~token_group();

        [[nodiscard]] bool is_valid() const { return type != NONE; }
        [[nodiscard]] bool is_stream() const { return type == STREAM; }

        token* operator->() { return &**this; }
        const token* operator->() const { return &**this; }
        token& operator*();
        const token& operator*() const;
        token_group& operator[](int index);
        const token_group& operator[](int index) const;

        bool operator==(const token_group &other) const;
        bool operator==(const std::vector<token_group> &other) const { return type == STREAM && tokens == other; }
        template<typename T>
        bool operator==(T &&other) const { return type == SINGLE && single_token == std::forward<T>(other); }

        static token_group parse(lexer& lexer);
    };

    struct lexer {
        explicit lexer(std::string data, const std::string& source);

        token next_token();
        [[nodiscard]] token peek_token() const;
        void skip_token();
        void push_back(const token &token);
        token_group next_group();
        token_group peek_group();
        void skip_group();
        void push_back(const token_group &group);

    private:
        const std::string data;
        location current;
    };

    std::ostream &operator<<(std::ostream &os, const token &token);
}


#endif //LEXER_H
