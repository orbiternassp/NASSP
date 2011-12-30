using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFScheduleEntry {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    private XmlNode xmlProgram;
    private XmlNamespaceManager ns;

    public MXFScheduleEntry(XmlNode p, XmlNamespaceManager n) {
      xmlProgram = p;
      ns = n;
    }

    public string Id {
      get { return xmlProgram.Attributes.GetNamedItem("bid").Value; }
    }

    public string ChannelId {
      get { return xmlProgram.Attributes.GetNamedItem("chid").Value; }
    }

    public string ProgramId {
      get { return xmlProgram.Attributes.GetNamedItem("pid").Value; }
    }

    public string StartTime {
      get {
        XmlNode n = xmlProgram.SelectSingleNode("ns:time", ns);
        if (n != null) {
          string start = n.Attributes.GetNamedItem("strt").Value;
          return start.Substring(0, start.Length - 3);
        }
        return String.Empty;
      }
    }

    public string EndTime {
      get {
        XmlNode n = xmlProgram.SelectSingleNode("ns:time", ns);
        if (n != null) {
          string start = n.Attributes.GetNamedItem("end").Value;
          return start.Substring(0, start.Length - 3);
        }
        return String.Empty;
      }
    }

    public string Duration {
      get {
        string dur = "0";
        XmlNode n = xmlProgram.SelectSingleNode("ns:time", ns);
        if (n != null) {
          dur = n.Attributes.GetNamedItem("dur").Value;
        }

        // Check Duration
        DateTime start = ConvertToDateTime(StartTime);
        DateTime end = ConvertToDateTime(EndTime);

        TimeSpan diff = end.Subtract(start);
        if (int.Parse(dur) != (int) diff.TotalSeconds) {
          log.Debug("Start/End/Duration mismatch. Channel: " + ChannelId + ", Durations: " + dur + " - " + diff.TotalSeconds.ToString() + ", Start: " + StartTime);
          dur = diff.TotalSeconds.ToString();
        }
        return dur;
      }
    }

    public DateTime ConvertToDateTime(string date) {
      return new DateTime(int.Parse(date.Substring(0, 4)),
                          int.Parse(date.Substring(5, 2)),
                          int.Parse(date.Substring(8, 2)),
                          int.Parse(date.Substring(11, 2)),
                          int.Parse(date.Substring(14, 2)),
                          int.Parse(date.Substring(17, 2)),
                          DateTimeKind.Utc);
    }

    public string AudioFormat {
      get {
        XmlNode n = xmlProgram.SelectSingleNode("ns:tchn", ns);
        if (n != null) {
          if (n.Attributes.GetNamedItem("b").Value == "1")
            return "4";
          else if (n.Attributes.GetNamedItem("a").Value == "1")
            return "2";
        }
        return "0";
      }
    }

    public bool IsLive {
      get {
        XmlNode n = xmlProgram.SelectSingleNode("ns:brdcst", ns);
        if (n != null) {
          if (n.Attributes.GetNamedItem("live").Value == "1")
            return true;
        }
        return false;
      }
    }

    public bool IsHdtv {
      get {
        // ARD (71), arte (58) are always HD as the EPG data is for SD. NOT ZDF (37), "Reich und Schön" is in 4:3 on ZDF HD!
        if (ChannelId == "71" || ChannelId == "58") {
          return true;
        }
        // BR (51), BR alpha (104), Spiegel TV Digital (482), phoenix (194), N24 (175), NatGeo Wild (626), NatGeo HD (624), NatGeo (453), 
        // Anixe SD (537), Kinowelt TV (450), TNT Film (633), Eins Plus (475) are wrongly not HD/16:9 in EPG data
        //
        // Spiegel TV Digital (482) is both but no epg data, seems to be more 4:3, so it is NOT hd for now
        if (ChannelId == "51" || ChannelId == "194" || ChannelId == "175" || ChannelId == "626" || ChannelId == "104" || ChannelId == "475" || 
            ChannelId == "624" || ChannelId == "453" || ChannelId == "537" || ChannelId == "450" || ChannelId == "633") {
          return true;
        }

        XmlNode n = xmlProgram.SelectSingleNode("ns:tchn", ns);
        if (n != null) {
          if (n.Attributes.GetNamedItem("h").Value == "1" || n.Attributes.GetNamedItem("e").Value == "1")  // HD flag (= 16:9 flag for auto zoom)
            return true;
        }
        return false;
      }
    }
  }
}
