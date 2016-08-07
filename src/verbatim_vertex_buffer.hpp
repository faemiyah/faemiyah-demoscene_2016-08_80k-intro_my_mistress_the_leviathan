#ifndef VERBATIM_VERTEX_BUFFER_HPP
#define VERBATIM_VERTEX_BUFFER_HPP

#include "verbatim_buffer.hpp"
#include "verbatim_program.hpp"
#include "verbatim_vertex.hpp"

/// Array buffer containing vertex data.
class VertexBuffer : public Buffer
{
  private:
    /// Current array buffer.
    static const VertexBuffer* g_bound_array_buffer;

  public:
    /// Bind this array buffer for rendering and/or modification.
    void bind() const
    {
      if(this == g_bound_array_buffer)
      {
        return;
      }

      dnload_glBindBuffer(GL_ARRAY_BUFFER, getId());

      g_bound_array_buffer = this;
      Program::reset_array_buffer();
    }

    /// Update this vertex buffer into the GPU.
    ///
    /// Empty data will not be updated.
    ///
    /// \param data Data to update.
    void update(const seq<Vertex> &data) const
    {
      if(!data)
      {
        return;
      }

#if 0
      for(const Vertex &vv : data)
      {
        std::cout << vv << std::endl;
      }
#endif

      bind();
      dnload_glBufferData(GL_ARRAY_BUFFER, data.getSizeBytes(), data.getData(), GL_STATIC_DRAW);
#if defined(USE_LD)
      vgl::increment_data_size_vertex(data.getSizeBytes());
#endif
    }

    /// Use this vertex buffer for rendering.
    ///
    /// \param op Program to use.
    void use(const Program &op) const
    {
      this->bind();

      if(!Program::select_array_buffer(this))
      {
        return;
      }

      vgl::disable_excess_attrib_arrays(4);
      op.attribPointer('P', 3, GL_FLOAT, false, sizeof(Vertex),
          static_cast<const uint8_t*>(NULL) + Vertex::POSITION_OFFSET);
      op.attribPointer('T', 4, GL_BYTE, true, sizeof(Vertex),
          static_cast<const uint8_t*>(NULL) + Vertex::TEXCOORD_OFFSET);
      op.attribPointer('N', 3, GL_BYTE, true, sizeof(Vertex),
          static_cast<const uint8_t*>(NULL) + Vertex::NORMAL_OFFSET);
      op.attribPointer('C', 3, GL_UNSIGNED_BYTE, true, sizeof(Vertex),
          static_cast<const uint8_t*>(NULL) + Vertex::COLOR_OFFSET);
      op.attribPointer('Q', 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex),
          static_cast<const uint8_t*>(NULL) + Vertex::WEIGHTS_OFFSET);
      op.attribPointer('R', 4, GL_UNSIGNED_BYTE, false, sizeof(Vertex),
          static_cast<const uint8_t*>(NULL) + Vertex::REFERENCES_OFFSET);
    }

  public:
    /// Unbind vertex buffer.
    static void unbind()
    {
      dnload_glBindBuffer(GL_ARRAY_BUFFER, 0);
      g_bound_array_buffer = NULL;
      Program::reset_array_buffer();
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "VertexBuffer(" << getId() << ')';
    }
#endif
};

const VertexBuffer *VertexBuffer::g_bound_array_buffer = NULL;

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
std::ostream& operator<<(std::ostream &lhs, const VertexBuffer &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
