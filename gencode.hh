#ifndef GENCODE
#define GENCODE

#include <unordered_map>

#include "exp.h"
#include "imp_value_visitor.hh"
#include "environment.hh"
#include <vector>
#include <utility>

using namespace std;

class ImpCODE : public ImpValueVisitor {
private:
  Environment<int> env;
  Environment<ImpType> type_env;
public:
  int current_offset;
  int label_counter;
  std::unordered_map<std::string, int> stack_offsets;
  ImpType current_exp_type;
  int string_counter = 0;
  vector<pair<string, string>> pending_strings;
  int etiquetas=0;
  void generate(Program*);
  void visit(Program*);
  void visit(Block*);
  void visit(VarDecList*);
  void visit(VarDec*);
  void visit(StatementList*);
  void visit(AssignStatement*);
  void visit(ReturnStatement *e);
  void visit(ForStatement *e);
  void visit(PrintStatement*);
  void visit(IfStatement*);
  void visit(FunDec*);
  void visit(FunDecList*);
  void visit(WhileStatement*);
  void visit(FCallExp* e);
  void visit(BinaryExp* e);
  void visit(BoolExp* e);
  void visit(NumberExp* e);
  void visit(IdentifierExp* e);
  void emit(const string& code) {
    cout << code << endl;
  }

  string new_label(const string& prefix) {
    return prefix + to_string(label_counter++);
  }

  string get_mov_instruction(ImpType::TType type) {
    switch (type) {
      case ImpType::INT:
      case ImpType::UINT:
      case ImpType::BOOL:
        return "movl";
      case ImpType::LONG:
        return "movq";
      default:
        return "movq"; // default
    }
  }

  string get_register(string base_reg, ImpType::TType type) {
    // Convierte RAX -> EAX para tipos de 32 bits
    if (type == ImpType::INT || type == ImpType::UINT || type == ImpType::BOOL) {
      if (base_reg == "rax") return "eax";
      if (base_reg == "rcx") return "ecx";
      if (base_reg == "rdx") return "edx";
      if (base_reg == "rsi") return "esi";
      if (base_reg == "rdi") return "edi";
      if (base_reg == "r8") return "r8d";
      if (base_reg == "r9") return "r9d";
    }
    return base_reg;
  }


  int get_type_size(ImpType::TType type) {
    switch (type) {
      case ImpType::INT:
      case ImpType::UINT:
      case ImpType::BOOL:
        return 4;
      case ImpType::LONG:
        return 8;
      default:
        return 8;
    }
  }

  string get_or_create_string_literal(const string& content) {
    string label = "str_" + to_string(string_counter++);

    string processed = content;
    size_t pos = 0;
    while ((pos = processed.find("\\n", pos)) != string::npos) {
      processed.replace(pos, 2, "\\n");
      pos += 2;
    }
    pos = 0;
    while ((pos = processed.find("\\t", pos)) != string::npos) {
      processed.replace(pos, 2, "\\t");
      pos += 2;
    }

    pending_strings.push_back({label, processed});

    return label;
  }


};


#endif

