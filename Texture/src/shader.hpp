#pragma once

#include <string>
#include <vector>
#include "main.hpp"

struct ShaderFailure {
    enum class Cause { Create, Compile, IO };
    Cause cause;
    char log[512];
    ShaderFailure(Cause cause): cause(cause) {}
    ShaderFailure(): cause(Cause::IO) {}
};

namespace vinz {
    class Shader {
        public:
            enum class Type {
                Vertex, Fragment
            };

            Shader() = delete;
            Shader(const std::string &filename, Type type);
            Shader(const std::vector<char> &shader, Type type);
            Shader(Shader &&s) = default;
            ~Shader();

            /* Forbid copy or assignment */
            Shader(Shader const &) = delete;
            Shader& operator=(Shader const &) = delete;
            Shader& operator=(Shader &&) = default;

            GLuint id;
        private:
            Type type;

            bool from_bytes(const std::vector<char> &shader, Type type,
                            ShaderFailure &cause);
    };

    class ShaderProgram {
        private:
            GLuint id;

        public:
            ShaderProgram():id(0) {}
            ~ShaderProgram();
            ShaderProgram(ShaderProgram const &) = delete;
            ShaderProgram(ShaderProgram &&) = default;

            ShaderProgram& operator=(ShaderProgram const &) = delete;
            ShaderProgram& operator=(ShaderProgram &&) = default;
            bool init(const Shader &vert, const Shader &frag,
                      ShaderFailure &cause);

            void setBool(const std::string &name, bool value) const;
            void setInt(const std::string &name, int value) const;
            void setFloat(const std::string &name, float value) const;
            void activate(void) const;
    };
};
