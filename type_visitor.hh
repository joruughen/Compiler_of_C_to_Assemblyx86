#ifndef TYPE_VISITOR
#define TYPE_VISITOR

#include "environment.hh"
#include "exp.h"
#include "imp_type.hh"

class TypeVisitor {
private:
  Environment<ImpType> var_env;
  Environment<ImpType> fun_env;
  string current_fun;
  void promote_operands(ImpType& t1, ImpType& t2);
public:
  virtual void visit(Program* p);
  virtual void visit(Block* b);  // nuevo
  virtual void visit(VarDecList* e);
  virtual void visit(VarDec* e);
  virtual void visit(StatementList* e);
  virtual void visit(AssignStatement* e);
  virtual void visit(PrintStatement* e);
  virtual void visit(IfStatement* e);
  virtual void visit(WhileStatement* e);
  virtual void visit(ForStatement* e);
  virtual void visit(FunDec* e);
  virtual void visit(FunDecList* e);
  virtual void visit(ReturnStatement* e);
  virtual ImpType visit(BinaryExp* e);
  virtual ImpType visit(NumberExp* e);
  virtual ImpType visit(BoolExp* e);
  virtual ImpType visit(IdentifierExp* e);
  virtual ImpType visit(FCallExp* e);
};


#endif
