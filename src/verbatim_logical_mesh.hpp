#ifndef VERBATIM_LOGICAL_MESH_HPP
#define VERBATIM_LOGICAL_MESH_HPP

#include "verbatim_compiled_mesh.hpp"
#include "verbatim_logical_edge.hpp"
#include "verbatim_logical_vertex.hpp"

/// Logical mesh.
///
/// Not an actual renderable mesh, needs compilation etc.
class LogicalMesh
{
#if defined(USE_LD)
  private:
    /// Number of discarded edges.
    static unsigned g_discarded_edges;

    /// Number of discarded faces.
    static unsigned g_discarded_faces;
#endif

  private:
    /// Logical vertex data.
    seq<LogicalVertex> m_vertices;

    /// Logical face data.
    seq<LogicalFace> m_faces;

    /// Logical edge data.
    seq<LogicalEdge> m_edges;

    /// Painting color.
    uvec4 m_paint_color;

    /// Current object id.
    unsigned m_block_id;

    /// Preserve invisible faces?
    bool m_preserve_invisible;

    /// Shadow volumes generated?
    bool m_shadow_generation;

  public:
    /// Constructor.
    ///
    /// \param paint_color Face paint color (default: white).
    LogicalMesh(const vec3 &paint_color = vec3(1.0f, 1.0f, 1.0f)) :
      m_paint_color(paint_color),
      m_block_id(0),
      m_preserve_invisible(false),
      m_shadow_generation(false) { }


  private:
    /// Add face (internal).
    ///
    /// \param op Face to add.
    /// \return Index of face added.
    unsigned addFace(const LogicalFace &op)
    {
      unsigned ret = getLogicalFaceCount();
      m_faces.push_back(op);
      return ret;
    }

    /// Add an edge.
    ///
    /// \param c1 First corner.
    /// \param c2 Second corner.
    /// \param lt Left face index.
    /// \param rt Right face index.
    void addEdge(unsigned c1, unsigned c2, unsigned lt, unsigned rt)
    {
      unsigned block_id = m_faces[lt].getBlockId();
      bool exists = m_faces[lt].isReal() && m_faces[rt].isReal();
#if defined(USE_LD)
      unsigned block_id_rt = m_faces[rt].getBlockId();
      if(block_id != block_id_rt)
      {
        std::ostringstream sstr;
        sstr << "object ids for edge do not match, " << block_id << " vs. " << block_id_rt;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      m_edges.emplace_back(c1, c2, lt, rt, block_id, exists);
    }

    /// Add all edges between two faces.
    ///
    /// \param lt Left face index.
    /// \param rt Right face index.
    void addEdges(unsigned lt, unsigned rt)
    {
      const LogicalFace &ll = m_faces[lt];
      const LogicalFace &rr = m_faces[rt];

      if(!ll.hasShadow() || !rr.hasShadow() || (ll.getBlockId() != rr.getBlockId()))
      {
        return;
      }
      ivec4 lnor(ll.getNormal());
      ivec4 rnor(rr.getNormal());
      if(sqr_error(lnor, rnor) < 3)
      {
#if defined(USE_LD) && 0
        std::cout << "not adding edge between same-facing faces: " << lt << ", " << rt << std::endl;
#endif
        return;
      }

      unsigned c1 = ll.getIndex(0);
      unsigned c2 = ll.getIndex(1);
      unsigned c3 = ll.getIndex(2);

      if(rr.hasEdge(c2, c1))
      {
        addEdge(c1, c2, lt, rt);
      }
      if(rr.hasEdge(c3, c2))
      {
        addEdge(c2, c3, lt, rt);
      }
      if(ll.isQuad())
      {
        unsigned c4 = ll.getIndex(3);

        if(rr.hasEdge(c4, c3))
        {
          addEdge(c3, c4, lt, rt);
        }
        if(rr.hasEdge(c1, c4))
        {
          addEdge(c4, c1, lt, rt);
        }
      }
      else
      {
        if(rr.hasEdge(c1, c3))
        {
          addEdge(c3, c1, lt, rt);
        }
      }
    }

    /// Add vertex (internal).
    ///
    /// \param op Vertex to add.
    /// \return Index of vertex added.
    unsigned addVertex(const LogicalVertex &op)
    {
      unsigned ret = getLogicalVertexCount();
#if defined(USE_LD)
      if(is_verbose())
      {
        std::cout << op.getPosition() << std::endl;
      }
#endif
      m_vertices.push_back(op);
      return ret;
    }

    /// Add vertex shape.
    ///
    /// \param vertices Vertex input.
    /// \param weights Weight input.
    /// \param references Reference input.
    /// \param vertices_amount Vertex element count.
    /// \param weights_amount Weight element count.
    /// \param references_amount Reference element count.
    /// \param scale Scale to multiply with.
    void addVertexShape(const int16_t *vertices, const uint8_t *weights, unsigned vertices_amount,
        unsigned weights_amount, float scale)
    {
#if defined(USE_LD)
      if(weights && ((vertices_amount * 2) != weights_amount))
      {
        std::ostringstream sstr;
        sstr << "vertex amount (" << vertices_amount << ") and weights amount (" << weights_amount <<
          ") do not match";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#else
      (void)weights_amount;
#endif
      for(unsigned ii = 0, jj = 0; (ii < vertices_amount); ii += 3, jj += 6)
      {
        vec3 ver(static_cast<float>(vertices[ii + 0]),
            static_cast<float>(vertices[ii + 1]),
            static_cast<float>(vertices[ii + 2]));

        // Horrible
        if(weights)
        {
          uvec4 wt(weights[jj + 0],
              weights[jj + 1],
              weights[jj + 2],
              0);
          uvec4 rf(weights[jj + 3],
              weights[jj + 4],
              weights[jj + 5],
              0);

          addVertex(ver * scale, wt, rf);
        }
        else
        {
          addVertex(ver * scale);
        }
      }
    }
    /// Add vertex shape (wrapper).
    ///
    /// \param vertices Vertex input.
    /// \param vertices_amount Vertex element count.
    /// \param scale Scale to multiply with.
    void addVertexShape(const int16_t *vertices, unsigned vertices_amount, float scale)
    {
      addVertexShape(vertices, NULL, vertices_amount, 0, scale);
    }

    /// Add face shape.
    ///
    /// \param faces Faces to add.
    /// \param faces_amount Face element count.
    void addFaceShape(const uint16_t* faces, unsigned faces_amount)
    {
      for(unsigned ii = 0; ii < faces_amount; ii += 3)
      {
        addFace(faces[ii + 0], faces[ii + 1], faces[ii + 2]);
      }
    }

    /// Append cap face to a compiled mesh.
    ///
    /// \param msh Mesh to append to.
    /// \param cap Cap to append.
    void appendCap(CompiledMesh &msh, const LogicalFace &face) const
    {
      vec3 c1 = getLogicalVertex(face.getIndex(0)).getPosition();
      vec3 c2 = getLogicalVertex(face.getIndex(1)).getPosition();
      vec3 c3 = getLogicalVertex(face.getIndex(2)).getPosition();

      msh.addCap(c1, c2, c3, face.getNormal(), face);

      if(face.isQuad())
      {
        vec3 c4 = getLogicalVertex(face.getIndex(3)).getPosition();

        msh.addCap(c3, c4, c1, face.getNormal(), face);
      }
    }

    /// Append edge to a compiled mesh.
    ///
    /// \param msh Mesh to append to.
    /// \param edge Edge to append.
    void appendEdge(CompiledMesh &msh, const LogicalEdge &edge) const
    {
      vec3 c1 = getLogicalVertex(edge.getVertexIndex(0)).getPosition();
      vec3 c2 = getLogicalVertex(edge.getVertexIndex(1)).getPosition();
      vec3 n1 = getLogicalFace(edge.getFaceIndex(0)).getNormal();
      vec3 n2 = getLogicalFace(edge.getFaceIndex(1)).getNormal();

      msh.addEdge(c1, c2, n1, n2, edge);
    }

    /// Append face to a compiled mesh.
    ///
    /// Does not append index data, only the vertex data.
    ///
    /// \param msh Mesh to append to.
    /// \param face Face to append.
    void appendFace(CompiledMesh &msh, const LogicalFace &face) const
    {
      static const float FLAT_FACTOR = 1.0f;
      const LogicalVertex &v1 = getLogicalVertex(face.getIndex(0));
      const LogicalVertex &v2 = getLogicalVertex(face.getIndex(1));
      const LogicalVertex &v3 = getLogicalVertex(face.getIndex(2));
      vec3 fn = face.getNormal() * FLAT_FACTOR;
      vec3 n1 = normalize(mix(v1.getNormal(), fn, FLAT_FACTOR));
      vec3 n2 = normalize(mix(v2.getNormal(), fn, FLAT_FACTOR));
      vec3 n3 = normalize(mix(v3.getNormal(), fn, FLAT_FACTOR));

      msh.addVertex(v1.getPosition(), face.getTexcoord(0), face.getTexcoordDirection(), n1, face.getColor(),
          v1.getWeights(), v1.getReferences());
      msh.addVertex(v2.getPosition(), face.getTexcoord(1), face.getTexcoordDirection(), n2, face.getColor(),
          v2.getWeights(), v2.getReferences());
      msh.addVertex(v3.getPosition(), face.getTexcoord(2), face.getTexcoordDirection(), n3, face.getColor(),
          v3.getWeights(), v3.getReferences());

      if(face.isQuad())
      {
        const LogicalVertex &v4 = getLogicalVertex(face.getIndex(3));
        vec3 n4 = normalize(mix(v4.getNormal(), fn, FLAT_FACTOR));

        msh.addVertex(v4.getPosition(), face.getTexcoord(3), face.getTexcoordDirection(), n4, face.getColor(),
            v4.getWeights(), v4.getReferences());
      }
    }

    /// Append face to a compiled mesh.
    ///
    /// Append both vertex and index data.
    ///
    /// \param msh Mesh to append to.
    /// \param face Face to append.
    /// \param index_base Base to add to every index.
    void appendFace(CompiledMesh &msh, const LogicalFace &face, unsigned index_base) const
    {
      appendFace(msh, face);

      msh.addFace(index_base + 0, index_base + 1, index_base + 2, face.getNormal(), face);

      if(face.isQuad())
      {
        msh.addFace(index_base + 2, index_base + 3, index_base + 0, face.getNormal(), face);
      }
    }

    /// Compile a logical mesh.
    ///
    /// Results in a compiled mesh that can be inserted into a vertex buffer.
    ///
    /// \param flatten_seed Random seed for flattening texcoords, 0 to not flatten.
    /// \return Compiled mesh.
    CompiledMeshUptr compile(bool flatten_seed = 0)
    {
      // Perform pre-compilation tasks before doing anything else.
      compilePre();

      for(LogicalVertex &vv : m_vertices)
      {
        vv.clearFaceReferences();
      }
      m_edges.clear();

      for(LogicalFace &vv : m_faces)
      {
        updateFaceNormal(vv);

#if defined(USE_LD)
        if(is_verbose())
        {
          std::cout << vv << std::endl;
        }
#endif
      }

      // Calculate edges when all face normals already calculated.
      for(unsigned ii = 0, ee = m_faces.size(); (ee > ii); ++ii)
      {
        for(unsigned jj = ii + 1; (ee > jj); ++jj)
        {
          addEdges(ii, jj);
        }
      }

      for(LogicalVertex &vv : m_vertices)
      {
        vv.collapse();
      }

      // Compilation tasks done, perform post-compilation tasks.
      compilePost(flatten_seed);

      CompiledMeshUptr ret(new CompiledMesh(m_block_id + 1));

      for(const LogicalFace &vv : m_faces)
      {
        if(!vv.isReal() && !hasPreserveInvisible())
        {
#if defined(USE_LD)
          ++g_discarded_faces;
#endif
          continue;
        }        
        appendFace(*ret, vv, ret->getVertexCount());
        if(vv.hasShadow())
        {
          appendCap(*ret, vv);
        }
      }

      for(const LogicalEdge &vv : m_edges)
      {
        if(!vv.isReal() && !hasPreserveInvisible())
        {
#if defined(USE_LD)
          ++g_discarded_edges;
#endif
          continue;
        }
        appendEdge(*ret, vv);
      }

      ret->optimize();

      return ret;
    }

    /// Prepare mesh for export - pre-compilation.
    ///
    /// \param flatten_seed Random seed for flattening texcoords, 0 to not flatten.
    void compilePost(unsigned flatten_seed)
    {
      if(0 != flatten_seed)
      {
        for(unsigned ii = 0; (m_faces.size() > ii); ++ii)
        {
          LogicalFace &vv = m_faces[ii];
          ivec4 nn(vv.getNormal());

          // SDBM hash wannabe :DDDDDDDD XD
          unsigned sdbm_hash = static_cast<unsigned>(nn[0] + 128);
          sdbm_hash += static_cast<unsigned>(nn[1] + 128) + (sdbm_hash * 65599);
          sdbm_hash += static_cast<unsigned>(nn[2] + 128) + (sdbm_hash * 65599);
          bsd_srand(sdbm_hash + flatten_seed);

          vec2 offset(frand(-1.0f, 1.0f), frand(-1.0f, 1.0f));
          vec2 direction(frand(-1.0f, 1.0f), frand(-1.0f, 1.0f));

          vv.setTexcoord(offset, normalize(direction));
        }
      }
    }

    /// Prepare mesh for export - post-compilation.
    void compilePre()
    {
      // Before merging vertices, check that all faces are appropriate to begin with.
      for(unsigned ii = 0; (m_faces.size() > ii);)
      {
        LogicalFace &vv = m_faces[ii];

        if(!vv.verify())
        {
          if(m_faces.size() > ii + 1)
          {
            m_faces[ii] = m_faces.back();
          }
          m_faces.pop_back();
        }
        else
        {
          ++ii;
        }
      }

      // Merge vertices that have identical location.
      for(unsigned ii = 0; (m_vertices.size() > ii + 1); ++ii)
      {
        LogicalVertex &vv = m_vertices[ii];

        for(unsigned jj = ii + 1; (m_vertices.size() > jj);)
        {
          if(vv.matches(m_vertices[jj]))
          {
#if 0
            std::cout << "merging vertices " << ii << " and " << jj << ": " << vv.getPosition() << ", " <<
              m_vertices[jj].getPosition() << std::endl;
#endif
            replaceVertexIndex(jj, ii);
            if(m_vertices.size() > jj + 1)
            {
              m_vertices[jj] = m_vertices.back();
              replaceVertexIndex(m_vertices.size() - 1, jj);
            }
            m_vertices.pop_back();
          }
          else
          {
            ++jj;
          }
        }
      }
    }

    /// Replace face indices.
    ///
    /// \param src Source face index.
    /// \param dst Destination face index.
    void replaceVertexIndex(unsigned src, unsigned dst)
    {
      for(unsigned ii = 0; (m_faces.size() > ii);)
      {
        LogicalFace &vv = m_faces[ii];

        if(!vv.replaceVertexIndex(src, dst))
        {
          if(m_faces.size() > ii + 1)
          {
            m_faces[ii] = m_faces.back();
          }
          m_faces.pop_back();
        }
        else
        {
          ++ii;
        }
      }
    }

    /// Update normal of one face (owned by this mesh).
    ///
    /// \param op Face input.
    void updateFaceNormal(LogicalFace &op)
    {
      LogicalVertex &v1 = getLogicalVertex(op.getIndex(0));
      LogicalVertex &v2 = getLogicalVertex(op.getIndex(1));
      LogicalVertex &v3 = getLogicalVertex(op.getIndex(2));
      const vec3 &c1 = v1.getPosition();
      const vec3 &c2 = v2.getPosition();
      const vec3 &c3 = v3.getPosition();

#if defined(USE_LD)
      if(is_verbose())
      {
        std::cout << "corner 1: " << c1 << std::endl;
        std::cout << "corner 2: " << c2 << std::endl;
        std::cout << "corner 3: " << c3 << std::endl;
      }
#endif

      if(op.isQuad())
      {
        LogicalVertex &v4 = getLogicalVertex(op.getIndex(3));
        const vec3 &c4 = v4.getPosition();

#if defined(USE_LD) && defined(EXTRA_DEBUG)
        if(is_verbose())
        {
          std::cout << "corner 4: " << c4 << std::endl;
        }
#endif

        vec3 n1 = normalize(cross(c2 - c1, c4 - c1));
        vec3 n2 = normalize(cross(c3 - c2, c1 - c2));
        vec3 n3 = normalize(cross(c4 - c3, c2 - c3));
        vec3 n4 = normalize(cross(c1 - c4, c3 - c4));

#if defined(USE_LD)
        if((0.99f >= dot(n1, n2)) || (0.99f >= dot(n1, n3)) || (0.99f >= dot(n1, n4)) ||
            (0.99f >= dot(n2, n3)) || (0.99f >= dot(n2, n4)) || (0.99f >= dot(n3, n4)))
        {
          std::ostringstream sstr;
          sstr << "non-planar quad: " << c1 << ", " << c2 << ", " << c3 << ", " << c4;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        op.setNormal(normalize(n1 + n2 + n3 + n4));

        v4.addFaceReference(op);
      }
      else
      {
        op.setNormal(normalize(cross(c3 - c2, c1 - c2)));
      }

      v1.addFaceReference(op);
      v2.addFaceReference(op);
      v3.addFaceReference(op);
    }

  public:
    /// Add individual vertex.
    ///
    /// \param pos Position.
    /// \return Index of vertex added.
    unsigned addVertex(const vec3 &pos)
    {
      return addVertex(LogicalVertex(pos));
    }

    /// Add individual vertex.
    ///
    /// \param pos Position.
    /// \param wt Weights.
    /// \param rf References.
    /// \return Index of vertex added.
    unsigned addVertex(const vec3 &pos, const uvec4 &wt, const uvec4 &rf)
    {
      return addVertex(LogicalVertex(pos, wt, rf));
    }

    /// Add vertex wrapper.
    ///
    /// \param px X position.
    /// \param py Y position.
    /// \param pz Z position.
    /// \return Index of vertex added.
    unsigned addVertex(float px, float py, float pz)
    {
      return addVertex(vec3(px, py, pz));
    }

    /// Add individual face.
    ///
    /// \param c1 First corner.
    /// \param c2 Second corner.
    /// \param c3 Third corner.
    /// \param col Color.
    /// \param exists Face existence.
    /// \return Index of face added.
    unsigned addFace(unsigned c1, unsigned c2, unsigned c3, bool exists)
    {
#if defined(USE_LD)
      if(is_verbose())
      {
        std::cout << c1 << " ; " << c2 << " ; " << c3 << std::endl;
      }
#endif
      return addFace(LogicalFace(c1, c2, c3, m_paint_color, m_block_id, exists, m_shadow_generation));
    }
    /// Add individual face wrapper.
    ///
    /// \param c1 First corner.
    /// \param c2 Second corner.
    /// \param c3 Third corner.
    /// \return Index of face added.
    unsigned addFace(unsigned c1, unsigned c2, unsigned c3)
    {
      return addFace(c1, c2, c3, true);
    }


    /// Add individual face.
    ///
    /// \param c1 First corner.
    /// \param c2 Second corner.
    /// \param c3 Third corner.
    /// \param c4 Fourth corner.
    /// \param exists Face existence.
    /// \return Index of face added.
    unsigned addFace(unsigned c1, unsigned c2, unsigned c3, unsigned c4, bool exists)
    {
#if defined(USE_LD)
      if(is_verbose())
      {
        std::cout << c1 << " ; " << c2 << " ; " << c3 << " ; " << c4 << std::endl;
      }
#endif
      return addFace(LogicalFace(c1, c2, c3, c4, m_paint_color, m_block_id, exists, m_shadow_generation));
    }
    /// Add individual face wrapper.
    ///
    /// \param c1 First corner.
    /// \param c2 Second corner.
    /// \param c3 Third corner.
    /// \param c4 Fourth corner.
    /// \return Index of face added.
    unsigned addFace(unsigned c1, unsigned c2, unsigned c3, unsigned c4)
    {
      return addFace(c1, c2, c3, c4, true);
    }

    /// Add a shape.
    ///
    /// \param vertices Array of vertices.
    /// \param faces Array of faces.
    /// \param vertices_amount Self-explanatory.
    /// \param faces_amount Self-explanatory.
    /// \param scale Multiply with this scale.
    void initFromData(const int16_t* vertices, const uint16_t* faces, unsigned vertices_amount,
        unsigned faces_amount, float scale = 1.0f)
    {
      addVertexShape(vertices, vertices_amount, scale);
      addFaceShape(faces, faces_amount);
    }

    /// Add a shape.
    ///
    /// \param vertices Array of vertices.
    /// \param weights Array of weights.
    /// \param faces Array of faces.
    /// \param vertices_amount Self-explanatory.
    /// \param weights_amount Self-explanatory.
    /// \param faces_amount Self-explanatory.
    /// \param scale Multiply with this scale.
    void initFromData(const int16_t* vertices, const uint8_t *weights, const uint16_t* faces,
        unsigned vertices_amount, unsigned weights_amount, unsigned faces_amount, float scale = 1.0f)
    {
      addVertexShape(vertices, weights, vertices_amount, weights_amount, scale);
      addFaceShape(faces, faces_amount);
    }

    /// Advance object id.
    void advanceBlockId()
    {
      ++m_block_id;
    }
    /// Conditionally advance object id.
    ///
    /// \param op Only advance if op is true.
    void advanceBlockId(bool op)
    {
      if(op)
      {
        advanceBlockId();
      }
    }

    /// Find closest vertex to given point.
    ///
    /// \param pos Position to compare against.
    /// \param i0 First allowed index.
    /// \param i1 Last allowed index.
    /// \return Index of closesest vertex.
    unsigned findClosestVertex(const vec3 &pos, unsigned i0, unsigned i1)
    {
      float closest = FLT_MAX;
      unsigned ret = 0;

      for(unsigned ii = i0; (i1 >= ii); ++ii)
      {
        const vec3 diff = m_vertices[ii].getPosition() - pos;
        float len = length(diff);

        if(len < closest)
        {
          closest = len;
          ret = ii;
        }
      }

      return ret;
    }

    /// Find closest vertex to given point.
    ///
    /// Searches among all vertices.
    ///
    /// \param pos Position to compare against.
    /// \return Index of closesest vertex.
    unsigned findClosestVertex(const vec3 &pos)
    {
#if defined(USE_LD)
      if(m_vertices.empty())
      {
        BOOST_THROW_EXCEPTION(std::runtime_error("can't search an empty vertex array"));
      }
#endif

      return findClosestVertex(pos, 0, m_vertices.size() - 1);
    }

    /// Flip an amount of prevously added faces.
    ///
    /// \param op Number of previous faces to flip (default: 1).
    void flipLastFaces(unsigned op = 1)
    {
#if defined(USE_LD)
      if(m_faces.size() < op)
      {
        std::ostringstream sstr;
        sstr << "trying to flip " << op << " faces when only " << m_faces.size() << " available";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      for(unsigned ii = m_faces.size() - 1; (0 < op); --op, --ii)
      {
        m_faces[ii].flip();
      }
    }

    /// Flip Y (or T, or V) component of texcoords of previously added faces.
    ///
    /// \param op Number of previous faces to flip (default: 1).
    void flipLastTexcoordsY(unsigned op = 1)
    {
#if defined(USE_LD)
      if(m_faces.size() < op)
      {
        std::ostringstream sstr;
        sstr << "trying to flip texcoords of " << op << " faces when only " << m_faces.size() << " available";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      for(unsigned ii = m_faces.size() - 1; (0 < op); --op, --ii)
      {
        m_faces[ii].flipTexcoordsY();
      }
    }

    /// Accessor.
    ///
    /// \param idx Index.
    /// \return Face at index.
    LogicalFace& getLogicalFace(unsigned idx)
    {
      return m_faces[idx];
    }
    /// Const accessor.
    ///
    /// \param idx Index.
    /// \return Face at index.
    const LogicalFace& getLogicalFace(unsigned idx) const
    {
      return m_faces[idx];
    }

    /// Accessor.
    ///
    /// \return Face count.
    unsigned getLogicalFaceCount() const
    {
      return static_cast<unsigned>(m_faces.size());
    }

    /// Get last added logical face.
    ///
    /// \return Previously added logical face.
    LogicalFace& getLogicalFaceLast()
    {
      return m_faces.back();
    }

    /// Accessor.
    ///
    /// \param idx Index.
    /// \return Vertex (not logical) at index.
    LogicalVertex& getLogicalVertex(unsigned idx)
    {
      return m_vertices[idx];
    }
    /// Const accessor.
    ///
    /// \param idx Index.
    /// \return Vertex (not logical) at index.
    const LogicalVertex& getLogicalVertex(unsigned idx) const
    {
      return m_vertices[idx];
    }

    /// Accessor.
    ///
    /// \return Logical vertex count.
    unsigned getLogicalVertexCount() const
    {
      return static_cast<unsigned>(m_vertices.size());
    }

    /// Insert into a vertex buffer.
    ///
    /// Compiles a mesh and inserts it into a vertex buffer.
    ///
    /// \param bufferp Target vertex buffer.
    /// \param flatten_seed Random seed used in flattening the texture coordinates.
    MeshUptr insert(GeometryBuffer &buffer, unsigned flatten_seed = 0)
    {
      CompiledMeshUptr msh = compile(flatten_seed);
      MeshUptr ret = msh->insert(buffer);

#if defined(USE_LD)
      if(!ret)
      {
        std::ostringstream sstr;
        sstr << "failed to insert " << *msh << " into " << buffer;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return ret;
    }

    /// Accessor.
    ///
    /// \return Paint color.
    const uvec4& getPaintColor() const
    {
      return m_paint_color;
    }
    /// Setter.
    ///
    /// \param op New paint color.
    void setPaintColor(const uvec4 &op)
    {
      m_paint_color = op;
    }
    /// Setter.
    ///
    /// \param op New paint color.
    void setPaintColor(const vec3 &op)
    {
      setPaintColor(uvec4(op));
    }
    /// Setter.
    ///
    /// \param pr Red component.
    /// \param pg Green component.
    /// \param pb Blue component.
    void setPaintColor(float pr, float pg, float pb)
    {
      setPaintColor(vec3(pr, pg, pb));
    }

    /// Tell if invisible faces are preserved.
    ///
    /// \return True if yes, false if no.
    bool hasPreserveInvisible() const
    {
      return m_preserve_invisible;
    }
    /// Setter.
    ///
    /// \param op True to preserve invisible faces.
    void setPreserveInvisible(bool op)
    {
      m_preserve_invisible = op;
    }

    /// Tell if shadow generation is on.
    ///
    /// \return True if yes, false if no.
    bool hasShadowGeneration() const
    {
      return m_shadow_generation;
    }
    /// Setter.
    ///
    /// \param op True to generate shadows.
    void setShadowGeneration(bool op)
    {
      m_shadow_generation = op;
    }
    
#if defined(USE_LD)
  public:
    /// Accessor.
    ///
    /// \return Number of discarded edges.
    static unsigned get_discarded_edge_count()
    {
      return g_discarded_edges;
    }

    /// Accessor.
    ///
    /// \return Number of discarded faces.
    static unsigned get_discarded_face_count()
    {
      return g_discarded_faces;
    }
#endif

#if defined(USE_LD)
  public:
    /// \brief Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "LogicalMesh(" << this << ')';
    }
#endif
};

#if defined(USE_LD)
unsigned LogicalMesh::g_discarded_edges = 0;
unsigned LogicalMesh::g_discarded_faces = 0;
#endif

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const LogicalMesh &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
