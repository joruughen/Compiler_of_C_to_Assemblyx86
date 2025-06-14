#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h"
#include "parser.h"

using namespace std;

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            cout << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

Parser::Parser(Scanner* sc):scanner(sc) {
    previous = NULL;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        cout << "Error en el primer token: " << current->text << endl;
        exit(1);
    }
}

VarDec* Parser::parseVarDec() {
    VarDec* vd = NULL;

    if (!match(Token::ID)) {
        cout << "Error: se esperaba un identificador." << endl;
        exit(1);
    }
    string type = previous->text;
    list<string> ids;
    if (!match(Token::ID)) {
        cout << "Error: se esperaba un identificador." << endl;
        exit(1);
    }
    ids.push_back(previous->text);
    while (match(Token::COMA)) {
        if (!match(Token::ID)) {
            cout << "Error: se esperaba un identificador después de ','." << endl;
            exit(1);
        }
        ids.push_back(previous->text);
    }
    if (!match(Token::PC)) {
        cout << "Error: se esperaba un ';' al final de la declaración." << endl;
        exit(1);
    }
    vd = new VarDec(type, ids);

    return vd;
}

VarDecList* Parser::parseVarDecList() {
    VarDecList* vdl = new VarDecList();
    VarDec* aux;
    aux = parseVarDec();
    while (aux != NULL) {
        vdl->add(aux);
        aux = parseVarDec();
    }
    return vdl;
}

StatementList* Parser::parseStatementList() {
    StatementList* sl = new StatementList();
    sl->add(parseStatement());
    while (match(Token::PC)) {
        sl->add(parseStatement());
    }
    return sl;
}


Block* Parser::parseBlock() {
    VarDecList* vdl = parseVarDecList();
    StatementList* sl = parseStatementList();
    return new Block(vdl, sl);
}


FunDec* Parser::parseFunDec() {
    if (!match(Token::ID)) {
        cout << "Error: se esperaba un tipo de retorno para la función." << endl;
        exit(1);
    }
    string type = previous->text;
    if (!match(Token::ID)) {
        cout << "Error: se esperaba un identificador para la función." << endl;
        exit(1);
    }
    string id = previous->text;
    ParamDecList* params = new ParamDecList();
    if (match(Token::PI)) {
        if (match(Token::ID)) {
            params->add(new ParamDec(type, previous->text));
            while (match(Token::COMA)) {
                if (!match(Token::ID)) {
                    cout << "Error: se esperaba un identificador para el parámetro." << endl;
                    exit(1);
                }
                params->add(new ParamDec(type, previous->text));
            }
        }
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' al final de los parámetros." << endl;
            exit(1);
        }
    }
    Block* block = parseBlock();
    return new FunDec(type, id, params, block);
}



FunDecList* Parser::parseFunDecList() {
    FunDecList* fundecs = new FunDecList();
    FunDec* aux;
    aux = parseFunDec();
    while (aux != NULL) {
        fundecs->add(aux);
        aux = parseFunDec();
    }
    return fundecs;
}



Program* Parser::parseProgram() {
    if (!match(Token::INCLUDE)) {
        cout << "Error: se esperaba un #include." << endl;
        exit(1);
    }
    if (!match(Token::LT)) {
        cout << "Error: se esperaba un <." << endl;
        exit(1);
    }
    if (!match(Token::STDIOH)) {
        cout << "Error: se esperaba un <stdio.h>." << endl;
        exit(1);
    }
    if (!match(Token::GT)) {
        cout << "Error: se esperaba un >." << endl;
        exit(1);
    }
    string stdio_h = "#include <stdio.h>";
    FunDecList* funDecs = parseFunDecList();
    FunDec* mainFun = nullptr;

    // Buscar y extraer la función main
    for (auto it = funDecs->funDecs.begin(); it != funDecs->funDecs.end(); ) {
        if ((*it)->id == "main") {
            if (mainFun != nullptr) {
                cout << "Error: más de una función main encontrada." << endl;
                exit(1);
            }
            mainFun = *it;
            it = funDecs->funDecs.erase(it);
        } else {
            ++it;
        }
    }
    if (mainFun == nullptr) {
        cout << "Error: no se encontró la función main." << endl;
        exit(1);
    }
    return new Program(stdio_h, funDecs, mainFun);
}

list<Stm*> Parser::parseStmList() {
    list<Stm*> slist;
    slist.push_back(parseStatement());
    while(match(Token::PC)) {
        slist.push_back(parseStatement());
    }
    return slist;
}

Stm* Parser::parseStatement() {
    Stm* s = NULL;
    Exp* e = NULL;
    Block* tb = NULL; //true case
    Block* fb = NULL; //false case
    string lex;
    if (current == NULL) {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }

    if (match(Token::ID)) {
        lex = previous->text;

        if (match(Token::ASSIGN)) {
            e = parseCExp();
            s = new AssignStatement(lex, e);
        }

    } else if (match(Token::PRINTF)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después de 'print'." << endl;
            exit(1);
        }
        if (match(Token::STRING)) {
            lex = previous->text;
        }
        if (!match(Token::COMA)) {
            if (!match(Token::PD)) {
                cout<<"missing pd";
                exit(1);
            }
            return new PrintfStatement(lex);
        }
        ArgList* args;
        args = parseArgList();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        s = new PrintfStatement(lex,args);
    }
    else if (match(Token::IF)) {
        if (!match(Token::PI)) {
            cout<<"missing (";
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout<<"missing pd";
            exit(1);
        }
        if (!match(Token::CBI)) {
            cout << "Error: se esperaba '{' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBlock();

        if (match(Token::ELSE)) {
            if (!match(Token::CBI)) {
                cout<<"missing {";
                exit(1);
            }
            fb = parseBody();
            if (!match(Token::CBD)) {
                cout<<"missing }";
                exit(1);
            }
            return new IfStatement(e, tb, fb);
        }
        if (!match(Token::CBD)) {
            cout<<"missing }";
            exit(1);
        }
        s = new IfStatement(e, tb, fb);

    }
    else if (match(Token::WHILE)) {
        if (!match(Token::PI)) {
            cout<<"missing (";
            exit(1);
        }
        e = parseCExp();

        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        if (!match(Token::CBI)) {
            cout<<"missing {";
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::CBD)) {
            cout << "Error: se esperaba '}' al final de la declaración." << endl;
            exit(1);
        }
        s = new WhileStatement(e, tb);

    }
    else if (match(Token::FOR)) {
        if (match(Token::TYPE)) {
            string type = previous->text;
            if (match(Token::ID)) {
                if (match(Token::ASSIGN)) {
                    e = parseCExp();
                }
                AssignStatement* as = new AssignStatement(previous->text, e);
            }
        }
        else {
            if (!match(Token::ID)) {
                cout<<"missing id";
                exit(1);
            }
            if (!match(Token::ASSIGN)) {
                cout<<"missing assign";
                exit(1);
            }
            if (!match(Token::NUM)) {
                cout<<"missing num";
            }
        }

    }
    else {
        cout << "Error: Se esperaba un identificador o 'print', pero se encontró: " << *current << endl;
        exit(1);
    }
    return s;
}

Exp* Parser::parseCExp(){
    Exp* left = parseExpression();
    if (match(Token::LT) || match(Token::LE) || match(Token::EQ)){
        BinaryOp op;
        if (previous->type == Token::LT){
            op = LT_OP;
        }
        else if (previous->type == Token::LE){
            op = LE_OP;
        }
        else if (previous->type == Token::EQ){
            op = EQ_OP;
        }
        Exp* right = parseExpression();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseExpression() {
    Exp* left = parseTerm();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else if (previous->type == Token::MINUS){
            op = MINUS_OP;
        }
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseTerm() {
    Exp* left = parseFactor();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else if (previous->type == Token::DIV){
            op = DIV_OP;
        }
        Exp* right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseFactor() {
    Exp* e;
    Exp* e1;
    Exp* e2;
    if (match(Token::TRUE)){
        return new BoolExp(1);
    }else if (match(Token::FALSE)){
        return new BoolExp(0);
    }
    else if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID)) {
        string texto = previous->text;
        return new IdentifierExp(previous->text);
    }
    else if (match(Token::PI)){
        e = parseCExp();
        if (!match(Token::PD)){
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    cout << "Error: se esperaba un número o identificador." << endl;
    exit(0);
}

