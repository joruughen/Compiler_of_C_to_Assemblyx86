#include "imp_interpreter.h"

ImpValue BinaryExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue BoolExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue NumberExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue IdentifierExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}






void AssignStatement::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void PrintStatement::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void IfStatement::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void WhileStatement::accept(ImpValueVisitor* v) {
    return v->visit(this);
}





void StatementList::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void VarDec::accept(ImpValueVisitor* v) {
    return v->visit(this);
}


void VarDecList::accept(ImpValueVisitor* v) {
    return v->visit(this);
}



void Body::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void Program::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void ImpInterpreter::interpret(Program* p) {
    env.clear();
    p->accept(this);
    return;
}



void ImpInterpreter::visit(Program* p) {
    env.add_level();
    p->body->accept(this);
}

void ImpInterpreter::visit(Body* b) {
    env.add_level();
    b->vardecs->accept(this);
    b->slist->accept(this);
    env.remove_level();
    return;
}

void ImpInterpreter::visit(VarDecList* decs) {
    list<VarDec*>::iterator it;
    for (it = decs->vardecs.begin(); it != decs->vardecs.end(); ++it) {
        (*it)->accept(this);
    }
    return;
}


void ImpInterpreter::visit(VarDec* vd) {
    list<string>::iterator it;
    ImpValue v;
    ImpVType tt = ImpValue::get_basic_type(vd->type);
    if (tt == NOTYPE) {
        cout << "Tipo invalido: " << vd->type << endl;
        exit(0);
    }
    v.set_default_value(tt);
    for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
        env.add_var(*it, v);
    }
    return;
}



void ImpInterpreter::visit(StatementList* s) {
    list<Stm*>::iterator it;
    for (it = s->stms.begin(); it != s->stms.end(); ++it) {
        (*it)->accept(this);
    }
    return;
}

void ImpInterpreter::visit(AssignStatement* s) {
    ImpValue v = s->rhs->accept(this);
    if (!env.check(s->id)) {
        cout << "Variable " << s->id << " undefined" << endl;
        exit(0);
    }
    ImpValue lhs = env.lookup(s->id);
    if (lhs.type != v.type) {
        cout << "Type Error en Assign: Tipos de variable " << s->id;
        cout << " no coinciden" << endl;
        exit(0);
    }
    env.update(s->id, v);
    return;
}

void ImpInterpreter::visit(PrintStatement* s) {
    ImpValue v = s->e->accept(this);
    cout << v << endl;
    return;
}

void ImpInterpreter::visit(IfStatement* s) {
    ImpValue v = s->condition->accept(this);
    if (v.type != TBOOL) {
        cout << "Type error en If: esperaba bool en condicional" << endl;
        exit(0);
    }
    if(v.bool_value){
        s->then->accept(this);
    }
    else{
        s->els->accept(this);
    }
    return;
}

void ImpInterpreter::visit(WhileStatement* s) {
    ImpValue v = s->condition->accept(this);
    if (v.type != TBOOL) {
        cout << "Type error en WHILE: esperaba bool en condicional" << endl;
        exit(0);
    }
    while(s->condition->accept(this).bool_value){
        s->b->accept(this);
    }
}


ImpValue ImpInterpreter::visit(BinaryExp* e) {
    ImpValue result;
    ImpValue v1 = e->left->accept(this);
    ImpValue v2 = e->right->accept(this);
    if (v1.type != TINT || v2.type != TINT) {
        cout << "Error de tipos: operandos en operacion binaria tienen que ser "
                "enteros"
             << endl;
        exit(0);
    }
    int iv, iv1, iv2;
    bool bv;
    ImpVType type = NOTYPE;
    iv1 = v1.int_value;
    iv2 = v2.int_value;
    switch (e->op) {
        case PLUS_OP:
            iv = iv1 + iv2;
            type = TINT;
            break;
        case MINUS_OP:
            iv = iv1 - iv2;
            type = TINT;
            break;
        case MUL_OP:
            iv = iv1 * iv2;
            type = TINT;
            break;
        case DIV_OP:
            iv = iv1 / iv2;
            type = TINT;
            break;
        case LT_OP:
            bv = (iv1 < iv2) ? 1 : 0;
            type = TBOOL;
            break;
        case LE_OP:
            bv = (iv1 <= iv2) ? 1 : 0;
            type = TBOOL;
            break;
        case EQ_OP:
            bv = (iv1 == iv2) ? 1 : 0;
            type = TBOOL;
            break;
    }
    if (type == TINT)
        result.int_value = iv;
    else
        result.bool_value = bv;
    result.type = type;
    return result;
}

ImpValue ImpInterpreter::visit(NumberExp* e) {
    ImpValue v;
    v.set_default_value(TINT);
    v.int_value = e->value;
    return v;
}

ImpValue ImpInterpreter::visit(BoolExp* e) {
    ImpValue v;
    v.set_default_value(TBOOL);
    v.int_value = e->value;
    return v;
}

ImpValue ImpInterpreter::visit(IdentifierExp* e) {
    if (env.check(e->name))
        return env.lookup(e->name);
    else {
        cout << "Variable indefinida: " << e->name << endl;
        exit(0);
    }
}



