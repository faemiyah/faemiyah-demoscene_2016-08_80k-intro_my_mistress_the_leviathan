#ifndef VERBATIM_BONE_STATE_HPP
#define VERBATIM_BONE_STATE_HPP

#include "verbatim_mat4.hpp"

/// One position of bone.
///
/// Can be neutral position.
class BoneState
{
  private:
    /// Position data (neutral).
    vec3 m_pos;

    /// Quaternion data (neutral).
    quat m_rot;

  public:
    /// Empty constructor.
    BoneState() { }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param rot Rotation.
    BoneState(const vec3 &pos, const quat &rot) :
      m_pos(pos),
      m_rot(rot) { }

  public:
    /// Accessor.
    ///
    /// \return Position.
    vec3& getPosition()
    {
      return m_pos;
    }
    /// Const accessor.
    ///
    /// \return Position.
    const vec3& getPosition() const
    {
      return m_pos;
    }

    /// Accessor.
    ///
    /// \return Rotation.
    quat& getRotation()
    {
      return m_rot;
    }
    /// Const accessor.
    ///
    /// \return Rotation.
    const quat& getRotation() const
    {
      return m_rot;
    }
    
#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << m_pos << " ; " << m_rot;
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const BoneState &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
