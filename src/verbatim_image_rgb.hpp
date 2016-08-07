#ifndef VERBATIM_IMAGE_RGB_HPP
#define VERBATIM_IMAGE_RGB_HPP

#include "verbatim_image.hpp"
#include "verbatim_uptr.hpp"
#include "verbatim_vec3.hpp"

/// RGB image class.
class ImageRGB : public Image
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    ImageRGB(unsigned width, unsigned height) :
      Image(width, height, 3) { }

  public:
    /// Set pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pr Red component.
    /// \param pg Green component.
    /// \param pb Blue component.
    void setPixel(unsigned px, unsigned py, float pr, float pg, float pb)
    {
      setValue(px, py, 0, pr);
      setValue(px, py, 1, pg);
      setValue(px, py, 2, pb);
    }
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param col 3-component vector as color.
    void setPixel(unsigned px, unsigned py, const vec3 &col)
    {
      setPixel(px, py, col[0], col[1], col[2]);
    }
};

/// Convenience typedef.
typedef uptr<ImageRGB> ImageRGBUptr;

#endif
