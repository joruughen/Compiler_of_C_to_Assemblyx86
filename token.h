#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
    enum Type {
        INCLUDE, STDIOH , PLUS, MINUS, MUL, DIV, AND, OR, NUM, ERR,
        PD, PI, CBI, CBD, STRING, ID, PRINTF, ASSIGN,
        PC, LT, LE, GT, GE, EQ, NEQ, NOT,
        IF, ELSE, WHILE, FOR,
        COMA, PCOMA, RETURN, END
    };
//CBI = curly brackets {

    Type type;
    std::string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const std::string& source, int first, int last);

    friend std::ostream& operator<<(std::ostream& outs, const Token& tok);
    friend std::ostream& operator<<(std::ostream& outs, const Token* tok);
};

#endif // TOKEN_H