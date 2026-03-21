#ifndef FALLINGSAND_SHADER_H
#define FALLINGSAND_SHADER_H

#include <string>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
  // constructor reads and builds the shader
  Shader(const char *vertex_path, const char *fragment_path);

  // use/activate the shader
  void use();

  // utility uniform functions
  void set_bool(const std::string &name, bool value) const;
  void set_int(const std::string &name, int value) const;
  void set_float(const std::string &name, float value) const;

private:
  void check_compile_errors(unsigned int shader, std::string type);

  unsigned int id_;
};

#endif //FALLINGSAND_SHADER_H