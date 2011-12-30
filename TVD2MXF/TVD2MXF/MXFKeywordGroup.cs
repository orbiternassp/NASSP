using System;
using System.Collections.Generic;
using System.Text;

namespace TVD2MXF {
  class MXFKeywordGroup {

    private MXFKeyword group;

    public List<MXFKeyword> Keywords;

    public MXFKeywordGroup(MXFKeyword g) {
      group = g;
      Keywords = new List<MXFKeyword>();
    }

    public MXFKeyword Group {
      get { return group; }
    }

    public string Id {
      get { return group.Id; }
    }
  }
}
