module;

#include <iostream>
#include <string>

export module greeter_module;

export void Greeter(const std::string& name) {
  std::cout << "From Greeter in module: Hello, " << name << "!" << std::endl;
}