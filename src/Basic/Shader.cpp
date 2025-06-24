#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    try {

        // otwieranie plików z podanych ścieżek
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // odczyt całych plików do strumieni stringów
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        vShaderFile.close();
        fShaderFile.close();
        // zmiana strumieni na string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

    }
    catch (std::ios_base::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    // zmiana stringów na ciągi stałych charów
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // wypisanie błędów kompilacji
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) [[unlikely]] {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // fragment Shader
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // wypisanie błędów kompilacji
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) [[unlikely]]
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) [[unlikely]]
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // usuwamy shadery bo są już podpięte
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    std::string vertexCode;
    std::string geoCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream gShaderFile;
    std::ifstream fShaderFile;

    try {

        // otwieranie plików z podanych ścieżek
        vShaderFile.open(vertexPath);
        gShaderFile.open(geometryPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, gShaderStream, fShaderStream;
        // odczyt całych plików do strumieni stringów
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();
        // zmiana strumieni na string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geoCode = gShaderStream.str();

    }
    catch (std::ios_base::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    // zmiana stringów na ciągi stałych charów
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geoCode.c_str();

    unsigned int vertex, fragment, geometry;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // wypisanie błędów kompilacji
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) [[unlikely]] {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // geometry Shader
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    // wypisanie błędów kompilacji
    glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
    if (!success) [[unlikely]] {
        glGetShaderInfoLog(geometry, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // fragment Shader

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // wypisanie błędów kompilacji
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) [[unlikely]]
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, geometry);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) [[unlikely]]
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // usuwamy shadery bo są już podpięte
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

void Shader::use() noexcept {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const noexcept {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), int(value));
}

void Shader::setInt(const std::string& name, int value) const noexcept {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const noexcept {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const noexcept {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const noexcept {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const noexcept {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const noexcept {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const noexcept {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

[[nodiscard]] int Shader::getUniformLocation(const std::string& name) {
    
    if (uniformLocationCache.contains(name))
        return uniformLocationCache[name];

    const int location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) [[unlikely]] {
        std::cerr << "Warning: uniform '" << name << "' not found in shader!\n";
    }

    uniformLocationCache[name] = location;
    return location;

}
