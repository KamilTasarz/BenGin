#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader {

private:

    std::unordered_map<std::string, int> uniformLocationCache;

public:
    
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const char* vertexPath, const char* fragmentPath, const char* getmetryPath);
    void use() noexcept;

    void setBool(const std::string& name, bool value) const noexcept;
    void setInt(const std::string& name, int value) const noexcept;
    void setFloat(const std::string& name, float value) const noexcept;
    void setVec2(const std::string& name, const glm::vec2& value) const noexcept;
    void setVec3(const std::string& name, const glm::vec3& value) const noexcept;
    void setVec4(const std::string& name, const glm::vec4& value) const noexcept;
    void setVec4(const std::string& name, float x, float y, float z, float w) const noexcept;
    void setMat4(const std::string& name, const glm::mat4& mat) const noexcept;

    [[nodiscard]] int getUniformLocation(const std::string& name);

};
