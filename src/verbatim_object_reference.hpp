#ifndef VERBATIM_OBJECT_REFERENCE_HPP
#define VERBATIM_OBJECT_REFERENCE_HPP

#include "verbatim_animation_state.hpp"
#include "verbatim_object.hpp"
#include "verbatim_program.hpp"

/// Object reference.
///
/// As object, but contains both the object's own transformation and the full transformation.
class ObjectReference
{
  private:
    /// The object itself verbatim.
    const Object &m_object;

    /// Animation state (may be null).
    const AnimationState *m_animation_state;

    /// World space transformation.
    mat4 m_world;

    /// Full screen space transformation (projection * camera * world).
    mat4 m_screen;

    /// Light space transformation.
    mat4 m_light;

    /// Object world matrix (rotation-only).
    mat3 m_orientation;

    /// Can we render in an optimistic manner?
    bool m_optimistic;

  public:
    /// Constructor.
    ///
    /// \param object Object to render.
    /// \param program Program to use.
    /// \param screen Screen space transformation.
    /// \param light Light space transformation.
    /// \param transform Object's own world space transformation.
    ///
    ObjectReference(const Object &object, const mat4 &screen, const mat4 &light, const mat4 &transform,
        const AnimationState *state = NULL) :
      m_object(object),
      m_animation_state(state),
      m_world(transform),
      m_screen(screen * transform),
      m_light(light * transform),
      m_orientation(transform.getRotation()),
      m_optimistic(true) { }

  public:
    /// Draw this object reference.
    ///
    /// \param op Program to use.
    void drawGeometry(const Program &op) const
    {
      op.uniform('W', m_world);
      op.uniform('M', m_screen);
      op.uniform('S', m_light);
      op.uniform('B', m_orientation);

      if(m_animation_state)
      {
        const mat4 *bone_data = m_animation_state->getBoneData();
        unsigned bone_count = m_animation_state->getBoneCount();

        op.uniform('E', bone_data, bone_count);
      }

      m_object.drawGeometry(op, m_optimistic);
    }

    /// Draw shadow edges.
    ///
    /// \param op Program to use.
    void drawShadowEdges(const Program &op) const
    {
      op.uniform('W', m_world);
      op.uniform('B', m_orientation);

      m_object.drawShadowEdges(op, m_optimistic);
    }

    /// Draw shadow caps.
    ///
    /// \param op Program to use.
    void drawShadowCaps(const Program &op) const
    {
      op.uniform('W', m_world);
      op.uniform('B', m_orientation);

      m_object.drawShadowCaps(op, m_optimistic);
    }

    /// Tell if optimistic rendering is on.
    ///
    /// \return True if yes, false if no.
    bool isOptimistic() const
    {
      return m_optimistic;
    }
    /// Setter.
    ///
    /// \param op New optimistic rendering flag.
    void setOptimistic(bool op)
    {
      m_optimistic = op;
    }
};

#endif
