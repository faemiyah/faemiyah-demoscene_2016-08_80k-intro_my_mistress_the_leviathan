#ifndef VERBATIM_COLOR_HPP
#define VERBATIM_COLOR_HPP

#include "verbatim_gl.hpp"
#include "verbatim_vec3.hpp"

/// 4-component unsigned integer vector class.
///
/// Practically, represents color.
class uvec4
{
  private:
    /// Data.
    uint8_t m_data[4];

  public:
    /// Empty constructor.
    uvec4() { }

    /// Constructor.
    ///
    /// \param cr Red component.
    /// \param cg Green component.
    /// \param cb Blue component.
    /// \param ca Alpha component.
    uvec4(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca)
    {
      m_data[0] = cr;
      m_data[1] = cg;
      m_data[2] = cb;
      m_data[3] = ca;
    }

    /// Constructor.
    ///
    /// \param op Input vector.
    uvec4(const vec3 &op)
    {
      m_data[0] = static_cast<uint8_t>(op[0] * 255.0f + 0.5f);
      m_data[1] = static_cast<uint8_t>(op[1] * 255.0f + 0.5f);
      m_data[2] = static_cast<uint8_t>(op[2] * 255.0f + 0.5f);
      m_data[3] = 255;
    }

  public:
    /// Accessor.
    ///
    /// \return Data.
    const uint8_t* getData() const
    {
      return m_data;
    }

    /// Access operator.
    ///
    /// \return Value.
    uint8_t& operator[](unsigned idx)
    {
      return m_data[idx];
    }

    /// Const access operator.
    ///
    /// \return Value.
    const uint8_t& operator[](unsigned idx) const
    {
      return m_data[idx];
    }

  public:
    /// Mix two vectors.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    /// \return Result color.
    friend uvec4 mix(const uvec4 &lhs, const uvec4 &rhs, float ratio)
    {
      return uvec4(mix(lhs[0], rhs[0], ratio),
          mix(lhs[1], rhs[1], ratio),
          mix(lhs[2], rhs[2], ratio),
          mix(lhs[3], rhs[3], ratio));
    }

    /// Modulate two vectors.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result color.
    friend uvec4 modulate(const uvec4 &lhs, const uvec4 &rhs)
    {
      return uvec4(modulate(lhs[0], rhs[0]),
          modulate(lhs[1], rhs[1]),
          modulate(lhs[2], rhs[2]),
          modulate(lhs[3], rhs[3]));
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[ " << static_cast<unsigned>(m_data[0]) << " ; " << static_cast<unsigned>(m_data[1]) <<
        " ; " << static_cast<unsigned>(m_data[2]) << " ; " << static_cast<unsigned>(m_data[3]) << " ]";
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const uvec4 &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
