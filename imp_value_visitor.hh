#ifndef IMP_VALUE_VISITOR
#define IMP_VALUE_VISITOR

#include "exp.h"
#include "imp_value.hh"

class ImpValueVisitor {
public:
  virtual void visit(Program* p) = 0;
  virtual void visit(Block* b) = 0;  // nuevo
  virtual void visit(VarDecList* e) = 0;
  virtual void visit(VarDec* e) = 0;
  virtual void visit(StatementList* e) = 0;
  virtual void visit(AssignStatement* e) = 0;
  virtual void visit(PrintStatement* e) = 0;
  virtual void visit(IfStatement* e) = 0;
  virtual void visit(WhileStatement* e) = 0;
  virtual void visit(ForStatement* e) = 0;
  virtual void visit(FunDec* e) = 0;
  virtual void visit(FunDecList* e) = 0;
  virtual void visit(ReturnStatement* e) = 0;
  virtual void visit(BinaryExp* e) = 0;
  virtual void visit(NumberExp* e) = 0;
  virtual void visit(BoolExp* e) = 0;
  virtual void visit(IdentifierExp* e) = 0;
  virtual void visit(FCallExp* e) = 0;
};


#endif
