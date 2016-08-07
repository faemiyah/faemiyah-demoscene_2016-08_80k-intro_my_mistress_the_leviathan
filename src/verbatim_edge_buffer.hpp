#ifndef VERBATIM_EDGE_BUFFER_HPP
#define VERBATIM_EDGE_BUFFER_HPP

#include "verbatim_edge.hpp"
#include "verbatim_vertex_buffer.hpp"

/// Array buffer containing edge data.
class EdgeBuffer : public VertexBuffer
{
  public:
    /// Update this edge buffer into the GPU.
    ///
    /// Empty data will not be updated.
    ///
    /// \param data Data to update.
    void update(const seq<EdgeVertex> &data) const
    {
      if(!data)
      {
        return;
      }

#if 0
      for(const Edge &vv : data)
      {
        std::cout << vv << std::endl;
      }
#endif

      bind();
      dnload_glBufferData(GL_ARRAY_BUFFER, data.getSizeBytes(), data.getData(), GL_STATIC_DRAW);
#if defined(USE_LD)
      vgl::increment_data_size_edge(data.getSizeBytes());
#endif
    }

    /// Bind this buffer.
    ///
    /// \param op Program to use.
    void use(const Program &op) const
    {
      bind();

      if(!Program::select_array_buffer(this))
      {
        return;
      }

      vgl::disable_excess_attrib_arrays(2);
      op.attribPointer('P', 3, GL_FLOAT, false, sizeof(EdgeVertex),
          static_cast<const uint8_t*>(NULL) + EdgeVertex::POSITION_OFFSET);
      op.attribPointer('N', 3, GL_BYTE, true, sizeof(EdgeVertex),
          static_cast<const uint8_t*>(NULL) + EdgeVertex::NORMAL_OFFSET);
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "EdgeBuffer(" << getId() << ')';
    }
#endif
};

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
std::ostream& operator<<(std::ostream &lhs, const EdgeBuffer &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
