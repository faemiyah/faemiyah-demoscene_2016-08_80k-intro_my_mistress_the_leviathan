#ifndef INTRO_MAZEPRIMITIVES_HPP
#define INTRO_MAZEPRIMITIVES_HPP

//######################################
// Maze primitive generation constants #
//######################################

#define MAZE_WALL_TYPE_PLAIN						0
#define MAZE_WALL_TYPE_LARGE_ARC_1			1
#define MAZE_WALL_TYPE_LARGE_ARC_2			2
#define MAZE_WALL_TYPE_LARGE_ARC_3			3
#define MAZE_WALL_TYPE_SMALL_ARC_1			4
#define MAZE_WALL_TYPE_SMALL_ARC_2			5
#define MAZE_WALL_TYPE_SMALL_ARC_3			6
#define MAZE_WALL_TYPE_HALF_ARC					7
#define MAZE_WALL_TYPE_DOUBLE_HALF_ARC	8
#define MAZE_WALL_TYPE_PILLARS					9
#define MAZE_WALL_TYPE_ROOF							10
#define MAZE_WALL_TYPE_CENTRAL_PILLAR		11

#define MAZE_WALL_TYPES									12

#define MAZE_LARGE_ARC_WIDTH_1	(2.5f*MAZE_LEDGE_WIDTH)
#define MAZE_LARGE_ARC_WIDTH_2	(4.5f*MAZE_LEDGE_WIDTH)
#define MAZE_LARGE_ARC_WIDTH_3	(6.5f*MAZE_LEDGE_WIDTH)

#define LEDGE_END_NEAR 0
#define LEDGE_END_FAR 1

// Database types
#define LEDGE_DATABASE_BODY_1 0
#define LEDGE_DATABASE_BODY_2 1
#define LEDGE_DATABASE_BODY_3 2
#define LEDGE_DATABASE_BODY_4 3
#define LEDGE_DATABASE_BODY_5 4
#define LEDGE_DATABASE_BODY_6 5
#define LEDGE_DATABASE_BODY_7 6
#define LEDGE_DATABASE_RAMP_CW 7
#define LEDGE_DATABASE_RAMP_CCW 8
#define LEDGE_DATABASE_NONE 9
#define LEDGE_DATABASE_ROUNDED_CW 10
#define LEDGE_DATABASE_ROUNDED_CCW 11
#define LEDGE_DATABASE_WEDGE_CW 12
#define LEDGE_DATABASE_WEDGE_CCW 13
#define LEDGE_DATABASE_RAMP_EXIT_CW 14
#define LEDGE_DATABASE_RAMP_EXIT_CCW 15


#define MAZE_LEDGE_TYPES 16

//######################################
// Functions for maze ledges ###########
//######################################

/// Creates a rounded ledge ending out of prism objects. Assumes Y-up.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint of initial edge vector.
/// \param p2 Endpoint of initial edge vector.
/// \param radius Width of fan.
/// \param height Height of fan.
/// \param wedge_fraction How large portion of the ledge is consumed by the angled part, rest is solid floor.
/// \param arc Fan arc direction and magnitude.
/// \param detail How many prism elements to use.
/// \param flags CSG flags.
static void mesh_generate_rounded_ledge_end(LogicalMesh &input, const vec3 &p1, const vec3 &p2, float radius,
	float height, float wedge_fraction, float arc, int detail, CsgFlag flags = CSG_ALL)
{
  float rotationstep = arc / static_cast<float>(detail);
  vec3 upstep = height*wedge_fraction*vec3(0, 1, 0);
  vec3 initial_dir = p2 - p1;

  for (int ii = 0; ii < detail; ii++) {
    // Annoying tromp to disable the correct flags related to fan rotation direction and so on
    CsgFlag temp_flags = flags;
    if (ii == 0) {
      temp_flags.disable(CsgFlag::LEFT);
    }
    else if (ii == detail - 1) {
      temp_flags.disable(CsgFlag::RIGHT);
    }
    else {
      temp_flags.disable(CsgFlag::LEFT);
      temp_flags.disable(CsgFlag::RIGHT);
    }
    mesh_generate_wedge_nqa(input, p1, p1 + initial_dir.rotateY(static_cast<float>(ii)*rotationstep + rotationstep / 2.0f), 0.0f, radius, rotationstep, height*wedge_fraction, temp_flags & CsgFlag::NO_TOP);
		mesh_generate_wedge_nqa(input, p1 + upstep, p1 + initial_dir.rotateY(static_cast<float>(ii)*rotationstep + rotationstep / 2.0f) + upstep, radius, radius, rotationstep, height*(1.0f - wedge_fraction), temp_flags & CsgFlag::NO_BOTTOM);
  }
}

/// Creates a ledge body or ramp for the maze. Assumes Y-up.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint of ledge left edge.
/// \param p2 Endpoint of ledge left edge.
/// \param width Width of ledge.
/// \param height Height of ledge.
/// \param wedge_fraction How large portion of the ledge is consumed by the angled part, rest is solid floor.
/// \param flags CSG flags.
static void mesh_generate_ledge(LogicalMesh &input, const vec3 &p1, const vec3 &p2, bool ramp, bool direction, float ramp_height, float width, float height, float wedge_fraction, CsgFlag flags = CSG_ALL) {
	vec3 ledge_axis = p2 - p1;
	vec3 ledge_axis_n = normalize(ledge_axis);

	vec3 p1_actual = p1;
	vec3 p2_actual = p2;

	vec3 up = vec3(0, 1, 0);
	vec3 ledge_normal = cross(ledge_axis_n, up);

	if (ramp) {
		if (direction == DIRECTION_NORMAL)
			p2_actual = p2 + up*ramp_height;
		else
			p1_actual = p1 + up*ramp_height;
	}

	//Generate ledge
	mesh_generate_ledge_segment_nqa(input, p2_actual, p1_actual, width, height*wedge_fraction, flags & CSG_NO_TOP & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_LEFT);
	//Ledge top
	vec3 offset = up*height*wedge_fraction - 0.5f*ledge_normal*width;
	mesh_generate_wall_nqa(input, p1_actual + offset, p2_actual + offset, height*(1.0f - wedge_fraction), height*(1.0f - wedge_fraction), width, flags & CSG_NO_BOTTOM & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_LEFT);
}

/// Creates a ledge ending for the maze. Assumes Y-up.
///
/// \param input Mesh to append to.
/// \param p1 Startpoint of ledge left edge.
/// \param p2 p2-p1 Is the ledge direction associated with the ledge_end.
/// \param direction Clockwise or counterclockwise.
/// \param type Type of ledge end to draw.
/// \param width Width of ledge end.
/// \param height Height of ledge end.
/// \param wedge_fraction How large portion of the ledge is consumed by the angled part, rest is solid floor.
/// \param flags CSG flags.
static void mesh_generate_ledge_end(LogicalMesh &input, const vec3 &p1, const vec3 &p2, bool direction,
    int type, float width, float height, float wedge_fraction, CsgFlag flags = CSG_ALL)
{
  vec3 ledge_axis = p2 - p1;
  vec3 ledge_axis_n = normalize(ledge_axis);

  vec3 up = vec3(0, 1, 0);
  vec3 ledge_normal = cross(ledge_axis_n, up);

  // Offset for top calculations
  vec3 offset = up*height*wedge_fraction - 0.5f*ledge_normal*width;

  // Draw the end
  if(direction == LEDGE_END_NEAR)
  {
    // Near end
    switch (type)
    {
      case MAZE_LEDGE_END_NONE:
        mesh_generate_ledge_segment_nqa(input, p2, p1, width, height*wedge_fraction, flags & CSG_NO_TOP & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_LEFT);
        mesh_generate_wall_nqa(input, p2 + offset, p1 + offset, height*(1.0f - wedge_fraction), height*(1.0f - wedge_fraction), width, flags & CSG_NO_BOTTOM & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_RIGHT);
        break;
      case MAZE_LEDGE_END_ROUNDED:
        mesh_generate_rounded_ledge_end(input, p2, p2 - ledge_normal, width, height, wedge_fraction, (float)M_PI / 2.0f, 3, flags & CSG_NO_RIGHT & CSG_NO_LEFT);
        break;
      case MAZE_LEDGE_END_WEDGE:
        mesh_generate_pyramid(input, ledge_axis_n*width, up*wedge_fraction*height, p2 - ledge_axis_n*width*0.5f + up*wedge_fraction*height*0.5f, p2 - ledge_normal*width + up*wedge_fraction*height, flags & CSG_NO_TOP & CSG_NO_LEFT & CSG_NO_BACK & CSG_NO_RIGHT);
        mesh_generate_wedge_nqa(input, p2 + up*wedge_fraction*height, p2 - ledge_normal - ledge_axis_n + up*wedge_fraction*height, width, width, (float)M_PI / 2.0f, (1.0f - wedge_fraction)*height, flags & CSG_NO_BOTTOM & CSG_NO_LEFT & CSG_NO_RIGHT & CSG_NO_BACK);
        break;
      default:
#if defined(USE_LD)
        {
          std::ostringstream sstr;
          sstr << "unknown end type: " << type;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        break;
    }
  }
  else // LEDGE_END_FAR
  {
    // Far end
    switch (type)
    {
      case MAZE_LEDGE_END_NONE:
        mesh_generate_ledge_segment_nqa(input, p2, p1, width, height*wedge_fraction, flags & CSG_NO_TOP & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_LEFT);
        mesh_generate_wall_nqa(input, p2 + offset, p1 + offset, height*(1.0f - wedge_fraction), height*(1.0f - wedge_fraction), width, flags & CSG_NO_BOTTOM & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_RIGHT);
        break;
      case MAZE_LEDGE_END_ROUNDED:
        mesh_generate_rounded_ledge_end(input, p1, p1 - ledge_normal, width, height, wedge_fraction, -(float)M_PI / 2.0f, 3, flags & CSG_NO_RIGHT & CSG_NO_LEFT);
        break;
      case MAZE_LEDGE_END_WEDGE:
        mesh_generate_pyramid(input, ledge_axis_n*width, up*wedge_fraction*height, p1 + ledge_axis_n*width*0.5f + up*wedge_fraction*height*0.5f, p1 - ledge_normal*width + up*wedge_fraction*height, flags & CSG_NO_TOP & CSG_NO_RIGHT & CSG_NO_BACK & CSG_NO_LEFT);
        mesh_generate_wedge_nqa(input, p1 + up*wedge_fraction*height, p1 - ledge_normal + ledge_axis_n + up*wedge_fraction*height, width, width, (float)M_PI / 2.0f, (1.0f - wedge_fraction)*height, flags & CSG_NO_BOTTOM & CSG_NO_LEFT & CSG_NO_RIGHT & CSG_NO_BACK);
        break;
      default:
#if defined(USE_LD)
        {
          std::ostringstream sstr;
          sstr << "unknown end type: " << type;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        break;
    }
  }
}

//######################################
// Functions for maze walls ############
//######################################

/// Creates a Maze wall segment with a large arc.
/// Macro for the following function, spaghetti code, disregard.
/// CSG Flags are automatic and should not be touched atm.
static void mesh_generate_maze_wall_segment_large_arc(LogicalMesh &input, const vec3 &p1, const vec3 &p2, float wall_length, CsgFlag flags = CSG_ALL) {
	vec3 up = vec3(0, 1, 0);
	vec3 axis = p2 - p1;
	vec3 axis_n = normalize(axis);

	// Walls under opening
	mesh_generate_wall_nqa(input, p1, p1 + axis_n*wall_length, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_TOP & CSG_NO_BACK & CSG_NO_BOTTOM);
	mesh_generate_wall_nqa(input, p1 + axis_n*wall_length, p2 - axis_n*wall_length, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_BOTTOM);
	mesh_generate_wall_nqa(input, p2 - axis_n*wall_length, p2, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BACK  & CSG_NO_TOP & CSG_NO_FRONT & CSG_NO_BOTTOM);
	// Wall beside opening
	mesh_generate_wall_nqa(input, p1 + MAZE_LEDGE_HEIGHT*up, p1 + axis_n*wall_length + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BOTTOM & CSG_NO_TOP & CSG_NO_BACK);
	mesh_generate_wall_nqa(input, p2 + MAZE_LEDGE_HEIGHT*up, p2 - axis_n*wall_length + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BOTTOM & CSG_NO_TOP & CSG_NO_BACK);
	mesh_generate_wall_nqa(input, p1 + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI)*up, p1 + axis_n*wall_length + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI)*up, MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BOTTOM & CSG_NO_BACK & CSG_NO_TOP);
	mesh_generate_wall_nqa(input, p2 + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI)*up, p2 - axis_n*wall_length + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI)*up, MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BOTTOM & CSG_NO_BACK & CSG_NO_TOP);
	// Arcs
	mesh_generate_arc_nqa(input, p1 + axis_n*wall_length + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), p1 + axis_n*(wall_length + MAZE_LEDGE_WIDTH) + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTION / (MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_ARC_DETAIL / 2, flags & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_TOP);
	mesh_generate_arc_nqa(input, p2 - axis_n*wall_length + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), p2 - axis_n*(wall_length + MAZE_LEDGE_WIDTH) + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTION / (MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_ARC_DETAIL / 2, flags & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_TOP);
	// Ceiling
	mesh_generate_wall_nqa(input, p1 + axis_n*(wall_length + MAZE_LEDGE_WIDTH) + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT)*up, p2 - axis_n*(wall_length + MAZE_LEDGE_WIDTH) + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT)*up, MAZE_OPENING_MARGIN, MAZE_OPENING_MARGIN, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP);
}

/// Creates a Maze wall segment with a small arc.
/// Macro for the following function, spaghetti code, disregard.
/// CSG Flags are automatic and should not be touched atm.
static void mesh_generate_maze_wall_segment_small_arc(LogicalMesh &input, const vec3 &p1, const vec3 &p2, CsgFlag flags = CSG_ALL) {
	vec3 up = vec3(0, 1, 0);
	vec3 axis = p2 - p1;
	vec3 axis_n = normalize(axis);

	float opening_width = MAZE_LEDGE_WIDTH*0.6666f;

	mesh_generate_wall_nqa(input, p1, p1 + axis_n*(length(axis)*0.5f - opening_width), MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags);
	mesh_generate_wall_nqa(input, p1 + axis_n*(length(axis)*0.5f - opening_width), p2 - axis_n*(length(axis)*0.5f - opening_width), MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags | CSG_TOP);
	mesh_generate_wall_nqa(input, p2, p2 - axis_n*(length(axis)*0.5f - opening_width), MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags);
	mesh_generate_arced_wall(input, p1 + MAZE_LEDGE_HEIGHT*up, p2 + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT_FULL, MAZE_CELL_WALL_THICKNESS, opening_width*2.0f / length(axis), MAZE_OPENING_HEIGHT / MAZE_OPENING_HEIGHT_FULL, MAZE_ARC_FRACTION, MAZE_ARC_DETAIL, flags);
}

/// Creates a Maze wall segment.
/// No parameters as this is expected to be used to generate VBOs that can
/// later be copied around the whole structure.
///
/// \param input Mesh to append to.
/// \param placement Where to draw this - will be removed later.
/// \param type What type of a segment we will draw.
/// \param flags CSG Flags, will be removed later.
static void mesh_generate_maze_wall_segment(LogicalMesh &input, const vec3 &placement, int type, CsgFlag flags = CSG_ALL) {
	vec3 p1 = vec3(MAZE_CELL_WIDTH / 2.0f, 0, 0) + placement;
	vec3 up = vec3(0, 1, 0);
	vec3 p2 = vec3(-MAZE_CELL_WIDTH / 2.0f, 0, 0) + placement;
	vec3 axis = p2 - p1;
	vec3 axis_n = normalize(axis);
	float wall_length;

	// This always exists, making it simple
	switch (type) {
	case MAZE_WALL_TYPE_PLAIN:
		mesh_generate_wall_nqa(input, p1, p2, MAZE_CELL_HEIGHT, MAZE_CELL_HEIGHT, MAZE_CELL_WALL_THICKNESS,
                    flags & CSG_NO_BOTTOM & CSG_NO_TOP & CSG_NO_BACK & CSG_NO_FRONT);
		break;
	case MAZE_WALL_TYPE_ROOF:
		mesh_generate_wall_nqa(input, p1, p2, MAZE_CELL_HEIGHT, MAZE_CELL_HEIGHT, MAZE_CELL_WALL_THICKNESS,
                    (flags & CSG_NO_BOTTOM & CSG_NO_LEFT & CSG_NO_RIGHT & CSG_NO_BACK & CSG_NO_FRONT) | CSG_TOP);
		break;
	case MAZE_WALL_TYPE_CENTRAL_PILLAR:
		mesh_generate_wall_nqa(input, p1, p1 + MAZE_CELL_WALL_THICKNESS*axis_n, MAZE_CELL_HEIGHT,
                    MAZE_CELL_HEIGHT, MAZE_CELL_WALL_THICKNESS, (flags & CSG_NO_BOTTOM) | CSG_TOP);
		break;
	case MAZE_WALL_TYPE_HALF_ARC:
		// Walls under opening
		mesh_generate_wall_nqa(input, p1, p1 + axis_n*MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BOTTOM & CSG_NO_BACK);
		mesh_generate_wall_nqa(input, p1 + axis_n*MAZE_LEDGE_WIDTH, p2, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM & CSG_NO_FRONT);
		// Wall beside opening
		mesh_generate_wall_nqa(input, p1 + axis_n*MAZE_LEDGE_WIDTH + MAZE_LEDGE_HEIGHT*up, p2 + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BOTTOM & CSG_NO_TOP & CSG_NO_FRONT);
		mesh_generate_wall_nqa(input, p1 + axis_n*MAZE_LEDGE_WIDTH + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI)*up, p2 + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI)*up, MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BACK & CSG_NO_BOTTOM & CSG_NO_TOP & CSG_NO_FRONT);
		// Arc
		mesh_generate_arc_nqa(input, p1 + MAZE_LEDGE_WIDTH*axis_n + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), p1 + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTION / (MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_ARC_DETAIL / 2, flags & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_TOP);
		break;
	case MAZE_WALL_TYPE_DOUBLE_HALF_ARC:
		// Walls under opening
		mesh_generate_wall_nqa(input, p1, p1 + axis_n*MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BOTTOM & CSG_NO_BACK);
		mesh_generate_wall_nqa(input, p1 + axis_n*MAZE_LEDGE_WIDTH, p2 - axis_n*MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_wall_nqa(input, p2 - axis_n*MAZE_LEDGE_WIDTH, p2, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BACK & CSG_NO_BOTTOM & CSG_NO_FRONT);
		// Wall beside opening
		mesh_generate_wall_nqa(input, p1 + axis_n*MAZE_LEDGE_WIDTH + MAZE_LEDGE_HEIGHT*up, p2 - axis_n*MAZE_LEDGE_WIDTH + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BOTTOM & CSG_NO_TOP);
		mesh_generate_wall_nqa(input, p1 + axis_n*MAZE_LEDGE_WIDTH + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI)*up, p2 - axis_n*MAZE_LEDGE_WIDTH + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI)*up, MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BACK & CSG_NO_FRONT & CSG_NO_BOTTOM & CSG_NO_TOP);
		// Arcs
		mesh_generate_arc_nqa(input, p1 + MAZE_LEDGE_WIDTH*axis_n + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), p1 + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTION / (MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_ARC_DETAIL / 2, flags & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_FRONT);
		mesh_generate_arc_nqa(input, p2 - axis_n*MAZE_LEDGE_WIDTH + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), p2 + up*(MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI, MAZE_CELL_WALL_THICKNESS, MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTION / (MAZE_OPENING_HEIGHT_FULL - MAZE_OPENING_HEIGHT*MAZE_ARC_FRACTIONI), MAZE_ARC_DETAIL / 2, flags & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_FRONT);
		break;
	case MAZE_WALL_TYPE_LARGE_ARC_1:
		wall_length = (MAZE_CELL_WIDTH - MAZE_LARGE_ARC_WIDTH_1) / 2.0f;
		// All except pillars
		mesh_generate_maze_wall_segment_large_arc(input, p1, p2, wall_length);
		// Pillars
		mesh_generate_pillar_nqa(input, p1 + axis_n*MAZE_CELL_WIDTH*+0.5f + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		break;
	case MAZE_WALL_TYPE_LARGE_ARC_2:
		wall_length = (MAZE_CELL_WIDTH - MAZE_LARGE_ARC_WIDTH_2) / 2.0f;
		// All except pillars
		mesh_generate_maze_wall_segment_large_arc(input, p1, p2, wall_length);
		// Pillars
		mesh_generate_pillar_nqa(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f + MAZE_LEDGE_WIDTH) + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_pillar_nqa(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH) + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		break;
	case MAZE_WALL_TYPE_LARGE_ARC_3:
		wall_length = (MAZE_CELL_WIDTH - MAZE_LARGE_ARC_WIDTH_3) / 2.0f;
		// All except pillars
		mesh_generate_maze_wall_segment_large_arc(input, p1, p2, wall_length);
		// Pillars
		mesh_generate_pillar_nqa(input, p1 + axis_n*MAZE_CELL_WIDTH*+0.5f + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_pillar_nqa(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f + MAZE_LEDGE_WIDTH*2.0f) + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_pillar_nqa(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*2.0f) + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		break;
	case MAZE_WALL_TYPE_PILLARS:
		// Floor and Roof
		mesh_generate_wall_nqa(input, p1, p2, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_BOTTOM & CSG_NO_FRONT & CSG_NO_BACK);
		mesh_generate_wall_nqa(input, p1 + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT)*up, p2 + (MAZE_LEDGE_HEIGHT + MAZE_OPENING_HEIGHT)*up, MAZE_OPENING_MARGIN, MAZE_OPENING_MARGIN, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP);
		// Pillars
		mesh_generate_pillar_nqa(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f + MAZE_LEDGE_WIDTH*1.0f) + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_pillar_nqa(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*1.0f) + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_pillar_nqa(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f + MAZE_LEDGE_WIDTH*3.0f) + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_pillar_nqa(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*3.0f) + MAZE_LEDGE_HEIGHT*up, MAZE_OPENING_HEIGHT, 1, flags & CSG_NO_TOP & CSG_NO_BOTTOM);
		break;
	case MAZE_WALL_TYPE_SMALL_ARC_1:
		mesh_generate_maze_wall_segment_small_arc(input, p1, p2, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		break;
	case MAZE_WALL_TYPE_SMALL_ARC_2:
		mesh_generate_wall_nqa(input, p1, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*2.0f), MAZE_CELL_HEIGHT, MAZE_CELL_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_maze_wall_segment_small_arc(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*2.0f), p1 + axis_n*MAZE_CELL_WIDTH*0.5f, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_maze_wall_segment_small_arc(input, p2 - axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*2.0f), p2 - axis_n*MAZE_CELL_WIDTH*0.5f, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_wall_nqa(input, p2, p2 - axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*2.0f), MAZE_CELL_HEIGHT, MAZE_CELL_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		break;
	case MAZE_WALL_TYPE_SMALL_ARC_3:
		mesh_generate_wall_nqa(input, p1, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*3.0f), MAZE_CELL_HEIGHT, MAZE_CELL_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_maze_wall_segment_small_arc(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*3.0f), p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*1.0f), flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_maze_wall_segment_small_arc(input, p1 + axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*1.0f), p2 - axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*1.0f), flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_maze_wall_segment_small_arc(input, p2 - axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*3.0f), p2 - axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*1.0f), flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		mesh_generate_wall_nqa(input, p2, p2 - axis_n*(MAZE_CELL_WIDTH*0.5f - MAZE_LEDGE_WIDTH*3.0f), MAZE_CELL_HEIGHT, MAZE_CELL_HEIGHT, MAZE_CELL_WALL_THICKNESS, flags & CSG_NO_FRONT & CSG_NO_BACK & CSG_NO_TOP & CSG_NO_BOTTOM);
		break;
	default:
#if defined(USE_LD)
		{
			std::ostringstream sstr;
			sstr << "invalid selection: " << type;
			BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
		}
#endif
		break;
	}
}

#endif
