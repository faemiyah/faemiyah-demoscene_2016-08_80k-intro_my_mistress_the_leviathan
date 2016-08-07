#ifndef VERBATIM_LOGICAL_FACE_HPP
#define VERBATIM_LOGICAL_FACE_HPP

#include "verbatim_element.hpp"
#include "verbatim_vec2.hpp"
#include "verbatim_vec3.hpp"

/// Logical face class.
///
/// Up to quads supported.
class LogicalFace : public Element
{
#if defined(USE_LD)
  private:
    static unsigned g_degenerate_count;
#endif

  private:
    /// Number of corners.
    unsigned m_num_corners;

    /// Corner points.
    unsigned m_indices[4];

    /// Corner texcoords.
    vec2 m_texcoord[4];

    /// Direction texcoord.
    vec2 m_texcoord_direction;

    /// Face color.
    uvec4 m_color;

    /// Average normal.
    vec3 m_normal;

    /// Does this face have shadow.
    ///
    /// If false, edges among other faces will not cast a shadow.
    bool m_shadowed;

  public:
    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param c2 Second corner point.
    /// \param c3 Third corner point.
    /// \param color Color.
    /// \param exists Does the face exist?
    /// \param shadow Does this face genrate shadow?
    /// \param block_id Which block (within the mesh) does this face belong to?
    LogicalFace(unsigned c1, unsigned c2, unsigned c3, const uvec4 &color, unsigned block_id, bool exists,
        bool shadow) :
      Element(block_id, exists),
      m_num_corners(3),
      m_texcoord_direction(0.0f, 0.0f),
      m_color(color),
      m_shadowed(shadow)
    {
      m_indices[0] = c1;
      m_indices[1] = c2;
      m_indices[2] = c3;
      m_texcoord[0] = vec2(0.0f, 0.0f);
      m_texcoord[1] = vec2(0.5f, 0.5f);
      m_texcoord[2] = vec2(1.0f, 1.0f);
    }

    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param c2 Second corner point.
    /// \param c3 Third corner point.
    /// \param c4 Fourth corner point.
    /// \param exists Does the face exist?
    /// \param shadow Does this face genrate shadow?
    /// \param block_id Which block (within the mesh) does this face belong to?
    LogicalFace(unsigned c1, unsigned c2, unsigned c3, unsigned c4, const uvec4 &color, unsigned block_id,
        bool exists, bool shadow) :
      Element(block_id, exists),
      m_num_corners(4),
      m_texcoord_direction(0.0f, 0.0f),
      m_color(color),
      m_shadowed(shadow)
    {
      m_indices[0] = c1;
      m_indices[1] = c2;
      m_indices[2] = c3;
      m_indices[3] = c4;
      m_texcoord[0] = vec2(0.0f, 0.0f);
      m_texcoord[1] = vec2(1.0f, 0.0f);
      m_texcoord[2] = vec2(1.0f, 1.0f);
      m_texcoord[3] = vec2(0.0f, 1.0f);
    }

  private:
    /// Remove a corner.
    ///
    /// \param idx Index to remove.
    void removeCorner(unsigned idx)
    {
      --m_num_corners;
      for(unsigned ii = idx; (m_num_corners > ii); ++ii)
      {
        m_indices[ii] = m_indices[ii + 1];
      }
    }

  public:
    /// Changes winding of a face.
    void flip()
    {
      std::swap(m_indices[0], m_indices[2]);
    }
    /// Flip Y (or T, or V) texture coordinates.
    void flipTexcoordsY()
    {
      for(unsigned ii = 0; (4 > ii); ++ii)
      {
        vec2 &vv = m_texcoord[ii];
        vv[1] = 1.0f - vv[1];
      }
    }

    /// Accessor.
    ///
    /// \param idx Corner index.
    /// \return Vertex index for given corner.
    unsigned getIndex(unsigned idx) const
    {
#if defined(USE_LD)
      if(m_num_corners <= idx)
      {
        std::ostringstream sstr;
        sstr << "accessing illegal corner index " << idx << " (max: " << (m_num_corners - 1) << ")";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return m_indices[idx];
    }

    /// Accessor.
    ///
    /// \return Color.
    const uvec4& getColor() const
    {
      return m_color;
    }
    /// Setter.
    ///
    /// \param op New color.
    void setColor(const uvec4 &op)
    {
      m_color = op;
    }

    /// Accessor.
    ///
    /// \return Corner count.
    unsigned getIndexCount() const
    {
      return m_num_corners;
    }

    /// Accessor.
    ///
    /// \return Normal data.
    const vec3& getNormal() const
    {
      return m_normal;
    }
    /// Setter.
    ///
    /// \param op New normal.
    void setNormal(const vec3 &op)
    {
      m_normal = op;
    }

    /// Accessor.
    ///
    /// \param idx Corner index.
    /// \return Texcoords at index.
    const vec2& getTexcoord(unsigned idx) const
    {
      return m_texcoord[idx];
    }
    /// Setter.
    ///
    /// \param idx Corner index.
    /// \param texcoord New texcoords.
    void setTexcoord(unsigned idx, const vec2 &texcoord)
    {
      m_texcoord[idx] = texcoord;
    }
    /// Setter.
    ///
    /// \param texcoord Texture coordinate to set on all vertices.
    void setTexcoord(const vec2 &texcoord, const vec2 &direction = vec2(0.0f, 0.0f))
    {
      m_texcoord[0] = texcoord;
      m_texcoord[1] = texcoord;
      m_texcoord[2] = texcoord;
      m_texcoord_direction = direction;
      // Pointless to check if face is a quad or not, just set on everything.
      m_texcoord[3] = texcoord;
    }

    /// Accessor.
    ///
    /// \return Texcoord direction.
    const vec2& getTexcoordDirection() const
    {
      return m_texcoord_direction;
    }
    /// Setter.
    ///
    /// \param op New texcoord direction.
    void setTexcoordDirection(const vec2 &op)
    {
      m_texcoord_direction = op;
    }

    /// Tell if a given edge exists.
    ///
    /// \param c1 First corner.
    /// \param c2 Second corner.
    /// \return True if yes, false if no.
    unsigned hasEdge(unsigned c1, unsigned c2) const
    {
      if(getIndex(0) == c1)
      {
        return (getIndex(1) == c2);
      }
      if(getIndex(1) == c1)
      {
        return (getIndex(2) == c2);
      }
      if(isQuad())
      {
        if(getIndex(2) == c1)
        {
          return (getIndex(3) == c2);
        }
        if(getIndex(3) == c1)
        {
          return (getIndex(0) == c2);
        }
      }
      else
      {
        if(getIndex(2) == c1)
        {
          return (getIndex(0) == c2);
        }
      }
      return false;
    }

    /// Tell if the face has a shadow.
    ///
    /// \return True if yes, false if no.
    bool hasShadow() const
    {
      return m_shadowed;
    }

    /// Tell if the face is a quad (as opposed to a triangle).
    ///
    /// \return True if yes, false if no.
    bool isQuad() const
    {
      return (4 <= m_num_corners);
    }

    /// Replace vertex index.
    ///
    /// \param src Source index.
    /// \param dst Destination index.
    /// \return True if the face is still valid after the replace, false if not.
    bool replaceVertexIndex(unsigned src, unsigned dst)
    {
      for(unsigned ii = 0; (m_num_corners > ii); ++ii)
      {
        if(src == m_indices[ii])
        {
          m_indices[ii] = dst;
        }
      }

      return verify();
    }

    /// Try to repair any inconsistencies in the face.
    ///
    /// \return True if the face is appropriate.
    bool verify()
    {
      // If node is quad and non-adjacent indices are identical, can be directly dropped.
      if(isQuad())
      {
        if((m_indices[0] == m_indices[2]) || (m_indices[1] == m_indices[3]))
        {
#if defined(USE_LD)
          ++g_degenerate_count;
#endif
          return false;
        }
      }

      // Remove adjacent identical edges.
      for(unsigned ii = 0; (m_num_corners > ii);)
      {
        unsigned jj = ii + 1;
        if(jj >= m_num_corners)
        {
          jj = 0;
        }

        if(m_indices[ii] == m_indices[jj])
        {
          removeCorner(jj);
        }
        else
        {
          ++ii;
        }
      }

      // Check for degeneracy.
      if(3 > m_num_corners)
      {
#if defined(USE_LD)
        ++g_degenerate_count;
#endif
        return false;
      }
      return true;
    }

#if defined(USE_LD)
  public:
    /// Static accessor.
    ///
    /// \return Degenerate face count.
    static unsigned get_degenerate_count()
    {
      return g_degenerate_count;
    }
#endif

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      ostr << "[ " << m_indices[0] << " ; " << m_indices[1] << " ; " << m_indices[2];

      if(3 < m_num_corners)
      {
        ostr << " ; " << m_indices[3];
      }

      return ostr << " ] | " << m_normal;
    }
#endif
};

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const LogicalFace &rhs)
{
  return rhs.put(lhs);
}
#endif

#if defined(USE_LD)
unsigned LogicalFace::g_degenerate_count = 0;
#endif

#endif
