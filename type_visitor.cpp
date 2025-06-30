#include "type_visitor.hh"

vector<ImpType> parse_format_string(const string& format) {
    vector<ImpType> expected_types;

    for (size_t i = 0; i < format.length(); i++) {
        if (format[i] == '%' && i + 1 < format.length()) {
            char specifier = format[i + 1];
            ImpType type;

            switch (specifier) {
                case 'd':
                case 'i':
                    type.ttype = ImpType::INT;
                    expected_types.push_back(type);
                    break;
                case 'u':
                    type.ttype = ImpType::UINT;
                    expected_types.push_back(type);
                    break;
                case 'l':
                    // Check for %ld
                    if (i + 2 < format.length() &&
                        (format[i + 2] == 'd' || format[i + 2] == 'i')) {
                        type.ttype = ImpType::LONG;
                        expected_types.push_back(type);
                        i++; // Skip the 'l'
                        } else {
                            cout << "Unsupported printf format specifier: %l"
                                 << (i + 2 < format.length() ? format[i + 2] : '?') << endl;
                            exit(1);
                        }
                    break;
                case '%':
                    // Literal % - no parameter expected
                    break;
                default:
                    cout << "Unsupported printf format specifier: %" << specifier << endl;
                    exit(1);
            }
            i++; // Skip the specifier character
        }
    }

    return expected_types;
}

bool is_printf_compatible(const ImpType& actual, const ImpType& expected) {
    // Allow printing unsigned as int with %d (like C does, though value may be odd)
    if ((expected.ttype == ImpType::INT && actual.ttype == ImpType::UINT) ||
        (expected.ttype == ImpType::UINT && actual.ttype == ImpType::INT)) {
        return true;
        }
    // Allow printing unsigned as long with %ld (C allows this, value may be odd)
    if ((expected.ttype == ImpType::LONG && actual.ttype == ImpType::UINT) ||
        (expected.ttype == ImpType::LONG && actual.ttype == ImpType::INT)) {
        return true;
        }
    // Exact match
    if (actual.ttype == expected.ttype) {
        return true;
    }
    // Allow bool to be printed as int (common in C)
    if (expected.ttype == ImpType::INT && actual.ttype == ImpType::BOOL) {
        return true;
    }
    if (actual.ttype == ImpType::FUN) {
        if (actual.types.back() == expected.ttype) {
            return true;
        }
    }
    // No other implicit conversions for your limited type system
    return false;
}


void TypeVisitor::promote_operands(ImpType& t1, ImpType& t2) {
    // Balancing
    if (t1.ttype == ImpType::LONG || t2.ttype == ImpType::LONG) {
        t1.ttype = ImpType::LONG;
        t2.ttype = ImpType::LONG;
    }
    else if (t1.ttype == ImpType::UINT || t2.ttype == ImpType::UINT) {
        t1.ttype = ImpType::UINT;
        t2.ttype = ImpType::UINT;
    }
}

void TypeVisitor::visit(Program *p) {
    fun_env.add_level();
    for (auto fdec : p->fundecs->fundecs) {
        list<string> param_types;
        for (auto& param : fdec->param_types) {
            param_types.push_back(param);
        }
        // this->current_fun = fdec->id; not neccesary
        ImpType func_type;
        if (!func_type.set_fun_type(param_types, fdec->type)) {
            cerr << "Invalid function type for " << fdec->id << endl;
            exit(1);
        }
        fun_env.add_var(fdec->id, func_type);
    }

    //main function
    ImpType main_type;
    list<string> main_param_types(p->main->param_types.begin(),p->main->param_types.end());
    main_type.set_fun_type(main_param_types,p->main->type);
    if (main_type.types.back() != ImpType::INT) {
        cerr << "Invalid main function type" << endl;
        exit(1);
    }
    fun_env.add_var("main", main_type);
    // Second pass: process function bodies
    p->fundecs->accept(this);
    this->current_fun = "main";
    p->main->accept(this);
    fun_env.remove_level();
}

void TypeVisitor::visit(Block *b) {
    var_env.add_level();
    b->vardecs->accept(this);
    b->slist->accept(this);
    var_env.remove_level();
}

void TypeVisitor::visit(VarDecList *e) {
    for (auto v:e->vardecs) {
        v->accept(this);
    }
}
//WE DIDNT MAKE THE TYPEVISIT THE ARGUMENTS THEY ARE NOT BEING PASSED AS VARIABLES
// right how do we map variable to arguments

void TypeVisitor::visit(VarDec *e) {
    for (auto it = e->vars.begin(); it != e->vars.end(); ++it) {
        if (var_env.check(*it)) {
            cout << "Variable " << e << " already defined" << endl;
            exit(1);
        }
        else {
            ImpType type;
            type.set_basic_type(e->type);
            var_env.add_var(*it, type);
        }
    }
}

void TypeVisitor::visit(StatementList *e) {
    for (auto s:e->stms) {
        s->accept(this);
    }
}

void TypeVisitor::visit(AssignStatement *e) {
    ImpType var_type = var_env.lookup(e->id);
    ImpType rhs_type = e->rhs->accept(this);

    // Si alguno es función, usar su tipo de retorno
    if (var_type.ttype == ImpType::FUN && !var_type.types.empty()) {
        var_type.ttype = var_type.types.back();
    }
    if (rhs_type.ttype == ImpType::FUN && !rhs_type.types.empty()) {
        rhs_type.ttype = rhs_type.types.back();
    }

    // Permitir conversiones implícitas entre tipos numéricos compatibles
    if ((var_type.ttype == ImpType::UINT && rhs_type.ttype == ImpType::INT) ||
        (var_type.ttype == ImpType::LONG && (rhs_type.ttype == ImpType::INT || rhs_type.ttype == ImpType::UINT)) ||
        (var_type.ttype == ImpType::INT && rhs_type.ttype == ImpType::LONG)) {
        cerr << "Warning: posible conversión implícita en asignación" << endl;
        } else if (!var_type.match(rhs_type)) {
            cerr << "Error: tipos incompatibles en asignación" << endl;
            exit(1);
        }
}

void TypeVisitor::visit(PrintStatement *e) {
    // Parse format string to determine expected parameter types
    vector<ImpType> expected_types = parse_format_string(e->s);

    // Check parameter count matches format specifiers
    if (expected_types.size() != e->param_list.size()) {
        cout << "printf parameter count mismatch: format string expects "
             << expected_types.size() << " parameters, got "
             << e->param_list.size() << endl;
        exit(1);
    }

    for (size_t i = 0; i < e->param_list.size(); i++) {
        ImpType actual_type = e->param_list[i]->accept(this);
        ImpType expected_type = expected_types[i];

        // Allow some implicit conversions for printf
        if (!is_printf_compatible(actual_type, expected_type)) {
            cout << "printf parameter " << (i + 1) << " type mismatch: "
                 << "expected " << expected_type << ", got " << actual_type << endl;
            exit(1);
        }
    }
}

void TypeVisitor::visit(IfStatement *e) {
    ImpType condType = e->condition->accept(this);
    ImpType int_type;
    int_type.set_basic_type(ImpType::BOOL);
    if (!condType.match(int_type)) {
        cout << "If condition must be int convertible, got " << condType << endl;
        exit(1);
    }
    e->then->accept(this);
    if (e->els != nullptr) {
        e->els->accept(this);
    }
}


void TypeVisitor::visit(WhileStatement *e) {
    ImpType condType = e->condition->accept(this);
    ImpType int_type;
    int_type.set_basic_type(ImpType::BOOL);
    if (!condType.match(int_type)) {
        cout << "While condition must be int convertible, got " << condType << endl;
        exit(1);
    }
    e->b->accept(this);
}

void TypeVisitor::visit(ForStatement *e) {
    var_env.add_level();
    if (e->dcl != nullptr) {
        e->dcl->accept(this);
    }
    if (e->init != nullptr) {
        e->init->accept(this);
    }
    if (e->condition != nullptr) {
        ImpType condType = e->condition->accept(this);
        ImpType int_type;
        int_type.set_basic_type(ImpType::BOOL);
        if (!condType.match(int_type)) {
            cout << "For condition must be int convertible, got " << condType << endl;
            exit(1);
        }
    }

    if (e->update != nullptr) {
        e->update->accept(this);
    }

    e->body->accept(this);//... for(int x = 0,...,..){int x = 12;}?? what is expected behaviour??

    var_env.remove_level();
}

void TypeVisitor::visit(FunDec* e) {
    var_env.add_level();//for argument params
    //type visit the arguments
    for (int i = 0; i < e->param_types.size(); i++) {
        ImpType type;
        type.set_basic_type(e->param_types[i]);
        var_env.add_var(e->param_ids[i], type);
    }
    e->block->accept(this);
    var_env.remove_level();
}

void TypeVisitor::visit(FunDecList *e) {
    for (auto f:e->fundecs) {
        this->current_fun = f->id;
        f->accept(this);
    }
}

void TypeVisitor::visit(ReturnStatement *e) {
    ImpType current_func_type = fun_env.lookup(current_fun);
    ImpType return_type = e->e->accept(this);
    if (current_func_type.types.back() != return_type.ttype) { //exactamente igual
        cerr << "Return type mismatch in " << current_fun << endl;
    }
}

ImpType TypeVisitor::visit(BinaryExp *e) {
    ImpType left = e->left->accept(this);
    ImpType right = e->right->accept(this);

    // Si alguno es función, usar su tipo de retorno
    if (left.ttype == ImpType::FUN && !left.types.empty()) {
        left.ttype = left.types.back();
    }
    if (right.ttype == ImpType::FUN && !right.types.empty()) {
        right.ttype = right.types.back();
    }

    // Operadores lógicos requieren bool
    if (e->op == AND_OP || e->op == OR_OP) {
        if (left.ttype != ImpType::BOOL || right.ttype != ImpType::BOOL) {
            cerr << "Error: operadores lógicos requieren operandos booleanos" << endl;
            exit(1);
        }
        ImpType result;
        result.ttype = ImpType::BOOL;
        return result;
    }

    // Operadores de comparación
    if (e->op >= LT_OP && e->op <= NE_OP) {
        if (!left.match(right)) {
            cerr << "Error: tipos incompatibles en comparación" << endl;
            exit(1);
        }
        ImpType result;
        result.ttype = ImpType::BOOL;
        return result;
    }

    // Operadores aritméticos
    if (!left.match(right)) {
        cerr << "Error: tipos incompatibles en operación aritmética" << endl;
        exit(1);
    }
    promote_operands(left, right);
    return left;
}

ImpType TypeVisitor::visit(NumberExp *e) {
    ImpType type;

    if (e->literal_type == "long int" || e->literal_type == "long") {
        type.ttype = ImpType::LONG;
    } else if (e->literal_type == "unsigned int") {
        type.ttype = ImpType::UINT;
    } else if (e->literal_type == "unsigned long") {
        type.ttype = ImpType::LONG;  // Por ahora tratamos unsigned long como long
    } else {
        type.ttype = ImpType::INT;
    }

    return type;
}

ImpType TypeVisitor::visit(BoolExp *e) {
    ImpType type;
    type.ttype = ImpType::BOOL;
    return type;
}

ImpType TypeVisitor::visit(IdentifierExp *e) {
    return var_env.lookup(e->name);
}

ImpType TypeVisitor::visit(FCallExp *e) {
    //falta evaluar los argumentos con lo q se llaman
    return fun_env.lookup(e->id);
}
