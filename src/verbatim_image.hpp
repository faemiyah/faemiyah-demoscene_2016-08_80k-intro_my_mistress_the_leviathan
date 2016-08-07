#ifndef VERBATIM_IMAGE_HPP
#define VERBATIM_IMAGE_HPP

#include "verbatim_gl.hpp"
#include "verbatim_vec2.hpp"

/// Base image class.
class Image
{
  private:
    /// Image data.
    float *m_data;

    /// uint8_t data for textures.
    uint8_t *m_export_data;

    /// Width.
    unsigned m_width;

    /// Height.
    unsigned m_height;

    /// Number of channels.
    unsigned m_channels;

  private:
    /// Deleted copy constructor.
    Image(const Image&) = delete;
    /// Deleted assignment.
    Image& operator=(const Image&) = delete;

  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    /// \param channels Number of channels.
    Image(unsigned width, unsigned height, unsigned channels) :
      m_data(array_new(static_cast<float*>(NULL), width * height * channels)),
      m_export_data(NULL),
      m_width(width),
      m_height(height),
      m_channels(channels) { }

    /// Destructor.
    ~Image()
    {
      array_delete(m_data);
      array_delete(m_export_data);
    }

  protected:
    /// Replace data.
    ///
    /// \param op New data.
    void replaceData(float *op)
    {
      array_delete(m_data);
      m_data = op;
    }

    /// Get value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \return Value.
    float getValue(unsigned px, unsigned py, unsigned ch) const
    {
      unsigned idx = (py * m_width + px) * m_channels;

      return m_data[idx + ch];
    }
    /// Set value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \param val Value.
    void setValue(unsigned px, unsigned py, unsigned ch, float val)
    {
      unsigned idx = (py * m_width + px) * m_channels;

      m_data[idx + ch] = val;
    }

    /// Sample from the image.
    ///
    /// Note that this happens at the origin of texels as opposed to center.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    /// \param nearest True to sample nearest as opposed to linear.
    /// \return Sampled color.
    float sample(float px, float py, unsigned pc, bool nearest) const
    {
      float fwidth = static_cast<float>(m_width);
      float fheight = static_cast<float>(m_height);
      if(0.0f <= px)
      {
        px = dnload_fmodf(px, 1.0f) * fwidth;
      }
      else
      {
        px = (1.0f - dnload_fmodf(-px, 1.0f)) * fwidth;
      }
      if(0.0f <= py)
      {
        py = dnload_fmodf(py, 1.0f) * fheight;
      }
      else
      {
        py = (1.0f - dnload_fmodf(-py, 1.0f)) * fheight;
      }

      unsigned ux = static_cast<unsigned>(px);
      unsigned uy = static_cast<unsigned>(py);
      float fract_x = px - static_cast<float>(ux);
      float fract_y = py - static_cast<float>(uy);
      unsigned x1 = ux;
      unsigned x2 = (ux + 1) % m_width;
      unsigned y1 = uy;
      unsigned y2 = (uy + 1) % m_height;

      if(nearest)
      {
        if(fract_x < 0.5f)
        {
          if(fract_y < 0.5f)
          {
            return getValue(x1, y1, pc);
          }
          return getValue(x1, y2, pc);
        }
        if(fract_y < 0.5f)
        {
          return getValue(x2, y1, pc);
        }
        return getValue(x2, y2, pc);
      }

      return mix(mix(getValue(x1, y1, pc), getValue(x2, y1, pc), fract_x),
          mix(getValue(x1, y2, pc), getValue(x2, y2, pc), fract_x), fract_y);
    }

    /// Sample (in a bilinear fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    float sampleLinear(float px, float py, unsigned pc) const
    {
      return sample(px, py, pc, false);
    }
    /// Sample (in a nearest fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    float sampleNearest(float px, float py, unsigned pc) const
    {
      return sample(px, py, pc, true);
    }

  public:
    /// Apply a low-pass filter over the texture.
    ///
    /// This low-pass filter will wrap around the texture edges.
    ///
    /// \param op Kernel size.
    void filterLowpass(int op)
    {
#if defined(USE_LD)
      if(4 < getChannelCount())
      {
        std::ostringstream sstr;
        sstr << "cannot filter texture with " << getChannelCount() << " channels";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      unsigned element_count = getWidth() * getHeight() * getChannelCount();
      float *replacement_data = array_new(static_cast<float*>(NULL), element_count);
      int iwidth = static_cast<int>(getWidth());
      int iheight = static_cast<int>(getWidth());
      int ichannels = static_cast<int>(getChannelCount());

      for(int ii = 0; (ii < iwidth); ++ii)
      {
        for(int jj = 0; (jj < iwidth); ++jj)
        {
          float values[] = { 0.0f, 0.0f, 0.0f, 0.0f };
          int idx = (jj * iwidth + ii) * ichannels;
          int divisor = 0;

          for(int kk = -op; (kk <= op); ++kk)
          {
            int rx = ii + kk;

            if(rx < 0)
            {
              rx = iwidth + rx;
            }
            else if(rx >= iwidth)
            {
              rx -= iwidth;
            }

            for(int ll = -op; (ll <= op); ++ll)
            {
              int ry = jj + ll;

              if(ry < 0)
              {
                ry = iheight + ry;
              }
              else if(ry >= iheight)
              {
                ry -= iheight;
              }

              unsigned ux = static_cast<unsigned>(rx);
              unsigned uy = static_cast<unsigned>(ry);

              for(unsigned mm = 0; (getChannelCount() > mm); ++mm)
              {
                values[mm] += getValue(ux, uy, mm);
              }
              ++divisor;
            }
          }

          for(int mm = 0; (ichannels > mm); ++mm)
          {
            replacement_data[idx + mm] = values[mm] / static_cast<float>(divisor);
          }
        }
      }

      replaceData(replacement_data);
    }

    /// Regenerate export data.
    ///
    /// Creates the export data when called from current floating point data.
    ///
    /// \return Export data.
    uint8_t* getExportData()
    {
#if defined(RENDER_RGB24_BYTES) && (3 != RENDER_RGB24_BYTES)
      if(3 == m_channels)
      {
#if (2 == RENDER_RGB24_BYTES)
        unsigned count = m_width * m_height;
        uint16_t* edata = array_new(static_cast<uint16_t*>(NULL), count);

        for(unsigned ii = 0, jj = 0; (ii < count); ++ii, jj += 3)
        {
          uint16_t rr = static_cast<uint16_t>(0.5f + clamp(m_data[jj + 0], 0.0f, 1.0f) * 31.0f);
          uint16_t gg = static_cast<uint16_t>(0.5f + clamp(m_data[jj + 1], 0.0f, 1.0f) * 63.0f);
          uint16_t bb = static_cast<uint16_t>(0.5f + clamp(m_data[jj + 2], 0.0f, 1.0f) * 31.0f);

          edata[ii] = static_cast<uint16_t>(rr << 11) | static_cast<uint16_t>(gg << 5) | bb;
        }

        array_delete(m_export_data);
        m_export_data = reinterpret_cast<uint8_t*>(edata);
#elif (4 == RENDER_RGB24_BYTES)
        unsigned count = m_width * m_height;
        unsigned iter_end = count * 3;

        m_export_data = array_new(m_export_data, count * 4);

        uint8_t *edata = reinterpret_cast<uint8_t*>(m_export_data);

        for(unsigned ii = 0, jj = 0; (ii < iter_end); ii += 3, jj += 4)
        {
          edata[jj + 0] = static_cast<uint8_t>(0.5f + clamp(m_data[ii + 0], 0.0f, 1.0f) * 255.0f);
          edata[jj + 1] = static_cast<uint8_t>(0.5f + clamp(m_data[ii + 1], 0.0f, 1.0f) * 255.0f);
          edata[jj + 2] = static_cast<uint8_t>(0.5f + clamp(m_data[ii + 2], 0.0f, 1.0f) * 255.0f);
          edata[jj + 3] = 255;
        }
#else
#error "unknown value for RENDER_RGB24_BYTES (convert)"
#endif
      }
      else
#endif
      {
        unsigned count = m_width * m_height * m_channels;

        m_export_data = array_new(m_export_data, count);

        for(unsigned ii = 0; (ii < count); ++ii)
        {
          m_export_data[ii] = static_cast<uint8_t>(0.5f + clamp(m_data[ii], 0.0f, 1.0f) * 255.0f);
        }
      }

      return m_export_data;
    }

    /// Accessor.
    ///
    /// \return Width.
    unsigned getWidth() const
    {
      return m_width;
    }

    /// Accessor.
    ///
    /// \return Height.
    unsigned getHeight() const
    {
      return m_height;
    }

    /// Accessor.
    ///
    /// \return Number of channels.
    unsigned getChannelCount() const
    {
      return m_channels;
    }

    /// Fill image with noise.
    ///
    /// \param nfloor Noise floor.
    /// \param nceil Noise ceiling.
    void noise(float nfloor = 0.0f, float nceil = 1.0f)
    {
      unsigned element_count = getWidth() * getHeight() * getChannelCount();

      for(unsigned ii = 0; (element_count > ii); ++ii)
      {
        m_data[ii] = frand(nfloor, nceil);
      }
    }

    /// Normalize color level.
    ///
    /// \param channel Channel
    /// \param ambient Ambient level (default: 0.0f).
    void normalize(unsigned channel, float ambient)
    {
      float min_value = FLT_MAX;
      float max_value = -FLT_MAX;

      for(unsigned ii = 0; (m_width > ii); ++ii)
      {
        for(unsigned jj = 0; (m_height > jj); ++jj)
        {
          float val = getValue(ii, jj, channel);

          min_value = std::min(val, min_value);
          max_value = std::max(val, max_value);
        }
      }

      float mul = (1.0f - ambient) / (max_value - min_value);

      //std::cout << "max: " << max_value << " ; min: " << min_value << " ; mul: " << mul << std::endl;

      for(unsigned ii = 0; (m_width > ii); ++ii)
      {
        for(unsigned jj = 0; (m_height > jj); ++jj)
        {
          float val = getValue(ii, jj, channel);

          setValue(ii, jj, channel, (mul * (val - min_value)) + ambient);
        }
      }
    }
};

#endif
