#ifndef INTRO_MAZE_HPP
#define INTRO_MAZE_HPP

#include "intro_csg.hpp"

//######################################
// Constants for maze generation #######
//######################################

#define COLOR_MAZELEDGE vec3(1.0f, 0.99f, 0.95f)
#define COLOR_MAZEWALL vec3(0.95f, 0.96f, 1.0f)
#define COLOR_COLISEUM vec3(0.93f, 0.94f, 1.0f)

#define MAZE_WIDTH 2
#define MAZE_HEIGHT 3

#define MAZE_CELL_WALL_THICKNESS 1.0f
#define MAZE_CELL_HEIGHT 5.25f
#define MAZE_CELL_WIDTH 16.0f
#define MAZE_CELL_WIDTH_TOTAL (MAZE_CELL_WIDTH+MAZE_CELL_WALL_THICKNESS)
#define MAZE_OPENING_HEIGHT 3.0f
#define MAZE_OPENING_MARGIN 0.5f
#define MAZE_OPENING_HEIGHT_FULL (MAZE_OPENING_HEIGHT+MAZE_OPENING_MARGIN)
#define MAZE_ARC_FRACTION 0.4f
#define MAZE_ARC_FRACTIONI (1.0f-MAZE_ARC_FRACTION)
#define MAZE_LEDGE_HEIGHT (MAZE_CELL_HEIGHT-MAZE_OPENING_HEIGHT-MAZE_OPENING_MARGIN)
#define MAZE_LEDGE_HEIGHT_FRACTION 0.9f
#define MAZE_LEDGE_WIDTH MAZE_LEDGE_HEIGHT
#define MAZE_ARC_DETAIL 8

#define MAZE_LEDGE_END_NONE 0
#define MAZE_LEDGE_END_WEDGE 1
#define MAZE_LEDGE_END_ROUNDED 2

#define MAZE_START_X 0.0f //(-(MAZE_WIDTH/2)*MAZE_CELL_WIDTH)
#define MAZE_START_Y 0.0f //(-MAZE_HEIGHT*MAZE_CELL_HEIGHT)
#define MAZE_START_Z 0.0f //MAZE_START_X

#define AQUEDUCT_RADIUS_INNER 22.0f
#define AQUEDUCT_RADIUS_OUTER 38.0f
#define AQUEDUCT_HEIGHT_INNER 16.0f
#define AQUEDUCT_HEIGHT_OUTER 26.0f
#define AQUEDUCT_WIDTH_INNER 7.5f
#define AQUEDUCT_WIDTH_OUTER 7.5f
#define AQUEDUCT_DETAIL 12
#define AQUEDUCT_ARC_DETAIL 8
#define AQUEDUCT_FLOOR_THICKNESS 0.5f
#define AQUEDUCT_WALL_THICKNESS 0.75f
#define AQUEDUCT_STORIES 2
#define AQUEDUCT_OUTER_PILLAR_AMOUNT 30

//######################################
// Maze Resources ######################
//######################################

#include "intro_mazeprimitives.hpp"
#include "intro_mazeresources.hpp"

//######################################
// Primitives ##########################
//######################################

static void mesh_generate_aqueduct_circle(LogicalMesh &input, float radius, float width, float height, float detail, int type) {
  vec3 origin = vec3(0, 0, 0);
  vec3 up = vec3(0, 1, 0);

	float singlearc = static_cast<float>(2 * M_PI) / detail;

  for (int ii = 0; ii < detail; ii++) {
		float ratio1 = static_cast<float>(2 * M_PI*(static_cast<float>(ii) / detail));
		float ratio2 = static_cast<float>(2 * M_PI*(static_cast<float>(ii + 1) / detail));
    //printf("%f %f\n", ratio1, ratio2);
		float dir_x1 = static_cast<float>(dnload_cosf(ratio1));
		float dir_z1 = static_cast<float>(dnload_sinf(ratio1));
		float dir_x2 = static_cast<float>(dnload_cosf(ratio2));
		float dir_z2 = static_cast<float>(dnload_sinf(ratio2));
    vec3 dir_1 = origin + vec3(dir_x1, 0, dir_z1)*radius;
    vec3 dir_2 = origin + vec3(dir_x2, 0, dir_z2)*radius;

    vec3 dir_next = dir_2 - dir_1;

    vec3 wedge_axis = normalize(dir_1 - origin)*dnload_cosf(singlearc / 2)*width;
    vec3 wedge_normal = normalize(cross(wedge_axis, up))*dnload_sinf(singlearc / 2)*width;
    vec3 wedge_edge_dir = normalize(wedge_axis + wedge_normal);

    //Build wall
    vec3 wedge_start = dir_1;
    mesh_generate_wedge_nqa(input, wedge_start, wedge_start + dir_1, width, width, singlearc, height, CSG_NO_LEFT & CSG_NO_RIGHT);
    vec3 wall_start = dir_1 + wedge_edge_dir*width*0.5f;

    // Outer circle has more wall so that the shadows can be observed
    switch (type)
    {
      case 0:
        //mesh_generate_wall_nqa(input, wall_start, wall_start + dir_next, height, height, width, CSG_NO_FRONT & CSG_NO_BACK);
        mesh_generate_arced_wall(input, wall_start, wall_start + dir_next, height, width, 0.8f, 0.85f, 0.3f, AQUEDUCT_ARC_DETAIL, CSG_NO_FRONT & CSG_NO_BACK);
				mesh_generate_wedge_nqa(input, wedge_start, origin, length(dir_next)*0.1f, length(dir_next)*0.1f, static_cast<float>(M_PI) - singlearc, height, CSG_NO_LEFT & CSG_NO_RIGHT);
        break;
      case 1:
        mesh_generate_arced_wall(input, wall_start, wall_start + dir_next, height, width, 0.6f, 0.85f, 0.3f, AQUEDUCT_ARC_DETAIL, CSG_NO_FRONT & CSG_NO_BACK);
				mesh_generate_wedge_nqa(input, wedge_start, origin, length(dir_next)*0.25f, length(dir_next)*0.25f, static_cast<float>(M_PI) - singlearc, height, CSG_NO_LEFT & CSG_NO_RIGHT);
        break;
      default:
#if defined(USE_LD)
        {
          std::ostringstream sstr;
          sstr << "unknown type: " << type;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        break;
    }
  }
}

static void mesh_generate_end_scene(LogicalMesh &input)
{
  mesh_generate_aqueduct_circle(input, AQUEDUCT_RADIUS_INNER, AQUEDUCT_WIDTH_INNER, AQUEDUCT_HEIGHT_INNER, 12, 0);
  mesh_generate_aqueduct_circle(input, AQUEDUCT_RADIUS_OUTER, AQUEDUCT_WIDTH_OUTER, AQUEDUCT_HEIGHT_OUTER, 12, 1);

  vec3 pillarpos = vec3(14.0f, 0.0f, 0.0f);

  mesh_generate_pillar(input, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 14.0f, 0.0f), 4, 12, 2);
  for (int ii = 0; ii < 6; ii++)
  {
    pillarpos = pillarpos.rotateY(static_cast<float>(M_PI / 3.0));
    mesh_generate_pillar(input, pillarpos, pillarpos + (19.0f + static_cast<float>(ii) * 1.5f) * CSG_DIR_UP,
        2, 8, 1);
  }

  // Magically get good placement for pillars
  bsd_srand(19); // or 10?
	
  for (int ii = 0; ii < AQUEDUCT_OUTER_PILLAR_AMOUNT; ii++)
  {
    pillarpos = vec3(AQUEDUCT_RADIUS_OUTER + AQUEDUCT_WIDTH_OUTER + 3.0f + frand(2.0f * AQUEDUCT_RADIUS_OUTER), -2.0f, 0.0f);
    pillarpos = pillarpos.rotateY(frand(2.0f*static_cast<float>(M_PI))-0.15f);
    mesh_generate_pillar(input, pillarpos, pillarpos + (15.0f + frand(20.0f))*CSG_DIR_UP, 2, 8, 2);
  }
}

#endif
