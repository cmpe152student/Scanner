#include <cctype>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

enum token_type {
  AND,
  ARRAY,
  ASM,
  BEGIN,
  BREAK,
  CASE,
  CONST,
  CONSTRUCTOR,
  CONTINUE,
  DESTRUCTOR,
  DIV,
  DO,
  DOWNTO,
  ELSE,
  END,
  FALSE,
  FILE_,
  FOR,
  FUNCTION,
  GOTO,
  IF,
  IMPLEMENTATION,
  IN,
  INLINE,
  INTERFACE,
  LABEL,
  MOD,
  NIL,
  NOT,
  OBJECT,
  OF,
  ON,
  OPERATOR,
  OR,
  PACKED,
  PROCEDURE,
  PROGRAM,
  RECORD,
  REPEAT,
  SET,
  SHL,
  SHR,
  STRING,
  THEN,
  TO,
  TRUE,
  TYPE,
  UNIT,
  UNTIL,
  USES,
  VAR,
  WHILE,
  WITH,
  XOR,
  INTEGER,
  REAL,
  INDENTIFIER,
  PLUSOP,
  END_OF_FILE,
  MINUSOP,
  MULTOP,
  DIVOP,
  ASSIGN,
  EQUAL,
  NE,
  LTEQ,
  GTEQ,
  LT,
  GT,
  PLUSEQUAL,
  MINUSEQUAL,
  MULTEQUAL,
  DIVEQUAL,
  CARAT,
  SEMICOLON,
  COMMA,
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  LBRACE,
  RBRACE,
  LCOMMENT,
  RCOMMENT,
  COLON,
};

std::vector<std::string> token_string{
    "AND",         "ARRAY",
    "ASM",         "BEGIN",
    "BREAK",       "CASE",
    "CONST",       "CONSTRUCTOR",
    "CONTINUE",    "DESTRUCTOR",
    "DIV",         "DO",
    "DOWNTO",      "ELSE",
    "END",         "FALSE",
    "FILE_",       "FOR",
    "FUNCTION",    "GOTO",
    "IF",          "IMPLEMENTATION",
    "IN",          "INLINE",
    "INTERFACE",   "LABEL",
    "MOD",         "NIL",
    "NOT",         "OBJECT",
    "OF",          "ON",
    "OPERATOR",    "OR",
    "PACKED",      "PROCEDURE",
    "PROGRAM",     "RECORD",
    "REPEAT",      "SET",
    "SHL",         "SHR",
    "STRING",      "THEN",
    "TO",          "TRUE",
    "TYPE",        "UNIT",
    "UNTIL",       "USES",
    "VAR",         "WHILE",
    "WITH",        "XOR",
    "INTEGER",     "REAL",
    "INDENTIFIER", "PLUSOP",
    "END_OF_FILE", "MINUSOP",
    "MULTOP",      "DIVOP",
    "ASSIGN",      "EQUAL",
    "NE",          "LTEQ",
    "GTEQ",        "LT",
    "GT",          "PLUSEQUAL",
    "MINUSEQUAL",  "MULTEQUAL",
    "DIVEQUAL",    "CARAT",
    "SEMICOLON",   "COMMA",
    "LPAREN",      "RPAREN",
    "LBRACKET",    "RBRACKET",
    "LBRACE",      "RBRACE",
    "LCOMMENT",    "RCOMMENT",
    "COLON",
};

struct Token {
  token_type type;
  std::string lex;
  int line;
};

std::ostream &operator<<(std::ostream &os, const Token &token) {
  os << token_string[token.type];
  return os;
}

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

  char peek() {
    if (is_at_end()) return '\0';
    return source[current];
  }

  bool match(char expected) {
    if (is_at_end()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
  }

  void consume_block_comment() {
    /** discard '*' */
    this->advance();
    this -> add_token(LCOMMENT);
    for (;;) {
      if (is_at_end()) {
        std::cerr << "Unclosed comment (* *) block line: " << this -> line << '\n';
	exit(69);
      }
      char c = this->advance();
      if (c == '\n') {
        this->line++;
      } else if (c == '*' and this->peek() == ')') {
	this -> add_token(RCOMMENT);
	this->advance();
        return;
      } else if (c == '(' and this->peek() == '*') {
        this->consume_block_comment();
	
      }
    }
  }

  // void identifier() {
  //   while () {

  //   }
  // }

 public:
  Scanner(std::string &Source)
      : source(Source), start(0), current(0), line(1){};

  std::vector<Token> scanTokens() {
    while (!is_at_end()) {
      start = current;
      this->scan_token();
    }

    return tokens;
  };

  bool is_alpha(char c) { return c == '_' or isalpha(c); }

  bool is_alpha_numeric(char c) { return is_alpha(c) or std::isdigit(c); }

  void scan_token() {
    char c = advance();
    if (c == '+') {
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
    } else if (c == ' ' || c == '\r' || c == '\t') {
    } else if (c == '\n') {
      line++;
    } else if (c == '/') {
      if (match('/')) {
        // A comment goes until the end of the line.
        while (peek() != '\n' && !is_at_end()) advance();
      } else if (match('=')) {
        add_token(DIVEQUAL);
      } else {
        add_token(DIVOP);
      }
    } else {
      if (std::isdigit(c)) {
        // number();
      } else if (isalpha(c)) {
        // identifier();
      } else {
        std::cerr << "Unexpected character\n";
      }
    }
  }
};

void run(std::string &source) {
  Scanner scanner(source);
  for (const Token &token : scanner.scanTokens()) std::cout << token << '\n';
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    std::cerr << "USAGE: scan [something.pas]\n";
    return 1;
  }

  std::string input{std::istreambuf_iterator<char>{std::cin}, {}};
  run(input);
}
