using System;
using System.Collections.Generic;
using System.Text;

namespace TVD2MXF {
  class MXFGuideImage {

    private int id;
    private string fileName;

    public MXFGuideImage(int i, string f) {
      id = i;
      fileName = f;
    }

    public int Id {
      get { return id; }
    }

    public string FileName {
      get { return fileName; }
    }
  }
}
