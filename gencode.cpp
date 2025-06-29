#include "gencode.hh"

void FunDec::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void BinaryExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void BoolExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void NumberExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void IdentifierExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void FCallExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}
void AssignStatement::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void PrintStatement::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void ReturnStatement::accept(ImpValueVisitor *v) {
    return v->visit(this);
}

void ForStatement::accept(ImpValueVisitor* v) {
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



void Block::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

void Program::accept(ImpValueVisitor* v) {
    return v->visit(this);
}
void ImpCODE::generate(Program* p) {
    // Data section for strings/constants
    emit(".data");
    emit("print_fmt: .string \"%ld\\n\"");

    // Text section (code)
    emit(".text");

    // Generate all functions first
    for (auto fdec : p->fundecs->fundecs) {
        visit(fdec);
    }


    // Calculate stack space needed for main
    current_offset = 0;
    p->main->accept(this);

    // Main return
    emit("  movl $0, %eax");
    emit("  leave");
    emit("  ret");
    emit(".section .note.GNU-stack,\"\",@progbits");
}

void ImpCODE::visit(Program* p) {
    // Process global declarations if you have them
    env.add_level();
    p->main->accept(this);  // Process main's body
    env.remove_level();
}

void ImpCODE::visit(VarDecList* decs) {
    for (auto it : decs->vardecs) {
        it->accept(this);
    }
}
void ImpCODE::visit(VarDec* vd) {
    for (const auto& var : vd->vars) {
        current_offset -= 8;  // 8 bytes per variable
        stack_offsets[var] = current_offset;

        // Initialize to 0 if you want
        emit("movq $0, " + to_string(current_offset) + "(%rbp)");
    }
}

void ImpCODE::visit(Block* b) {
    env.add_level();
    int old_offset = current_offset;

    b->vardecs->accept(this);
    if (current_offset < old_offset) {
        emit("subq $" + to_string(old_offset - current_offset) + ", %rsp");
    }

    b->slist->accept(this);

    // Restore stack pointer if needed
    if (current_offset < old_offset) {
        emit("addq $" + to_string(old_offset - current_offset) + ", %rsp");
    }

    current_offset = old_offset;
    env.remove_level();
}

void ImpCODE::visit(StatementList* s) {
    for (auto it : s->stms) {
        it->accept(this);
    }
}

void ImpCODE::visit(AssignStatement* s) {
    // If the variable is not declared, reserve space in the stack
    if (!stack_offsets.count(s->id)) {
        current_offset -= 8;
        stack_offsets[s->id] = current_offset;
        emit("subq $8, %rsp");
    }
    s->rhs->accept(this);  // Result in RAX
    emit("movq %rax, " + to_string(stack_offsets[s->id]) + "(%rbp)");
}

void ImpCODE::visit(ForStatement *e) {
    e->init->accept(this);
    etiquetas++;
    cout << "for_" << etiquetas << ":" << endl;
    // cout << "  testq %rax, %rax" << endl;
    e->condition->accept(this);
    cout << "  testq %rax, %rax" << endl;
    cout << "  je endfor_" << etiquetas << endl;
    // cout << "  testq %rax, %rax" << endl;
    e->body->accept(this);
    e->update->accept(this);
    cout << "  jmp for_" << etiquetas << endl;
    cout << "endfor_" << etiquetas<< ":" << endl;
}

void ImpCODE::visit(PrintStatement* s) {
    // s->e->accept(this);
    for (auto it : s->param_list) {
        it->accept(this);
        cout << "  movq %rax, %rsi" << endl;
        cout << "  leaq print_fmt(%rip), %rdi" << endl;
        cout << "  movl $0, %eax" << endl;
        cout << "  call printf@PLT" << endl;
    }
}
void ImpCODE::visit(IfStatement* s) {
    string else_label = new_label("else_");
    string end_label = new_label("endif_");

    s->condition->accept(this);
    emit("test %rax, %rax");
    emit("je " + else_label);

    s->then->accept(this);
    emit("jmp " + end_label);

    emit(else_label + ":");
    if (s->els) s->els->accept(this);

    emit(end_label + ":");
}

void ImpCODE::visit(WhileStatement* s) {
    string start_label = new_label("while_");
    string end_label = new_label("endwhile_");

    emit(start_label + ":");
    s->condition->accept(this);
    emit("test %rax, %rax");
    emit("je " + end_label);

    s->b->accept(this);
    emit("jmp " + start_label);

    emit(end_label + ":");
}
void ImpCODE::visit(FCallExp* e) {
    // Push arguments in reverse order
    for (auto it = e->params.rbegin(); it != e->params.rend(); ++it) {
        (*it)->accept(this);
        emit("push %rax");
    }

    emit("call " + e->id);

    // Clean up stack (adjust for 8 bytes per argument)
    if (!e->params.empty()) {
        emit("addq $" + to_string(8 * e->params.size()) + ", %rsp");
    }
}

void ImpCODE::visit(ReturnStatement* e) {
    e->e->accept(this);  // Result in RAX
    emit("movq %rbp, %rsp");
    emit("pop %rbp");
    emit("ret");
}


void ImpCODE::visit(BinaryExp* e) {
    e->left->accept(this);
    emit("pushq %rax");
    e->right->accept(this);
    emit("movq %rax, %rcx");
    emit("popq %rax");

    switch (e->op) {
        case PLUS_OP:
            emit("addq %rcx, %rax");
            break;
        case MINUS_OP:
            emit("subq %rcx, %rax");
            break;
        case MUL_OP:
            emit("imulq %rcx, %rax");
            break;
        case DIV_OP:
            emit("cqto");
            emit("idivq %rcx");
            break;
        case LT_OP:
            emit("cmpq %rcx, %rax");
            emit("movl $0, %eax");
            emit("setl %al");
            emit("movzbq %al, %rax");
            break;
        case LE_OP:
            emit("cmpq %rcx, %rax");
            emit("movl $0, %eax");
            emit("setle %al");
            emit("movzbq %al, %rax");
            break;
        case EQ_OP:
            emit("cmpq %rcx, %rax");
            emit("movl $0, %eax");
            emit("sete %al");
            emit("movzbq %al, %rax");
            break;
        case GT_OP:
            emit("cmpq %rcx, %rax");
            emit("setg %al");
            emit("movzbq %al, %rax");
            break;
        case GE_OP:
            emit("cmpq %rcx, %rax");
            emit("setge %al");
            emit("movzbq %al, %rax");
            break;
        case NE_OP:
            emit("cmpq %rcx, %rax");
            emit("setne %al");
            emit("movzbq %al, %rax");
            break;
        case AND_OP:
            emit("testq %rax, %rax");
            emit("setne %al");
            emit("testq %rcx, %rcx");
            emit("setne %dl");
            emit("andb %dl, %al");
            emit("movzbq %al, %rax");
            break;
        case OR_OP:
            emit("testq %rax, %rax");
            emit("setne %al");
            emit("testq %rcx, %rcx");
            emit("setne %dl");
            emit("orb %dl, %al");
            emit("movzbq %al, %rax");
            break;
        default:
            cerr << "Error: Operador binario no soportado: " << e->op << endl;
            exit(1);
    }
}

void ImpCODE::visit(NumberExp* e) {
    emit("movq $" + to_string(e->value) + ", %rax");
}

void ImpCODE::visit(BoolExp* e) {
    emit("movq $" + string(e->value ? "1" : "0") + ", %rax");
}

void ImpCODE::visit(IdentifierExp* e) {
    if (stack_offsets.count(e->name)) {
        emit("movq " + to_string(stack_offsets[e->name]) + "(%rbp), %rax");
    } else {
        cerr << "Undefined variable: " << e->name << endl;
        exit(1);
    }
}


void ImpCODE::visit(FunDec* fdec) {
    // Function prologue
    emit(".globl " + fdec->id);
    emit(fdec->id + ":");
    emit("  pushq %rbp");
    emit("  movq %rsp, %rbp");

    // Set up stack frame
    current_offset = 0;
    stack_offsets.clear();

    // Process parameters (System V AMD64 calling convention)
    // First 6 args in registers: RDI, RSI, RDX, RCX, R8, R9
    // For simplicity, we'll assume all params are on stack
    int param_offset = 16; // Above RBP (8) and return address (8)
    for (auto param : fdec->param_ids) {
        stack_offsets[param] = param_offset;
        param_offset += 8;
    }

    // Process function body
    fdec->block->accept(this);

    // Function epilogue
    emit("  leave");
    emit("  ret");
}

void ImpCODE::visit(FunDecList *fdecs) {
    for (auto e: fdecs->fundecs) {
        e->accept(this);
    }
}
