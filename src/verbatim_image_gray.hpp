#ifndef IMAGE_GRAY_HPP
#define IMAGE_GRAY_HPP

#include "verbatim_image.hpp"
#include "verbatim_uptr.hpp"

/// Grayscale image.
class ImageGray : public Image
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    ImageGray(unsigned width, unsigned height) :
      Image(width, height, 1) { }

  public:
    /// Sample (in a bilinear fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \return Sampled color.
    float sampleLinear(float px, float py) const
    {
      return Image::sampleLinear(px, py, 0);
    }
    /// Sample (in a nearest fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \return Sampled color.
    float sampleNearest(float px, float py) const
    {
      return Image::sampleNearest(px, py, 0);
    }

    /// Get pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \return Pixel value at given coordinates.
    float getPixel(unsigned px, unsigned py) const
    {
      return getValue(px, py, 0);
    }
    /// Set pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param op Gray level.
    void setPixel(unsigned px, unsigned py, float op)
    {
      setValue(px, py, 0, op);
    }
};

/// Convenience typedef.
typedef uptr<ImageGray> ImageGrayUptr;

#endif
