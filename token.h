#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
    enum Type {
        PLUS, MINUS, MUL, DIV, AND,OR, NUM, ERR, PD, PI, ID, PRINTF, INCLUDE, ASSIGN, PC,LT, LE, EQ, GT,GE,
        NE,IF, ELSE, CHI,CHD,STRING,WHILE,COMA, FOR, PRIMITIVE_TYPE,MODIFIER_TYPE , RETURN,NOT,END,HT,POINT
    };

    Type type;
    std::string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const std::string& source, int first, int last);

    friend std::ostream& operator<<(std::ostream& outs, const Token& tok);
    friend std::ostream& operator<<(std::ostream& outs, const Token* tok);
};

#endif // TOKEN_H