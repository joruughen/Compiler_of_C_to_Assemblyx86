// #ifndef IMP_VALUE
// #define IMP_VALUE
//
// #include <sstream>
// #include <iostream>
// #include <stdlib.h>
// #include <cstring>
//
// using namespace std;
//
//
// enum ImpVType { NOTYPE=0, TINT, TBOOL,TUINT,TLONG };
//
// class ImpValue {
// public:
//   ImpValue();
//   ImpVType type;
//   int int_value;
//   long long_value;
//   unsigned int uint_value;
//   bool bool_value;
//   void set_default_value(ImpVType tt);
//   static ImpVType get_basic_type(string s);
// };
//
// std::ostream& operator << ( std::ostream& outs, const ImpValue & v );
//
//
// #endif
