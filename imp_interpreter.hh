// #ifndef IMP_INTERPRETER
// #define IMP_INTERPRETER
//
// #include <unordered_map>
//
// #include "exp.h"
// #include "imp_value_visitor.hh"
// #include "environment.hh"
//
// using namespace std;
//
// class ImpInterpreter : public ImpValueVisitor {
// private:
//   Environment<ImpValue> env;
// public:
//   void interpret(Program*);
//   void visit(Program*);
//   void visit(Block*);
//   void visit(VarDecList*);
//   void visit(VarDec*);
//   void visit(StatementList*);
//   void visit(AssignStatement*);
//   void visit(PrintStatement*);
//   void visit(IfStatement*);
//   void visit(WhileStatement*);
//   void visit(ReturnStatement*);
//   void visit(ForStatement*);
//   ImpValue visit(BinaryExp* e);
//   ImpValue visit(BoolExp* e);
//   ImpValue visit(NumberExp* e);
//   ImpValue visit(IdentifierExp* e);
//   ImpValue visit(FCallExp* e);
//
// };
//
//
// #endif
//
