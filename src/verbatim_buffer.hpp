#ifndef VERBATIM_BUFFER_HPP
#define VERBATIM_BUFFER_HPP

#include "verbatim_realloc.hpp"

/// Represents a GL array/element buffer
class Buffer
{
  private:
    /// Buffer id.
    GLuint m_id;

  private:
    /// Deleted copy constructor.
    Buffer(const Buffer&) = delete;
    /// Deleted assignment.
    Buffer& operator=(const Buffer&) = delete;

  protected:
    /// Constructor.
    Buffer()
    {
      dnload_glGenBuffers(1, &m_id);
    }

    /// Destructor.
    ~Buffer()
    {
      dnload_glDeleteBuffers(1, &m_id);
    }

  public:
    /// Accessor.
    ///
    /// \return Identifier.
    GLuint getId() const
    {
      return m_id;
    }
};

#endif
