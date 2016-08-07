#ifndef GLSL_MAZERESOURCES_HPP
#define GLSL_MAZERESOURCES_HPP

#define CONDITION_DIAGONAL 1
#define CONDITION_DIRECT_1 2
#define CONDITION_DIRECT_2 3
#define CONDITION_FULL 4
#define WALL_RANDOMIZER_SIZE 10

//static uint16_t walls_diagonal[WALL_RANDOMIZER_SIZE] = { 3, 3, 4, 4, 6, 6, 7, 7, 8, 8};
static uint8_t walls_diagonal[WALL_RANDOMIZER_SIZE] = { 1, 1, 2, 2, 3, 4, 4, 5, 5, 6 };
static uint8_t walls_direct_1[WALL_RANDOMIZER_SIZE] = { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
static uint8_t walls_direct_2[WALL_RANDOMIZER_SIZE] = { 8, 8, 8, 8, 8, 8, 9, 9, 9, 9};
static uint8_t walls_full[WALL_RANDOMIZER_SIZE] =     { 3, 3, 3, 6, 6, 6, 9, 9, 9, 9};

// How much further than ledge center will the ledge reach?
static float diagonal_length[MAZE_WALL_TYPES] =
{
  - MAZE_LEDGE_WIDTH*2.0f + MAZE_CELL_WIDTH, //N/A
  - MAZE_LEDGE_WIDTH*2.0f + MAZE_CELL_WIDTH / 2.0f + MAZE_LEDGE_WIDTH * 1.25,		// Large Arc 1
  - MAZE_LEDGE_WIDTH*2.0f + MAZE_CELL_WIDTH / 2.0f + MAZE_LEDGE_WIDTH * 2.25f,		// Large Arc 2
  - MAZE_LEDGE_WIDTH*2.0f + MAZE_CELL_WIDTH / 2.0f + MAZE_LEDGE_WIDTH * 3.25f,		// Large Arc 3
  - MAZE_LEDGE_WIDTH*2.0f + MAZE_CELL_WIDTH / 2.0f + MAZE_LEDGE_WIDTH * 0.6666f,	// Small Arc 1
  - MAZE_LEDGE_WIDTH*2.0f + MAZE_CELL_WIDTH / 2.0f + MAZE_LEDGE_WIDTH * 1.6666f,	// Small Arc 2
  - MAZE_LEDGE_WIDTH*2.0f + MAZE_CELL_WIDTH / 2.0f + MAZE_LEDGE_WIDTH * 2.6666f,	// Small Arc 3
};

//######################################
// Maze cell class #####################
//######################################

#define EAST 0
#define NORTH 1
#define WEST 2
#define SOUTH 3

#define LEDGE_NONE 0
#define LEDGE_NORMAL 1
#define LEDGE_DIAGONAL 2
#define LEDGE_RAMP 3
#define LEDGE_RAMP_EXIT 4

/// Maze cell class.
class MazeCell
{
  public:
    bool ledge_direction[4];
    bool ramp_direction[4];
    bool ledge_in_use[4][2];
    bool ledge_no_use[4][2];
    uint8_t ledge_end[4][2];
    bool wall_direction[4];
    uint8_t ledge[4];
    uint8_t wall[4];

    static uint8_t randomizeWall(int condition) {
      switch (condition) {
        case CONDITION_DIAGONAL:
          return walls_diagonal[urand(WALL_RANDOMIZER_SIZE)];
          break;
        case CONDITION_DIRECT_1:
          return walls_direct_1[urand(WALL_RANDOMIZER_SIZE)];
          break;
        case CONDITION_DIRECT_2:
          return walls_direct_2[urand(WALL_RANDOMIZER_SIZE)];
          break;
        case CONDITION_FULL:
          return walls_full[urand(WALL_RANDOMIZER_SIZE)];
          break;
        default:
#if defined(USE_LD)
          {
            std::ostringstream sstr;
            sstr << "invalid wall selection: " << condition;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
          }
#endif
          // Error situation
          return 0;
      }
    }

    static void setWallsBasedOnLedgeInformation(MazeCell &cell_current, MazeCell &cell_next, int right,
        int top, int left, int bottom)
    {
      // Both sides have full ledge
      if (cell_current.hasLedgeNormal(right) && cell_next.hasLedgeNormal(left)) {
        //std::cout << "Full ledge both sides FIRED!" << std::endl;
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_FULL));
        // This mode sacrifices the edges of the walls of the connection
        cell_current.setLedgeNoUse(right, LEDGE_END_FAR, true);
        cell_current.setLedgeNoUse(right, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_NEAR, true);
        // This mode needs no edges
      }
      // Both sides have both north and south ledges
      else if ((cell_current.hasLedgeNormal(bottom) && cell_next.hasLedgeNormal(bottom)) && (cell_current.hasLedge(top) && cell_next.hasLedge(top))) {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIRECT_2));
        // This mode sacrifices the edges of the walls of the connection
        cell_current.setLedgeNoUse(right, LEDGE_END_FAR, true);
        cell_current.setLedgeNoUse(right, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_NEAR, true);
        // This mode needs the edges near walls
        cell_current.setLedgeInUse(top, LEDGE_END_FAR, true);
        cell_current.setLedgeInUse(bottom, LEDGE_END_NEAR, true);
        cell_next.setLedgeInUse(top, LEDGE_END_NEAR, true);
        cell_next.setLedgeInUse(bottom, LEDGE_END_FAR, true);
      }
      // South & North ledges present on opposite sides of the wall
      else if (cell_current.hasLedgeNormal(bottom) && cell_next.hasLedgeNormal(top)) {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIAGONAL));
        cell_current.setLedge(right, LEDGE_DIAGONAL);
        cell_next.setLedge(left, LEDGE_DIAGONAL);
        cell_next.setLedgeDirection(left, DIRECTION_INVERSE);
        cell_current.setLedgeEnd(right, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);
        cell_current.setLedgeEnd(right, LEDGE_END_NEAR, MAZE_LEDGE_END_ROUNDED);
        cell_next.setLedgeEnd(left, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);
        cell_next.setLedgeEnd(left, LEDGE_END_NEAR, MAZE_LEDGE_END_ROUNDED);
        cell_current.setLedgeEnd(bottom, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
        cell_next.setLedgeEnd(top, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
        // We need the top right corner of the current cell and the bottom-left of the next one to remain as-is
        // Also the wall where the diagonal is off-limits
        cell_current.setLedgeNoUse(right, LEDGE_END_FAR, true);
        cell_current.setLedgeNoUse(right, LEDGE_END_NEAR, true);
        cell_current.setLedgeNoUse(top, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(bottom, LEDGE_END_NEAR, true);
        // These edges are needed
        cell_current.setLedgeInUse(bottom, LEDGE_END_FAR, true);
        cell_next.setLedgeInUse(top, LEDGE_END_FAR, true);
      }
      // North & south ledges present on opposite sides of the wall
      else if (cell_current.hasLedgeNormal(top) && cell_next.hasLedgeNormal(bottom)) {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIAGONAL));
        cell_current.setLedge(right, LEDGE_DIAGONAL);
        cell_next.setLedge(left, LEDGE_DIAGONAL);
        cell_current.setLedgeDirection(right, DIRECTION_INVERSE);
        cell_current.setLedgeEnd(right, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
        cell_current.setLedgeEnd(right, LEDGE_END_FAR, MAZE_LEDGE_END_ROUNDED);
        cell_next.setLedgeEnd(left, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
        cell_next.setLedgeEnd(left, LEDGE_END_FAR, MAZE_LEDGE_END_ROUNDED);
        cell_current.setLedgeEnd(top, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);
        cell_next.setLedgeEnd(bottom, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);
        // We need the bottom right corner of the current cell and the top-left of the next one to remain as-is
        // Also the wall where the diagonal is off-limits
        cell_current.setLedgeNoUse(right, LEDGE_END_NEAR, true);
        cell_current.setLedgeNoUse(right, LEDGE_END_FAR, true);
        cell_current.setLedgeNoUse(bottom, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(top, LEDGE_END_FAR, true);
        // These edges are needed
        cell_current.setLedgeInUse(top, LEDGE_END_NEAR, true);
        cell_next.setLedgeInUse(bottom, LEDGE_END_NEAR, true);
      }
      // This side has only right edge and the other side has both north and south ledges
      else if ((cell_current.hasLedgeNormal(bottom) == false) &&
          (cell_current.hasLedgeNormal(top) == false) &&
          cell_current.hasLedgeNormal(right) &&
          cell_next.hasLedge(top) &&
          cell_next.hasLedge(bottom)) {
        cell_current.setWall(right, MAZE_WALL_TYPE_DOUBLE_HALF_ARC);
        cell_current.setLedgeEnd(right, LEDGE_END_FAR, MAZE_LEDGE_END_ROUNDED);
        cell_current.setLedgeEnd(right, LEDGE_END_NEAR, MAZE_LEDGE_END_ROUNDED);
        // This mode sacrifices the edges of the walls of the connection
        cell_current.setLedgeNoUse(right, LEDGE_END_FAR, true);
        cell_current.setLedgeNoUse(right, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_NEAR, true);
        // This mode needs the edges on the opposite side
        cell_next.setLedgeInUse(top, LEDGE_END_NEAR, true);
        cell_next.setLedgeInUse(bottom, LEDGE_END_FAR, true);
      }
      // This side has only top and bottom edges and the other side has only left edge
      else if (cell_current.hasLedgeNormal(bottom) &&
          cell_current.hasLedgeNormal(top) &&
          cell_next.hasLedge(left) &&
          (cell_next.hasLedge(top) == false) &&
          (cell_next.hasLedge(bottom) == false)) {
        cell_current.setWall(right, MAZE_WALL_TYPE_DOUBLE_HALF_ARC);
        cell_next.setLedgeEnd(left, LEDGE_END_FAR, MAZE_LEDGE_END_ROUNDED);
        cell_next.setLedgeEnd(left, LEDGE_END_NEAR, MAZE_LEDGE_END_ROUNDED);
        // This mode sacrifices the edges of the walls of the connection
        cell_current.setLedgeNoUse(right, LEDGE_END_FAR, true);
        cell_current.setLedgeNoUse(right, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_NEAR, true);
        // This mode needs the edges on this side
        cell_current.setLedgeInUse(top, LEDGE_END_FAR, true);
        cell_current.setLedgeInUse(bottom, LEDGE_END_NEAR, true);
      }
      // Both south walls have a ledge
      else if (cell_current.hasLedgeNormal(bottom) && cell_next.hasLedgeNormal(bottom))  {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIRECT_1));
        //cell_current.setWallDirection(right, DIRECTION_INVERSE);
        // This mode sacrifices the edges of the walls of the connection
        cell_current.setLedgeNoUse(right, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        // These edges are needed
        cell_current.setLedgeInUse(bottom, LEDGE_END_FAR, true);
        cell_next.setLedgeInUse(bottom, LEDGE_END_NEAR, true);
      }
      // Both north walls have a ledge
      else if (cell_current.hasLedgeNormal(top) && cell_next.hasLedgeNormal(top))  {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIRECT_1));
        cell_current.setWallDirection(right, DIRECTION_INVERSE);
        // This mode sacrifices the edges of the walls of the connection
        cell_current.setLedgeNoUse(right, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_NEAR, true);
        // These edges are needed
        cell_current.setLedgeInUse(top, LEDGE_END_NEAR, true);
        cell_next.setLedgeInUse(top, LEDGE_END_FAR, true);
      }
      // This side has right edge and the other side has south edge
      else if (cell_current.hasLedgeNormal(right) && cell_next.hasLedgeNormal(bottom))  {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIRECT_1));
        cell_current.setLedgeEnd(right, LEDGE_END_FAR, MAZE_LEDGE_END_ROUNDED);
        // This mode sacrifices no edges
        cell_current.setLedgeNoUse(right, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(bottom, LEDGE_END_FAR, true);
        // These edges are needed
        cell_current.setLedgeInUse(right, LEDGE_END_FAR, true);
        cell_next.setLedgeInUse(bottom, LEDGE_END_FAR, true);
      }
      // This side has right edge and the other side has north edge
      else if (cell_current.hasLedgeNormal(right) && cell_next.hasLedgeNormal(top))  {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIRECT_1));
        //cell_current.setWallDirection(right, DIRECTION_INVERSE);
        cell_current.setLedgeEnd(right, LEDGE_END_NEAR, MAZE_LEDGE_END_ROUNDED);
        // This mode sacrifices the edges it uses
        cell_current.setLedgeNoUse(right, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(top, LEDGE_END_NEAR, true);
        // These edges are needed
        cell_current.setLedgeInUse(right, LEDGE_END_NEAR, true);
        cell_next.setLedgeInUse(top, LEDGE_END_NEAR, true);
      }
      // This side has south edge the other side has left edge 
      else if (cell_current.hasLedgeNormal(bottom) && cell_next.hasLedgeNormal(left))  {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIRECT_1));
        //cell_current.setWallDirection(right, DIRECTION_INVERSE);
        cell_next.setLedgeEnd(left, LEDGE_END_NEAR, MAZE_LEDGE_END_ROUNDED);
        // This mode sacrifices the edges it uses
        cell_current.setLedgeNoUse(bottom, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_NEAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        // These edges are needed
        cell_current.setLedgeInUse(bottom, LEDGE_END_NEAR, true);
        cell_next.setLedgeInUse(left, LEDGE_END_NEAR, true);
      }
      // This side has north edge the other side has left edge 
      else if (cell_current.hasLedgeNormal(top) && cell_next.hasLedgeNormal(left))  {
        cell_current.setWall(right, cell_current.randomizeWall(CONDITION_DIRECT_1));
        cell_current.setWallDirection(right, DIRECTION_INVERSE);
        cell_next.setLedgeEnd(left, LEDGE_END_FAR, MAZE_LEDGE_END_ROUNDED);
        // This mode sacrifices the edges it uses
        cell_current.setLedgeNoUse(top, LEDGE_END_FAR, true);
        cell_next.setLedgeNoUse(left, LEDGE_END_FAR, true);
        // These edges are needed
        cell_current.setLedgeInUse(top, LEDGE_END_FAR, true);
        cell_next.setLedgeInUse(left, LEDGE_END_FAR, true);
      }
      cell_next.setWall(left, cell_current.getWall(right));
    }

    void unpackLedgeData(uint8_t input) {
      for (int ii = 0; ii < 4; ii++) {
        if (input & (0x01 << ii))
          this->setLedge(ii, LEDGE_NORMAL);
      }
      //this->setLedge(EAST, (input & 0x01) ? LEDGE_NORMAL : LEDGE_NONE);
      //this->setLedge(NORTH, (input & 0x02) ? LEDGE_NORMAL : LEDGE_NONE);
      //this->setLedge(WEST, (input & 0x04) ? LEDGE_NORMAL : LEDGE_NONE);
      //this->setLedge(SOUTH, (input & 0x08) ? LEDGE_NORMAL : LEDGE_NONE);
    }

    void unpackRampData(uint8_t input) {
      for (int ii = 0; ii < 4; ii++) {
        if (input & (0x01 << ii)) {
          this->setLedge(ii, LEDGE_RAMP);
#if defined(USE_LD) && defined(DEBUG) && 0
          std::cout << "Set ramp to position: " << ii << std::endl;
#endif
          if (input & 0x10 << ii) {
            this->setRampDirection(ii, DIRECTION_INVERSE);
          }
          else {
            this->setRampDirection(ii, DIRECTION_NORMAL);
          }
        }
      }
    }

    void setExitRampEnds() {
      for (int ii = 0; ii < 4; ii++) {
        if (this->hasLedgeRamp(ii)) {
          this->setLedge(ii, LEDGE_RAMP_EXIT); // Only set HERE because this is never needed anywhere else
          if (this->getRampDirection(ii) == DIRECTION_INVERSE) {
            this->setLedgeEnd(ii, LEDGE_END_NEAR, MAZE_LEDGE_END_ROUNDED);
          }
          else {
            this->setLedgeEnd(ii, LEDGE_END_FAR, MAZE_LEDGE_END_ROUNDED);
          }
        }
      }
    }

    void cleanLedges() {
      for (int ii = 0; ii < 4; ii++) {
        int right = (EAST + ii) % 4;
        int top = (NORTH + ii) % 4;
        if (this->hasLedgeNormal(right) && this->hasLedgeNormal(top)) {
          this->setLedgeEnd(right, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
          this->setLedgeEnd(top, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);

        }
      }
    }

    static void attemptRamp(MazeCell &lower, MazeCell &upper) {
      // 4 possible directions
      for (int ii = 0; ii < 4; ii++) {
        int right = (EAST + ii) % 4;
        int top = (NORTH + ii) % 4;
        int left = (WEST + ii) % 4;
        int bottom = (SOUTH + ii) % 4;
        if (lower.hasLedgeNormal(bottom) && upper.hasLedgeNormal(top)) {
          if (lower.getWall(left) == MAZE_WALL_TYPE_PLAIN) {
            // If the ends are not being used and we don't need the end we are removing, we can create the ramp.
            if((upper.getLedgeNoUse(left, LEDGE_END_NEAR) == false) &&
                (upper.getLedgeInUse(left, LEDGE_END_FAR) == false) && 
                (lower.getLedgeNoUse(left, LEDGE_END_FAR) == false) &&
                (lower.getLedgeInUse(left, LEDGE_END_NEAR) == false) &&
                (upper.hasLedge(left) == false)) {
#if defined(USE_LD) && defined(DEBUG) && 0
              printf("Ramp location found, bottom: %i, up: %i, at LEFT\n\tClose edge ledge type %i, Far edge %i\n", bottom, top, lower.getLedge(bottom), upper.getLedge(top));
#endif
              lower.setLedge(left, LEDGE_RAMP);
              lower.setRampDirection(left, DIRECTION_NORMAL);
              lower.setLedgeEnd(left, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
              lower.setLedgeEnd(left, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);
              lower.setLedgeEnd(bottom, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);
              upper.setLedgeEnd(top, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
              lower.setLedgeInUse(left, LEDGE_END_FAR, true);
              lower.setLedgeNoUse(left, LEDGE_END_FAR, true);
              //lower.setLedgeNoUse(left, LEDGE_END_NEAR, true);
              upper.setLedgeInUse(left, LEDGE_END_NEAR, true);
              upper.setLedgeNoUse(left, LEDGE_END_NEAR, true);
              //							lower.setLedgeInUse(left, LEDGE_END_NEAR, true);
              //							lower.setLedgeNoUse(left, LEDGE_END_NEAR, true);
              //							lower.setLedgeNoUse(left, LEDGE_END_FAR, true);
              //							upper.setLedgeInUse(left, LEDGE_END_FAR, true);
              //							upper.setLedgeNoUse(left, LEDGE_END_FAR, true);
            }
          }
          // If the ends are not being used and we don't need the end we are removing, we can create the ramp.
          if (lower.getWall(right) == MAZE_WALL_TYPE_PLAIN) {
            if ((upper.getLedgeNoUse(right, LEDGE_END_FAR) == false) &&
                (upper.getLedgeInUse(right, LEDGE_END_NEAR) == false) &&
                (lower.getLedgeNoUse(right, LEDGE_END_NEAR) == false) &&
                (lower.getLedgeInUse(right, LEDGE_END_FAR) == false) &&
                (upper.hasLedge(right) == false)) {
#if defined(USE_LD) && defined(DEBUG) && 0
              printf("Ramp location found, bottom: %i, up: %i, at RIGHT\n\tClose edge ledge type %i, Far edge %i\n", bottom, top, lower.getLedge(bottom), upper.getLedge(top));
#endif
              lower.setLedge(right, LEDGE_RAMP);
              lower.setRampDirection(right, DIRECTION_INVERSE);
              lower.setLedgeEnd(right, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
              lower.setLedgeEnd(right, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);
              lower.setLedgeEnd(bottom, LEDGE_END_NEAR, MAZE_LEDGE_END_WEDGE);
              upper.setLedgeEnd(top, LEDGE_END_FAR, MAZE_LEDGE_END_WEDGE);
              lower.setLedgeInUse(right, LEDGE_END_NEAR, true);
              lower.setLedgeNoUse(right, LEDGE_END_NEAR, true);
              //lower.setLedgeNoUse(right, LEDGE_END_FAR, true);
              upper.setLedgeInUse(right, LEDGE_END_FAR, true);
              upper.setLedgeNoUse(right, LEDGE_END_FAR, true);
              //							lower.setLedgeInUse(right, LEDGE_END_FAR, true);
              //							lower.setLedgeNoUse(right, LEDGE_END_NEAR, true);
              //							lower.setLedgeNoUse(right, LEDGE_END_FAR, true);
              //							upper.setLedgeInUse(right, LEDGE_END_NEAR, true);
              //							upper.setLedgeNoUse(right, LEDGE_END_NEAR, true);
            }
          }
        }
      }
    }

    /// Constructor.
    ///
    /// \param newledges Ledges to create.
    MazeCell()
    {
      ledge[EAST] = ledge[NORTH] = ledge[WEST] = ledge[SOUTH] = LEDGE_NONE;
      wall[EAST] = wall[NORTH] = wall[WEST] = wall[SOUTH] = MAZE_WALL_TYPE_PLAIN;
      ledge_in_use[EAST][LEDGE_END_NEAR] = ledge_in_use[NORTH][LEDGE_END_NEAR] = ledge_in_use[WEST][LEDGE_END_NEAR] = ledge_in_use[SOUTH][LEDGE_END_NEAR] = false;
      ledge_in_use[EAST][LEDGE_END_FAR] = ledge_in_use[NORTH][LEDGE_END_FAR] = ledge_in_use[WEST][LEDGE_END_FAR] = ledge_in_use[SOUTH][LEDGE_END_FAR] = false;
      ledge_no_use[EAST][LEDGE_END_NEAR] = ledge_no_use[NORTH][LEDGE_END_NEAR] = ledge_no_use[WEST][LEDGE_END_NEAR] = ledge_no_use[SOUTH][LEDGE_END_NEAR] = false;
      ledge_no_use[EAST][LEDGE_END_FAR] = ledge_no_use[NORTH][LEDGE_END_FAR] = ledge_no_use[WEST][LEDGE_END_FAR] = ledge_no_use[SOUTH][LEDGE_END_FAR] = false;
      ledge_end[EAST][LEDGE_END_NEAR] = MAZE_LEDGE_END_NONE;
      ledge_end[EAST][LEDGE_END_FAR] = MAZE_LEDGE_END_NONE;
      ledge_end[NORTH][LEDGE_END_NEAR] = MAZE_LEDGE_END_NONE;
      ledge_end[NORTH][LEDGE_END_FAR] = MAZE_LEDGE_END_NONE;
      ledge_end[WEST][LEDGE_END_NEAR] = MAZE_LEDGE_END_NONE;
      ledge_end[WEST][LEDGE_END_FAR] = MAZE_LEDGE_END_NONE;
      ledge_end[SOUTH][LEDGE_END_NEAR] = MAZE_LEDGE_END_NONE;
      ledge_end[SOUTH][LEDGE_END_FAR] = MAZE_LEDGE_END_NONE;
      ledge_direction[EAST] = DIRECTION_NORMAL;
      ledge_direction[NORTH] = DIRECTION_NORMAL;
      ledge_direction[WEST] = DIRECTION_NORMAL;
      ledge_direction[SOUTH] = DIRECTION_NORMAL;
      wall_direction[EAST] = DIRECTION_NORMAL;
      wall_direction[NORTH] = DIRECTION_NORMAL;
      wall_direction[WEST] = DIRECTION_NORMAL;
      wall_direction[SOUTH] = DIRECTION_NORMAL;
    }

    /// Set desired ledge
    ///
    /// \param newledges Ledges to create.
    void setLedge(int index, uint8_t type)
    {
      ledge[index] = type;
    }

    /// Set wall
    ///
    /// \param direction Direction to set.
    /// \param type Type to set into.
    void setWall(int direction, uint8_t type)
    {
      wall[direction] = type;
    }

    /// Get wall
    ///
    /// \param direction Direction to set.
    uint8_t getWall(int direction)
    {
      return wall[direction];
    }

    /// Set wall direction
    ///
    /// \param direction Direction to set.
    /// \param direction Direction to set the direction into.
    void setWallDirection(int direction, bool value)
    {
      wall_direction[direction] = value;
    }

    /// Get wall direction
    ///
    /// \param direction Direction to get direction from.
    bool getWallDirection(int direction)
    {
      return wall_direction[direction];
    }

    /// Set ledge direction
    ///
    /// \param direction Direction to set.
    /// \param direction Direction to set the direction into.
    void setLedgeDirection(int direction, bool value)
    {
      ledge_direction[direction] = value;
    }

    /// Get ledge direction
    ///
    /// \param direction Direction to get direction of.
    bool getLedgeDirection(int direction)
    {
      return ledge_direction[direction];
    }

    /// Set ramp direction
    ///
    /// \param direction Direction to set.
    /// \param direction Direction to set the direction into.
    void setRampDirection(int direction, bool value)
    {
      ramp_direction[direction] = value;
    }

    /// Get ramp direction
    ///
    /// \param direction Direction to set.
    /// \param direction Direction to set the direction into.
    bool getRampDirection(int direction)
    {
      return ramp_direction[direction];
    }

    /// Set ledge end type
    ///
    /// \param direction Direction to set.
    /// \param edge Near or far edge.
    /// \param type Type to set the end into.
    void setLedgeEnd(int direction, int edge, uint8_t type)
    {
      //std::cout << "Setting ledge end " << direction << " edge " << edge << " to " << type << std::endl;
      ledge_end[direction][edge] = type;
    }

    /// Get ledge end type
    ///
    /// \param direction Direction to get.
    /// \param edge Near or far edge.
    uint8_t getLedgeEnd(int direction, int end)
    {
      return ledge_end[direction][end];
    }

    /// Get ledge type.
    ///
    /// \return Ledge type.
    uint8_t getLedge(int direction) const
    {
      return ledge[direction];
    }

    /// Tell if ledge exists.
    ///
    /// \return True if yes, false if no.
    bool hasLedge(int direction) const
    {
      return ledge[direction] != LEDGE_NONE;
    }

    /// Tell if ledge exists and is normal.
    ///
    /// \return True if yes, false if no.
    bool hasLedgeNormal(int direction) const
    {
      return ledge[direction] == LEDGE_NORMAL;
    }

    /// Tell if ledge exists and is diagonal.
    ///
    /// \return True if yes, false if no.
    bool hasLedgeDiagonal(int direction) const
    {
      return ledge[direction] == LEDGE_DIAGONAL;
    }

    /// Tell if ramp exists.
    ///
    /// \return True if yes, false if no.
    bool hasLedgeRamp(int direction) const
    {
      return ledge[direction] == LEDGE_RAMP;
    }

    /// Tell if an exit ramp exists.
    ///
    /// \return True if yes, false if no.
    bool hasLedgeRampExit(int direction) const
    {
      return ledge[direction] == LEDGE_RAMP_EXIT;
    }

    /// Tell if ledge is being used by current maze.
    ///
    /// \return True if yes, false if no.
    bool getLedgeInUse(int direction, int end) const
    {
      return ledge_in_use[direction][end];
    }

    /// Set ledge usage status
    ///
    /// \return True if yes, false if no.
    void setLedgeInUse(int direction, int end, bool used)
    {
      ledge_in_use[direction][end] = used;
    }

    /// Tell if ledge end can be used in current maze.
    ///
    /// \return True if yes, false if no.
    bool getLedgeNoUse(int direction, int end) const
    {
      return ledge_no_use[direction][end];
    }

    /// Set ledge usage status
    ///
    /// \return True if yes, false if no.
    void setLedgeNoUse(int direction, int end, bool used)
    {
      ledge_no_use[direction][end] = used;
    }

    /// Get the type that the ledge database uses for this ledge
    ///
    /// \param direction Direction to check.
    /// \return Ledge database index.
    int getLedgeDatabaseType(int direction) {
      if (this->hasLedgeNormal(direction) || this->hasLedgeDiagonal(direction))
      {
        return LEDGE_DATABASE_BODY_1;
      }
      else if(this->hasLedgeRamp(direction))
      {
        if(this->getRampDirection(direction) == DIRECTION_NORMAL)
        {
          return LEDGE_DATABASE_RAMP_CCW;
        }
        else // DIRECTION_INVERSE
        {
            return LEDGE_DATABASE_RAMP_CW;
        }
      }
      else if (this->hasLedgeRampExit(direction))
      {
        if(this->getRampDirection(direction) == DIRECTION_NORMAL)
        {
          return LEDGE_DATABASE_RAMP_EXIT_CCW;
        }
        else // DIRECTION_INVERSE
        {
          return LEDGE_DATABASE_RAMP_EXIT_CW;
        }
      }
#if defined(USE_LD)
      {
        std::ostringstream sstr;
        sstr << "unknown database type at " << direction;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return -1;
    }

    /// Get the type that the ledge database uses for this ledge end
    ///
    /// \param direction Direction to check.
    /// \return Ledge database index.
    int getLedgeEndDatabaseType(int direction, int end) {
      //std::cout << "Trying to return ledge end " << dir << " edge " << edge << ". It is: " << ledge_end[dir][edge] << std::endl;

      switch (ledge_end[direction][end]) {
        case MAZE_LEDGE_END_NONE:
          return LEDGE_DATABASE_NONE;
        case MAZE_LEDGE_END_ROUNDED:
          if (end == LEDGE_END_FAR) {
            return LEDGE_DATABASE_ROUNDED_CW;
          }
          else {
            return LEDGE_DATABASE_ROUNDED_CCW;
          }
        case MAZE_LEDGE_END_WEDGE:
          if (end == LEDGE_END_FAR) {
            return LEDGE_DATABASE_WEDGE_CW;
          }
          else {
            return LEDGE_DATABASE_WEDGE_CCW;
          }
        default:
#if defined(USE_LD)
          {
            std::ostringstream sstr;
            sstr << "unknown database type at " << direction << " edge " << end;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
          }
#endif
          return -1;
      }
    }
};

/// Smart pointer type.
typedef uptr<MazeCell> MazeCellUptr;

//######################################
// Maze Wall and Ledge classes #########
//######################################

class MazeLedge
{
  private:
    MeshUptr m_mesh;

  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to use.
    /// \param type Type of wall to create.
    MazeLedge(GeometryBuffer &buf, int type)
    {
      construct(buf, type);
    }

    void construct(GeometryBuffer &buf, int type)
    {
      LogicalMesh mesh(COLOR_MAZELEDGE);
      vec3 p1(0, 0, 0);
      vec3 p2(-MAZE_LEDGE_WIDTH*2.0f + MAZE_CELL_WIDTH, 0, 0);
      vec3 p3(MAZE_LEDGE_WIDTH, 0, 0);
      switch (type) {
        case LEDGE_DATABASE_BODY_1:
        case LEDGE_DATABASE_BODY_2:
        case LEDGE_DATABASE_BODY_3:
        case LEDGE_DATABASE_BODY_4:
        case LEDGE_DATABASE_BODY_5:
        case LEDGE_DATABASE_BODY_6:
        case LEDGE_DATABASE_BODY_7:
          mesh_generate_ledge(mesh, p1, p1 + vec3(diagonal_length[type], 0, 0), false, DIRECTION_NORMAL, 0.0f, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_RAMP_CW:
          mesh_generate_ledge(mesh, p1, p2, true, DIRECTION_NORMAL, MAZE_CELL_HEIGHT, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_RAMP_CCW:
          mesh_generate_ledge(mesh, p1, p2, true, DIRECTION_INVERSE, MAZE_CELL_HEIGHT, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_RAMP_EXIT_CW:
          mesh_generate_ledge(mesh, p1, p2, true, DIRECTION_NORMAL, MAZE_CELL_HEIGHT - MAZE_LEDGE_HEIGHT, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_RAMP_EXIT_CCW:
          mesh_generate_ledge(mesh, p1, p2, true, DIRECTION_INVERSE, MAZE_CELL_HEIGHT - MAZE_LEDGE_HEIGHT, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_NONE:
          mesh_generate_ledge_end(mesh, p1, p3, DIRECTION_NORMAL, MAZE_LEDGE_END_NONE, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_ROUNDED_CW:
          mesh_generate_ledge_end(mesh, p1, p3, DIRECTION_NORMAL, MAZE_LEDGE_END_ROUNDED, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_ROUNDED_CCW:
          mesh_generate_ledge_end(mesh, p1, p3, DIRECTION_INVERSE, MAZE_LEDGE_END_ROUNDED, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_WEDGE_CW:
          mesh_generate_ledge_end(mesh, p1, p3, DIRECTION_NORMAL, MAZE_LEDGE_END_WEDGE, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        case LEDGE_DATABASE_WEDGE_CCW:
          mesh_generate_ledge_end(mesh, p1, p3, DIRECTION_INVERSE, MAZE_LEDGE_END_WEDGE, MAZE_LEDGE_WIDTH, MAZE_LEDGE_HEIGHT, MAZE_LEDGE_HEIGHT_FRACTION);
          break;
        default:
#if defined(USE_LD)
          {
            std::ostringstream sstr;
            sstr << "Unknown database type: " << type;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
          }
#endif
          break;
      }

      m_mesh = mesh.insert(buf, 1);
    }

    const Mesh& getMesh() const
    {
      return *m_mesh;
    }
};

class MazeWall
{
  private:
    MeshUptr m_mesh;

  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to use.
    /// \param type Type of wall to create.
    MazeWall(GeometryBuffer &buf, int type)
    {
      construct(buf, type);
    }

    void construct(GeometryBuffer &buf, int type)
    {
      LogicalMesh mesh(COLOR_MAZEWALL);
      mesh_generate_maze_wall_segment(mesh, vec3(0, 0, 0), type);
      m_mesh = mesh.insert(buf, 1);
    }

    const Mesh& getMesh() const
    {
      return *m_mesh;
    }
};

//######################################
// Maze resources class ################
//######################################

/// Convenience typedef.
typedef uptr<MazeLedge> MazeLedgeUptr;
typedef uptr<MazeWall> MazeWallUptr;

/// Maze resources class.
class MazeResources
{
  private:
    MazeWallUptr walls[MAZE_WALL_TYPES];
    MazeLedgeUptr ledges[MAZE_LEDGE_TYPES];

  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to use.
    MazeResources(GeometryBuffer &buf)
    {
      for (int ii = 0; ii < MAZE_WALL_TYPES; ii++)
      {
        walls[ii] = new MazeWall(buf, ii);
      }
      for (int ii = 0; ii < MAZE_LEDGE_TYPES; ii++)
      {
        ledges[ii] = new MazeLedge(buf, ii);
      }
    }

    /// Insert a copies of ledge segements into cell location.
    ///
    /// \param db Object database to use.
    /// \param input MazeCell to take ledges from.
    /// \param origin Origin vector of cell.
    /// \param tex Texture to use.
    void InsertLedges(ObjectDatabase &db, MazeCell &input, const vec3 &origin, const Texture *tex) const
    {
      // Cell origin is at the centre of the walls surrounding it
      float padding = MAZE_CELL_WALL_THICKNESS / 2.0f;
      // Get inside the cell instead of inside the wall

      vec3 rot(0, 0, 0);

      // Cycle through directions
      for (int ii = 0; ii < 4; ii++) {
        // Does this have an appropriate ledge?
        if (input.hasLedge(ii)) {

          // Initialization of translation, can't do it earlier beause the vector must only exist in this context
          vec3 tx = origin + vec3(padding, 0, -padding);

          // Set offsets
          vec3 near_end_offset(0, 0, 0);
          vec3 far_end_offset(0, 0, 0);
          if (input.hasLedgeRamp(ii)) {
            if (input.getRampDirection(ii) == DIRECTION_NORMAL) {
              near_end_offset += vec3(0, MAZE_CELL_HEIGHT, 0);
            }
            else {
              far_end_offset += vec3(0, MAZE_CELL_HEIGHT, 0);
            }
          }

          if (input.hasLedgeRampExit(ii)) {
            if (input.getRampDirection(ii) == DIRECTION_NORMAL) {
              near_end_offset += vec3(0, MAZE_CELL_HEIGHT - MAZE_LEDGE_HEIGHT, 0);
            }
            else {
              far_end_offset += vec3(0, MAZE_CELL_HEIGHT - MAZE_LEDGE_HEIGHT, 0);
            }
          }

          // Get ledge databasetype
          int ledgedatabasetype = input.getLedgeDatabaseType(ii);
#if defined(USE_LD) && defined(DEBUG) && 0
          std::cout << "Ledge type to insert: " << ledgedatabasetype << " Actual: " << input.getLedge(ii);
#endif
          float ledge_length = diagonal_length[0]; // Base value

          // We might have a diagonal wall. If so, the length of the ledge is lower than usual
          if (input.hasLedgeDiagonal(ii)) {
            ledgedatabasetype += input.getWall(ii); // Ugly hax, but the ledges are in the same order in the database as they are in the original ledge type list.
            ledge_length = diagonal_length[input.getWall(ii)];  // This is because I got to know you are not allowed to scale objects after the whole infrastructure had already been done.
          }
#if defined(USE_LD) && defined(DEBUG) && 0
          std::cout << " After Transformation: " << ledgedatabasetype << std::endl;
#endif
          //sca = vec3(diagonal_length[input.getWall(ii)] - MAZE_LEDGE_WIDTH*2.0f, 1, 1);
          //else
          //sca = vec3(MAZE_CELL_WIDTH - MAZE_LEDGE_WIDTH*2.0f, 1, 1);

          switch(ii)
          {
            case EAST:
              rot = vec3(static_cast<float>(M_PI) / 2.0f, 0, 0);
              if (input.getLedgeDirection(ii) == DIRECTION_NORMAL)
                tx += vec3(MAZE_CELL_WIDTH, 0, -MAZE_LEDGE_WIDTH); // OK
              else
                tx += vec3(MAZE_CELL_WIDTH, 0, -MAZE_CELL_WIDTH + ledge_length + MAZE_LEDGE_WIDTH); // OK
              near_end_offset += vec3(0, 0, MAZE_LEDGE_WIDTH);
              far_end_offset += vec3(0, 0, -ledge_length);
              break;

            case NORTH:
              rot = vec3(static_cast<float>(M_PI), 0, 0);
              if (input.getLedgeDirection(ii) == DIRECTION_NORMAL)
                tx += vec3(MAZE_CELL_WIDTH - MAZE_LEDGE_WIDTH, 0, -MAZE_CELL_WIDTH); // OK
              else
                tx += vec3(ledge_length + MAZE_LEDGE_WIDTH, 0, -MAZE_CELL_WIDTH); // OK
              near_end_offset += vec3(MAZE_LEDGE_WIDTH, 0, 0);
              far_end_offset += vec3(-ledge_length, 0, 0);
              break;

            case WEST:
              rot = vec3(3.0f*static_cast<float>(M_PI) / 2.0f, 0, 0);
              if (input.getLedgeDirection(ii) == DIRECTION_NORMAL)
                tx += vec3(0, 0, -ledge_length - MAZE_LEDGE_WIDTH); // OK
              else
                tx += vec3(0, 0, -MAZE_CELL_WIDTH + MAZE_LEDGE_WIDTH); // OK
              near_end_offset += vec3(0, 0, -MAZE_LEDGE_WIDTH);
              far_end_offset += vec3(0, 0, ledge_length);
              break;

            case SOUTH:
              rot = vec3(0, 0, 0);
              if (input.getLedgeDirection(ii) == DIRECTION_NORMAL)
                tx += vec3(+MAZE_CELL_WIDTH - ledge_length - MAZE_LEDGE_WIDTH, 0, 0);
              else
                tx += vec3(MAZE_LEDGE_WIDTH, 0, 0);
              near_end_offset += vec3(-MAZE_LEDGE_WIDTH, 0, 0);
              far_end_offset += vec3(ledge_length, 0, 0);
              break;

            default:
#if defined(USE_LD)
              {
                std::ostringstream sstr;
                sstr << "unknown direction: " << ii;
                BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
              }
#endif
              break;
          }

          //printf("tx Ledge: [%f, %f, %f]\n", tx.x(), tx.y(), tx.z());

          //std::cout << "Near end: " << input.getLedgeEndDatabaseType(ii, LEDGE_END_NEAR) << std::endl;
          //std::cout << "Far end: " << input.getLedgeEndDatabaseType(ii, LEDGE_END_FAR) << std::endl;

          // Add actual ledge
          //db.addObject(ledges[LEDGE_BODY]->getMesh(), mat4::rotation_euler(rot.x(), rot.y(), rot.z(), tx), tex);
          //db.addObject(ledges[ledgedatabasetype]->getMesh(), mat4::transformation_euler(sca, rot, tx), tex);
          db.addObject(ledges[ledgedatabasetype]->getMesh(), mat4::rotation_euler(rot.x(), rot.y(), rot.z(), tx), tex);
          // Note that the following is INDEED inverted with respect to everything else since it seems that I've thought in inverse direction compared to earlier
          db.addObject(ledges[input.getLedgeEndDatabaseType(ii, LEDGE_END_FAR)]->getMesh(), mat4::rotation_euler(rot.x(), rot.y(), rot.z(), tx + near_end_offset), tex);
          db.addObject(ledges[input.getLedgeEndDatabaseType(ii, LEDGE_END_NEAR)]->getMesh(), mat4::rotation_euler(rot.x(), rot.y(), rot.z(), tx + far_end_offset), tex);
        }
      }
    }

    /// Insert a wall into location.
    ///
    /// \param db Object database to use.
    /// \param input MazeCell to take ledges from.
    /// \param direction Direction of wall to select.
    /// \param rot Rotation vector.
    /// \param tx Translation vector.
    /// \param tex Texture to use.
    void InsertWall(ObjectDatabase &db, int type, int direction, const vec3 &rot, const vec3 &tx,
        const Texture *tex) const
    {
      //printf("tx Wall: [%f, %f, %f]\n", tx.x(), tx.y(), tx.z());
      //int type = input.getWall(direction);
			float invert = static_cast<float>(M_PI)*static_cast<float>(direction); //input.getWallDirection(direction);
      db.addObject(walls[type]->getMesh(), mat4::rotation_euler(rot.x() + invert, rot.y(), rot.z(), tx), tex);
    }

  public:
    /// Accessor.
    ///
    /// \param idx Index of ledge.
    /// \return Ledge mesh by index.
    const Mesh& getLedge(unsigned idx) const
    {
#if defined(USE_LD)
      if(MAZE_LEDGE_TYPES <= idx)
      {
        std::ostringstream sstr;
        sstr << "trying to access ledge index " << idx << " of " << MAZE_LEDGE_TYPES;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return ledges[idx]->getMesh();
    }

    /// Accessor.
    ///
    /// \param idx Index of wall.
    /// \return Wall mesh by index.
    const Mesh& getWall(unsigned idx) const
    {
#if defined(USE_LD)
      if(MAZE_WALL_TYPES <= idx)
      {
        std::ostringstream sstr;
        sstr << "trying to access wall index " << idx << " of " << MAZE_WALL_TYPES;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return walls[idx]->getMesh();
    }
};

/// Convenience typedef.
typedef uptr<MazeResources> MazeResourcesUptr;

/// Maze class.
class Maze
{
  private:
    /// Cell array.
    seq<MazeCell> m_cells;

    /// Width (and depth).
    unsigned m_w;

    /// Height.
    unsigned m_h;

    /// Maze mesh.
    MeshUptr m_mesh;

  public:
    /// Constructor.
    ///
    /// \param width How wide and how deep the maze is.
    /// \param height How high the maze is.
    Maze(unsigned width, unsigned height, const MazeResources &mazeresources, ObjectDatabase &db,
        GeometryBuffer &buf, const Texture *tex, const vec3 &maze_start, const uint8_t *ledge_data = NULL,
        const uint8_t *ramp_data = NULL) :
      m_w(width),
      m_h(height)
    {
      for(unsigned ii = 0; ((width * height * width) > ii); ++ii)
      {
        m_cells.emplace_back();
      }

      construct(mazeresources, db, buf, tex, maze_start, ledge_data, ramp_data);
    }

  public:
    /// Get a mazecell
    ///
    /// \param x X.
    /// \param y Y.
    /// \param z Z.
    MazeCell& getCell(unsigned x, unsigned y, unsigned z)
    {
      return m_cells[y*m_w*m_w + z*m_w + x];
    }

    /// Get width dimension in cells
    unsigned getWidth() const
    {
      return m_w;
    }

    /// Get height dimension in cells
    unsigned getHeight() const
    {
      return m_h;
    }

    /// GEnerate maze.
    void construct(const MazeResources &mazeresources, ObjectDatabase &db, GeometryBuffer &buf,
        const Texture *tex, const vec3 &maze_start, const uint8_t *ledge_data = NULL,
        const uint8_t *ramp_data = NULL)
    {
      LogicalMesh msh;

      unsigned maze_width = getWidth();
      unsigned maze_height = getHeight();

      // First randomizing run, set ledges
      for(unsigned ii = 0; ii < maze_width; ii++)
      {
        for(unsigned jj = 0; jj < maze_height; jj++)
        {
          for(unsigned kk = 0; kk < maze_width; kk++)
          {
            // The ledge structure of the maze can be given as a table.
            // If it is not, randomize it.
            if (ledge_data == NULL)
            {
              getCell(ii, jj, kk).setLedge(EAST, static_cast<uint8_t>(urand(2))); // No ramps yet
              getCell(ii, jj, kk).setLedge(WEST, static_cast<uint8_t>(urand(2)));
              getCell(ii, jj, kk).setLedge(NORTH, static_cast<uint8_t>(urand(2)));
              getCell(ii, jj, kk).setLedge(SOUTH, static_cast<uint8_t>(urand(2)));
            }
            // Unpack data from table 
            else
            {
              getCell(ii, jj, kk).unpackLedgeData(ledge_data[jj*maze_width*maze_width + kk*maze_width + ii]);
#if defined(USE_LD) && defined(DEBUG) && 0
              std::cout << "After unpack ledges are [" << getCell(ii, jj, kk).getLedge(EAST) << ", " << getCell(ii, jj, kk).getLedge(NORTH) << ", " << getCell(ii, jj, kk).getLedge(WEST) << ", " << getCell(ii, jj, kk).getLedge(SOUTH) << "]" << std::endl;
#endif
            }
          }
        }
      }

      // Manual ledges
      /*getCell(0, 0, 0).setLedge(WEST, LEDGE_NORMAL);
        getCell(0, 0, 0).setLedge(EAST, LEDGE_NORMAL);
        getCell(0, 0, 0).setLedge(NORTH, LEDGE_NORMAL);*/
      /*getCell(0, 0, 0).setLedge(SOUTH, LEDGE_NORMAL);
        getCell(1, 0, 0).setLedge(NORTH, LEDGE_NORMAL);
        getCell(0, 0, 1).setLedge(NORTH, LEDGE_NORMAL);
        getCell(1, 0, 1).setLedge(SOUTH, LEDGE_NORMAL);*/
      /*getCell(0, 0, 0).setLedge(SOUTH, LEDGE_NORMAL);
        getCell(1, 0, 0).setLedge(SOUTH, LEDGE_NORMAL);
        getCell(0, 1, 0).setLedge(NORTH, LEDGE_NORMAL);
        getCell(1, 1, 0).setLedge(NORTH, LEDGE_NORMAL);*/

      // Second randomizing run, set walls based on ledges
      for(unsigned ii = 0; ii < maze_width; ii++)
      {
        for(unsigned jj = 0; jj < maze_height; jj++)
        {
          for(unsigned kk = 0; kk < maze_width; kk++)
          {
            // Eastern walls
            if (ii < maze_width - 1)
            {
              getCell(ii, jj, kk).setWallsBasedOnLedgeInformation(getCell(ii, jj, kk), getCell(ii + 1, jj, kk), EAST, NORTH, WEST, SOUTH);
            }
            // Northern walls
            if (kk < maze_width - 1)
            {
              getCell(ii, jj, kk).setWallsBasedOnLedgeInformation(getCell(ii, jj, kk), getCell(ii, jj, kk + 1), NORTH, WEST, SOUTH, EAST);
            }
#if defined(USE_LD) && defined(DEBUG) && 0
            std::cout << "After setting walls, ledges are [" << getCell(ii, jj, kk).getLedge(EAST) << ", " << getCell(ii, jj, kk).getLedge(NORTH) << ", " << getCell(ii, jj, kk).getLedge(WEST) << ", " << getCell(ii, jj, kk).getLedge(SOUTH) << "]" << std::endl;
#endif
          }
        }
      }

      // Third optimization run, clean up ledge ends based on other ledges available and generate ramps on positions which allow them
      for(unsigned ii = 0; ii < maze_width; ii++)
      {
        for(unsigned jj = 0; jj < maze_height; jj++)
        {
          for(unsigned kk = 0; kk < maze_width; kk++)
          {
            // Clean ledge corners
            getCell(ii, jj, kk).cleanLedges();

            // Try to create a ramps between cells
            if (jj < maze_height - 1)
            {
#if defined(USE_LD) && defined(DEBUG) && 0
              printf("Attempting ramps @ cell [%i %i %i]\n", ii, jj, kk);
#endif
              getCell(ii, jj, kk).attemptRamp(getCell(ii, jj, kk), getCell(ii, jj + 1, kk));
            }
          }
        }
      }
      //getCell(1, 6, 0).attemptRamp(getCell(1, 6, 0), getCell(1, 7, 0));

      // Fourth round, draw the ledges based on wall information
      // Also draw the walls themselves
      for(unsigned ii = 0; ii < maze_width; ii++)
      {
        for(unsigned jj = 0; jj < maze_height; jj++)
        {
          for(unsigned kk = 0; kk < maze_width; kk++)
          {
            float cell_left = maze_start.x() + static_cast<float>(ii)*MAZE_CELL_WIDTH_TOTAL;
            float cell_bottom = maze_start.y() + static_cast<float>(jj)*MAZE_CELL_HEIGHT;
            float cell_back = maze_start.z() - static_cast<float>(kk)*MAZE_CELL_WIDTH_TOTAL;

            vec3 cell_origin = vec3(cell_left, cell_bottom, cell_back);

            // If there are forced rams in this cell, put them in
            if (ramp_data != NULL) {
              getCell(ii, jj, kk).unpackRampData(ramp_data[jj*maze_width*maze_width + kk*maze_width + ii]);
            }

            // Ramps are not automatically generated for the final floor.
            if (jj == maze_height - 1)
            {
              getCell(ii, jj, kk).setExitRampEnds();
            }

            // Draw Ledges
#if defined(USE_LD) && defined(DEBUG) && 0
            std::cout << "Attempting to insert ledges at [" << ii << ", " << jj << ", " << kk << "]" << std::endl;
#endif
            mazeresources.InsertLedges(db, getCell(ii, jj, kk), cell_origin, tex);

            // Add walls, these are always included
            if (ii < maze_width - 1)
							mazeresources.InsertWall(db, getCell(ii, jj, kk).getWall(EAST), getCell(ii, jj, kk).getWallDirection(EAST), vec3(3.0f*static_cast<float>(M_PI) / 2.0f, 0, 0), cell_origin + vec3(1.0f, 0, -0.5f)*MAZE_CELL_WIDTH_TOTAL, tex);

            if (kk<maze_width - 1)
              mazeresources.InsertWall(db, getCell(ii, jj, kk).getWall(NORTH), getCell(ii, jj, kk).getWallDirection(NORTH), vec3(0, 0, 0), cell_origin + vec3(0.5f, 0, -1)*MAZE_CELL_WIDTH_TOTAL, tex);

            if ((ii < maze_width - 1) && (kk<maze_width - 1))
              // Add pillar to make sure that the maze has no holes
              // Creates SOME overdraw, but should be minor.
              mazeresources.InsertWall(db, MAZE_WALL_TYPE_CENTRAL_PILLAR, DIRECTION_NORMAL, vec3(0, 0, 0), cell_origin + vec3(0.5f*MAZE_CELL_WIDTH_TOTAL + MAZE_CELL_WALL_THICKNESS, 0, -1.0f*MAZE_CELL_WIDTH_TOTAL), tex);

            // Build roof on last iteration
            if (jj == maze_height - 1) {
              // East
							mazeresources.InsertWall(db, MAZE_WALL_TYPE_ROOF, getCell(ii, jj, kk).getWallDirection(EAST), vec3(static_cast<float>(M_PI) / 2.0f, 0, 0), cell_origin + vec3(1.0f, 0, -0.5f)*MAZE_CELL_WIDTH_TOTAL, tex);
              // North
              mazeresources.InsertWall(db, MAZE_WALL_TYPE_ROOF, getCell(ii, jj, kk).getWallDirection(NORTH), vec3(0, 0, 0), cell_origin + vec3(0.5f, 0, -1)*MAZE_CELL_WIDTH_TOTAL, tex);
            }

            // South wall at kk==0
            //if (kk == 0)
            //mazeresources.InsertWall(db, getCell(ii, jj, kk), SOUTH, vec3(0, 0, 0), cell_origin + vec3(0.5f, 0, 0)*MAZE_CELL_WIDTH_TOTAL, tex);

            // West wall at ii==0
            //if (ii == 0)
            //mazeresources.InsertWall(db, getCell(ii, jj, kk), WEST, vec3((float)M_PI / 2.0f, 0, 0), cell_origin + vec3(0, 0, -0.5f)*MAZE_CELL_WIDTH_TOTAL, tex);
          }
        }
      }

      m_mesh = msh.insert(buf, 23);

      // Still need to add generated mesh.
      db.addObject(*m_mesh, mat4::identity());
    }
};

/// Convenience typedef.
typedef uptr<Maze> MazeUptr;

#endif
