#ifndef VERBATIM_MESH_HPP
#define VERBATIM_MESH_HPP

#include "verbatim_geometry_buffer.hpp"
#include "verbatim_index_block.hpp"
#include "verbatim_uptr.hpp"

/// Renderable mesh.
class Mesh
{
  private:
    /// Geomery buffer this object has been baked into.
    const GeometryBuffer &m_buffer;

    /// Index collections.
    seq<IndexBlock> m_blocks;

  public:
    /// Constructor.
    ///
    /// \param geometry_buffer Vertex buffer bound to.
    /// \param vertex_count Vertex count.
    Mesh(const GeometryBuffer &geometry_buffer) :
      m_buffer(geometry_buffer) { }

    /// Destructor.
    ~Mesh() { }

  public:
    /// Add index block.
    ///
    /// \param faces Faces.
    /// \param Edges Edges.
    void addIndexBlock(const IndexRun &faces, const IndexRun &edges, const IndexRun &caps)
    {
      m_blocks.emplace_back(m_buffer, faces, edges, caps);
    }

    /// Accessor.
    ///
    /// \param idx Index to access.
    /// \return Index block at given index.
    const IndexBlock& getBlock(unsigned idx) const
    {
      return m_blocks[idx];
    }
    /// Accessor.
    ///
    /// \return Number of index blocks in this mesh.
    unsigned getBlockCount() const
    {
      return m_blocks.size();
    }
};

/// Smart pointer type.
typedef uptr<Mesh> MeshUptr;

#endif
