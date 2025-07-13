#pragma once

#include <string>

class Shader {
  public:
    unsigned int ID;
    Shader(const char *const vertexSrc, const char *const fragSrc);
    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void set2Float(const std::string &name, float value1, float value2) const;
};
