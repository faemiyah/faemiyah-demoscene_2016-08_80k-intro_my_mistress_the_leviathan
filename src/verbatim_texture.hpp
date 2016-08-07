#ifndef VERBATIM_TEXTURE_HPP
#define VERBATIM_TEXTURE_HPP

#include "verbatim_image.hpp"

/// Filtering mode
enum Filtering
{
  NEAREST,
  BILINEAR,
  TRILINEAR
};

/// Texture class.
class Texture
{
  private:
    /// Number of texture units managed by this class.
    static unsigned const MAX_TEXTURE_UNITS = 2;

  private:
    /// Currently bound textures. One for each texture unit.
    static Texture const *g_current_texture[MAX_TEXTURE_UNITS];

    /// Currently active texture unit.
    static unsigned g_active_texture_unit;

  private:
    /// Texture width.
    unsigned m_width;

    /// Texture height.
    unsigned m_height;

    /// OpenGL texture name.
    GLuint m_id;

  private:
    /// Deleted copy constructor.
    Texture(const Texture&) = delete;
    /// Deleted assignment.
    Texture& operator=(const Texture&) = delete;

  public:
    /// Constructor.
    Texture() :
      m_width(0),
      m_height(0),
      m_id(0) { }

    /// Destructor.
    ~Texture()
    {
#if defined(USE_LD)
      unbind();

      if(m_id)
      {
        glDeleteTextures(1, &m_id);
      }
#endif
    }

  private:
    /// Update internal operation.
    ///
    /// \param width Width of the texture
    /// \param height Height of the texture
    /// \param channels Number of channels, 0 for depth texture.
    /// \param data Pointter to texture data, must be one byte per color channel per texel.
    /// \param clamp True to clamp texture instead of wrapping (default: false).
    /// \param nearest Use nearest-neightbor filtering (as opposed to linear).
    void update(unsigned width, unsigned height, unsigned channels, void *data, bool clamp = false,
        Filtering filtering = BILINEAR)
    {
#if defined(USE_LD)
      if(0 < m_id)
      {
        std::ostringstream sstr;
        sstr << "updating texture that already has an existing id: " << m_id;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
      unsigned data_size;
#endif

      m_width = width;
      m_height = height;
      dnload_glGenTextures(1, &m_id);

      const Texture* prev_texture = g_current_texture[g_active_texture_unit];
      bind(g_active_texture_unit);
      dnload_glBindTexture(GL_TEXTURE_2D, m_id); 

      // Set format, R8G8B8A8 is default.
      {
        GLenum format = GL_RGBA;
        GLenum type = GL_UNSIGNED_BYTE;
#if defined(USE_LD)
        unsigned type_size = 1;
#endif

        if(3 == channels)
        {
#if defined(RENDER_RGB24_BYTES) && (3 != RENDER_RGB24_BYTES)
#if (2 == RENDER_RGB24_BYTES)
          format = GL_RGB;
          type = GL_UNSIGNED_SHORT_5_6_5;
#if defined(USE_LD)
          type_size = 2;
#endif
#elif (4 == RENDER_RGB24_BYTES)
#if defined(USE_LD)
          type_size = 4;
#endif
#else
#error "unknown value for RENDER_RGB24_BYTES (update)"
#endif
#else
          format = GL_RGB;
#if defined(USE_LD)
          type_size = 3;
#endif
#endif
        }
        else if(2 == channels)
        {
          format = GL_LUMINANCE_ALPHA;
        }
        else if(1 == channels)
        {
          format = GL_LUMINANCE;
        }
        else if(0 >= channels)
        {
          format = GL_DEPTH_COMPONENT;
          type = GL_UNSIGNED_SHORT;
#if defined(USE_LD)
          type_size = 2;
#endif
        }

        dnload_glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), static_cast<GLsizei>(width),
            static_cast<GLsizei>(height), 0, format, type, data);
#if defined(USE_LD)
        data_size = width * height * type_size;
#endif
      }

      GLint wrap_mode = clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
      dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
      dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);

      // 'nearest' -filtering forced.
      if(NEAREST == filtering)
      {
        dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      }
      // Normal textures use trilinear filtering.
      else
      {
        if(data)
        {
          if(TRILINEAR == filtering)
          {
            dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          }
          else
          {
            dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
          }
          dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          dnload_glGenerateMipmap(GL_TEXTURE_2D);
#if defined(USE_LD)
          data_size = (data_size * 4) / 3;
#endif
        }
        // Bilinear is default.
        else
        {
          dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          dnload_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
      }

#if 0
      // For depth textures, we're not comparing depth on access.
      if(GL_DEPTH_COMPONENT == format)
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
      }
#endif

      if(prev_texture)
      {
        prev_texture->bind(g_active_texture_unit);
      }
#if defined(USE_LD)
      else
      {
        unbind();
      }
      vgl::increment_data_size_texture(data_size);
#endif
    }

#if defined(USE_LD)
    /// Unbind texture from whichever texture unit it's bound to.
    void unbind() const
    {
      unsigned original_texture_unit = g_active_texture_unit;
  
      for(unsigned ii = 0; (ii < MAX_TEXTURE_UNITS); ++ii)
      {
        if(g_current_texture[ii] && (g_current_texture[ii]->m_id == m_id))
        {
          select_texture_unit(ii);
          glBindTexture(GL_TEXTURE_2D, 0);
          g_current_texture[ii] = NULL;
        }
      }

      select_texture_unit(original_texture_unit);
    }
#endif

  public:
    /// Bind texture to given texture unit 
    ///
    /// \param op Unit to bind to.
    void bind(unsigned op) const
    {
      if(g_current_texture[op] != this)
      {
        select_texture_unit(op);
        dnload_glBindTexture(GL_TEXTURE_2D, m_id);
        g_current_texture[op] = this;
      }
    }

    /// Accessor.
    ///
    /// \return Texture id.
    unsigned getId() const
    {
      return m_id;
    }

    /// Accessor.
    ///
    /// \return Texture width.
    unsigned getWidth() const
    {
      return m_width;
    }

    /// Accessor.
    ///
    /// \return Texture height.
    unsigned getHeight() const
    {
      return m_height;
    }

    /// Update texture contents with an image.
    ///
    /// \param img Image to update with.
    /// \param filtering Filtering mode.
    /// \param clamp True to clamp texture instead of wrapping (default: false).
    void update(Image &image, Filtering filtering = BILINEAR, bool clamp = false)
    {
      update(image.getWidth(), image.getHeight(), image.getChannelCount(), image.getExportData(), clamp,
          filtering);
    }

    /// Update contents with nothing.
    ///
    /// Usable for framebuffer textures. By default, 4 channels since RGB framebuffer is an extension.
    ///
    /// \param width Texture width.
    /// \param height Texture height.
    /// \param channels Number of channels, use 0 for depth texture.
    /// \param filtering Filtering mode.
    void update(unsigned width, unsigned height, unsigned channels = 4, Filtering filtering = BILINEAR)
    {
      update(width, height, channels, NULL, true, filtering);
    }

  private:
    /// Convert number of channels to a format.
    ///
    /// \param channels Number of channels.
    /// \return Corresponding GL texture format.
    static GLenum channels_to_gl_format(unsigned channels)
    {
      if(4 == channels)
      {
        return GL_RGBA;
      }
      if(3 == channels)
      {
        return GL_RGB;
      }
      if(2 == channels)
      {
        return GL_LUMINANCE_ALPHA;
      }
      if(1 == channels)
      {
        return GL_LUMINANCE;
      }
#if defined(USE_LD)
      if(0 != channels)
      {
        std::ostringstream sstr;
        sstr << "invalid number of channels for a texture: " << channels;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return GL_DEPTH_COMPONENT;
    }

    /// Activate a texture unit.
    ///
    /// \param op Texture unit to activate.
    static void select_texture_unit(unsigned op)
    {
      if(g_active_texture_unit != op)
      {
        dnload_glActiveTexture(GL_TEXTURE0 + op);
        g_active_texture_unit = op;
      }
    }    
};

const Texture *Texture::g_current_texture[Texture::MAX_TEXTURE_UNITS] = { NULL, NULL };
unsigned Texture::g_active_texture_unit = 0;

#endif
