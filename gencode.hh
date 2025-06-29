#ifndef GENCODE
#define GENCODE

#include <unordered_map>

#include "exp.h"
#include "imp_value_visitor.hh"
#include "environment.hh"

using namespace std;

class ImpCODE : public ImpValueVisitor {
private:
  Environment<int> env;
public:
  int current_offset;
  int label_counter;
  std::unordered_map<std::string, int> stack_offsets;
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
};


#endif

