using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFScheduleEntry {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    private MXFChannel channel;
    private MXFProgram program;
    private XmlNode xmlProgram;

    public MXFScheduleEntry(MXFChannel ch, MXFProgram p, XmlNode xml) {
      channel = ch;
      program = p;
      xmlProgram = xml;
    }

    public MXFChannel Channel {
      get {
        return channel;
      }
    }

    public MXFProgram Program {
      get {
        return program;
      }
    }

    public DateTime StartTime {
      get {
        string st = xmlProgram.Attributes.GetNamedItem("starttime").Value;
        // e.g. 2012-05-31 01:00
        DateTime start = new DateTime(int.Parse(st.Substring(0, 4)),
                                      int.Parse(st.Substring(5, 2)),
                                      int.Parse(st.Substring(8, 2)),
                                      int.Parse(st.Substring(11, 2)),
                                      int.Parse(st.Substring(14, 2)),
                                      0,
                                      DateTimeKind.Local);
        return start;
      }
    }

    public string XmlStartTime {
      get {
        return this.StartTime.ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ss");
      }
    }

    public string AudioFormat {
      get {
        if (xmlProgram.Attributes.GetNamedItem("audio_dolbydigital").Value == "1") {
          // Dolby Digital
          return "4";
        }
        if (xmlProgram.Attributes.GetNamedItem("audio_dolbysurround").Value == "1") {
          // Dolby
          return "3";
        }
        if (xmlProgram.Attributes.GetNamedItem("audio_stereo").Value == "1") {
          // Stereo
          return "2";
        }
        if (xmlProgram.Attributes.GetNamedItem("audio_mono").Value == "1") {
          // Mono
          return "1";
        }
        return "0";
      }
    }

    public bool IsLive {
      get {
        if (xmlProgram.Attributes.GetNamedItem("islive").Value == "1")
          return true;
      
        return false;
      }
    }

    public bool IsHdtv {
      get {

        // TODO WDR HD, ATV auch NICHT HD

        // Sat1 (39), Pro7 (40), ATV (115),  Kinowelt TV (450) are wrongly not HD/16:9 in all EPG data
        if (Channel.Id == "39" || Channel.Id == "40" || Channel.Id == "115" || Channel.Id == "450") {
          return true;
        }

        // TV Browser Daten
        if (xmlProgram.Attributes.GetNamedItem("aspect_16_9").Value == "1" || xmlProgram.Attributes.GetNamedItem("hd").Value == "1") {
          return true;
        }

        return false;

        //
        // Old stuff
        //

        /*

        // ARD (71), arte (58) are always HD as the EPG data is for SD. NOT ZDF (37), "Reich und Schön" is in 4:3 on ZDF HD!
        if (Channel.Id == "71" || Channel.Id == "58") {
          return true;
        }
        // BR (51), BR alpha (104), Spiegel TV Digital (482), phoenix (194), N24 (175), NatGeo Wild (626), NatGeo HD (624), NatGeo (453), 
        // Anixe SD (537), Kinowelt TV (450), TNT Film (633), Eins Plus (475) are wrongly not HD/16:9 in EPG data
        //
        // Spiegel TV Digital (482) is both but no epg data, seems to be more 4:3, so it is NOT hd for now
        if (Channel.Id == "51" || Channel.Id == "194" || Channel.Id == "175" || Channel.Id == "626" || Channel.Id == "104" || Channel.Id == "475" || 
            Channel.Id == "624" || Channel.Id == "453" || Channel.Id == "537" || Channel.Id == "450" || Channel.Id == "633") {
          return true;
        }
        return false;
          
        */
      }
    }
  }
}
