#ifndef VERBATIM_VERTEX_HPP
#define VERBATIM_VERTEX_HPP

#include "verbatim_ivec4.hpp"
#include "verbatim_vec2.hpp"
#include "verbatim_uvec4.hpp"

/// Vertex class.
class Vertex
{
  public:
    /// Position offset.
    static const ptrdiff_t POSITION_OFFSET = 0;

    /// Color offset.
    static const ptrdiff_t TEXCOORD_OFFSET = sizeof(vec3);

    /// Normal offset.
    static const ptrdiff_t NORMAL_OFFSET = TEXCOORD_OFFSET + sizeof(ivec4);

    /// Normal offset.
    static const ptrdiff_t COLOR_OFFSET = NORMAL_OFFSET + sizeof(ivec4);

    /// Weights offset.
    static const ptrdiff_t WEIGHTS_OFFSET = COLOR_OFFSET + sizeof(uvec4);

    /// References offset.
    static const ptrdiff_t REFERENCES_OFFSET = WEIGHTS_OFFSET + sizeof(uvec4);

  private:
    /// Position data.
    vec3 m_position;

    /// Texcoord data.
    ivec4 m_texcoord;

    /// Normal data.
    ivec4 m_normal;

    /// Face normal data.
    uvec4 m_color;

    /// Weight data.
    uvec4 m_weights;

    /// References data.
    uvec4 m_references;

  public:
    /// Constructor.
    ///
    /// \param pos Position.
    /// \param tex_offset Texcoord offset.
    /// \param tex_direction Texcoord flow direction.
    /// \param nor Normal.
    /// \param fnor Face normal.
    Vertex(const vec3 &pos, const vec2 tex_offset, const vec2 tex_direction, const vec3 &nor,
        const uvec4 &color, const uvec4 &weights, const uvec4 &references) :
      m_position(pos),
      m_texcoord(tex_offset, tex_direction),
      m_normal(nor),
      m_color(color),
      m_weights(weights),
      m_references(references) { }

  public:
    /// Accessor.
    ///
    /// \return Position data.
    const vec3& getPosition() const
    {
      return m_position;
    }
    /// Setter.
    ///
    /// \param op New position.
    void setPosition(const vec3 &op)
    {
      m_position = op;
    }

    /// Accessor.
    ///
    /// \return Texcoord data.
    const ivec4 getTexcoord() const
    {
      return m_texcoord;
    }
    /// Setter.
    ///
    /// \param op New texcoord.
    void setTexcoord(const ivec4 &op)
    {
      m_texcoord = op;
    }

    /// Accessor.
    ///
    /// \return Normal.
    const ivec4& getNormal() const
    {
      return m_normal;
    }
    /// Setter.
    ///
    /// \param nor Normal to set.
    void setNormal(const vec3 &nor)
    {
      m_normal = ivec4(nor);
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

#if defined(USE_LD)
  public:
    /// \param Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[" << getPosition() << ", " << getTexcoord() << ", " << m_normal << ", " << m_weights <<
        ", " << m_references << ']';
    }
#endif
};

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const Vertex &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
