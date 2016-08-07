#ifndef VERBATIM_LOGICAL_VERTEX_HPP
#define VERBATIM_LOGICAL_VERTEX_HPP

#include "verbatim_logical_face.hpp"

/// Logical vertex class.
///
/// Only limited number of faces can attach to a vertex.
class LogicalVertex
{
  public:
    /// Maximum number of attached faces.
    static const unsigned MAX_VERTEX_FACES = 128;

    /// Maximum allowed vertex error.
    static const float MAX_POSITION_ERROR;

#if defined(USE_LD)
  private:
    /// Minimum error encountered.
    static float g_min_position_error;

    /// Minimum error encountered.
    static float g_max_merge_error;

    /// Number of vertices merged.
    static unsigned g_merge_count;
#endif

  private:
    /// Position data.
    vec3 m_position;

    /// Normal data.
    vec3 m_normal;

    /// Weight data.
    uvec4 m_weights;

    /// Reference data.
    uvec4 m_references;

    /// Face listing.
    LogicalFace *m_face_references[MAX_VERTEX_FACES];

    /// Face count.
    unsigned m_face_count;

  public:
    /// Empty constructor.
    LogicalVertex() { }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param texcoord Texcoord.
    LogicalVertex(const vec3 &pos) :
      m_position(pos),
      m_weights(0, 0, 0, 0),
      m_references(0, 0, 0, 0),
      m_face_count(0) { }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param texcoord Texcoord.
    LogicalVertex(const vec3 &pos, const uvec4 &weights, const uvec4 &references) :
      m_position(pos),
      m_weights(weights),
      m_references(references),
      m_face_count(0) { }

  public:
    /// Add reference to face.
    ///
    /// \param op Face.
    void addFaceReference(LogicalFace *op)
    {
#if defined(USE_LD)
      if(m_face_count >= MAX_VERTEX_FACES)
      {
        std::cerr << "too many faces connected to a vertex" << std::endl;
        teardown();
        exit(1);
      }
#endif

      m_face_references[m_face_count] = op;
      ++m_face_count;
    }
    /// Add reference to face wrapper.
    ///
    /// \param op Face.
    void addFaceReference(LogicalFace &op)
    {
      addFaceReference(&op);
    }

    /// Clear face references.
    void clearFaceReferences()
    {
      m_face_count = 0;
    }

    /// Collapse face references.
    ///
    /// Calculates normal averaging from face reference normals.
    void collapse()
    {
      const float FACE_CORPOREAL_WEIGHT = 1.0f;
      const float FACE_INCORPOREAL_WEIGHT = 0.05f;
      vec3 normal(0.0f, 0.0f, 0.0f);

      for(unsigned ii = 0; (ii < m_face_count); ++ii)
      {
        LogicalFace* face = m_face_references[ii];

        normal += face->getNormal() * (face->isReal() ? FACE_CORPOREAL_WEIGHT : FACE_INCORPOREAL_WEIGHT);
      }
      m_normal = normalize(normal);
    }

    /// Accessor.
    ///
    /// \return Normal.
    vec3& getNormal()
    {
      return m_normal;
    }
    /// Const accessor.
    ///
    /// \return Normal.
    const vec3& getNormal() const
    {
      return m_normal;
    }

    /// Accessor.
    ///
    /// \return Position.
    vec3& getPosition()
    {
      return m_position;
    }
    /// Const accessor.
    ///
    /// \return Position.
    const vec3& getPosition() const
    {
      return m_position;
    }

    /// Accessor.
    ///
    /// \return References.
    uvec4& getReferences()
    {
      return m_references;
    }
    /// Const accessor.
    ///
    /// \return References.
    const uvec4& getReferences() const
    {
      return m_references;
    }

    /// Accessor.
    ///
    /// \return Weights.
    uvec4& getWeights()
    {
      return m_weights;
    }
    /// Const accessor.
    ///
    /// \return Weights.
    const uvec4& getWeights() const
    {
      return m_weights;
    }

    /// Does this match given another vertex.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return True if yes, false if no.
    bool matches(const LogicalVertex &rhs) const
    {
      vec3 diff = rhs.getPosition() - m_position;
      float error = std::abs(diff[0]) + std::abs(diff[1]) + std::abs(diff[2]);

      if(MAX_POSITION_ERROR >= error)
      {
#if defined(USE_LD)
        if(g_max_merge_error < error)
        {
          g_max_merge_error = error;
        }
        ++g_merge_count;
#endif
        return true;
      }
#if defined(USE_LD)
      if(g_min_position_error > error)
      {
        g_min_position_error = error;
      }
#endif
      return false;
    }

#if defined(USE_LD)
  public:
    /// Static accessor.
    ///
    /// \return Get minimum error that was not merged.
    static float get_max_merge_error()
    {
      return g_max_merge_error;
    }

    /// Static accessor.
    ///
    /// \return Get minimum vertex position error that was not met.
    static float get_min_position_error()
    {
      return g_min_position_error;
    }

    /// Static accessor.
    ///
    /// \return Number of merged vertices.
    static unsigned get_merge_count()
    {
      return g_merge_count;
    }
#endif
};

const float LogicalVertex::MAX_POSITION_ERROR = 0.01f;
#if defined(USE_LD)
float LogicalVertex::g_max_merge_error = 0.0f;
float LogicalVertex::g_min_position_error = FLT_MAX;
unsigned LogicalVertex::g_merge_count = 0;
#endif

#endif
