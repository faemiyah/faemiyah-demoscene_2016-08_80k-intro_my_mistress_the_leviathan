#ifndef VERBATIM_INDEX_BUFFER_HPP
#define VERBATIM_INDEX_BUFFER_HPP

#include "verbatim_buffer.hpp"
#include "verbatim_seq.hpp"

/// Buffer containing index data.
class IndexBuffer : public Buffer
{
  private:
    /// Current element buffer.
    static const IndexBuffer *g_bound_element_buffer;

  public:
    /// Bind this element buffer for rendering and/or modification.
    void bind() const
    {
      if(this == g_bound_element_buffer)
      {
        return;
      }

      dnload_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getId());
      g_bound_element_buffer = this;
    }

    /// Update this element buffer into the GPU.
    ///
    /// Empty data will not be updated.
    ///
    /// \param data Data to update.
    void update(const seq<uint16_t> &data) const
    {
      if(!data)
      {
        return;
      }

#if 0
      for(unsigned ii = 0; (ii < data.size()); ii += 3)
      {
        std::cout << data[ii] << ", " << data[ii + 1] << ", " << data[ii + 2] << std::endl;
      }
#endif

      bind();
      dnload_glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.getSizeBytes(), data.getData(), GL_STATIC_DRAW);
#if defined(USE_LD)
      vgl::increment_data_size_index(data.getSizeBytes());
#endif
    }

  public:
    /// Unbind vertex buffer.
    static void unbind()
    {
      dnload_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      g_bound_element_buffer = NULL;
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "IndexBuffer(" << getId() << ')';
    }
#endif
};

const IndexBuffer *IndexBuffer::g_bound_element_buffer = NULL;

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
std::ostream& operator<<(std::ostream &lhs, const IndexBuffer &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
