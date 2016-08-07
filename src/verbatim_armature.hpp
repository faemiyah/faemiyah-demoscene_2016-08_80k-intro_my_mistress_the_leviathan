#ifndef VERBATIM_ARMATURE_HPP
#define VERBATIM_ARMATURE_HPP

#include "verbatim_bone.hpp"

/// Bone.
class Armature
{
  private:
    /// Child bones.
    seq<Bone> m_bones;

  public:
    /// Empty constructor.
    Armature() { }

    /// Constructor into bone data.
    ///
    /// \param bdata Bone data.
    /// \param bones_amount Element count of bone data.
    /// \param hdata Hierarchy data.
    /// \param hierarchy_amount Element count of hierarchy data.
    Armature(const int16_t *bdata, const uint8_t *hdata, unsigned bones_amount, unsigned hierarchy_amount,
        float scale = 1.0f)
    {
      initFromData(bdata, hdata, bones_amount, hierarchy_amount, scale);
    }

  public:
    /// Add a new bone.
    ///
    /// \param idx Index.
    /// \param pos Position.
    /// \return Reference to the bone created.
    Bone& addBone(unsigned idx, const vec3 &pos)
    {
      return m_bones.emplace_back(idx, pos);
    }

    /// Accessor.
    ///
    /// \param idx Bone index.
    /// \return Bone reference.
    Bone& getBone(unsigned idx)
    {
      return m_bones[idx];
    }
    /// Const accessor
    ///
    /// \param idx Bone index.
    /// \return Bone reference.
    const Bone& getBone(unsigned idx) const
    {
      return m_bones[idx];
    }

    /// Accessor.
    ///
    /// \return Bone count.
    unsigned getBoneCount() const
    {
      return m_bones.size();
    }

    /// Hierarchically transform all bones.
    ///
    /// \param matrices Matrix data.
    void hierarchicalTransform(mat3 *matrices) const
    {
      for(Bone &vv : m_bones)
      {
        if(vv.getParent())
        {
          continue;
        }

        vv.recursiveTransform(matrices);
      }
    }

    /// Initialize armature data from data blobs.
    ///
    /// \param bdata Bone data.
    /// \param bones_amount Element count of bone data.
    /// \param hdata Hierarchy data.
    /// \param hierarchy_amount Element count of hierarchy data.
    void initFromData(const int16_t *bdata, const uint8_t *hdata, unsigned bones_amount,
        unsigned hierarchy_amount, float scale)
    {
#if defined(USE_LD)
      if(!m_bones.empty())
      {
        BOOST_THROW_EXCEPTION(std::runtime_error("trying to init non-empty armature from data"));
      }
#endif
      for(unsigned ii = 0, idx = 0; (ii < bones_amount); ii += 3, ++idx)
      {
        vec3 pos(static_cast<float>(bdata[ii + 0]),
            static_cast<float>(bdata[ii + 1]),
            static_cast<float>(bdata[ii + 2]));

        addBone(idx, pos * scale);
      }

      {
        const uint8_t *iter = hdata;

        for(Bone &vv : m_bones)
        {
          for(uint8_t child_count = *iter++; (0 < child_count); --child_count)
          {
            Bone& child = getBone(*iter++);

            vv.addChild(&child);
          }
        }

#if defined(USE_LD)
        if(hdata + hierarchy_amount != iter)
        {
          std::ostringstream sstr;
          sstr << "reference data inconsistency: " << static_cast<unsigned>(iter - hdata) << " vs. " <<
            hierarchy_amount;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#else
        (void)hierarchy_amount;
#endif
      }
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "Armature: " << m_bones.size() << " bones";
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const Armature &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
