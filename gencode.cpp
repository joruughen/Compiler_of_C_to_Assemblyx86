#include "gencode.h"

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

void ImpCODE::interpret(Program* p) {
    env.clear();
    etiquetas = 0;
    current_offset = 0;

    cout << ".data" << endl;
    cout << "print_fmt: .string \"%ld\\n\"" << endl;
    cout << ".text" << endl;
    cout << ".globl main" << endl;
    cout << "main:" << endl;
    cout << "  pushq %rbp" << endl;
    cout << "  movq %rsp, %rbp" << endl;
    p->accept(this);  
    cout << "  movl $0, %eax" << endl;
    cout << "  leave" << endl;
    cout << "  ret" << endl;
    cout << ".section .note.GNU-stack,\"\",@progbits" << endl;
}

void ImpCODE::visit(Program* p) {
    env.add_level();
    p->body->accept(this);
    env.remove_level();
}

void ImpCODE::visit(Body* b) {
    env.add_level();
    long old_offset = current_offset;
    b->vardecs->accept(this);
    long locals_size = -current_offset;
    if (locals_size > 0)
        cout << "  subq $" << locals_size << ", %rsp" << endl;

    b->slist->accept(this);
    current_offset = old_offset;
    env.remove_level();
}

void ImpCODE::visit(VarDecList* decs) {
    for (auto it : decs->vardecs) {
        it->accept(this);
    }
}

void ImpCODE::visit(VarDec* vd) {
    ImpVType tt = ImpValue::get_basic_type(vd->type);
    for (const auto& var : vd->vars) {
        ImpValue v;
        v.set_default_value(tt);
        env.add_var(var, v);
        current_offset -= 8;
        stack_offsets[var] = current_offset;
    }
}

void ImpCODE::visit(StatementList* s) {
    for (auto it : s->stms) {
        it->accept(this);
    }
}
void ImpCODE::visit(AssignStatement* s) {
    ImpValue val = s->rhs->accept(this); 
    cout << "  movq %rax, " << stack_offsets[s->id] << "(%rbp)" << endl;
    env.update(s->id, val);  
}

void ImpCODE::visit(PrintStatement* s) {
    s->e->accept(this);
    cout << "  movq %rax, %rsi" << endl;
    cout << "  leaq print_fmt(%rip), %rdi" << endl;
    cout << "  movl $0, %eax" << endl;
    cout << "  call printf@PLT" << endl;
}

void ImpCODE::visit(IfStatement* s) {
    int lbl = etiquetas++;
    s->condition->accept(this);
    cout << "  cmpq $0, %rax" << endl;
    cout << "  je else_" << lbl << endl;
    s->then->accept(this);
    cout << "  jmp endif_" << lbl << endl;
    cout << "else_" << lbl << ":" << endl;
    if (s->els) s->els->accept(this);
    cout << "endif_" << lbl << ":" << endl;
}

void ImpCODE::visit(WhileStatement* s) {
    int lbl_cond = etiquetas++;
    int lbl_end  = etiquetas++;
    cout << "while_" << lbl_cond << ":" << endl;
    s->condition->accept(this);
    cout << "  testq %rax, %rax" << endl;
    cout << "  je endwhile_" << lbl_end << endl;
    s->b->accept(this);
    cout << "  jmp while_" << lbl_cond << endl;
    cout << "endwhile_" << lbl_end << ":" << endl;
}



ImpValue ImpCODE::visit(BinaryExp* e) {
    ImpValue result;
    e->left->accept(this);        
    cout << "  pushq %rax" << endl;
    e->right->accept(this);       
    cout << "  movq %rax, %rcx" << endl; 
    cout << "  popq %rax" << endl;    

    switch (e->op) {
        case PLUS_OP:
            cout << "  addq %rcx, %rax" << endl;
            result.type = TINT;
            break;

        case MINUS_OP:
            cout << "  subq %rcx, %rax" << endl;
            result.type = TINT;
            break;

        case MUL_OP:
            cout << "  imulq %rcx, %rax" << endl;
            result.type = TINT;
            break;

        case DIV_OP:
            cout << "  cqto" << endl;
            cout << "  idivq %rcx" << endl;
            result.type = TINT;
            break;

        case LT_OP:
            cout << "  cmpq %rcx, %rax" << endl;
            cout << "  movl $0, %eax" << endl;
            cout << "  setl %al" << endl;
            result.type = TBOOL;
            break;

        case LE_OP:
            cout << "  cmpq %rcx, %rax" << endl;
            cout << "  movl $0, %eax" << endl;
            cout << "  setle %al" << endl;
            result.type = TBOOL;
            break;

        case EQ_OP:
            cout << "  cmpq %rcx, %rax" << endl;
            cout << "  movl $0, %eax" << endl;
            cout << "  sete %al" << endl;
            result.type = TBOOL;
            break;

        default:
            cerr << "Operador binario no soportado." << endl;
            exit(1);
    }

    return result;
}

ImpValue ImpCODE::visit(NumberExp* e) {
    ImpValue v;
    v.set_default_value(TINT);
    v.int_value = e->value;
    cout << "  movq $" << e->value << ", %rax" << endl;
    return v;
}

ImpValue ImpCODE::visit(BoolExp* e) {
    ImpValue v;
    v.set_default_value(TBOOL);
    v.bool_value = e->value;
    cout << "  movq $" << (e->value ? 1 : 0) << ", %rax" << endl;
    return v;
}

ImpValue ImpCODE::visit(IdentifierExp* e) {
    if (env.check(e->name)) {
        cout << "  movq " << stack_offsets[e->name] << "(%rbp), %rax" << endl;
        return env.lookup(e->name);
    } else {
        cerr << "Error: variable " << e->name << " no declarada" << endl;
        exit(1);
    }
}