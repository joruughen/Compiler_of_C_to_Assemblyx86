#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h"
#include "parser.h"
#include <stop_token>

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

Program* Parser::parseProgram() {
    Program* p = new Program();
    p->includes = parseInclude();
    p->fundecs = parseFunDecList();
    p->main = p->fundecs->fundecs.back();
    if (p->main->id != "main") {
        cout<<"se esperaba funcion main";
        exit(1);
    }
    p->fundecs->fundecs.pop_back();
    return p;
}

list<string> Parser::parseInclude() {
    list<string> lib;
    while (match(Token::HT)) {
        if (match(Token::ID)) {
            if (previous->text != "include") {
                cout<<"missing include";
                exit(1);
            }
        }
        string s = "";
        if (match(Token::LT)) {
            match(Token::ID);
            s+=previous->text;
            if (match(Token::POINT)) {
                s+=previous->text;
                if (match(Token::ID)){s+=previous->text;}
            };
            lib.push_back(s);
            match(Token::GT);
        }
        else if (match(Token::STRING)) {
            lib.push_back(previous->text);
        }
    }
    return lib;
}

FunDec* Parser::parseFunDec() {
    FunDec* vd = NULL;
    string type = "";
    if (match(Token::MODIFIER_TYPE) || match(Token::PRIMITIVE_TYPE)) {
        type+=previous->text;
        if (previous->type == Token::MODIFIER_TYPE) {
            if (!match(Token::PRIMITIVE_TYPE)) {
                cout<<"se esperaba primitivo tipo";
                exit(1);
            };
            type+=" "+ previous->text;
        }
        FunDec* fu = new FunDec();
        fu->type = type;
        match(Token::ID);
        fu->id = previous->text;
        if (!match(Token::PI)) {
            cout<<"( missing";
            exit(1);
        }
        //parse param declist
        while(match(Token::PRIMITIVE_TYPE) || match(Token::MODIFIER_TYPE)) {
            type="";
            type+=previous->text;
            if (previous->type == Token::MODIFIER_TYPE) {
                if (!match(Token::PRIMITIVE_TYPE)) {
                    cout<<"se esperaba primitivo tipo"; //podiramos modificar para q acepte solo long con int por defecto
                    exit(1);
                };
                type+=" "+ previous->text;
            }
            fu->param_types.push_back(type);
            if (!match(Token::ID)) {
                cout<<"id missing";
                exit(1);
            }
            fu->param_ids.push_back(previous->text);
            if (!match(Token::COMA)) {
                break;
            }
        }
        match(Token::PD);
        match(Token::CHI);
        fu->block = parseBlock();
        if (!match(Token::CHD)) {
            cout<<"missing }";
            exit(1);
        }
        vd = fu;
    }
    return vd;
}

FunDecList* Parser::parseFunDecList() {
    FunDecList* vdl = new FunDecList();
    FunDec* aux;
    aux = parseFunDec();
    while (aux != NULL) {
        vdl->add(aux);
        aux = parseFunDec();
    }
    return vdl;
}

Block* Parser::parseBlock() {
    VarDecList* vdl = parseVarDecList();
    StatementList* sl = parseStatementList();
    return new Block(vdl, sl);
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

VarDec* Parser::parseVarDec() {
    VarDec* vd = NULL;
    string type = "";
    if (match(Token::MODIFIER_TYPE) || match(Token::PRIMITIVE_TYPE)) {
        type+=previous->text;
        if (previous->type == Token::MODIFIER_TYPE) {
            if (!match(Token::PRIMITIVE_TYPE)) {
                cout<<"se esperaba primitivo tipo";
                exit(1);
            };
            type+=" "+ previous->text;
        }
        list<string> id_list;
        while (match(Token::ID)) {
            id_list.push_back(previous->text);
            if (match(Token::PC)) {
                break;
            }
            if (!match(Token::COMA)) {
                cout<<"missing ,";
                exit(1);
            }
        }
        vd = new VarDec(type,id_list);
        }
    return vd;
}

StatementList* Parser::parseStatementList() {
    StatementList* sl = new StatementList();
    Stm* aux = parseStatement();
    while (aux!=NULL) {
        sl->add(aux);
        aux = parseStatement();
    }
    return sl;
}

Stm* Parser::parseStatement() {
    Stm* s = NULL;
    Exp* e = NULL;
    Block* tb = NULL; //true case
    Block* fb = NULL; //false case

    if (current == NULL) {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }
    if (match(Token::ID)) {
        string id = previous->text;
        if (!match(Token::ASSIGN)) {
            cout<<"se esperaba ="<<endl;
            exit(1);
        }
        e = parseAExp();
        if (!match(Token::PC)) {
            cout<<"missing ;";
            exit(1);
        }
        return new AssignStatement(id, e);
    }
    else if (match(Token::PRINTF)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después de 'print'." << endl;
            exit(1);
        }
        if (!match(Token::STRING)) {
            cout << "Error: se esperaba un 'string'." << endl;
            exit(1);
        }
        string s = previous->text;
        if (match(Token::PD)) {
            if (match(Token::PC)) {
                return new PrintStatement(s);
            }
            else {
                cout<<"missing ;";
                exit(1);
            }
        }
        vector<Exp*> param_list;
        while (match(Token::COMA)) {
            param_list.push_back(parseAExp());
        }
        if (!match(Token::PD)) {
            cout<<"missing )";
            exit(1);
        }
        if (!match(Token::PC)) {
            cout<<"missing ;";
            exit(1);
        }
        return new PrintStatement(s,param_list);
    }
    else if (match(Token::IF)) {
        if (!match(Token::PI)) {
            cout<<"missing (";
            exit(1);
        }
        e = parseAExp();
        if (!match(Token::PD)) {
            cout<<"missing )";
            exit(1);
        }
        if (!match(Token::CHI)) {
            cout<<"missing {";
            exit(1);
        }
        tb = parseBlock();
        if (!match(Token::CHD)) {
            cout<<"missing }";
            exit(1);
        }
        if (match(Token::ELSE)) {
            if (!match(Token::CHI)) {
                cout<<"missing {";
                exit(1);
            }
            fb = parseBlock();
            if (!match(Token::CHD)) {
                cout<<"missing }";
                exit(1);
            }
        }
        s = new IfStatement(e, tb, fb);
    }
    else if (match(Token::WHILE)) {
        if (!match(Token::PI)) {
            cout<<"missing (";
            exit(1);
        }
        e = parseAExp();
        if (!match(Token::PD)) {
            cout<<"missing )";
            exit(1);
        }
        if (!match(Token::CHI)) {
            cout<<"missing {";
            exit(1);
        }
        tb = parseBlock();
        if (!match(Token::CHD)) {
            cout<<"missing }";
            exit(1);
        }
        s = new WhileStatement(e, tb);
    }
    else if (match(Token::FOR)) {
        Stm* init = NULL;
        VarDec* dcl = NULL;
        if (!match(Token::PI)) {
            cout<<"missing (";
            exit(1);
        }
        string id;
        string type = "";
        if (match(Token::MODIFIER_TYPE) || match(Token::PRIMITIVE_TYPE)) {
            type+=previous->text;
            if (previous->type == Token::MODIFIER_TYPE) {
                if (!match(Token::PRIMITIVE_TYPE)) {
                    cout<<"se esperaba primitivo tipo";
                    exit(1);
                };
                type+=" "+ previous->text;
            }
        }
        if (!match(Token::ID)) {
            cout<<"missing id";
            exit(1);
        }
        id = previous->text;
        list<string> ids;
        ids.push_back(id);
        if (type != "") {
            dcl = new VarDec(type,ids);
        }
        if (!match(Token::ASSIGN)) {
            cout<<"missing =";
            exit(1);
        }
        Exp* e1 = parseAExp();
        init = new AssignStatement(id,e1);
        match(Token::PC);

        Exp* cond = parseAExp();
        match(Token::PC);
        //parse updatestmt
        Stm* update = parseUpdateStmt();
        if (!match(Token::PD)) {
            cout<<"missing )";
            exit(1);
        }
        if (!match(Token::CHI)) {
            cout<<"missing {";
            exit(1);
        }
        tb = parseBlock();
        if (!match(Token::CHD)) {
            cout<<"missing }";
            exit(1);
        }
        s = new ForStatement(dcl,init,cond,update,tb);
    }
    else if (match(Token::RETURN)) {
        e = parseAExp();
        if (!match(Token::PC)) {
            cout<<"missing ;"<<endl;
            exit(1);
        }

        s = new ReturnStatement(e);
    }
    return s;
}

Stm* Parser::parseUpdateStmt() {
    if (!match(Token::ID)) {
        cout<<"missing id";
        exit(1);
    }
    string id = previous->text;
    BinaryOp op;
    if (match(Token::PLUS) || match(Token::MINUS) ) {
        if (previous->type == Token::PLUS && match(Token::PLUS)) {
            op = PLUS_OP;
        }
        else if (previous->type == Token::MINUS && match(Token::MINUS)) {
            op = MINUS_OP;
        }
        return new AssignStatement(id,new BinaryExp(new IdentifierExp(id),new NumberExp(1),op));
    }
    else if (match(Token::ASSIGN)) {
        Exp* e1 = parseAExp();
        return new AssignStatement(id,e1);
    }
    else {
        cout<<"error for update not token expected";
        exit(1);
    }
}

Exp* Parser::parseAExp() {
    Exp* e = parseBExp();
    BinaryOp op = OR_OP;
    while (match(Token::OR)) {
        Exp* e2 = parseBExp();
        e = new BinaryExp(e, e2, op);
    }
    return e;
}

Exp * Parser::parseBExp() {
    Exp* e = parseCExp();
    BinaryOp op = AND_OP;
    while (match(Token::AND)) {
        Exp* e2 = parseCExp();
        e = new BinaryExp(e, e2, op);
    }
    return e;
}

Exp* Parser::parseCExp(){
    Exp* left = parseExpression();
    if (match(Token::LT) || match(Token::LE) || match(Token::EQ) || match(Token::GT) || match(Token::GE) || match(Token::NE)){
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
        else if (previous->type == Token::GT) {
            op = GT_OP;
        }
        else if (previous->type == Token::GE) {
            op =GE_OP;
        }
        else if (previous->type == Token::NE) {
            op = NE_OP;
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
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID)) {
        string id = previous->text;

        if (!match(Token::PI)) {
            return new IdentifierExp(id);
        }
        //parse fcallexp
        list<Exp*> args;
        while (!match(Token::PD)) { //check
            args.push_back(parseCExp());
            match(Token::COMA);
        }
        return new FCallExp(args, id);
    }
    else if (match(Token::PI)){
        e = parseAExp(); //not?
        if (!match(Token::PD)){
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
            }
        return e;
    }
    else if (match(Token::NOT)) {
            if (match(Token::PI)){
                e = parseAExp(); //not?
                if (!match(Token::PD)){
                    cout << "Falta paréntesis derecho" << endl;
                    exit(0);
                }
                return e;
            }
            else if (match(Token::ID)) {
                string id = previous->text;
                if (match(Token::PI)) {
                    list<Exp*> args;
                    while (!match(Token::PD)) { //check
                        args.push_back(parseCExp());
                        match(Token::COMA);
                    }
                    return new FCallExp(args, id); //not???
                }
            }
    }
    cout << "Error: se esperaba un bonito input xd." << endl;
    exit(0);
}
