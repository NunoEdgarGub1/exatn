#ifndef PARSER_TAPROLPARSER_HPP_
#define PARSER_TAPROLPARSER_HPP_

#include "antlr4-runtime.h"
#include "num_server.hpp"

namespace exatn {

namespace parser {

/**
 * An Antlr error listener for handling parsing errors.
 */
class TAProLErrorListener : public antlr4::BaseErrorListener {
public:
  void syntaxError(antlr4::Recognizer *recognizer,
                   antlr4::Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    std::ostringstream output;
    output << "Invalid TAProL source: ";
    output << "line " << line << ":" << charPositionInLine << " " << msg;
    std::cerr << output.str() << "\n";
  }
};

/**
 */
class TAProLInterpreter {
public:
  virtual ~TAProLInterpreter() {}
  void interpret(const std::string &src);
};

} // namespace parser
} // namespace exatn

#endif
