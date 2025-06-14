#ifndef IMP_VALUE_VISITOR
#define IMP_VALUE_VISITOR

#include "exp.h"
#include "imp_value.h"

class ImpValueVisitor {
public:
  virtual void visit(Program* p) = 0;
  virtual void visit(Body* b) = 0; 
  virtual void visit(VarDecList* e) = 0;
  virtual void visit(VarDec* e) = 0;
  virtual void visit(StatementList* e) = 0;
  virtual void visit(AssignStatement* e) = 0;
  virtual void visit(PrintfStatement* e) = 0;
  virtual void visit(IfStatement* e) = 0;
  virtual void visit(WhileStatement* e) = 0;
  virtual ImpValue visit(BinaryExp* e) = 0;
  virtual ImpValue visit(BoolExp* e) = 0;
  virtual ImpValue visit(NumberExp* e) = 0;
  virtual ImpValue visit(IdentifierExp* e) = 0;
};


#endif
