#ifndef VERBATIM_OBJECT_HPP
#define VERBATIM_OBJECT_HPP

#include "verbatim_bounding_volume.hpp"
#include "verbatim_index_block.hpp"
#include "verbatim_texture.hpp"
#include "verbatim_uptr.hpp"

// Forward declaration.
class ObjectGroup;

/// Renderable object.
///
/// Includes a reference to a compiled mesh and a transformation.
class Object
{
  private:
    /// Compiled mesh being rendered.
    const IndexBlock *m_block;

    /// Texture used for this object, may be NULL.
    ///
    /// If texture exists, it will be bound to the first texture unit upon rendering.
    const Texture *m_texture;

    /// Object group (if applicable).
    ObjectGroup *m_group;

    /// Transformation.
    ///
    /// May or may not be used when sending 
    mat4 m_transform;

    /// Bounding volume.
    BoundingVolume m_bounding_volume;

  public:
    /// Constructor.
    ///
    /// \param mesh Mesh to attach to.
    /// \param transform Transformation to use.
    /// \param tex Texture to use.
    /// \param grp Object group (may be null).
    Object(const IndexBlock &block, const mat4 &transform = mat4::identity(), const Texture *tex = NULL,
        ObjectGroup *grp = NULL) :
      m_block(&block),
      m_texture(tex),
      m_group(grp),
      m_transform(transform),
      m_bounding_volume(block, mat4::unpack_rta(transform, 0)) { }

  public:
    /// Tell if this object conflicts with given bounding volume.
    ///
    /// Checks only on XZ-plane.
    ///
    /// \param op Volume to test against.
    /// \return True if yes, false if no.
    bool conflictsXZ(const BoundingVolume &op) const
    {
      return m_bounding_volume.conflictsXZ(op);
    }

    /// Draw geometry of this object.
    ///
    /// \param prg Program to use.
    /// \param optimistic Can we be render in an optimistic manner?
    void drawGeometry(const Program &prg, bool optimistic) const
    {
      if(m_texture)
      {
        m_texture->bind(0);
      }
      m_block->drawGeometry(prg, !optimistic);
    }

    /// Draw shadow edges of this object.
    ///
    /// \param prg Program to use.
    /// \param optimistic Can we be render in an optimistic manner?
    void drawShadowEdges(const Program &prg, bool optimistic) const
    {
      m_block->drawShadowEdges(prg, !optimistic);
    }

    /// Draw shadow caps of this object.
    ///
    /// \param prg Program to use.
    /// \param optimistic Can we be render in an optimistic manner?
    void drawShadowCaps(const Program &prg, bool optimistic) const
    {
      m_block->drawShadowCaps(prg, !optimistic);
    }

    /// Access bounding volume.
    ///
    /// \return Bounding volume reference.
    const BoundingVolume& getBoundingVolume() const
    {
      return m_bounding_volume;
    }

    /// Get object center.
    ///
    /// \return Bounding volume center.
    const vec3& getCenter() const
    {
      return m_bounding_volume.getCenter();
    }

    /// Get comparison value.
    ///
    /// Used for sorting.
    ///
    /// \return Comparison value from bounding volume.
    float getComparisonValue() const
    {
      return getCenter()[2];
    }

    /// Accessor.
    ///
    /// \return Object group.
    ObjectGroup* getGroup() const
    {
      return m_group;
    }

    /// Accessor.
    ///
    /// \return Texture.
    const Texture* getTexture() const
    {
      return m_texture;
    }

    /// Accessor.
    ///
    /// \return Current transform.
    const mat4& getTransform() const
    {
      return m_transform;
    }
    /// Setter.
    ///
    /// \param op New transform.
    void setTransform(const mat4 &op)
    {
      m_transform = op;
    }
    /// Unpack transform.
    ///
    /// \param frame Frame index.
    /// \return Unpacked SRT matrix.
    mat4 unpackTransform(int frame) const
    {
      return mat4::unpack_rta(m_transform, frame);
    }

  private:
    /// Less than operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return True if this is considered smaller than rhs.
    bool operator<(const Object &rhs) const
    {
      return (getComparisonValue() < rhs.getComparisonValue());
    }

  public:
    /// Object comparator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return negative if lhs < rhs, positive if lhs > rhs, 0 otherwise.
    static int qsort_cmp_object(const void *lhs, const void *rhs)
    {
      const Object *aa = static_cast<const Object*>(lhs);
      const Object *bb = static_cast<const Object*>(rhs);

      if(*aa < *bb)
      {
        return -1;
      }
      if(*bb < *aa)
      {
        return 1;
      }
      return 0;
    }
};

/// Convenience typedef.
typedef uptr<Object> ObjectUptr;

#endif
