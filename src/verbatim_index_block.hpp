#ifndef VERBATIM_INDEX_BLOCK
#define VERBATIM_INDEX_BLOCK

#include "verbatim_index_run.hpp"

/// One block of indices.
class IndexBlock
{
  private:
    /// Referred geometry buffer.
    const GeometryBuffer &m_buffer;

    /// Face indices.
    IndexRun m_faces;

    /// Edge indices.
    IndexRun m_edges;

    /// Cap indices.
    IndexRun m_caps;

  public:
    /// Constructor.
    ///
    /// \param buffer Buffer reference.
    /// \param vertex_base Vertex base offset.
    /// \param faces Index run for geometry.
    /// \param edges Index run for shadow extrusion.
    /// \param caps Index run for shadow caps.
    IndexBlock(const GeometryBuffer &buffer, const IndexRun &faces, const IndexRun &edges,
        const IndexRun &caps) :
      m_buffer(buffer),
      m_faces(faces),
      m_edges(edges),
      m_caps(caps) { }

  public:
    /// Draw indexed geometry.
    ///
    /// \param op Program to use for drawing.
    void drawGeometry(const Program &op, bool full = false) const
    {
      m_buffer.useGeometry(op);

      m_faces.drawTriangles(full);
    }

    /// Draw shadow volume extruded data.
    ///
    /// \param op Program to use for drawing.
    void drawShadowEdges(const Program &op, bool full = false) const
    {
      m_buffer.useShadow(op);

      m_edges.drawTriangles(full);
    }

    /// Draw shadow volume cap data.
    ///
    /// \param op Program to use for drawing.
    void drawShadowCaps(const Program &op, bool full = false) const
    {
      m_buffer.useShadow(op);

      m_caps.drawTriangles(full);
    }

    /// Accessor.
    ///
    /// \param idx Index to access.
    /// \return Given index.
    unsigned getIndex(unsigned idx) const
    {
      return m_buffer.getIndex(m_faces.getBase() + idx);
    }
    /// Accessor.
    ///
    /// \return Index base.
    unsigned getIndexBase() const
    {
      return m_faces.getBase();
    }
    /// Accessor.
    ///
    /// \return Index count.
    unsigned getIndexCount() const
    {
      return m_faces.getCount();
    }

    /// Accessor.
    ///
    /// \param idx Index to access.
    /// \return Vertex at index.
    const Vertex& getVertex(unsigned idx) const
    {
      return m_buffer.getVertex(idx);
    }
};

#endif
