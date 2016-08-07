#ifndef VERBATIM_OBJECT_DATABASE_HPP
#define VERBATIM_OBJECT_DATABASE_HPP

#include "verbatim_object_group.hpp"

/// Object database.
///
/// Listing of all objects in the whole world.
class ObjectDatabase
{
  private:
    /// All objects.
    seq<Object> m_objects;

    /// Complete meshes.
    seq<ObjectGroup*> m_groups;

  public:
    /// Empty constructor.
    ObjectDatabase() { }

    /// Empty destructor.
    ~ObjectDatabase()
    {
#if defined(USE_LD)
      for(ObjectGroup *vv : m_groups)
      {
        delete vv;
      }
#endif
    }

  public:
    /// Add an object.
    ///
    /// Note that if mesh has multiple objects, all of them will be added.
    ///
    /// \param msh Mesh to add.
    /// \param transform Transform to use.
    /// \param tex Texture to use.
    void addObject(const Mesh &msh, const mat4 &transform, const Texture *tex = NULL)
    {
      ObjectGroup *grp = NULL;
      unsigned block_count = msh.getBlockCount();

      if(0 < block_count)
      {
        grp = new ObjectGroup(block_count);
        m_groups.push_back(grp);
      }

      for(unsigned ii = 0, ee = msh.getBlockCount(); (ii < ee); ++ii)
      {
        m_objects.emplace_back(msh.getBlock(ii), transform, tex, grp);
      }
    }

    /// Tell if a bounding volume conflicts with existing objects.
    ///
    /// Checks only on XZ-plane.
    ///
    /// \param volume Volume to insert.
    /// \return True if conflicts, false if not.
    bool conflictsXZ(const BoundingVolume &volume) const
    {
      for(const Object& vv : m_objects)
      {
        if(vv.conflictsXZ(volume))
        {
          return true;
        }
      }
      return false;
    }

    /// Find object with comparison value closest to given value.
    ///
    /// Comparison happens to the middle of an object. After acquiring the index,
    /// iteration needs to happen in both directions.
    ///
    /// \param op Value to look for.
    /// \return Index of object.
    unsigned find(float op) const
    {
#if 0
      {
        float min_value = FLT_MAX;
        float max_value = -FLT_MAX;

        for(unsigned ii = 0; (m_objects.size() > ii); ++ii)
        {
          float cmp = m_objects[ii].getComparisonValue();

          min_value = std::min(min_value, cmp);
          max_value = std::max(max_value, cmp);

          if(ii > 0)
          {
            if(cmp < m_objects[ii - 1].getComparisonValue())
            {
              std::cout << "wtf?\n";
            }
          }
          std::cout << "value " << ii << ": " << cmp << std::endl;
        }

        std::cout << "min: " << min_value << ", " << max_value << std::endl;
      }
#endif

      // No point in searching.
      if(1 >= m_objects.size())
      {
#if defined(USE_LD)
        if(m_objects.empty())
        {
          BOOST_THROW_EXCEPTION(std::runtime_error("no objects to find from"));
        }
#endif
        return 0;
      }

      // Binary search.
      unsigned aa = 0;
      unsigned bb = m_objects.size() - 1;
      for(;;)
      {
#if defined(USE_LD) && defined(DEBUG)
        if(aa == bb)
        {
          std::ostringstream sstr;
          sstr << "find error, low " << aa << ", high " << bb << std::endl;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        if(aa + 1 == bb)
        {
          float ca = m_objects[aa].getComparisonValue();

          if(op <= ca)
          {
            return aa;
          }

          float cb = m_objects[bb].getComparisonValue();
          if(op >= cb)
          {
            return bb;
          }

          return ((op - ca) <= (cb - op)) ? aa : bb;
        }

        unsigned ii = (aa + bb) / 2;
        float cmp = m_objects[ii].getComparisonValue();

        //std::cout << "cmp: " << cmp << " against op: " << op << std::endl;

        if(cmp > op)
        {
          bb = ii;
        }
        else
        {
          aa = ii;
        }
        //std::cout << "new range: " << aa << ", " << bb << std::endl;
      }
    }

    /// Accessor.
    ///
    /// \param idx Index of object to access.
    /// \return Object reference.
    const Object& getObject(unsigned idx) const
    {
      return m_objects[idx];
    }

    /// Accessor.
    ///
    /// \return Object count.
    unsigned getObjectCount() const
    {
      return m_objects.size();
    }

    /// Get the Y translation of latest added object.
    ///
    /// Will not function if no objects added yet.
    ///
    /// \return Y translation of latest object.
    float getLatestTranslationY() const
    {
      return m_objects[m_objects.size() - 1].getTransform()[13];
    }

    /// Sort object database.
    ///
    /// Sorts objects by Z-order so they can be accessed and only required elements need be rendered.
    void sort()
    {
      dnload_qsort(m_objects.getData(), m_objects.size(), sizeof(Object), Object::qsort_cmp_object);
    }
};

#endif
