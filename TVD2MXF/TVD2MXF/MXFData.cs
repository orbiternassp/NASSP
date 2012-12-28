using System;
using System.Collections.Generic;
using System.Text;

namespace TVD2MXF {
  class MXFData {

    public Dictionary<string, MXFChannel> Channels;
    public Dictionary<string, MXFKeyword> Keywords;
    public Dictionary<string, MXFKeywordGroup> KeywordGroups;
    public Dictionary<long, MXFProgram> Programs;
    public Dictionary<string, MXFPerson> Persons;
    public Dictionary<long, MXFSeriesInfo> SeriesInfos;
    public List<MXFGuideImage> GuideImages;
    public Dictionary<string, string> KeywordMapping;
    public int PersonId;

    public int tvmovieEntriesNotFound;

    public MXFData() {
      Channels = new Dictionary<string, MXFChannel>();
      Keywords = new Dictionary<string, MXFKeyword>();
      KeywordGroups = new Dictionary<string, MXFKeywordGroup>();
      Programs = new Dictionary<long, MXFProgram>();
      Persons = new Dictionary<string, MXFPerson>();
      SeriesInfos = new Dictionary<long, MXFSeriesInfo>();
      GuideImages = new List<MXFGuideImage>();
      KeywordMapping = new Dictionary<string, string>();
      PersonId = 1;
    }
  }
}
