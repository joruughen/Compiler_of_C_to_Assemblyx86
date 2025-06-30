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
    emit("print_fmt: .string \"%d\\n\"");
    emit("print_fmt_unsigned: .string \"%lu\\n\"");  // Añadir formato para unsigned
    emit("print_fmt_long: .string \"%ld\\n\"");      // Long usa el mismo formato

    // Text section (code)
    emit(".text");

    // Generate all functions first
    for (auto fdec : p->fundecs->fundecs) {
        fdec->accept(this);
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
    ImpType var_type;
    if (vd->type == "int") {
        var_type.ttype = ImpType::INT;
    } else if (vd->type == "unsigned int") {
        var_type.ttype = ImpType::UINT;
    } else if (vd->type == "long int") {
        var_type.ttype = ImpType::LONG;
    } else if (vd->type == "bool") {
        var_type.ttype = ImpType::BOOL;
    }

    int type_size = get_type_size(var_type.ttype);

    for (const auto& var : vd->vars) {
        current_offset -= 8;  // Siempre alinear a 8 bytes para el stack
        stack_offsets[var] = current_offset;
        type_env.add_var(var, var_type);

        // Inicializar a 0 usando la instrucción correcta
        string mov_inst = get_mov_instruction(var_type.ttype);
        if (type_size == 4) {
            emit(mov_inst + " $0, " + to_string(current_offset) + "(%rbp)");
        } else {
            emit("movq $0, " + to_string(current_offset) + "(%rbp)");
        }
    }
}

void ImpCODE::visit(Block* b) {
    env.add_level();
    type_env.add_level();  // AÑADIR ESTO
    int old_offset = current_offset;

    b->vardecs->accept(this);
    if (current_offset < old_offset) {
        emit("subq $" + to_string(old_offset - current_offset) + ", %rsp");
    }

    b->slist->accept(this);

    if (current_offset < old_offset) {
        emit("addq $" + to_string(old_offset - current_offset) + ", %rsp");
    }

    current_offset = old_offset;
    env.remove_level();
    type_env.remove_level();  // AÑADIR ESTO
}

void ImpCODE::visit(StatementList* s) {
    for (auto it : s->stms) {
        it->accept(this);
    }
}

void ImpCODE::visit(AssignStatement* s) {
    if (!stack_offsets.count(s->id)) {
        current_offset -= 8;
        stack_offsets[s->id] = current_offset;
        emit("subq $8, %rsp");
    }

    s->rhs->accept(this);  // Resultado en RAX/EAX

    // Obtener el tipo de la variable
    ImpType var_type;
    if (type_env.check(s->id)) {
        var_type = type_env.lookup(s->id);
    } else {
        var_type = current_exp_type;
        type_env.add_var(s->id, var_type);
    }


    string mov_inst = get_mov_instruction(var_type.ttype);
    string reg = get_register("rax", var_type.ttype);

    emit(mov_inst + " %" + reg + ", " + to_string(stack_offsets[s->id]) + "(%rbp)");
}

void ImpCODE::visit(ForStatement *e) {
    int for_num = ++etiquetas;
    string for_label = "for_" + to_string(for_num);
    string endfor_label = "endfor_" + to_string(for_num);

    e->init->accept(this);
    emit(for_label + ":");
    e->condition->accept(this);
    emit("testq %rax, %rax");
    emit("je " + endfor_label);
    e->body->accept(this);
    e->update->accept(this);
    emit("jmp " + for_label);
    emit(endfor_label + ":");
}

void ImpCODE::visit(PrintStatement* s) {
    // Primero, analizar el string de formato para encontrar especificadores
    string format = s->s;
    vector<string> format_specs;

    // Extraer especificadores de formato
    for (size_t i = 0; i < format.length() - 1; i++) {
        if (format[i] == '%') {
            if (format[i + 1] == 'd' || format[i + 1] == 'i') {
                format_specs.push_back("%d");
                i++;
            } else if (format[i + 1] == 'u') {
                format_specs.push_back("%u");
                i++;
            } else if (format[i + 1] == 'l') {
                if (i + 2 < format.length() &&
                    (format[i + 2] == 'd' || format[i + 2] == 'i')) {
                    format_specs.push_back("%ld");
                    i += 2;
                } else if (i + 2 < format.length() && format[i + 2] == 'u') {
                    format_specs.push_back("%lu");
                    i += 2;
                }
            } else if (format[i + 1] == '%') {
                // Escape %%
                i++;
            }
        }
    }

    // Verificar que el número de especificadores coincide con los parámetros
    if (format_specs.size() != s->param_list.size()) {
        cerr << "Error: número de argumentos no coincide con especificadores de formato" << endl;
        cerr << "Especificadores: " << format_specs.size() << ", Argumentos: " << s->param_list.size() << endl;
        exit(1);
    }

    // Si no hay parámetros, solo imprimir el string
    if (s->param_list.empty()) {
        emit("leaq .LC" + to_string(label_counter) + "(%rip), %rdi");
        emit("movl $0, %eax");
        emit("call printf@PLT");
        return;
    }

    // Para cada parámetro, evaluar y determinar el formato correcto
    size_t param_idx = 0;
    for (auto it : s->param_list) {
        it->accept(this);  // Evaluar la expresión, resultado en %rax/%eax

        // Determinar qué formato usar basado en el tipo de la expresión
        string format_to_use;

        // Si hay especificador explícito en el formato, verificar compatibilidad
        if (param_idx < format_specs.size()) {
            string requested_format = format_specs[param_idx];

            // Verificar compatibilidad entre tipo y formato
            if (current_exp_type.ttype == ImpType::UINT) {
                if (requested_format != "%u" && requested_format != "%lu") {
                    cerr << "Warning: usando formato " << requested_format
                         << " para unsigned int" << endl;

                }
                if (requested_format == "%d") {
                    format_to_use = "print_fmt";
                }
                else if (requested_format == "%ld") {
                    format_to_use = "print_fmt_long";
                }
                else {
                    format_to_use = "print_fmt_unsigned";
                }
            } else if (current_exp_type.ttype == ImpType::LONG) {
                if (requested_format != "%ld" && requested_format != "%lu") {
                    cerr << "Warning: usando formato " << requested_format
                         << " para long int" << endl;
                }
                format_to_use = "print_fmt_long";
            } else {
                // INT o BOOL
                format_to_use = "print_fmt";
            }
        } else {
            // Sin especificador, usar formato por defecto según el tipo
            switch (current_exp_type.ttype) {
                case ImpType::UINT:
                    format_to_use = "print_fmt_unsigned";
                    break;
                case ImpType::LONG:
                    format_to_use = "print_fmt_long";
                    break;
                default:
                    format_to_use = "print_fmt";
                    break;
            }
        }

        // CAMBIO IMPORTANTE: Mover el valor al registro correcto según el tipo
        if (current_exp_type.ttype == ImpType::INT) {
            // Para int con signo, extender signo de 32 a 64 bits
            emit("movslq %eax, %rsi");
        } else if (current_exp_type.ttype == ImpType::UINT || current_exp_type.ttype == ImpType::BOOL) {
            // Para unsigned int, zero extend (automático al mover a registro de 32 bits)
            emit("movl %eax, %esi");  // Zero extend de 32 a 64 bits
        } else {
            // Para long (64 bits)
            emit("movq %rax, %rsi");
        }

        emit("leaq " + format_to_use + "(%rip), %rdi");
        emit("movl $0, %eax");
        emit("call printf@PLT");

        param_idx++;
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
    int while_num = ++etiquetas;
    string start_label = "while_" + to_string(while_num);
    string end_label = "endwhile_" + to_string(while_num);

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
    ImpType left_type = current_exp_type;

    // Guardar el valor izquierdo
    if (left_type.ttype == ImpType::INT || left_type.ttype == ImpType::UINT ||
        left_type.ttype == ImpType::BOOL) {
        emit("pushq %rax");  // Siempre push 64 bits para mantener alineación
    } else {
        emit("pushq %rax");
    }

    e->right->accept(this);
    ImpType right_type = current_exp_type;

    // Determinar el tipo resultante
    if (e->op >= LT_OP && e->op <= NE_OP) {
        current_exp_type.ttype = ImpType::BOOL;
    } else if (e->op == AND_OP || e->op == OR_OP) {
        current_exp_type.ttype = ImpType::BOOL;
    } else {
        // Promoción de tipos para operaciones aritméticas
        if (left_type.ttype == ImpType::LONG || right_type.ttype == ImpType::LONG) {
            current_exp_type.ttype = ImpType::LONG;
        } else if (left_type.ttype == ImpType::UINT || right_type.ttype == ImpType::UINT) {
            current_exp_type.ttype = ImpType::UINT;
        } else {
            current_exp_type.ttype = ImpType::INT;
        }
    }

    // Mover operando derecho a RCX/ECX
    if (current_exp_type.ttype == ImpType::INT || current_exp_type.ttype == ImpType::UINT) {
        emit("movl %eax, %ecx");
        emit("popq %rax");  // Recuperar operando izquierdo
        emit("movl %eax, %eax");  // Asegurar que solo usamos 32 bits
    } else {
        emit("movq %rax, %rcx");
        emit("popq %rax");
    }

    switch (e->op) {
        case PLUS_OP:
            if (current_exp_type.ttype == ImpType::INT || current_exp_type.ttype == ImpType::UINT) {
                emit("addl %ecx, %eax");
            } else {
                emit("addq %rcx, %rax");
            }
            break;

        case MINUS_OP:
            if (current_exp_type.ttype == ImpType::INT || current_exp_type.ttype == ImpType::UINT) {
                emit("subl %ecx, %eax");
            } else {
                emit("subq %rcx, %rax");
            }
            break;

        case MUL_OP:
            if (current_exp_type.ttype == ImpType::UINT) {
                emit("mull %ecx");  // unsigned multiply de 32 bits
            } else if (current_exp_type.ttype == ImpType::INT) {
                emit("imull %ecx, %eax");  // signed multiply de 32 bits
            } else {
                emit("imulq %rcx, %rax");  // 64 bits
            }
            break;

        case DIV_OP:
            if (current_exp_type.ttype == ImpType::UINT) {
                emit("xorl %edx, %edx");  // Clear edx para división unsigned de 32 bits
                emit("divl %ecx");
            } else if (current_exp_type.ttype == ImpType::INT) {
                emit("cltd");  // Sign extend eax a edx:eax
                emit("idivl %ecx");
            } else {
                emit("cqto");
                emit("idivq %rcx");
            }
            break;

        // Comparaciones - siempre comparan el tamaño correcto
        case LT_OP:
        case LE_OP:
        case EQ_OP:
        case GT_OP:
        case GE_OP:
        case NE_OP:
            if (left_type.ttype == ImpType::INT || left_type.ttype == ImpType::UINT) {
                emit("cmpl %ecx, %eax");
            } else {
                emit("cmpq %rcx, %rax");
            }

            // El resultado siempre es 0 o 1 en eax
            emit("movl $0, %eax");

            switch(e->op) {
                case LT_OP: emit("setl %al"); break;
                case LE_OP: emit("setle %al"); break;
                case EQ_OP: emit("sete %al"); break;
                case GT_OP: emit("setg %al"); break;
                case GE_OP: emit("setge %al"); break;
                case NE_OP: emit("setne %al"); break;
            }

            emit("movzbl %al, %eax");  // Zero extend al a eax
            break;

        case AND_OP:
            emit("testq %rax, %rax");
            emit("setne %al");
            emit("testq %rcx, %rcx");
            emit("setne %cl");
            emit("andb %cl, %al");
            emit("movzbq %al, %rax");
            break;

        case OR_OP:
            emit("testq %rax, %rax");
            emit("setne %al");
            emit("testq %rcx, %rcx");
            emit("setne %cl");
            emit("orb %cl, %al");
            emit("movzbq %al, %rax");
            break;

        default:
            cerr << "Error: Operador binario no soportado: " << e->op << endl;
            exit(1);
    }

    // Si el resultado es de 32 bits y podríamos necesitarlo como 64 bits después,
    // extender con signo o cero según sea necesario
    if (current_exp_type.ttype == ImpType::INT) {
        emit("movslq %eax, %rax");  // Sign extend
    } else if (current_exp_type.ttype == ImpType::UINT) {
        emit("movl %eax, %eax");  // Zero extend (automático en x86-64)
    }
}

void ImpCODE::visit(NumberExp* e) {
    // Determinar el tipo basado en literal_type
    if (e->literal_type == "unsigned int") {
        current_exp_type.ttype = ImpType::UINT;
        // Para unsigned int, usar movl para cargar solo 32 bits
        emit("movl $" + to_string(e->value & 0xFFFFFFFF) + ", %eax");
    } else if (e->literal_type == "long int" || e->literal_type == "long") {
        current_exp_type.ttype = ImpType::LONG;
        emit("movq $" + to_string(e->value) + ", %rax");
    } else {
        current_exp_type.ttype = ImpType::INT;
        // Para int, usar movl
        emit("movl $" + to_string(static_cast<int>(e->value)) + ", %eax");
    }
}

void ImpCODE::visit(BoolExp* e) {
    emit("movq $" + string(e->value ? "1" : "0") + ", %rax");
    current_exp_type.ttype = ImpType::BOOL;
}

void ImpCODE::visit(IdentifierExp* e) {
    if (!stack_offsets.count(e->name)) {
        cerr << "Variable no declarada: " << e->name << endl;
        exit(1);
    }

    if (type_env.check(e->name)) {
        current_exp_type = type_env.lookup(e->name);
        string mov_inst = get_mov_instruction(current_exp_type.ttype);
        string reg = get_register("rax", current_exp_type.ttype);

        emit(mov_inst + " " + to_string(stack_offsets[e->name]) + "(%rbp), %" + reg);

        // Si es de 32 bits y estamos en un contexto de 64 bits,
        // podríamos necesitar extender el signo
        if (current_exp_type.ttype == ImpType::INT) {
            emit("movslq %eax, %rax");  // Sign extend de 32 a 64 bits
        }
    } else {
        current_exp_type.ttype = ImpType::INT;
        emit("movl " + to_string(stack_offsets[e->name]) + "(%rbp), %eax");
        emit("movslq %eax, %rax");
    }
}


void ImpCODE::visit(FunDec* fdec) {
    env.add_level();
    type_env.add_level();

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
    // For simplicity, we'll assume all params are on stack, but use correct size
    int param_offset = 16; // Above RBP (8) and return address (8)
    for (size_t i = 0; i < fdec->param_ids.size(); ++i) {
        const auto& param = fdec->param_ids[i];
        ImpType param_type;
        if (fdec->param_types[i] == "int") {
            param_type.ttype = ImpType::INT;
        } else if (fdec->param_types[i] == "unsigned int") {
            param_type.ttype = ImpType::UINT;
        } else if (fdec->param_types[i] == "long int" || fdec->param_types[i] == "long") {
            param_type.ttype = ImpType::LONG;
        } else if (fdec->param_types[i] == "bool") {
            param_type.ttype = ImpType::BOOL;
        }
        stack_offsets[param] = param_offset;
        type_env.add_var(param, param_type);
        param_offset += 8;
    }

    // Process function body
    fdec->block->accept(this);

    // Function epilogue
    emit("  leave");
    emit("  ret");

    type_env.remove_level();
    env.remove_level();
}

void ImpCODE::visit(FunDecList *fdecs) {
    for (auto e: fdecs->fundecs) {
        e->accept(this);
    }
}
