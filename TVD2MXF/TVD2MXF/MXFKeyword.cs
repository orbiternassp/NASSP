using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFKeyword {

    private string id;
    private string name;

    public MXFKeywordGroup Group;

    public MXFKeyword(string i, string n) {
      id = i;
      name = n;      
    }

    public string Id {
      get {
        return id;
      }
    }

    public string Name {
      get {
        return name;
      }
    }
  }
}
