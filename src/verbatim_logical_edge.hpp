#ifndef VERBATIM_LOGICAL_EDGE_HPP
#define VERBATIM_LOGICAL_EDGE_HPP

#include "verbatim_element.hpp"

/// Edge class.
class LogicalEdge : public Element
{
  private:
    /// Indices.
    unsigned m_vertex_indices[2];

    /// Left face index.
    unsigned m_face_indices[2];

  public:
    /// Constructor.
    ///
    /// \param v1 Vertex index 1.
    /// \param v2 Vertex index 2.
    /// \param lt Left face.
    /// \param rt Right face.
    /// \param block_id Block id.
    /// \param exists Existence.
    LogicalEdge(unsigned v1, unsigned v2, unsigned lt, unsigned rt, unsigned block_id, bool exists) :
      Element(block_id, exists)
    {
      m_vertex_indices[0] = v1;
      m_vertex_indices[1] = v2;
      m_face_indices[0] = lt;
      m_face_indices[1] = rt;
    }

  public:
    /// Accessor.
    ///
    /// \param idx Index.
    /// \return Face index.
    unsigned getFaceIndex(unsigned idx) const
    {
      return m_face_indices[idx];
    }

    /// Accessor.
    ///
    /// \param idx Index.
    /// \return Vertex index.
    unsigned getVertexIndex(unsigned idx) const
    {
      return m_vertex_indices[idx];
    }
};

#endif
