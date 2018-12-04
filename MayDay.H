#ifndef ___MayDay_H__INCLUDED___
#define ___MayDay_H__INCLUDED___
#include<iostream>

class MayDay // For testing purposes
{private:
  static bool s_exit;

public:
  static void StopOnExit(bool y) { s_exit = y; };
  static void Warning(std::string msg) { std::cout << msg << "\n"; };
  static void Error(std::string msg)
  {
    std::cout << msg << "\n";
    if(s_exit) std::exit(0);
  };
};



#endif //___MayDay_H__INCLUDED___