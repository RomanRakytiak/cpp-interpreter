#ifndef CORE_H
#define CORE_H

#include <memory>
#include <vector>
#include <functional>
#include <utility>
#include <cassert>
#include <limits>
#include <optional>
#include <span>
#include <string>
#include <variant>
#include <ostream>
#include <cstdint>
#include <unordered_map>

namespace project {
    struct Context;
    struct ASTExpression;
    class ProgramBuilder;
    class Symbol;
    class Literal;

    struct Unit {
        bool operator==(const Unit&) const { return true; }
        friend std::ostream& operator<<(std::ostream& os, const Unit& n) { return os << "()"; }
    };

    struct Variant {
        using function = std::function<Variant(Variant)>;
        using map_index = long long;
        using map = std::unordered_map<map_index, std::shared_ptr<Variant>>;

        Variant();
        explicit Variant(long long value);
        explicit Variant(size_t value);
        explicit Variant(double value);
        explicit Variant(function func);
        explicit Variant(map map);
        Variant(const Variant&) = default;
        Variant(Variant&&) = default;
        Variant& operator=(const Variant&) = default;
        Variant& operator=(Variant&&) = default;

        static Variant empty_map() { return Variant(map{}); }

        template<std::integral T>
        static Variant integer(T value);

        static Variant floating_point(const float value) { return Variant(static_cast<double>(value));}
        static Variant floating_point(const double value) { return Variant(static_cast<double>(value));}
        static Variant floating_point(const long double value) { return Variant(static_cast<double>(value));}

        [[nodiscard]] bool is_inlined() const;
        [[nodiscard]] bool jumps_on_jump_if_positive() const;

        [[nodiscard]] std::optional<Variant> overflow_add(const Variant &other) const;
        [[nodiscard]] std::optional<Variant> overflow_sub(const Variant &other) const;
        [[nodiscard]] std::optional<Variant> overflow_mul(const Variant &other) const;
        [[nodiscard]] std::optional<Variant> overflow_div(const Variant &other) const;
        [[nodiscard]] std::optional<Variant> overflow_mod(const Variant &other) const;
        [[nodiscard]] std::optional<Variant> negate() const;

        [[nodiscard]] std::optional<Variant> get(const Variant &index) const;
        [[nodiscard]] bool set(const Variant &index, Variant value);

        [[nodiscard]] Variant or_else(const Variant& other) const;
        [[nodiscard]] Variant and_else(const Variant& other) const;

        [[nodiscard]] std::optional<size_t> try_to_index() const;
        void unchecked_delete();
        Variant call(const std::span<Variant>& arguments);
        void swap(Variant& other) noexcept { std::swap(data, other.data); }

        bool operator==(const Variant& other) const;
        friend std::ostream& operator<<(std::ostream& stream, const Variant& variant);

    private:
        std::variant<size_t,
            long long,
            double,
            function,
            Unit,
            map
        > data;
    public:
        using variant = decltype(data);
    };

    struct Context {
        std::unordered_map<std::string, Symbol*> table;

        Context() = default;
        Context(const Context& other) = default;
        Context(Context&& other) = default;


        [[nodiscard]] Symbol* evaluate(const std::string& name) const;
        void new_symbol(const std::string &name, Symbol& symbol);

    };

    struct ProjectError : std::logic_error {
        explicit ProjectError(const std::string& msg) : logic_error(msg) {}
    };

    struct SyntaxError : ProjectError {
        explicit SyntaxError(const std::string& msg) : ProjectError(msg) {}
    };

    struct MissingExpression final : SyntaxError {
        explicit MissingExpression(const std::string& after)
            : SyntaxError("Missing expression somewhere after " + after) {}
    };

    struct MissingToken final : SyntaxError {
        explicit MissingToken(const std::string& token, const std::string& after)
            : SyntaxError("Missing token '" + token + "' somewhere after " + after) {}
    };

    struct UndefinedSymbol final : SyntaxError {
        explicit UndefinedSymbol(const std::string& name)
            : SyntaxError("Undefined symbol " + name) {}
    };

    struct UnexpectedEndOfFile final : SyntaxError {
        explicit UnexpectedEndOfFile()
            : SyntaxError("File ended, but it should not") {}
    };

    struct InvalidSymbolAddition final : ProjectError {
        InvalidSymbolAddition(const Symbol& first, const Symbol& second);
        InvalidSymbolAddition(const Variant& first, const Variant& second);
    };

    struct InvalidSymbolSubtraction final : ProjectError {
        InvalidSymbolSubtraction(const Symbol& first, const Symbol& second);
        InvalidSymbolSubtraction(const Variant& first, const Variant& second);
    };

    struct InvalidSymbolMultiplication final : ProjectError {
        InvalidSymbolMultiplication(const Symbol& first, const Symbol& second);
        InvalidSymbolMultiplication(const Variant& first, const Variant& second);
    };

    struct InvalidSymbolDivision final : ProjectError {
        InvalidSymbolDivision(const Symbol& first, const Symbol& second);
        InvalidSymbolDivision(const Variant& first, const Variant& second);
    };

    struct InvalidSymbolModulo final : ProjectError {
        InvalidSymbolModulo(const Symbol& first, const Symbol& second);
        InvalidSymbolModulo(const Variant& first, const Variant& second);
    };

    struct InvalidNumberOfArguments final : ProjectError {
        InvalidNumberOfArguments(const Symbol& function, size_t wants, size_t got);
    };


}

template<std::integral T>
project::Variant project::Variant::integer(T value) {
    if constexpr (std::is_signed_v<T>) {
        assert(std::numeric_limits<long long>::min() <= value);
        assert(value <= std::numeric_limits<long long>::max());
        return Variant(static_cast<long long>(value));
    }
    assert(value <= std::numeric_limits<size_t>::max());
    return Variant(static_cast<size_t>(value));
}

#endif //CORE_H

