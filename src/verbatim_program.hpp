#ifndef VERBATIM_PROGRAM_HPP
#define VERBATIM_PROGRAM_HPP

#include "verbatim_mat4.hpp"
#include "verbatim_shader.hpp"
#include "verbatim_vec2.hpp"

// Forward declaration.
class VertexBuffer;

/// Shader program class.
class Program
{
  private:
    /// Max amount of identifiers in a program (ASCII7). */
    static const int PROGRAM_MAX_IDENTIFIERS = 128;

  private:
    /// Currently bound program.
    static const Program* g_current_program;

    /// Currently bound array buffer for this program.
    static const VertexBuffer* g_programmed_array_buffer;

  private:
    /// OpenGL id.
    GLuint m_id;

    /// Vertex shader.
    Shader m_vertex_shader;

    /// Vertex shader.
    Shader m_fragment_shader;

    /// Attributes.
    GLint m_attributes[PROGRAM_MAX_IDENTIFIERS];

    /// Uniforms.
    GLint m_uniforms[PROGRAM_MAX_IDENTIFIERS];

  public:
    /// Constructor.
    ///
    /// \param vertex_source Vertex shader source.
    /// \param fragment_source Fragment shader source.
    Program(const char *vertex_source, const char *fragment_source) :
      m_id(dnload_glCreateProgram()),
      m_vertex_shader(GL_VERTEX_SHADER, vertex_source),
      m_fragment_shader(GL_FRAGMENT_SHADER, fragment_source)
    {
      link();
    }

  private:
    /// Link this program.
    ///
    /// Will terminate program on failure.
    void link()
    {
      dnload_glAttachShader(m_id, m_vertex_shader.getId());
      dnload_glAttachShader(m_id, m_fragment_shader.getId());

      dnload_glLinkProgram(m_id);

#if defined(USE_LD)
      std::string log = GlslShaderSource::get_program_info_log(m_id);
      if(0 < log.length())
      {
        std::cout << log << std::endl;
      }

      if(!GlslShaderSource::get_program_link_status(m_id))
      {
        teardown();
        exit(1);
      }

      if(is_verbose())
      {
        std::cout << "|program(" << m_id << ")\n";
      }
#endif

      for(int ii = 0; (ii < PROGRAM_MAX_IDENTIFIERS); ++ii)
      {
        char identifier_string[] = { static_cast<char>(ii), 0 };
        m_attributes[ii] = dnload_glGetAttribLocation(m_id, identifier_string);
        m_uniforms[ii] = dnload_glGetUniformLocation(m_id, identifier_string);
#if defined(USE_LD)
        if(is_verbose())
        {
          if(m_attributes[ii] >= 0)
          {
            std::cout << "|attribute '" << identifier_string << "': " << m_attributes[ii] << std::endl;
          }
          if(m_uniforms[ii] >= 0)
          {
            std::cout << "|uniform '" << identifier_string << "': " << m_uniforms[ii] << std::endl;
          }
        }
#endif
      }
    }

  public:
    /// Bind attribute.
    ///
    /// \param name Identifier name.
    /// \param GLint size Number of components.
    /// \param type Component type.
    /// \param normalized Should fixed-point values be normalized?
    /// \param pointer Pointer to first component.
    void attribPointer(int name, GLint size, GLenum type, bool normalized, GLsizei stride,
        const GLvoid* pointer) const
    {
      GLint attribute_name = m_attributes[name];
      if(0 <= attribute_name)
      {
        vgl::attrib_array_enable(attribute_name);
        dnload_glVertexAttribPointer(attribute_name, size, type, static_cast<GLboolean>(normalized), stride,
            pointer);
      }
    }

    /// Send uniform.
    ///
    /// \param name Identifier name.
    /// \param value Uniform value.
    void uniform(int name, GLint value) const
    {
      GLint loc = m_uniforms[name];
      if(0 <= loc)
      {
        dnload_glUniform1i(loc, value);
      }
    }

    /// Send uniform.
    ///
    /// \param name Identifier name.
    /// \param value Uniform value.
    void uniform(int name, GLfloat value) const
    {
      GLint loc = m_uniforms[name];
      if(0 <= loc)
      {
        dnload_glUniform1f(loc, value);
      }
    }

    /// Send uniform.
    ///
    /// \param name Identifier name.
    /// \param value Uniform value.
    void uniform(int name, const vec2 &value) const
    {
      GLint loc = m_uniforms[name];
      if(0 <= loc)
      {
        dnload_glUniform2fv(loc, 1, value.getData());
      }
    }

    /// Send uniform.
    ///
    /// \param name Identifier name.
    /// \param value Uniform value.
    void uniform(int name, const vec3 &value) const
    {
      GLint loc = m_uniforms[name];
      if(0 <= loc)
      {
        dnload_glUniform3fv(loc, 1, value.getData());
      }
    }

    /// Send uniform.
    ///
    /// \param name Identifier name.
    /// \param v1 First value component.
    /// \param v2 Second value component.
    /// \param v3 Third value component.
    /// \param v4 Fourth value component.
    void uniform(int name, float v1, float v2, float v3, float v4) const
    {
      GLint loc = m_uniforms[name];
      if(0 <= loc)
      {
        dnload_glUniform4f(loc, v1, v2, v3, v4);
      }
    }

    /// Send uniform.
    ///
    /// \param name Identifier name.
    /// \param value Uniform value.
    void uniform(int name, const mat3 &value) const
    {
      GLint loc = m_uniforms[name];
      if(0 <= loc)
      {
        dnload_glUniformMatrix3fv(loc, 1, static_cast<GLboolean>(false), value.getData());
      }
    }

    /// Send uniform.
    ///
    /// \param name Identifier name.
    /// \param data Uniform data.
    /// \param count Uniform count.
    void uniform(int name, const mat4 *data, unsigned count) const
    {
      GLint loc = m_uniforms[name];
      if(0 <= loc)
      {
        dnload_glUniformMatrix4fv(loc, count, static_cast<GLboolean>(false), data[0].getData());
      }
    }
    /// Send uniform (wrapper).
    ///
    /// \param name Identifier name.
    /// \param value Uniform value.
    void uniform(int name, const mat4 &value) const
    {
      uniform(name, &value, 1);
    }

    /// Use this program.
    void use() const
    {
      if(g_current_program == this)
      {
        return;
      }

      dnload_glUseProgram(m_id);
      g_current_program = this;
      reset_array_buffer();
    }

  public:
    /// Reset programmed vertex buffer.
    ///
    /// Must be done when changing a program.
    static void reset_array_buffer()
    {
      g_programmed_array_buffer = NULL;
    }

    /// Select a vertex buffer.
    ///
    /// \param op Vertex buffer to use.
    /// \return True if buffer was changed, false if not.
    static bool select_array_buffer(const VertexBuffer *op)
    {
      if(g_programmed_array_buffer == op)
      {
        return false;
      }
      g_programmed_array_buffer = op;
      return true;
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "Program(" << m_id << ")";
    }
#endif
};

const Program *Program::g_current_program = NULL;
const VertexBuffer *Program::g_programmed_array_buffer = NULL;

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
std::ostream& operator<<(std::ostream &lhs, const Program &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
