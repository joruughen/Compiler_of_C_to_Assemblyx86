#include "imp_type.hh"

#include "imp_value.hh"

const char* ImpType::type_names[7] = { "notype", "void", "int", "bool", "fun", "unsigned_int", "long" }; //debugger

bool ImpType::match(const ImpType& t) {
  if (this->ttype == INT && t.ttype == LONG) return true;
  if (this->ttype == LONG && t.ttype == INT) return true;
  if (this->ttype == INT && t.ttype == UINT) return true;
  if (this->ttype == UINT && t.ttype == INT) return true;
  if (this->ttype == ImpType::FUN) {
    if (t.types.size() != this->types.size()) return false;
    for (int i=0; i < this->types.size(); i++)
      if (t.types[i] != this->types[i]) return false;
  }
  if (this->ttype != t.ttype) return false;

  return true;
}

bool ImpType::set_basic_type(string s) {
  TType tt;
  bool ret = true;
  tt = string_to_type(s);
  if (tt == ImpType::NOTYPE) ret = false;
  this->ttype = tt;
  return ret;
}

bool ImpType::set_basic_type(TType tt) {
  bool ret = true;
  switch(tt) {
  case ImpType::INT:
  case ImpType::BOOL:
  case ImpType::VOID:
  case ImpType::LONG:
  case ImpType::UINT:
    this->ttype = tt; break;
  default:
    ret = false;
  }
  return ret;
}


bool ImpType::set_fun_type(list<string> slist, string s) {
  list<string>::iterator it;
  ttype = ImpType::NOTYPE;
  types.clear();
  for (it = slist.begin(); it != slist.end(); ++it) {
    ImpType type;
    type.set_basic_type(*it);
    if (type.ttype==ImpType::INT || type.ttype==ImpType::BOOL||type.ttype==ImpType::LONG||type.ttype==ImpType::UINT)
      types.push_back(type.ttype);
    else {     
      types.clear();
      return false;
    }
  }
  ImpType rtype;
  if (rtype.set_basic_type(s))
    types.push_back(rtype.ttype);
  else {
    types.clear();
    return false;
  }
  ttype = ImpType::FUN;
  return true;
}


ImpType::TType ImpType::string_to_type(string s) {
  TType tt;
  if (s.compare("int")==0) tt = ImpType::INT;
  else if (s.compare("bool")==0) tt = ImpType::BOOL;
  else if (s.compare("void")==0) tt = ImpType::VOID;
  else if (s.compare("long int")==0) tt = ImpType::LONG;
  else if (s.compare("unsigned int") == 0) tt = ImpType::UINT;
  else tt = ImpType::NOTYPE; 
  return tt;
}

std::ostream& operator << ( std::ostream& outs, const ImpType & type )
{
  outs << ImpType::type_names[type.ttype];
  if (type.ttype == ImpType::FUN) {
    int num_params = type.types.size()-1;
    ImpType::TType rtype = type.types[num_params];
    outs << "(";
    if (num_params > 0) outs << ImpType::type_names[type.types[0]];    
    for(int i=1; i<num_params; i++)
      outs << "," << ImpType::type_names[type.types[i]];
    outs << ")->" << ImpType::type_names[rtype];    
  }
  return outs;
}


