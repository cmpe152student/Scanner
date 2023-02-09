#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "misc.h"

//////////////////////////////////////////////////////////////////
// Token Object holds token type, its lex and the error number //
//////////////////////////////////////////////////////////////////
struct Token {
  token_type type;
  std::string lex;
  int line;
};

///////////////////////////////////////////////////////////////////////////////
// Token objects held in an optional type. Makes checking for nullity easier //
///////////////////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &os, const Token &token) {
  os << token_string[token.type];
  return os;
}

////////////////////////////////////////////////////////
// Gives labels to Tokens given an source std::string //
////////////////////////////////////////////////////////
class Scanner {
  std::string source;
  std::vector<Token> tokens;
  int start, current, line;

  bool is_at_end() { return current >= source.length(); }
  char advance() { return source[current++]; }
  void add_token(token_type type) {
    tokens.push_back(Token{
        type,
        source.substr(this->start, this->current),
        this->line,
    });
  }

  ////////////////////////////////////
  // Looks at the current character //
  ////////////////////////////////////
  char peek() {
    if (is_at_end()) return '\0';
    return source[current];
  }

  /////////////////////////////////////////////////////////////////////////////
  // Procedure that checks if character matches expected, if it does advance //
  // current  								     //
  /////////////////////////////////////////////////////////////////////////////
  bool match(char expected) {
    if (is_at_end()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
  }

  /////////////////////////////////////////////////////
  // Procedure skips all tokens within a comment box //
  /////////////////////////////////////////////////////
  void consume_block_comment() {
    /** discard '*' */
    this->advance();
    this->add_token(LCOMMENT);

    for (;;) {
      if (is_at_end()) {
        std::cerr << "Unclosed comment (* *) block line: " << this->line
                  << '\n';
        exit(69);
      }
      char c = this->advance();
      if (c == '\n') {
        this->line++;
      } else if (c == '*' and this->peek() == ')') {
        this->add_token(RCOMMENT);
        this->advance();
        return;
      } else if (c == '(' and this->peek() == '*') {
        this->consume_block_comment();
      }
    }
  }

  /////////////////////////////////////////////////////////////////
  // Reduced is_digit function. Errors related to std::isdigit.  //
  /////////////////////////////////////////////////////////////////
  bool is_digit(char c) { return c >= '0' and c <= '9'; }

  /////////////////////////
  // Added support for _ //
  /////////////////////////
  bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  ////////////////////////////////////////////////////////////////
  // Home cooked alphanumeric checker. Errors related to stdlib //
  ////////////////////////////////////////////////////////////////
  bool is_alpha_numeric(char c) {
    return this->is_alpha(c) or this->is_digit(c);
  }

  ///////////////////////////////////////////////////////////////////////
  // Function that looks ahead at next element. Does not change values //
  ///////////////////////////////////////////////////////////////////////
  char peek_next() {
    if (this->current + 1 >= this->source.length()) return '\0';
    return this->source[this->current + 1];
  }

  bool is_space(unsigned char c) {
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ||
            c == '\f');
  }

  ///////////////////////////////////////////////////////
  // Table driven identifier and reserved word checker //
  ///////////////////////////////////////////////////////
  void identifier() {
    while (this->is_alpha_numeric(this->peek())) this->advance();
    // Checks for reserved words
    std::string token = source.substr(this->start, this->current);
    std::istringstream ss{token};
    std::string text;
    ss >> text;
    // Upper case everything
    std::transform(text.cbegin(), text.cend(),
                   text.begin(),  // write to the same location
                   [](unsigned char c) { return std::toupper(c); });
    // Trim all non alpha numeric numbers
    // Check if the back of the string has anything added
    token_type type;
    if (auto search = keywords.find(text); search != keywords.end())
      type = search->second;
    else
      type = IDENTIFIER;

    this->add_token(type);
  }

  /////////////////////////////////////////////////////////////////////////////
  // FSM driven state machine where the next state determines whether number //
  // is real or integer							     //
  /////////////////////////////////////////////////////////////////////////////
  void number() {
    while (this->is_digit(this->peek())) this->advance();

    bool is_real = false;
    // look for fraction '.'
    if (peek() == '.' and this->is_digit(this->peek_next())) {
      is_real = true;
      this->advance();

      while (this->is_digit(this->peek())) this->advance();
    }

    if (is_real) {
      this->add_token(REAL);
    } else {
      this->add_token(INTEGER);
    }
  }

  ////////////////////////////////////////////////////
  // Table Driven FSM procedure for scanning tokens //
  ////////////////////////////////////////////////////
  void scan_token() {
    char c = advance();
    // skip whitespace
    if (c == ' ' || c == '\r' || c == '\t') {
      return;
    }
    // FSM finds operators first
    if (c == '+') {
      // Some cases require two operators
      // +=, +
      if (match('=')) {
        add_token(PLUSEQUAL);
      } else {
        add_token(PLUSOP);
      }
    } else if (c == '-') {
      // -=, -
      if (match('=')) {
        add_token(MINUSEQUAL);
      } else {
        add_token(MINUSOP);
      }
    } else if (c == '*') {
      // *=, *
      if (match('=')) {
        add_token(MULTEQUAL);
      } else {
        add_token(MULTOP);
      }
    } else if (c == ':') {
      // :=, =
      if (match('=')) {
        add_token(ASSIGN);
      } else {
        add_token(COLON);
      }
    } else if (c == '=') {
      add_token(EQUAL);
    } else if (c == '<') {
      //<=, <>, <
      if (match('=')) {
        add_token(LTEQ);
      } else if (match('>')) {
        add_token(NE);
      } else {
        add_token(LT);
      }
    } else if (c == '>') {
      //>, >=,
      if (match('=')) {
        add_token(GTEQ);
      } else {
        add_token(GT);
      }
    } else if (c == '^') {
      add_token(CARAT);
    } else if (c == ';') {
      add_token(SEMICOLON);
    } else if (c == ',') {
      add_token(COMMA);
    } else if (c == '(') {
      if (match('*')) {
        consume_block_comment();
      } else {
        add_token(LPAREN);
      }
    } else if (c == ')') {
      add_token(RPAREN);
    } else if (c == '[') {
      add_token(LBRACKET);
    } else if (c == ']') {
      add_token(RBRACKET);
    } else if (c == '{') {
      add_token(LBRACE);
    } else if (c == '}') {
      add_token(RBRACE);
    } else if (c == '\n') {
      line++;  // for debugging errors
    } else if (c == '/') {
      if (match('/')) {
        // A comment goes until the end of the line.
        while (peek() != '\n' && !is_at_end()) advance();
      } else if (match('=')) {
        add_token(DIVEQUAL);
      } else {
        add_token(DIVOP);
      }
    } else if (c == '\'') {
      // FSM for scanning string 'silly string' -> STRING
      while (peek() != '\'' and !is_at_end()) {
        if (peek() == '\n') line++;
        this->advance();
      }

      if (is_at_end()) {
        std::cerr << "Unterminated String line: " << line << std::endl;
        exit(21);
      }
      // Closing '
      this->advance();
      add_token(STRING);
    } else {
      // FSM for finding digit
      if (this->is_digit(c)) {
        this->number();
      } else if (this->is_alpha(c)) {
        // FSM for discovering identifier
        this->identifier();
      } else {
        std::cerr << "Unexpected character\n";
      }
    }
  }
  int tokens_index = 0;

 public:
  Scanner(std::string &Source) : source(Source), start(0), current(0), line(1) {
    this->scan_tokens();
  };

  // builds vector
  std::vector<Token> scan_tokens() {
    while (!is_at_end()) {
      this->start = this->current;
      this->scan_token();
    }

    return tokens;
  }

  std::optional<Token> next_token() {
    if (tokens_index < tokens.size()) return tokens[tokens_index++];
    return {};
  }
};

void run(std::string &source) {
  Scanner scanner(source);
  while (auto this_token = scanner.next_token()) {
    std::cout << *this_token << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    std::cerr << "USAGE: scanner [something.pas]\n";
    return 1;
  }

  std::string input{std::istreambuf_iterator<char>{std::cin}, {}};
  run(input);
}
