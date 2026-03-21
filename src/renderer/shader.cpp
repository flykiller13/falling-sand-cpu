#include "falling-sand/renderer/shader.h"


Shader::Shader(const char *vertex_path, const char *fragment_path) {
  // retrieve the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions(std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::badbit);

  try {
    // open files
    vShaderFile.open(vertex_path);
    fShaderFile.open(fragment_path);

    std::stringstream vShaderStream, fShaderStream;

    vShaderFile.clear();
    vShaderFile.seekg(0, std::ios::beg);

    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    // convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    // close file handlers
    vShaderFile.close();
    fShaderFile.close();

    std::cout << "Vertex shader loaded successfully from: " << vertex_path <<
        std::endl;
    std::cout << "Fragment shader loaded successfully from: " << fragment_path
        <<
        std::endl;
  } catch (std::ifstream::failure &e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    std::cout << "Exception: " << e.what() << std::endl;
    std::cout << "Vertex path: " << vertex_path << std::endl;
    std::cout << "Fragment path: " << fragment_path << std::endl;
  }

  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // compile shaders
  unsigned int vertex, fragment;

  // vertex Shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, nullptr);
  glCompileShader(vertex);
  check_compile_errors(vertex, "VERTEX");

  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, nullptr);
  glCompileShader(fragment);
  check_compile_errors(fragment, "FRAGMENT");

  // shader Program
  id_ = glCreateProgram();
  glAttachShader(id_, vertex);
  glAttachShader(id_, fragment);
  glLinkProgram(id_);
  check_compile_errors(id_, "PROGRAM");

  // delete the shaders as they're linked into our program now
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() {
  glUseProgram(id_);
}

void Shader::set_bool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}

void Shader::set_int(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::check_compile_errors(unsigned int shader, std::string type) {
  int success;
  char infoLog[1024];

  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
          << infoLog <<
          "\n -- --------------------------------------------------- -- "
          << std::endl;
    } else {
      std::cout << type << " shader compiled successfully" << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
          << infoLog <<
          "\n -- --------------------------------------------------- -- "
          << std::endl;
    } else {
      std::cout << "Shader program linked successfully" << std::endl;
    }
  }
}