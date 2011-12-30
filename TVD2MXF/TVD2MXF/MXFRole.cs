using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFRole {
    private XmlNode xmlRole;
    private XmlNamespaceManager ns;

    private MXFPerson person;

    public MXFRole(MXFPerson p, XmlNode r, XmlNamespaceManager n) {
      person = p;
      xmlRole = r;
      ns = n;
    }

    public MXFPerson Person {
      get { return person; }
    }

    public string RoleType {
      get { return xmlRole.Attributes.GetNamedItem("rt").Value; }
    }

    public string Rank {
      get { return xmlRole.Attributes.GetNamedItem("ord").Value; }
    }

    public string Character {
      get {
        XmlNode n = xmlRole.SelectSingleNode("ns:r[@dflt='1']", ns);
        if (n != null) return n.InnerText;
        return string.Empty;
      }
    }
  }
}
