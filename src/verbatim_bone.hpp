#ifndef VERBATIM_BONE_HPP
#define VERBATIM_BONE_HPP

#include "verbatim_mat3.hpp"
#include "verbatim_seq.hpp"

/// Bone.
class Bone
{
  private:
    /// Parent bone.
    const Bone *m_parent;

    /// Child bones.
    seq<Bone*> m_children;

    /// Position.
    vec3 m_position;

    /// Index of this bone.
    unsigned m_index;

  public:
    /// Constructor.
    ///
    /// \param idx Index of this bone.
    /// \param pos Position.
    Bone(unsigned idx, const vec3 &pos) :
      m_parent(NULL),
      m_position(pos),
      m_index(idx) { }

  private:
    /// Set parent.
    ///
    /// \param op New parent.
    void setParent(Bone* op)
    {
#if defined(USE_LD)
      if(m_parent)
      {
        std::ostringstream sstr;
        sstr << "bone " << this << " already has parent: " << m_parent;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      m_parent = op;
    }

  public:
    /// Create a new child bone.
    ///
    /// \param pos Position.
    /// \param rot Rotation.
    /// \return Reference to the child bone created.
    void addChild(Bone* op)
    {
      m_children.push_back(op);
      op->setParent(this);
    }

    /// Accessor.
    ///
    /// \return Matrix.
    unsigned getIndex() const
    {
      return m_index;
    }

    /// Accessor.
    ///
    /// \return Parent bone.
    const Bone* getParent() const
    {
      return m_parent;
    }

    /// Accessor.
    ///
    /// \return Position.
    const vec3& getPosition() const
    {
      return m_position;
    }

    /// Recursively transform child bones starting from this bone.
    ///
    /// \param matrices Matrix data.
    void recursiveTransform(mat3 *matrices)
    {
      if(m_parent)
      {
        matrices[m_index] = matrices[m_parent->getIndex()] * matrices[m_index]; // TODO: correct order?
      }

      for(Bone* vv : m_children)
      {
        vv->recursiveTransform(matrices);
      }
    }
};

#endif
