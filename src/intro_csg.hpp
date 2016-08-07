#ifndef INTRO_CSG_HPP
#define INTRO_CSG_HPP

// Pillar stuff
#define PILLAR_DETAIL_H 6
#define PILLAR_RADIUS 0.5f
#define PILLAR_DETAIL 9

static float pillarprofile1_r[PILLAR_DETAIL_H] = { 1.0f, 1.0f, 0.8f, 0.8f, 1.0f, 1.0f };
static float pillarprofile1_h[PILLAR_DETAIL_H] = { 0.0f, 0.075f, 0.125f, 0.875f, 0.925f, 1.0f };

static float pillarprofile2_r[PILLAR_DETAIL_H] = { 1.0f, 1.0f, 0.8f, 0.8f, 0.766f, 0.7f };
static float pillarprofile2_h[PILLAR_DETAIL_H] = { 0.0f, 0.075f, 0.125f, 0.95f, 0.975f, 1.0f };

// general defines
#define DIRECTION_NORMAL 0
#define DIRECTION_INVERSE 1

/// CSG existence flag.
class CsgFlag
{
  private:
    /// Combinatoric existence flag.
    static const uint8_t ALL = 0xFFU;

    /// Existence flag.
    static const uint8_t NORMAL_ORDER = (1 << 6);

    /// Existence flag.
    static const uint8_t CONTIGUOUS = (1 << 7);

  public:
    /// Existence flag.
    static const uint8_t LEFT = (1 << 0);

    /// Existence flag.
    static const uint8_t RIGHT = (1 << 1);

    /// Existence flag.
    static const uint8_t BOTTOM = (1 << 2);

    /// Existence flag.
    static const uint8_t TOP = (1 << 3);

    /// Existence flag.
    static const uint8_t FRONT = (1 << 4);

    /// Existence flag.
    static const uint8_t BACK = (1 << 5);

    /// Inverted existence flag.
    static const uint8_t NO_BACK = static_cast<uint8_t>(~BACK);

    /// Inverted existence flag.
    static const uint8_t NO_BOTTOM = static_cast<uint8_t>(~BOTTOM);

    /// Inverted existence flag.
    static const uint8_t NO_FRONT = static_cast<uint8_t>(~FRONT);

    /// Inverted existence flag.
    static const uint8_t NO_LEFT = static_cast<uint8_t>(~LEFT);

    /// Inverted existence flag.
    static const uint8_t NO_RIGHT = static_cast<uint8_t>(~RIGHT);

    /// Inverted existence flag.
    static const uint8_t NO_TOP = static_cast<uint8_t>(~TOP);

    /// Inverted existence flag.
    static const uint8_t INVERSE = static_cast<uint8_t>(~NORMAL_ORDER);

    /// Inverted existence flag.
    static const uint8_t SPLIT = static_cast<uint8_t>(~CONTIGUOUS);

  private:
    /// Flags
    uint8_t m_flags;

  public:
    /// Empty constructor.
    CsgFlag() :
      m_flags(ALL) { }

    /// Constructor.
    ///
    /// \param op Flags.
    CsgFlag(uint8_t op) :
      m_flags(op) { }

    /// Combinatoric constructor.
    ///
    /// \param op1 Combinatoric flag.
    /// \param op2 Combinatoric flag.
    CsgFlag(uint8_t op1, uint8_t op2) :
      m_flags(op1 & op2) { }

    /// Combinatoric constructor.
    ///
    /// \param op1 Combinatoric flag.
    /// \param op2 Combinatoric flag.
    /// \param op3 Combinatoric flag.
    CsgFlag(uint8_t op1, uint8_t op2, uint8_t op3) :
      m_flags(op1 & op2 & op3) { }

  public:
    /// Disable a flag.
    ///
    /// \param op Flag to disable.
    void disable(uint8_t op)
    {
      m_flags &= static_cast<uint8_t>(~op);
    }
    /// Disable a flag wrapper.
    ///
    /// \param op1 First flag to disable.
    /// \param op2 Second flag to disable.
    void disable(uint8_t op1, uint8_t op2)
    {
      disable(op1 | op2);
    }
    /// Disable a flag wrapper.
    ///
    /// \param op1 First flag to disable.
    /// \param op2 Second flag to disable.
    /// \param op3 Third flag to disable.
    void disable(uint8_t op1, uint8_t op2, uint8_t op3)
    {
      disable(op1 | op2 | op3);
    }
    /// Disable a flag wrapper.
    ///
    /// \param op1 First flag to disable.
    /// \param op2 Second flag to disable.
    /// \param op3 Third flag to disable.
    /// \param op4 Fourth flag to disable.
    void disable(uint8_t op1, uint8_t op2, uint8_t op3, uint8_t op4)
    {
      disable(op1 | op2 | op3 | op4);
    }

    /// Tell if back is set.
    ///
    /// \return True if yes, false if no.
    bool hasBack() const
    {
      return (m_flags & BACK);
    }

    /// Tell if bottom is set.
    ///
    /// \return True if yes, false if no.
    bool hasBottom() const
    {
      return (m_flags & BOTTOM);
    }

    /// Tell if front is set.
    ///
    /// \return True if yes, false if no.
    bool hasFront() const
    {
      return (m_flags & FRONT);
    }

    /// Tell if inverse is set.
    ///
    /// \return True if yes, false if no.
    bool hasInverseOrder() const
    {
      return ((m_flags & NORMAL_ORDER) == 0);
    }

    /// Tell if left is set.
    ///
    /// \return True if yes, false if no.
    bool hasLeft() const
    {
      return (m_flags & LEFT);
    }

    /// Tell if right is set.
    ///
    /// \return True if yes, false if no.
    bool hasRight() const
    {
      return (m_flags & RIGHT);
    }

    /// Tell if split is set.
    ///
    /// \return True if yes, false if no.
    bool hasSplit() const
    {
      return ((m_flags & CONTIGUOUS) == 0);
    }

    /// Tell if top is set.
    ///
    /// \return True if yes, false if no.
    bool hasTop() const
    {
      return (m_flags & TOP);
    }

    /// Invert order.
    void invertOrder()
    {
      m_flags ^= NORMAL_ORDER;
    }

    /// And operator.
    ///
    /// \param rhs Right-hand-side operand.
    CsgFlag operator&(const CsgFlag &rhs)
    {
      CsgFlag ret(m_flags);
      ret.m_flags &= rhs.m_flags;
      return ret;
    }

		/// Or operator.
		///
		/// \param rhs Right-hand-side operand.
		CsgFlag operator|(const CsgFlag &rhs)
		{
			CsgFlag ret(m_flags);
			ret.m_flags |= rhs.m_flags;
			return ret;
		}

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << m_flags;
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
std::ostream& operator<<(std::ostream &lhs, const CsgFlag &rhs)
{
  return rhs.put(lhs);
}
#endif

/// CSG flag declaration.
#define CSG_ALL CsgFlag()

/// CSG flag declaration.
#define CSG_NO_BACK CsgFlag(CsgFlag::NO_BACK)
#define CSG_BACK CsgFlag(CsgFlag::BACK)

/// CSG flag declaration.
#define CSG_NO_BOTTOM CsgFlag(CsgFlag::NO_BOTTOM)
#define CSG_BOTTOM CsgFlag(CsgFlag::BOTTOM)

/// CSG flag declaration.
#define CSG_NO_FRONT CsgFlag(CsgFlag::NO_FRONT)
#define CSG_FRONT CsgFlag(CsgFlag::FRONT)

/// CSG flag declaration.
#define CSG_INVERSE CsgFlag(CsgFlag::INVERSE)

/// CSG flag declaration.
#define CSG_NO_LEFT CsgFlag(CsgFlag::NO_LEFT)
#define CSG_LEFT CsgFlag(CsgFlag::LEFT)

/// CSG flag declaration.
#define CSG_NO_RIGHT CsgFlag(CsgFlag::NO_RIGHT)
#define CSG_RIGHT CsgFlag(CsgFlag::RIGHT)

/// CSG flag declaration.
#define CSG_SHADOW CsgFlag(CsgFlag::SHADOW)

/// CSG flag declaration.
#define CSG_NO_TOP CsgFlag(CsgFlag::NO_TOP)
#define CSG_TOP CsgFlag(CsgFlag::TOP)

/// Default forward direction.
#define CSG_DIR_FW vec3(0.0f, 0.0f, -1.0f)

/// Default up direction.
#define CSG_DIR_UP vec3(0.0f, 1.0f, 0.0f)

/// Add box form.
///
/// \param msh Mesh to append to.
/// \param p1 First corner.
/// \param p2 Second corner.
/// \param color Color.
/// \param up Up vector.
/// \param flags CSG flags.
static void mesh_generate_box(LogicalMesh &msh, const vec3 &p1, const vec3 &p2,
    const vec3 &forward = CSG_DIR_FW, const vec3 &up = CSG_DIR_UP, CsgFlag flags = CSG_ALL)
{
  vec3 diagonal = p2 - p1;
  vec3 unit_forward = normalize(forward);
  vec3 unit_diagonal = normalize(diagonal);
  vec3 unit_up = normalize(up);
  bool split = flags.hasSplit();

  if(dot(unit_diagonal, unit_up) < 0.0f)
  {
    unit_up = -unit_up;
    flags.invertOrder();
  }
  if(dot(unit_diagonal, unit_forward) < 0.0f)
  {
    unit_forward = -unit_forward;
    flags.invertOrder();
  }

  vec3 unit_right = normalize(cross(unit_forward, unit_up));
  unit_up = normalize(cross(unit_right, unit_forward));

  mat3 base(unit_right, unit_up, unit_forward);
  vec3 transformed_p1 = base * p1;
  vec3 transformed_p2 = base * p2;

#if defined(USE_LD)
  if(is_verbose())
  {
    std::cout << "p1: " << transformed_p1 << "\np2: " << transformed_p2 << std::endl;
  }
#endif

  float width = transformed_p2[0] - transformed_p1[0];
  float height = transformed_p2[1] - transformed_p1[1];

#if defined(USE_LD)
  if(is_verbose())
  {
    std::cout << "up" << unit_up << "\nright: " << unit_right << "\nforward: " << unit_forward << std::endl;
    std::cout << "width: " << width << " height: " << height << std::endl;
  }
#endif

  unsigned index_base = msh.getLogicalVertexCount();

  msh.addVertex(p1);
  msh.addVertex(p1 + width * unit_right);
  msh.addVertex(p1 + width * unit_right + height * unit_up);
  msh.addVertex(p1 + height * unit_up);
  msh.addVertex(p2 - width * unit_right - height * unit_up);
  msh.addVertex(p2 - height * unit_up);
  msh.addVertex(p2);
  msh.addVertex(p2 - width * unit_right);

#if 0
  std::cout << msh.getLogicalVertex(index_base + 0).getPosition() << std::endl;
  std::cout << msh.getLogicalVertex(index_base + 1).getPosition() << std::endl;
  std::cout << msh.getLogicalVertex(index_base + 2).getPosition() << std::endl;
  std::cout << msh.getLogicalVertex(index_base + 3).getPosition() << std::endl;
  std::cout << msh.getLogicalVertex(index_base + 4).getPosition() << std::endl;
  std::cout << msh.getLogicalVertex(index_base + 5).getPosition() << std::endl;
  std::cout << msh.getLogicalVertex(index_base + 6).getPosition() << std::endl;
  std::cout << msh.getLogicalVertex(index_base + 7).getPosition() << std::endl;
#endif

  msh.addFace(index_base + 4, index_base + 0, index_base + 3, index_base + 7, flags.hasLeft());
  msh.advanceBlockId(split);
  msh.addFace(index_base + 1, index_base + 5, index_base + 6, index_base + 2, flags.hasRight());
  msh.advanceBlockId(split);

  msh.addFace(index_base + 5, index_base + 1, index_base + 0, index_base + 4, flags.hasBottom());
  msh.advanceBlockId(split);
  msh.addFace(index_base + 2, index_base + 6, index_base + 7, index_base + 3, flags.hasTop());
  msh.advanceBlockId(split);

  msh.addFace(index_base + 0, index_base + 1, index_base + 2, index_base + 3, flags.hasFront());
  msh.advanceBlockId(split);
  msh.addFace(index_base + 5, index_base + 4, index_base + 7, index_base + 6, flags.hasBack());
  msh.advanceBlockId(split);

  if(flags.hasInverseOrder())
  {
    msh.flipLastFaces(6);
  }
}

/// Add box form.
///
/// \param msh Mesh to append to.
/// \param p1 First corner.
/// \param p2 Second corner.
/// \param color Color.
/// \param up Up vector.
/// \param flags CSG flags.
static void mesh_generate_box(LogicalMesh &msh, const vec3 &p1, float width, float height, float depth,
    CsgFlag flags = CSG_ALL)
{
  const vec3 half(width * 0.5f, height * 0.5f, depth * 0.5f);
  mesh_generate_box(msh, p1 - half, p1 + half, CSG_DIR_FW, CSG_DIR_UP, flags);
}

/// Add cylinder to mesh.
//
/// \param msh Mesh to append to.
/// \param p1 First vertex.
/// \param p2 Second vertex.
/// \param radius1 Radius at beginning of 'tube'.
/// \param radius2 Radius at end of 'tube'.
/// \param detail Detail level of 'tube'.
/// \param up Up vector.
/// \param flags CSG flags.
static void mesh_generate_cylinder(LogicalMesh &msh, const vec3 &p1, const vec3 &p2, float radius1,
    float radius2, unsigned detail, const vec3 &up = CSG_DIR_UP, CsgFlag flags = CSG_ALL)
{
  float fdetail = static_cast<float>(detail);
  vec3 unit_forward = normalize(p2 - p1);
  vec3 unit_up = normalize(up);

  // Might easily degenerate, prevent it.
  if(std::abs(dot(unit_up, unit_forward)) > 0.999f)
  {
    unit_up = CSG_DIR_FW;
  }

  vec3 unit_right = normalize(cross(unit_forward, unit_up));
  unit_up = normalize(cross(unit_right, unit_forward));

  unsigned index_base = msh.getLogicalVertexCount();

  msh.addVertex(p1);
  msh.addVertex(p2);

  for(unsigned ii = 0; (ii < detail); ++ii)
  {
    unsigned prev = ((ii == 0) ? ((detail - 1) * 2) : ((ii - 1) * 2)) + index_base + 2;
    unsigned next = (ii * 2) + index_base + 2;
    float angle = static_cast<float>(ii) / fdetail * static_cast<float>(M_PI * 2.0) +
      static_cast<float>(M_PI) / fdetail;
    float ca = dnload_cosf(angle);
    float sa = dnload_sinf(angle);

    msh.addVertex(p1 + ca * unit_right * radius1 + sa * unit_up * radius1);
    msh.addVertex(p2 + ca * unit_right * radius2 + sa * unit_up * radius2);

    msh.addFace(next, prev, prev + 1, next + 1, true);
    msh.addFace(index_base + 0, prev, next, flags.hasFront());
    msh.addFace(index_base + 1, next + 1, prev + 1, flags.hasBack());

    if(flags.hasInverseOrder())
    {
      msh.flipLastFaces(3);
    }
  }
}

/// Add wall object.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint.
/// \param p2 Endpoint.
/// \param up Up vector.
/// \param up height1 Height at startpoint.
/// \param up height2 Height at endpoint.
/// \param up thickness1 Wall thickness at startpoint.
/// \param up thickness2 Wall thickness at endpoint.
/// \param existence Which sides exist (default: all).
static void mesh_generate_wall(LogicalMesh &input, const vec3 &p1, const vec3 &p2, const vec3 &up, float height1, float height2, float thickness1, float thickness2, CsgFlag flags = CSG_ALL)
{
  vec3 wall_axis = p2 - p1;
  vec3 wall_normal = normalize(cross(wall_axis, up));
 
  //printf("Points: [%f %f %f], [%f %f %f]\n", p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
  //printf("H1 H2 T1 T2: [%f %f %f %f]\n", height1, height2, thickness1, thickness2);
 
  //printf("Existence flags: %u\n", existence);
 
 
  unsigned index_base = input.getLogicalVertexCount();
 
  if ((height1 == 0.0f) && (height2 == 0.0f))
  {
    return;
  }
 
  input.addVertex(p1 + wall_normal*thickness1 / 2.0f);
  input.addVertex(p1 + wall_normal*thickness1 / 2.0f + up*height1);
  input.addVertex(p2 + wall_normal*thickness2 / 2.0f + up*height2);
  input.addVertex(p2 + wall_normal*thickness2 / 2.0f);
 
  input.addVertex(p1 - wall_normal*thickness1 / 2.0f);
  input.addVertex(p1 - wall_normal*thickness1 / 2.0f + up*height1);
  input.addVertex(p2 - wall_normal*thickness2 / 2.0f + up*height2);
  input.addVertex(p2 - wall_normal*thickness2 / 2.0f);
 
  //printf("Adding left face, existence: %i\n", existence_left);
  input.addFace(index_base + 3, index_base + 2, index_base + 1, index_base, flags.hasLeft());
  //printf("Adding right face, existence: %i\n", existence_right);
  input.addFace(index_base + 4, index_base + 5, index_base + 6, index_base + 7, flags.hasRight());
 
  //printf("Adding back face, existence: %i\n", existence_back);
  input.addFace(index_base, index_base + 1, index_base + 5, index_base + 4, flags.hasBack());
  //printf("Adding front face, existence: %i\n", existence_front);
  input.addFace(index_base + 7, index_base + 6, index_base + 2, index_base + 3, flags.hasFront());
 
  //printf("Adding top face, existence: %i\n", existence_top);
  input.addFace(index_base + 1, index_base + 2, index_base + 6, index_base + 5, flags.hasTop());
  //printf("Adding bottom face, existence: %i\n\n", existence_bottom);
  input.addFace(index_base + 4, index_base + 7, index_base + 3, index_base, flags.hasBottom());
}
 
/// Wall object with a simplified argument list. Assumes Y-up.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint.
/// \param p2 Endpoint.
/// \param height1 Wall height at startpoint.
/// \param height2 Wall height at endpoint.
/// \param thickness Wall thickness.
/// \param flags CSG flags.
static void mesh_generate_wall_nqa(LogicalMesh &input, const vec3 &p1, const vec3 &p2, float height1, float height2,
    float thickness, CsgFlag flags = CSG_ALL)
{
    mesh_generate_wall(input, p1, p2, vec3(0, 1, 0), height1, height2, thickness, thickness, flags);
}
 
/// Add an arc object covering 90 degrees.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint (wide end).
/// \param p2 Endpoint (narrow end).
/// \param up Up vector (not sure if works perfectly).
/// \param height Height of the arc object.
/// \param thickness Thickness of the arc object.
/// \param arc_height_fraction How much of the arc object should the actual arc cover? Rest will be filled out wall.
/// \param arc_detail How many segments to form the arc of, ie. amount of polygons in the arc.
/// \param flags CSG flags.
/// \param block_reset How many segements to add before advancing block (0 to not advance).
static void mesh_generate_arc(LogicalMesh &input, const vec3 &p1, const vec3 &p2, const vec3 &up, float height,
    float thickness, float arc_height_fraction, unsigned arc_detail, CsgFlag flags = CSG_ALL, unsigned block_reset = 0)
{
    vec3 arc_axis = p2 - p1;
    float fdetail = static_cast<float>(arc_detail);
 
    for (unsigned ii = 0; ii < arc_detail; ii++)
    {
        float rationow = static_cast<float>(ii) / fdetail;
        float rationext = static_cast<float>(ii + 1) / fdetail;
        float heightnow = height * arc_height_fraction * dnload_sinf(rationow * static_cast<float>(M_PI*0.5));
        float distancenow = 1.0f - dnload_cosf(rationow * static_cast<float>(M_PI*0.5));
        float heightnext = height * arc_height_fraction * dnload_sinf(rationext * static_cast<float>(M_PI*0.5));
        float distancenext = 1.0f - dnload_cosf(rationext * static_cast<float>(M_PI*0.5));
        CsgFlag temp_flags = flags;
 
        if(block_reset && ((ii % block_reset) == 0))
        {
            input.advanceBlockId();
        }
 
        if (ii == 0)
        {
          temp_flags.disable(CsgFlag::FRONT);
        }
        else if (ii == arc_detail - 1)
        {
          temp_flags.disable(CsgFlag::BACK);
        }
        else
        {
          temp_flags.disable(CsgFlag::FRONT, CsgFlag::BACK);
        }
 
        mesh_generate_wall(input, p1 + distancenow*arc_axis + up*heightnow,
            p1 + distancenext*arc_axis + up*heightnext, up, height - heightnow, height - heightnext,
            thickness, thickness, temp_flags);
    }
}
 
/// Arc object with a simplified argument list. Assumes Y-up.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint (wide end).
/// \param p2 Endpoint (narrow end).
/// \param height Height of the arc object.
/// \param thickness Thickness of the arc object.
/// \param arc_height How much of the arc object should the actual arc cover? Rest will be filled out wall.
/// \param arc_detail How many segments to form the arc of, ie. amount of polygons in the arc.
/// \param flags CSG flags.
/// \param block_reset How many segements to add before advancing block (0 to not advance).
static void mesh_generate_arc_nqa(LogicalMesh &input, const vec3 &p1, const vec3 &p2, float height,
    float thickness, float arc_height, unsigned arc_detail, CsgFlag flags = CSG_ALL, unsigned block_reset = 0)
{
    mesh_generate_arc(input, p1, p2, vec3(0, 1, 0), height, thickness, arc_height, arc_detail, flags,
        block_reset);
}
 
/// Add a triangular prism object.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint of the prism.
/// \param p2 Endpoint direction of prism. More specifically p2-p1 defines the vector from the point of the prism towards the middle of its back edge.
/// \param up Up vector.
/// \param up sidelenght1 The length of the sides of the prism beside the startpoint, bottom.
/// \param up sidelenght2 The length of the sides of the prism beside the startpoint, up.
/// \param up arc Angle that the prism covers (radians). This and sidelength define its size.
/// \param up height Height of the prism.
/// \param flags CSG flags.
static void mesh_generate_wedge(LogicalMesh &input, const vec3 &p1, const vec3 &p2, const vec3 &up,
    float sidelength1, float sidelength2, float arc, float height, CsgFlag flags = CSG_ALL)
{
    //printf("Points: [%f %f %f], [%f %f %f]\n", p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
    //printf("S1 S2 Arc: [%f %f %f]\n", sidelength1, sidelength2, arc);

    vec3 wedge_axis_b, wedge_normal_b, wedge_normal_actual_b;
    vec3 wedge_axis_u, wedge_normal_u, wedge_normal_actual_u;

		unsigned index_base = input.getLogicalVertexCount();

		input.addVertex(p1);
		input.addVertex(p1 + height*up);

    if (sidelength1 > 0.0f) {
        wedge_axis_b = normalize(p2 - p1)*dnload_cosf(arc / 2)*sidelength1;
        wedge_normal_b = normalize(cross(wedge_axis_b, up));
        wedge_normal_actual_b = wedge_normal_b*dnload_sinf(arc / 2)*sidelength1;

				input.addVertex(p1 + wedge_axis_b + wedge_normal_actual_b);
				input.addVertex(p1 + wedge_axis_b - wedge_normal_actual_b);
		}
		else {
			input.addVertex(p1);
			input.addVertex(p1);
		}
 
    if (sidelength2 > 0.0f) {
        wedge_axis_u = normalize(p2 - p1)*dnload_cosf(arc / 2)*sidelength2;
        wedge_normal_u = normalize(cross(wedge_axis_u, up));
        wedge_normal_actual_u = wedge_normal_u*dnload_sinf(arc / 2)*sidelength2;

				input.addVertex(p1 + wedge_axis_u + wedge_normal_actual_u + height*up);
				input.addVertex(p1 + wedge_axis_u - wedge_normal_actual_u + height*up);
		}
    else {
        input.addVertex(p1 + height*up);
        input.addVertex(p1 + height*up);
    }
 
    input.addFace(index_base + 3, index_base + 2, index_base, flags.hasBottom());
    input.addFace(index_base + 1, index_base + 4, index_base + 5, flags.hasTop());
 
    input.addFace(index_base, index_base + 2, index_base + 4, index_base + 1, flags.hasRight());
    input.addFace(index_base + 1, index_base + 5, index_base + 3, index_base, flags.hasLeft());
 
    input.addFace(index_base + 3, index_base + 5, index_base + 4, index_base + 2, flags.hasBack());

		if (arc < 0)
			input.flipLastFaces(5);
}
 
/// Triangular prism object with a simplified argument list. Assumes Y-up.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint of the prism.
/// \param p2 Endpoint direction of prism. More specifically p2-p1 defines the vector from the point of the prism towards the middle of its back edge.
/// \param sidelenght1 The length of the sides of the prism beside the startpoint, bottom.
/// \param sidelenght2 The length of the sides of the prism beside the startpoint, up.
/// \param arc Angle that the prism covers (radians). This and sidelength define its size.
/// \param height Height of the prism.
/// \param flags CSG flags.
static void mesh_generate_wedge_nqa(LogicalMesh &input, const vec3 &p1, const vec3 &p2, float sidelength1,
    float sidelength2, float arc, float height, CsgFlag flags = CSG_ALL)
{
    mesh_generate_wedge(input, p1, p2, vec3(0, 1, 0), sidelength1, sidelength2, arc, height, flags);
}

/// Creates a triangular object suitable for maze wall ledges.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint of ledge left edge.
/// \param p2 Endpoint of ledge left edge.
/// \param up Up vector.
/// \param width Width of ledge.
/// \param height Height of ledge.
/// \param flags CSG flags.
static void mesh_generate_ledge_segment(LogicalMesh &input, const vec3 &p1, const vec3 &p2, const vec3 &up,
	float width, float height, CsgFlag flags = CSG_ALL)
{
	vec3 ledge_axis = p2 - p1;
	vec3 ledge_normal = normalize(cross(ledge_axis, up));

	unsigned index_base = input.getLogicalVertexCount();

	input.addVertex(p1);
	input.addVertex(p1 + up*height);
	input.addVertex(p1 + up*height + ledge_normal*width);
	input.addVertex(p2);
	input.addVertex(p2 + up*height);
	input.addVertex(p2 + up*height + ledge_normal*width);

	input.addFace(index_base, index_base + 2, index_base + 1, flags.hasBack());
	input.addFace(index_base + 3, index_base + 4, index_base + 5, flags.hasFront());
	input.addFace(index_base, index_base + 1, index_base + 4, index_base + 3, flags.hasLeft());
	input.addFace(index_base, index_base + 3, index_base + 5, index_base + 2, flags.hasBottom());
	input.addFace(index_base + 1, index_base + 2, index_base + 5, index_base + 4, flags.hasTop());
}

/// Creates a triangular object suitable for maze wall ledges. Assumes Y-up.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint of ledge left edge.
/// \param p2 Endpoint of ledge left edge.
/// \param width Width of ledge.
/// \param height Height of ledge.
/// \param flags CSG flags.
static void mesh_generate_ledge_segment_nqa(LogicalMesh &input, const vec3 &p1, const vec3 &p2, float width, float height, CsgFlag flags = CSG_ALL) {
	mesh_generate_ledge_segment(input, p1, p2, vec3(0, 1, 0), width, height, flags);
}

/// Creates a pyramid shape, needed for ledges.
///
/// \param input Mesh to append to.
/// \param b1 Base vector 1.
/// \param b2 Base vector 2.
/// \param p1 Base center.
/// \param p2 Top vertex position.
/// \param flags CSG flags.
static void mesh_generate_pyramid(LogicalMesh &input, const vec3 &b1, vec3 b2, const vec3 &p1, const vec3 &p2,
    CsgFlag flags = CSG_ALL)
{
  vec3 corner1 = p1 - 0.5f*(b1 + b2);
  vec3 corner2 = p1 - 0.5f*(b1 - b2);
  vec3 corner3 = p1 - 0.5f*(-b1 - b2);
  vec3 corner4 = p1 - 0.5f*(-b1 + b2);

  unsigned index_base = input.getLogicalVertexCount();

  input.addVertex(corner1);
  input.addVertex(corner2);
  input.addVertex(corner3);
  input.addVertex(corner4);
  input.addVertex(p2);

  input.addFace(index_base, index_base + 1, index_base + 4, flags.hasLeft());
  input.addFace(index_base + 1, index_base + 2, index_base + 4, flags.hasTop());
  input.addFace(index_base + 2, index_base + 3, index_base + 4, flags.hasRight());
  input.addFace(index_base + 3, index_base, index_base + 4, flags.hasBottom());
  input.addFace(index_base + 3, index_base + 2, index_base + 1, index_base, flags.hasBack());
}

/// Generate pillar.
///
/// \param input Mesh to add to.
/// \param p1 Pillar starting point.
/// \param p2 Pillar ending point.
/// \param radius Pillar radius.
/// \param detail Pillar detail.
/// \param type Pillar type (?).
/// \param flags CSG flags.
static void mesh_generate_pillar(LogicalMesh &input, const vec3 &p1, const vec3 &p2, float radius,
	unsigned detail, int type, CsgFlag flags = CSG_ALL)
{
	vec3 pillar_axis = p2 - p1;

	bool existence_bottom = flags.hasBottom();
	bool existence_top = flags.hasBack();
	float *pillarprofile_r, *pillarprofile_h;

	for (int ii = 0; ii < PILLAR_DETAIL_H - 1; ii++)
	{
		CsgFlag temp_flags;

		if (ii == 0)
		{
			if (existence_bottom)
			{
				temp_flags = CSG_NO_FRONT;
			}
		}
		else if (ii == PILLAR_DETAIL_H - 2)
		{
			if (existence_top)
			{
				temp_flags = CSG_NO_BACK;
			}
		}
		switch (type) {
		case 1:
			pillarprofile_h = pillarprofile1_h;
			pillarprofile_r = pillarprofile1_r;
			break;
		default:
			pillarprofile_h = pillarprofile2_h;
			pillarprofile_r = pillarprofile2_r;
			break;
		}
		mesh_generate_cylinder(input,
			p1 + pillarprofile_h[ii] * pillar_axis, p1 + pillarprofile_h[ii + 1] * pillar_axis,
			pillarprofile_r[ii] * radius, pillarprofile_r[ii + 1] * radius,
			detail, CSG_DIR_UP, CSG_NO_FRONT);
	}
}

/// Generate pillar - no questions asked.
///
/// \param input Mesh to add to.
/// \param p Bottom point of pillar.
/// \param height Height of pillar in Y axis.
/// \param type Pillar type (?).
/// \param flags CSG flags.
static void mesh_generate_pillar_nqa(LogicalMesh &input, const vec3 &p, float height, int type,
	CsgFlag flags = CSG_ALL)
{
	mesh_generate_pillar(input, p, p + height*vec3(0, 1, 0), PILLAR_RADIUS, PILLAR_DETAIL, type,
		flags);
}

/// Add a wall object with an arc hollowed into it.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint.
/// \param p2 Endpoint.
/// \param height Height of the wall.
/// \param thickness Thickness of the wall.
/// \param arc_height How much of the wall height should the arc cover?
/// \param arc_height_fraction How much of the arc height should the actual arc cover? The rest will be straight wall.
/// \param arc_detail How many segments to form the arc of, ie. amount of polygons in the arc. Must be even.
/// \param flags CSG flags.
static void mesh_generate_arced_wall(LogicalMesh &input, const vec3 &p1, const vec3 &p2, float height,
    float thickness, float arc_width, float arc_height, float arc_height_fraction, unsigned arc_detail,
    CsgFlag flags = CSG_ALL)
{
  float wall_fraction = (1.0f - arc_width) / 2.0f;
  vec3 arc_axis = p2 - p1;
  vec3 arc_axis_actual = arc_axis*arc_width;
  vec3 arc_start = p1 + arc_axis*wall_fraction;
  vec3 arc_end = p1 + arc_axis*(1 - wall_fraction);
  vec3 up = vec3(0, 1, 0);

#if defined(USE_LD) && 0
  std::cout << "Existence flags: " << flags << std::endl;
#endif

  // Wall section before opening
  if (arc_width < 1.0f) {
    if (arc_height_fraction < 1.0)
    {
      mesh_generate_wall_nqa(input, p1, arc_start, height, height*arc_height*(1.0f - arc_height_fraction),
          thickness, (flags & CSG_NO_TOP) | CSG_FRONT);
      mesh_generate_wall_nqa(input, p1 + up*height, arc_start + up*height*arc_height*(1.0f - arc_height_fraction),
          0, height*(1.0f - arc_height*(1.0f - arc_height_fraction)), thickness, flags & CSG_NO_FRONT & CSG_NO_BOTTOM);
    }
    else
    {
      mesh_generate_wall_nqa(input, p1, arc_start, height, height, thickness, flags & CSG_NO_FRONT);
    }
  }

  float arc_total_height = 1.0f - arc_height + arc_height*arc_height_fraction;
  float arc_true_height = arc_height*arc_height_fraction;
  float arc_start_height = arc_height*(1.0f - arc_height_fraction);

  mesh_generate_arc_nqa(input, arc_start + up*height*arc_start_height,
      arc_start + 0.5f*arc_axis_actual + up*height*arc_start_height, arc_total_height*height, thickness,
      arc_true_height / arc_total_height, arc_detail / 2, (flags & CSG_NO_FRONT & CSG_NO_BACK) | CSG_BOTTOM);
  mesh_generate_arc_nqa(input, arc_end + up*height*arc_start_height,
      arc_end - 0.5f*arc_axis_actual + up*height*arc_start_height, arc_total_height*height, thickness,
      arc_true_height / arc_total_height, arc_detail / 2, (flags & CSG_NO_FRONT & CSG_NO_BACK) | CSG_BOTTOM);

  // Wall section after opening
  if (arc_width < 1.0f) {
    if (arc_height_fraction < 1.0)
    {
      mesh_generate_wall_nqa(input, arc_end, p2, height*arc_height*(1.0f - arc_height_fraction), height, thickness,
          (flags & CSG_NO_TOP) | CSG_BACK);
      mesh_generate_wall_nqa(input, arc_end + up*height*arc_height*(1.0f - arc_height_fraction), p2 + up*height,
          height*(1.0f - arc_height*(1.0f - arc_height_fraction)), 0, thickness, flags & CSG_NO_BACK & CSG_NO_BOTTOM);
    }
    else
    {
      mesh_generate_wall_nqa(input, arc_end, p2, height, height, thickness, flags & CSG_NO_BACK);
    }
  }
}

/// Generate one 'ring' of polygons.
///
/// This is highly specific, but used both for haamus and floating islands.
///
/// \param msh Target mesh.
/// \param h1 First height.
/// \param r1 First radius.
/// \param d1 First detail.
/// \param s1 First seed.
/// \param h2 Second height.
/// \param r2 Second radius.
/// \param d2 Second detail.
/// \param s2 Second seed.
/// \param invert True to invert faces.
void mesh_generate_polygon_ring(LogicalMesh &msh, float h1, float r1, unsigned d1, unsigned s1, float h2,
    float r2, unsigned d2, unsigned s2, bool invert)
{
  static const float RANDOM_FACTOR = 0.1f;
  static const float RANDOM_FACTOR_HT = 0.05f;

#if defined(USE_LD)
  if(d1 <= d2)
  {
    std::ostringstream sstr;
    sstr << "first detail level " << d1 << " must be greater than second " << d2;
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
#endif

  unsigned first_base = msh.getLogicalVertexCount();

  // First circle.
  bsd_srand(s1);

  float a1 = static_cast<float>(M_PI * 2.0) / static_cast<float>(d1);
  float a1_add = frand(0.0f, a1);

  for(unsigned ii = 0; (ii < d1); ++ii)
  {
    float angle = static_cast<float>(ii) * a1 + a1_add;
    float ca = dnload_cosf(angle);
    float sa = dnload_sinf(angle);
    vec3 point(ca * r1 + frand(-RANDOM_FACTOR * r1, RANDOM_FACTOR * r1),
        h1 + frand(-RANDOM_FACTOR_HT * r1, RANDOM_FACTOR_HT * r1),
        sa * r1 + frand(-RANDOM_FACTOR * r1, RANDOM_FACTOR * r1));

    msh.addVertex(point);
  }

  // Second circle.
  unsigned second_base = msh.getLogicalVertexCount();
  bsd_srand(s2);

  float a2 = static_cast<float>(M_PI * 2.0) / static_cast<float>(d2);
  float a2_add = frand(0.0f, a2);

  for(unsigned ii = 0; (ii < d2); ++ii)
  {
    float angle = static_cast<float>(ii) * a2 + a2_add;
    float ca = dnload_cosf(angle);
    float sa = dnload_sinf(angle);
    vec3 point(ca * r2 + frand(-RANDOM_FACTOR * r2, RANDOM_FACTOR * r2),
        h2 + frand(-RANDOM_FACTOR_HT * r2, RANDOM_FACTOR_HT * r2),
        sa * r2 + frand(-RANDOM_FACTOR * r2, RANDOM_FACTOR * r2));

    msh.addVertex(point);

    // Add faces.
    {
      unsigned ii0 = ii + first_base;
      unsigned ii1 = ii + 1;
      unsigned jj0 = ii + second_base;
      unsigned jj1 = ii + 1;

      if(ii1 >= d1)
      {
        ii1 = 0;
      }
      if(jj1 >= d2)
      {
        jj1 = 0;
      }
      ii1 += first_base;
      jj1 += second_base;

      if(invert)
      {
        msh.addFace(ii0, jj0, ii1);
        msh.addFace(ii1, jj0, jj1);
      }
      else
      {
        msh.addFace(ii0, ii1, jj0);
        msh.addFace(ii1, jj1, jj0);
      }
    }
  }

  if(invert)
  {
    msh.addFace(first_base + d2, second_base, first_base);
  }
  else
  {
    msh.addFace(first_base + d2, first_base, second_base);
  }
}


#endif
