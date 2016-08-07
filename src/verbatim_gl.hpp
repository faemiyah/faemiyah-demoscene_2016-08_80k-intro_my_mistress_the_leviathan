#ifndef VERBATIM_GL_HPP
#define VERBATIM_GL_HPP

#include "verbatim_realloc.hpp"

/// Clamp float.
///
/// \param val Value.
/// \param min_val Minimum value.
/// \param max_val Maximum value.
static float clamp(float val, float min_val, float max_val)
{
  return std::min(std::max(val, min_val), max_val);
}

/// Convert 4.12 signed fixed point number to floating point.
///
/// \param op Input number.
/// \return Converted number.
float fixed_4_12_to_float(int16_t op)
{
  return static_cast<float>(op) * (1.0f / 4096.0f);
}

/// Convert 8.8 signed fixed point number to floating point.
///
/// \param op Input number.
/// \return Converted number.
float fixed_8_8_to_float(int16_t op)
{
  return static_cast<float>(op) * (1.0f / 256.0f);
}

/// Convert 14.2 signed fixed point number to floating point.
///
/// \param op Input number.
/// \return Converted number.
float fixed_14_2_to_float(int16_t op)
{
  return static_cast<float>(op) * 0.25f;
}

/// Weight normalized space float away from half.
///
/// \param op Input value.
/// \return Weighted value.
static float fnorm_weigh_away(float op)
{
  if(op > 0.5f)
  {
    float ret = (1.0f - op) * 2.0f;
    return 1.0f - (ret * ret) * 0.5f;
  }
  float ret = op * 2.0f;
  return ret * ret * 0.5f;
}

/// Linear step function.
///
/// \param edge0 First edge.
/// \param edge1 Second edge.
/// \param value Value.
/// \return Linearly stepped value between edges, otherwise 0 before edge0 and 1 after edge1.
float linear_step(float edge0, float edge1, float value)
{
  if(value <= edge0)
  {
    return 0.0f;
  }
  else if(value >= edge1)
  {
    return 1.0f;
  }
  return (value - edge0) / (edge1 - edge0);
}

/// Linear step down function.
///
/// \param edge0 First edge.
/// \param edge1 Second edge.
/// \param value Value.
/// \return Linearly stepped value between edges, otherwise 1 before edge0 and 0 after edge1.
float linear_step_down(float edge0, float edge1, float value)
{
  if(value <= edge0)
  {
    return 1.0f;
  }
  else if(value >= edge1)
  {
    return 0.0f;
  }
  return (edge1 - value) / (edge1 - edge0);
}

/// Mix two floats.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Ratio.
/// \return Mixing value.
static float mix(float lhs, float rhs, float ratio)
{
  return lhs + (rhs - lhs) * ratio;
}

/// Mix two unsigned integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing value.
static uint8_t mix(uint8_t lhs, uint8_t rhs, float ratio)
{
  float c1 = static_cast<float>(lhs);
  float c2 = static_cast<float>(rhs);
  float ret = std::min(std::max((c2 - c1) * ratio + c1, 0.0f), 255.0f);
  return static_cast<uint8_t>(ret + 0.5f);
}

/// Modulate two unsigned integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Modulated value.
static uint8_t modulate(uint8_t lhs, uint8_t rhs)
{
  float ret = (static_cast<float>(lhs) / 255.0f) * (static_cast<float>(rhs) / 255.0f);
  return static_cast<uint8_t>((ret * 255.0f) + 0.5f);
}

/// "Verbatim GL" namespace.
namespace vgl
{
  /// Common enum for modes.
  enum OperationMode
  {
    /// Disable mode.
    DISABLED,

    /// Premultiplied alpha.
    PREMULTIPLIED,

    /// Alpha.
    ALPHA,

    /// Carmack reverse.
    CARMACK,
  };

  /// \cond
  static const GLint MAX_ATTRIB_ARRAYS = 6;
  static bool g_attrib_arrays_enabled[MAX_ATTRIB_ARRAYS] =
  {
    false,
    false,
    false,
    false,
    false,
    false,
  };
  /// \endcond

  /// \cond
  static bool g_blend_enabled = false;
  static OperationMode g_blend_mode = DISABLED;
  static uint32_t g_color_clear = 0x00000000;
  static bool g_color_write = true;
  static bool g_cull_enabled = false;
  static GLenum g_cull_face = GL_BACK;
  static GLenum g_depth_func = GL_LESS;
  static bool g_depth_test = false;
  static bool g_depth_write = false;
  static bool g_polygon_offset = false;
  static int g_polygon_offset_units = 0;
  static bool g_stencil_func = GL_FALSE;
  static OperationMode g_stencil_operation = DISABLED;
  static bool g_stencil_test = false;
  /// \endcond

#if defined(USE_LD)
  /// \cond
  static unsigned g_data_size_edge = 0;
  static unsigned g_data_size_index = 0;
  static unsigned g_data_size_texture = 0;
  static unsigned g_data_size_vertex = 0;
  /// \endcond
#endif
  
  /// Enable one vertex attribute.
  ///
  /// \param name Name to enable.
  static void attrib_array_enable(GLint name)
  {
#if defined(USE_LD)
    if((MAX_ATTRIB_ARRAYS <= name) || (0 > name))
    {
      std::ostringstream sstr;
      sstr << "enabling attribute index " << name << " (" << MAX_ATTRIB_ARRAYS << " supported)";
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
#endif
    if(!g_attrib_arrays_enabled[name])
    {
      dnload_glEnableVertexAttribArray(name);
    }
  }

  /// Set blending mode.
  ///
  /// \param op New blending mode.
  static void blend_mode(OperationMode op)
  {
    if(DISABLED == op)
    {
      if(g_blend_enabled)
      {
        dnload_glDisable(GL_BLEND);
        g_blend_enabled = false;
      }
    }
    else
    {
      if(!g_blend_enabled)
      {
        dnload_glEnable(GL_BLEND);
        g_blend_enabled = true;
      }

      if(g_blend_mode != op)
      {
        if(op == PREMULTIPLIED)
        {
          dnload_glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }
        else // Default is alpha.
        {
          dnload_glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }
        g_blend_mode = op;
      }
    }
  }

  /// Clear current framebuffer.
  ///
  /// \param op Bit mask of buffers to clear.
  static void clear_buffers(GLbitfield op)
  {
#if defined(RENDER_CLEAR_ALL_BUFFERS)
    dnload_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    (void)op;
#else
    dnload_glClear(op);
#endif
  }

  /// Set color clearing mode.
  ///
  /// \param op New color clearing mode.
  static void color_clear(uint32_t op)
  {
    if(g_color_clear != op)
    {
      static const float CLEAR_MUL = 1.0f / 255.0f;
      float rc = static_cast<float>(op >> 24) * CLEAR_MUL;
      float gc = static_cast<float>((op >> 16) & 0xFF) * CLEAR_MUL;
      float bc = static_cast<float>((op >> 8) & 0xFF) * CLEAR_MUL;
      float ac = static_cast<float>(op & 0xFF) * CLEAR_MUL;

      dnload_glClearColor(rc, gc, bc, ac);
      g_color_clear = op;
    }
  }

  /// Set color writing mode.
  ///
  /// \param op True to enable, false to disable.
  static void color_write(bool op)
  {
    if(op)
    {
      if(!g_color_write)
      {
        dnload_glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        g_color_write = true;
      }
    }
    else
    {
      if(g_color_write)
      {
        dnload_glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        g_color_write = false;
      }
    }
  }

  /// Set culling mode.
  ///
  /// \param op Mode, GL_FALSE to disable.
  static void cull_face(GLenum op)
  {
    if(GL_FALSE == op)
    {
      if(g_cull_enabled)
      {
        dnload_glDisable(GL_CULL_FACE);
        g_cull_enabled = false;
      }
    }
    else
    {
      if(!g_cull_enabled)
      {
        dnload_glEnable(GL_CULL_FACE);
        g_cull_enabled = true;
      }
      if(g_cull_face != op)
      {
        dnload_glCullFace(op);
        g_cull_face = op;
      }
    }
  }

  /// Set depth testing mode.
  ///
  /// \param op Depth testing mode, GL_FALSE to disable.
  static void depth_test(GLenum op)
  {
    if(GL_FALSE == op)
    {
      if(g_depth_test)
      {
        dnload_glDisable(GL_DEPTH_TEST);
        g_depth_test = false;
      }
    }
    else
    {
      if(!g_depth_test)
      {
        dnload_glEnable(GL_DEPTH_TEST);
        g_depth_test = true;
      }
      if(g_depth_func != op)
      {
        dnload_glDepthFunc(op);
        g_depth_func = op;
      }
    }
  }

  /// Set depth writing mode.
  ///
  /// \param op True to enable, false to disable.
  static void depth_write(bool op)
  {
    if(op)
    {
      if(!g_depth_write)
      {
        dnload_glDepthMask(GL_TRUE);
        g_depth_write = true;
      }
    }
    else
    {
      if(g_depth_write)
      {
        dnload_glDepthMask(GL_FALSE);
        g_depth_write = false;
      }
    }
  }

  /// Disable extra vertex attribute arrays.
  ///
  /// \param op Index of first array to disable.
  static void disable_excess_attrib_arrays(unsigned op)
  {
    for(unsigned ii = op; (ii < MAX_ATTRIB_ARRAYS); ++ii)
    {
      if(g_attrib_arrays_enabled[ii])
      {
        dnload_glDisableVertexAttribArray(ii);
        g_attrib_arrays_enabled[ii] = false;
      }
    }
  }

  /// Set polygon offset.
  ///
  /// \param op Units to offset.
  static void polygon_offset(int op)
  {
    if(!op)
    {
      if(g_polygon_offset)
      {
        dnload_glDisable(GL_POLYGON_OFFSET_FILL);
        g_polygon_offset = false;
      }
    }
    else
    {
      if(!g_polygon_offset)
      {
        dnload_glEnable(GL_POLYGON_OFFSET_FILL);
        g_polygon_offset = true;
      }
      if(g_polygon_offset_units != op)
      {
        dnload_glPolygonOffset(1.0f, static_cast<float>(op));
        g_polygon_offset_units = op;
      }
    }
  }

  /// Set stencil mode.
  ///
  /// \param op New mode.
  static void stencil_mode(GLenum op)
  {
    if(GL_FALSE == op)
    {
      if(g_stencil_test)
      {
        dnload_glDisable(GL_STENCIL_TEST);
        g_stencil_test = false;
      }
    }
    else
    {
      if(!g_stencil_test)
      {
        dnload_glEnable(GL_STENCIL_TEST);
        g_stencil_test = true;
      }
      if(g_stencil_func != op)
      {
        dnload_glStencilFunc(op, 0, 0xFFFFFFFFU);
        g_stencil_func = op;
      }
    }
  }

  /// Set stencil operation.
  ///
  /// \param op New mode.
  static void stencil_operation(OperationMode op)
  {
    if(g_stencil_operation != op)
    {
      if(op == CARMACK)
      {
        dnload_glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        dnload_glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
      }
      else // Default is nothing.
      {
        dnload_glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
      }
      g_stencil_operation = op;
    }
  }
  
#if defined(USE_LD)
  /// Get an error string corresponding to a GL error.
  ///
  /// \param op GL error.
  /// \return Error string.
  const char* error_str(GLenum err)
  {
    switch(err)
    {
      case GL_NO_ERROR:
        return "GL_NO_ERROR";

      case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

      case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

      case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

      case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

      case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

      default:
        break;
    }

    return "unknown error";
  }

  /// Perform error check.
  ///
  /// Throws an error on failure.
  static void error_check()
  {
    GLenum err = glGetError();
    if(GL_NO_ERROR != err)
    {
      std::ostringstream sstr;
      sstr << "GL error " << err << ": " << error_str(err);
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }

  /// Accessor.
  ///
  /// \return Total edge data size used.
  static unsigned get_data_size_edge()
  {
    return g_data_size_edge;
  }
  /// Increment data size.
  ///
  /// \param op Edge data size used.
  static unsigned increment_data_size_edge(unsigned op)
  {
    return g_data_size_edge += op;
  }

  /// Accessor.
  ///
  /// \return Total index data size used.
  static unsigned get_data_size_index()
  {
    return g_data_size_index;
  }
  /// Increment data size.
  ///
  /// \param op Index data size used.
  static unsigned increment_data_size_index(unsigned op)
  {
    return g_data_size_index += op;
  }

  /// Accessor.
  ///
  /// \return Total index data size used.
  static unsigned get_data_size_texture()
  {
    return g_data_size_texture;
  }
  /// Increment data size.
  ///
  /// \param op Texture data size used.
  static unsigned increment_data_size_texture(unsigned op)
  {
    return g_data_size_texture += op;
  }

  /// Accessor.
  ///
  /// \return Total vertex data size used.
  static unsigned get_data_size_vertex()
  {
    return g_data_size_vertex;
  }
  /// Increment data size.
  ///
  /// \param op Edge data size used.
  static unsigned increment_data_size_vertex(unsigned op)
  {
    return g_data_size_vertex += op;
  }
#endif
}

#endif
