using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFKeyword {

    private XmlNode xmlTerm;
    private XmlNamespaceManager ns;

    private string id;
    private string name;

    public MXFKeywordGroup Group;

    public MXFKeyword(XmlNode t, XmlNamespaceManager n) {
      xmlTerm = t;
      ns = n;
    }

    public MXFKeyword(string i, string n) {
      xmlTerm = null;
      id = i;
      name = n;      
    }

    public string Id {
      get {
        if (xmlTerm == null)
          return id;

        string i = xmlTerm.Attributes.GetNamedItem("id").Value;
        if (i.Contains(".")) {
          return "k" + i.Split(new char[]{'.'})[1];
        } else {
          return "k" + (Int32.Parse(i) / 100).ToString();
        }
      }
    }

    public string RefId {
      get {
        if (xmlTerm == null)
          return id;

        return xmlTerm.Attributes.GetNamedItem("id").Value;
      }
    }

    public string Name {
      get {
        if (xmlTerm == null)
          return name;

        XmlNode n = xmlTerm.SelectSingleNode("ns:name[@lang='deu']", ns);
        if (n != null) return n.InnerText;
        return string.Empty;
      }
    }

  }
}
