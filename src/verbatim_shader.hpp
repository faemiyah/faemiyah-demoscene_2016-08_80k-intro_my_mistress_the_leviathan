#ifndef VERBATIM_SHADER_HPP
#define VERBATIM_SHADER_HPP

#include "verbatim_seq.hpp"

/// Shader class.
class Shader
{
  private:
    /// GL identifier.
    const GLuint m_id;

    /// Shader parts.
    seq<const GLchar*> m_parts;

  private:
    /// Deleted copy constructor.
    Shader(const Shader&) = delete;
    /// Deleted assignment.
    Shader& operator=(const Shader&) = delete;

  public:
    /// Constructor.
    ///
    /// \param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
    /// \param part1 Shader part.
    Shader(GLenum type, const char *part1) :
      m_id(dnload_glCreateShader(type))
    {
      m_parts.push_back(part1);

      this->compile();
    }

  public:
   /// Accessor.
   ///
   /// \return OpenGL id.
   GLuint getId()
   {
     return m_id;
   }

#if defined(USE_LD)
   /// Get a string representation of the shader.
   ///
   /// \return String representation in (more) human-readable form.
   std::string str() const
   {
     GlslShaderSource glsl_source;

     for(const GLchar *vv : m_parts)
     {
       glsl_source.add(std::string(vv));
     }

#if defined(DNLOAD_GLESV2)
     return glsl_source.str();
#else
     return GlslShaderSource::convert_glesv2_gl(glsl_source.str());
#endif
   }
#endif

  private:
   /// Compile this shader.
   ///
   /// Will terminate program on failure.
   void compile() const
   {
#if defined(USE_LD)
     std::string pretty_source = this->str();
     const GLchar *pretty_source_c_str = pretty_source.c_str();
     dnload_glShaderSource(m_id, 1, &pretty_source_c_str, NULL);
#else
     dnload_glShaderSource(m_id, m_parts.size(), const_cast<const GLchar**>(m_parts.getData()), NULL);
#endif

     dnload_glCompileShader(m_id);

#if defined(USE_LD)
     std::string log = GlslShaderSource::get_shader_info_log(m_id);
     if(0 < log.length())
     {
       std::cout << GlslShaderSource::string_add_line_numbers(pretty_source) << std::endl;
       std::cout << log << std::endl;
     }

     if(!GlslShaderSource::get_shader_compile_status(m_id))
     {
       teardown();
       exit(1);
     }
#endif
   }
};

#endif
