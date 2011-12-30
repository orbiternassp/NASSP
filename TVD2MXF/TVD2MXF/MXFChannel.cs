using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFChannel {

    private XmlNode xmlChannel;
    private XmlNamespaceManager ns;

    public List<MXFScheduleEntry> ScheduleEntries;
    public MXFGuideImage GuideImage;

    public MXFChannel(XmlNode c, XmlNamespaceManager n) {
      xmlChannel = c;
      ns = n;

      ScheduleEntries = new List<MXFScheduleEntry>();
    }

    public string Id {
      get { return xmlChannel.Attributes.GetNamedItem("id").Value; }
    }    

    public string Name {
      get {
        XmlNode n = xmlChannel.SelectSingleNode("ns:DName/ns:Long", ns);
        if (n != null) return n.InnerText;
        return string.Empty;
      }
    }
  }
}
