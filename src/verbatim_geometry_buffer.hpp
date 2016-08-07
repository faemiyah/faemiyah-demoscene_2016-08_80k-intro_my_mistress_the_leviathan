#ifndef VERBATIM_GEOMETRY_BUFFER_HPP
#define VERBATIM_GEOMETRY_BUFFER_HPP

#include "verbatim_edge_buffer.hpp"
#include "verbatim_index_buffer.hpp"

/// Geometry buffer.
///
/// Collection of other buffer data.
class GeometryBuffer
{
  private:
    /// Vertex buffer.
    VertexBuffer m_vertex_buffer;

    /// Index buffer.
    IndexBuffer m_index_buffer;

    /// Edge buffer.
    EdgeBuffer m_edge_buffer;

    /// Edge index buffer.
    IndexBuffer m_edge_index_buffer;

    /// Vertex array.
    seq<Vertex> m_vertices;

    /// Index array.
    seq<uint16_t> m_indices;

    /// Edge array.
    seq<EdgeVertex> m_edge_vertices;

    /// Edge index array.
    seq<uint16_t> m_edge_indices;

  private:
    /// Find a matching edge vertex or if not found, append it.
    ///
    /// \param pos Position
    /// \param nor Normal.
    /// \return Index of vertex found or appended.
    unsigned appendVertex(const vec3 &pos, const ivec4 &nor)
    {
      for(unsigned ii = 0, ee = m_edge_vertices.size(); (ii != ee); ++ii)
      {
        if(m_edge_vertices[ii].matches(pos, nor))
        {
          return ii;
        }
      }

      m_edge_vertices.emplace_back(pos, nor);
#if defined(USE_LD)
      if(is_verbose())
      {
        std::cout << "Warning: adding vertex " << m_edge_vertices.back() << " during capping phase\n";
      }
#endif
      return m_edge_vertices.size() - 1;
    }

  public:
    /// Add cap.
    ///
    /// Essentially adds a triangle that fulfills position and normal requirements.
    ///
    /// By definition, if edges have been added, vertices for the triangle must exist.
    ///
    /// \param v1 First vertex.
    /// \param v2 Second vertex.
    /// \param v3 Third vertex.
    /// \param nor Normal.
    void addCap(const vec3 &v1, const vec3 &v2, const vec3 &v3, const ivec4 &nor)
    {
      unsigned c1 = appendVertex(v1, nor);
      unsigned c2 = appendVertex(v2, nor);
      unsigned c3 = appendVertex(v3, nor);
#if defined(USE_LD)
      if((0xFFFFU < c1) || (0xFFFFU < c2) || (0xFFFFU < c3))
      {
        std::ostringstream sstr;
        sstr << "illegal indices for cap face: " << c1 << ", " << c2 << ", " << c3;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      m_edge_indices.push_back(static_cast<uint16_t>(c1));
      m_edge_indices.push_back(static_cast<uint16_t>(c2));
      m_edge_indices.push_back(static_cast<uint16_t>(c3));
    }

    /// Add edge to this geometry buffer.
    ///
    /// \aram op Edge to add.
    void addEdge(const Edge &op)
    {
#if defined(USE_LD)
      if(is_verbose())
      {
        std::cout << op << std::endl;
      }
#endif
      unsigned edge_base = m_edge_vertices.size();
      m_edge_vertices.push_back(op.getVertex(0));
      m_edge_vertices.push_back(op.getVertex(1));
      m_edge_vertices.push_back(op.getVertex(2));
      m_edge_vertices.push_back(op.getVertex(3));
      m_edge_indices.push_back(static_cast<uint16_t>(edge_base + 0));
      m_edge_indices.push_back(static_cast<uint16_t>(edge_base + 1));
      m_edge_indices.push_back(static_cast<uint16_t>(edge_base + 2));
      m_edge_indices.push_back(static_cast<uint16_t>(edge_base + 2));
      m_edge_indices.push_back(static_cast<uint16_t>(edge_base + 1));
      m_edge_indices.push_back(static_cast<uint16_t>(edge_base + 3));
    }

    /// Add face to this geometry buffer.
    ///
    /// \param op Index to add.
    void addIndex(uint16_t op)
    {
#if defined(USE_LD)
      if(is_verbose())
      {
        std::cout << op << std::endl;
      }
#endif
      m_indices.push_back(op);
    }

    /// Add vertex to this geometry buffer.
    ///
    /// \param op Vertex to add.
    void addVertex(const Vertex &op)
    {
#if defined(USE_LD)
      if(is_verbose())
      {
        std::cout << op << std::endl;
      }
#endif
      m_vertices.push_back(op);
    }

    /// Tell if amount of vertices fits in this geometry buffer.
    ///
    /// \param op Number of offered vertices.
    /// \return True if fits, false if not.
    bool fitsVertices(unsigned op)
    {
      return (0xFFFFU >= (m_vertices.size() + op));
    }

    /// Tell if amount of edge vertices fits in this geometry buffer.
    ///
    /// \param Number of offered edges.
    bool fitsEdgeVertices(unsigned op)
    {
      return (0xFFFFU >= (m_edge_vertices.size() + op));
    }

    /// Accessor.
    ///
    /// \return Edge index count.
    unsigned getEdgeIndexCount() const
    {
      return m_edge_indices.size();
    }

    /// Accessor.
    ///
    /// \param idx Index to access.
    /// \return Index at given index.
    unsigned getIndex(unsigned idx) const
    {
      return static_cast<unsigned>(m_indices[idx]);
    }
    /// Accessor.
    ///
    /// \return Index count.
    unsigned getIndexCount() const
    {
      return m_indices.size();
    }

    /// Accessor.
    ///
    /// \param idx Index to access.
    /// \return Vertex at given index.
    const Vertex& getVertex(unsigned idx) const
    {
      return m_vertices[idx];
    }
    /// Accessor.
    ///
    /// \return Vertex count.
    unsigned getVertexCount() const
    {
      return m_vertices.size();
    }

    /// Update this geometry buffer into the GPU.
    void update()
    {
      m_vertex_buffer.update(m_vertices);
      m_index_buffer.update(m_indices);
      m_edge_buffer.update(m_edge_vertices);
      m_edge_index_buffer.update(m_edge_indices);
    }

    /// Use this geometry buffer for rendering indexed geometry.
    ///
    /// \param op Program to use.
    void useGeometry(const Program &op) const
    {
      m_vertex_buffer.use(op);
      m_index_buffer.bind();
    }

    /// Use this geometry buffer for rendering shadow volume data.
    ///
    /// \param op Program to use.
    void useShadow(const Program &op) const
    {
      m_edge_buffer.use(op);
      m_edge_index_buffer.bind();
    }

    /// Unbind vertex buffer.
    static void unbind()
    {
      VertexBuffer::unbind();
      IndexBuffer::unbind();
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[" << m_vertex_buffer << ", " << m_index_buffer << ", " << m_edge_buffer << "]";
    }

    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream &lhs, const GeometryBuffer &rhs)
    {
      return rhs.put(lhs);
    }
#endif
};

#endif
