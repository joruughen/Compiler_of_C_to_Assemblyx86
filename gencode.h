#ifndef GENCODE
#define GENCODE

#include <unordered_map>

#include "exp.h"
#include "imp_value_visitor.h"
#include "environment.h"

using namespace std;

class ImpCODE : public ImpValueVisitor {
private:
  Environment<ImpValue> env;
  

public:
  int current_offset;
  std::unordered_map<std::string, int> stack_offsets;
  int etiquetas=0;
  void interpret(Program*);
  void visit(Program*);
  void visit(Body*);
  void visit(VarDecList*);
  void visit(VarDec*);
  void visit(StatementList*);
  void visit(AssignStatement*);
  void visit(PrintStatement*);
  void visit(IfStatement*);
  void visit(WhileStatement*);
  ImpValue visit(BinaryExp* e);
  ImpValue visit(BoolExp* e);
  ImpValue visit(NumberExp* e);
  ImpValue visit(IdentifierExp* e);
};


#endif

