#ifndef ERRORS_H
#define ERRORS_H

#include <iostream>

namespace traf {

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

#endif //ERRORS_H
