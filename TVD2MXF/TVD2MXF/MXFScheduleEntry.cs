using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFScheduleEntry {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    private XmlNode xmlProgram;
    private XmlNamespaceManager ns;
    private bool? tvbrowserWidescreen = null;
    private bool? tvbrowserHd = null;

    public MXFScheduleEntry(XmlNode p, XmlNamespaceManager n, XmlDocument xmlTVBrowserDoc, MXFData data) {
      xmlProgram = p;
      ns = n;

      // TVBrowser Data
      string chid = xmlProgram.Attributes.GetNamedItem("chid").Value;
      if (data.Channels.ContainsKey(chid)) {
        string chName = data.Channels[chid].TVBrowserName;
        if (chName != "") {
          XmlNode timeNode = xmlProgram.SelectSingleNode("ns:time", ns);
          if (timeNode != null) {
            string startTime = timeNode.Attributes.GetNamedItem("strt").Value;
            startTime = startTime.Substring(0, startTime.Length - 3);
            DateTime start = new DateTime(int.Parse(startTime.Substring(0, 4)),
                                          int.Parse(startTime.Substring(5, 2)),
                                          int.Parse(startTime.Substring(8, 2)),
                                          int.Parse(startTime.Substring(11, 2)),
                                          int.Parse(startTime.Substring(14, 2)),
                                          int.Parse(startTime.Substring(17, 2)),
                                          DateTimeKind.Utc);
            start = start.ToLocalTime();
            startTime = start.ToString("yyyy-MM-dd HH:mm");

            XmlNode chNode = xmlTVBrowserDoc.DocumentElement.SelectSingleNode("channel[name='" + chName + "']");
            if (chNode != null) {
              XmlNode progNode = chNode.SelectSingleNode("programs/program[@starttime='" + startTime + "']");
              if (progNode != null) {
                if (progNode.Attributes.GetNamedItem("widescreen").Value == "1") {
                  tvbrowserWidescreen = true;
                } else {
                  tvbrowserWidescreen = false;
                }
                if (progNode.Attributes.GetNamedItem("hd").Value == "1") {
                  tvbrowserHd = true;
                } else {
                  tvbrowserHd = false;
                }
              } else {
                data.tvbrowserEntriesNotFound++;
                //log.Debug("TV Browser program not found: " + chName + " at " + startTime);
              }
            } else {
              log.Warn("TV Browser channel " + chName + " not found.");
            }
          }
        }
      }
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

        // TODO WDR HD, ATV auch HD, Kinowelt TV oft HD

        // Sat1 (39), Pro7 (40), ATV (115) are wrongly not HD/16:9 in all EPG data
        if (ChannelId == "39" || ChannelId == "40" || ChannelId == "115") {
          return true;
        }

        // TODO Wie gut sind die TV Browser Daten?
        if (tvbrowserWidescreen.HasValue && tvbrowserHd.HasValue) {
          return (tvbrowserWidescreen.Value || tvbrowserHd.Value);
        }

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
