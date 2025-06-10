#ifndef IMP_TYPE
#define IMP_TYPE

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <list>
#include <vector>

using namespace std;

class ImpType {
public:
  enum TType { NOTYPE=0, VOID, INT, BOOL, FUN };
  static const char* type_names[5];
  TType ttype;
  vector<TType> types;
  bool match(const ImpType&);
  bool set_basic_type(string s);
  bool set_basic_type(TType tt);
  bool set_fun_type(list<string> slist, string s);
private:
  TType string_to_type(string s);
};

std::ostream& operator << ( std::ostream& outs, const ImpType & type );


#endif
