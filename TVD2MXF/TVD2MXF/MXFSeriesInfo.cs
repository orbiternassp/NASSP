using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFSeriesInfo {

    private string id;
    private string title;

    public Dictionary<string, MXFProgram> AdditionalTitles;

    public MXFSeriesInfo(string i, string t) {
      id = i;
      title = t;

      AdditionalTitles = new Dictionary<string, MXFProgram>(StringComparer.CurrentCultureIgnoreCase);
    }

    public string Id {
      get { return "si" + id; }
    }

    public string Title {
      get { return title; }
    }
  }
}
