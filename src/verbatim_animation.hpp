#ifndef VERBATIM_ANIMATION_HPP
#define VERBATIM_ANIMATION_HPP

#include "verbatim_animation_frame.hpp"

/// Animation path.
class Animation
{
  private:
    /// Frame data.
    seq<AnimationFrameUptr> m_frames;

    /// Is this animation hierarchical?
    bool m_hierarchical;

  public:
    /// Empty constructor.
    Animation(bool hierarchical = false) :
      m_hierarchical(hierarchical) { }

    /// Constructor.
    ///
    /// \param data Animation data.
    /// \param bone_amount Amount of bone elements.
    /// \param animation_amount Amount of animation elements.
    /// \param scale Model scale.
    /// \param hierarchical Is the animation hierarchical?
    Animation(const int16_t *data, unsigned bone_amount, unsigned animation_amount, float scale = 1.0f,
        bool hierarchical = false) :
      m_hierarchical(hierarchical)
    {
      initFromData(data, bone_amount, animation_amount, scale);
    }

  public:
    /// Accessor.
    ///
    /// \param idx Index.
    AnimationFrame& getFrame(unsigned idx)
    {
      return *(m_frames[idx]);
    }
    /// Const accessor.
    ///
    /// \param idx Index.
    const AnimationFrame& getFrame(unsigned idx) const
    {
      return *(m_frames[idx]);
    }

    /// Accessor.
    ///
    /// \return Frame count.
    unsigned getFrameCount() const
    {
      return m_frames.size();
    }

    /// Constructor.
    ///
    /// \param data Animation data.
    /// \param bone_amount Amount of bone elements.
    /// \param animation_amount Amount of animation elements.
    void initFromData(const int16_t *data, unsigned bone_amount, unsigned animation_amount, float scale)
    {
      unsigned frame_amount = bone_amount / 3 * 7;

#if defined(USE_LD)
      if(animation_amount % (frame_amount + 1) != 0)
      {
        std::ostringstream sstr;
        sstr << "incompatible bone (" << bone_amount << ") and animation (" << animation_amount << ") amounts";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif

      for(unsigned ii = 0; (ii < animation_amount); ii += frame_amount + 1)
      {
        m_frames.emplace_back(new AnimationFrame(data + ii, frame_amount, scale));
      }
    }

    /// Tell if this animation is hierarchical.
    ///
    /// \return True if yes, false if no.
    bool isHierarchical() const
    {
      return m_hierarchical;
    }
    /// Set hierarchical status of this animation.
    ///
    /// \param op New hierarchical status.
    void setHierarchical(bool op)
    {
      m_hierarchical = op;
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      ostr << "Animation: " << m_frames.size() << " frames\n";
      for(const AnimationFrameUptr &vv : m_frames)
      {
        ostr << *vv << std::endl;
      }
      return ostr;
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const Animation &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
