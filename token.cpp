#include <iostream>
#include "token.h"

using namespace std;

Token::Token(Type type):type(type) { text = ""; }

Token::Token(Type type, char c):type(type) { text = string(1, c); }

Token::Token(Type type, const string& source, int first, int last):type(type) {
    text = source.substr(first, last);
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
    switch (tok.type) {
        case Token::PLUS: outs << "TOKEN(PLUS)"; break;
        case Token::MINUS: outs << "TOKEN(MINUS)"; break;
        case Token::MUL: outs << "TOKEN(MUL)"; break;
        case Token::DIV: outs << "TOKEN(DIV)"; break;
        case Token::NUM: outs << "TOKEN(NUM)"; break;
        case Token::ERR: outs << "TOKEN(ERR)"; break;
        case Token::PD: outs << "TOKEN(PD)"; break;
        case Token::PI: outs << "TOKEN(PI)"; break;
        case Token::ID: outs << "TOKEN(ID)"; break;
        case Token::PRINTF: outs << "TOKEN(PRINTF)"; break;
        case Token::ASSIGN: outs << "TOKEN(ASSIGN)"; break;
        case Token::PC: outs << "TOKEN(PC)"; break;
        case Token::LT: outs << "TOKEN(LT)"; break;
        case Token::LE: outs << "TOKEN(LE)"; break;
        case Token::EQ: outs << "TOKEN(EQ)"; break;
        case Token::IF: outs << "TOKEN(IF)"; break;
        case Token::AND: outs << "TOKEN(AND)"; break;
        case Token::ELSE: outs << "TOKEN(ELSE)"; break;
        case Token::CHD: outs << "TOKEN(CHD)"; break;
        case Token::WHILE: outs << "TOKEN(WHILE)"; break;
        case Token::COMA: outs << "TOKEN(COMA)"; break;
        case Token::CHI: outs << "TOKEN(CHI)"; break;
        case Token::GE: outs << "TOKEN(GE)"; break;
        case Token::GT: outs << "TOKEN(GT)"; break;
        case Token::FOR : outs << "TOKEN(FOR)"; break;
        case Token::INCLUDE : outs << "TOKEN(INCLUDE)"; break;
        case Token::NE : outs << "TOKEN(NE)"; break;
        case Token::OR : outs << "TOKEN(OR)"; break;
        case Token::RETURN : outs << "TOKEN(RETURN)"; break;
        case Token::STRING : outs << "TOKEN(STRING)"; break;
        case Token::PRIMITIVE_TYPE : outs << "TOKEN(PRIMITIVE TYPE)"; break;
        case Token::MODIFIER_TYPE : outs << "TOKEN(MODIFIER TYPE)"; break;
        case Token::NOT: outs << "TOKEN(NOT)"; break;
        case Token::END: outs << "TOKEN(END)"; break;
        case Token::HT: outs << "TOKEN(HT)"; break;
        case Token::POINT: outs << "TOKEN(POINT)"; break;
        default: outs << "TOKEN(UNKNOWN)"; break;
    }
    return outs;
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}