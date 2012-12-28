using System;
using System.Collections.Generic;
using System.Text;

namespace TVD2MXF {
  class MXFEpisodeInfo {

    private MXFSeriesInfo seriesInfo;
    public string Episode;
    public int? SeasonNo;
    public int? EpisodeNo;

    public MXFEpisodeInfo(MXFSeriesInfo si) {
      seriesInfo = si;
    }

    public MXFSeriesInfo SeriesInfo {
      get {
        return seriesInfo;
      }
    }
  }
}
