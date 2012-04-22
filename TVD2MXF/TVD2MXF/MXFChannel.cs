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

    public string TVMovieName {
      get {
        if (IgnoreChannel(Name)) {
          return string.Empty;
        }

        switch (Name) {
          case "Das Erste":
            return "ARD";
          case "ATV":
            return "ATV+";
          case "München TV":
            return "";  // gibt's nicht
          case "ZDFkultur":
            return "ZDF Kultur";
          case "Tele 5":
            return "Tele5";
          case "Sat.1":
            return "SAT1";
          case "ProSieben":
            return "PRO7";
          case "Super RTL":
            return "S RTL";
          case "kabel eins":
            return "Kabel";
          case "History":
            return "History Channel";
          case "National Geographic Channel":
            return "National Geographic";
          case "13TH STREET Universal":
            return "13th Street";
          case "Syfy":
            return "SCI FI";
          case "EinsPlus":
            return "EinsMuXx";
          case "HR":
            return "H3";
          case "DMAX":
            return "XXP";
          case "BR":
            return "B3";
          case "Passion":
            return "RTL Passion";
          case "Nickelodeon / Comedy Central":
            return "";  // passt garnicht?
          case "Sky Sport HD 1":
            return "SKY Sport HD";
          case "National Geographic Channel HD":
            return "National Geographic HD";
          case "Nat Geo Wild":
            return "National Geographic Wild";
          case "TNT Film":
            return "Turner Classic Movies";
          case "SPORT1":
            return "DSF";
          case "n-tv":
            return "NTV";
          case "Servus TV":
            return "ServusTV";
        }
        return Name;
      }
    }

    public string TVBrowserName {
      get {
        if (IgnoreChannel(Name)) {
          return string.Empty;
        }

        switch (Name) {
          case "Das Erste":
            return "Das Erste (ARD)";
          case "BR alpha":
            return "BR-alpha";
          case "Animal Planet":
            return "";  // gibts nicht?
          case "EinsFestival":
            return "Einsfestival";
          case "Phoenix":
            return "PHOENIX";
          case "Planet":
            return "";  // gibts nicht?
          case "München TV":
            return "";  // gibts nicht?
          case "VH1 Classic":
            return "";  // gibts nicht?
          case "RTL II":
            return "RTL2";
          case "Silverline":
            return "";  // gibts nicht?
          case "Motors TV":
            return "";  // gibts nicht?
          case "MTV Dance":
            return "";  // gibts nicht?
          case "Extreme Sports Channel":
            return "";  // gibts nicht?
          case "NDR":
            return "NDR Hamburg";
          case "13TH STREET Universal":
            return "13th Street Universal";
          case "MDR":
            return "MDR Sachsen";
          case "Das VIERTE":
            return "DAS VIERTE";
          case "Gute Laune TV":
            return "";  // gibts nicht?
          case "SWR":
            return "SWR BW";
          case "Eurosport 2":
            return "Eurosport2";
          case "RBB":
            return "RBB Berlin";
          case "Anixe HD":
            return "ANIXE";
          case "Animax":
            return "ANIMAX";
          case "ORF 1":
            return "ORF eins";
          case "Arte":
            return "arte";
          case "Nickelodeon / Comedy Central":
            return "";  // gibts nicht?
          case "TNT Serie":
            return "TNT SERIE";
          case "NICK Jr.":
            return "";
          case "Sky Cinema Hits":
            return "";
          case "Sky Sport Austria":
            return "";
          case "National Geographic Channel HD":
            return "National Geographic HD";
          case "Nat Geo Wild":
            return "NatGeo Wild";
          case "TNT Film":
            return "TNT FILM";
          case "SPORT1":
            return "Sport1";
          case "Servus TV":
            return "ServusTV";
          case "BBC Entertainment":
            return "";
          case "Euronews":
            return "";
          case "Sixx":
            return "";
        }
        return Name;
      }
    }

    private bool IgnoreChannel(string channel) {
      switch (channel) {
        case "TV.Berlin":
        case "Beate-Uhse.TV":
        case "Bloomberg TV":
        case "HH 1":
        case "HSE24":
        case "Junior":
        case "Rhein-Neckar-Fernsehen":
        case "SF Info":
        case "Star TV":
        case "Tele Bärn":
        case "Tele Bielingue":
        case "Tele Basel":
        case "Tele Züri":
        case "Bibel TV":
        case "Deutsche Welle TV":
        case "Tele 1":
        case "Tele M1":
        case "Go TV":
        case "rheinmainTV":
        case "E! Entertainment":
        case "Fashion TV":
        case "BonGusto":
        case "Disney Junior":
        case "Nautical Channel":
        case "Trace.TV":
        case "MTV Hits UK":
        case "ESPN Classic":
        case "Channel 21":
        case "Blue Hustler":
        case "Deluxe Music":
        case "NRW TV":
        case "3plus":
        case "Nicktoons":
        case "sportdigital":
        case "FOX":
        case "yourfamily":
        case "Austria 9":
        case "SF1":
        case "ESPN America":
        case "Baby TV":
        case "BabyFirstTV":
        case "SF2":
        case "MTV Live HD":
        case "Travelchannel":
        case "Sky Select":
        case "iMusic":
        case "Yavido":
        case "auto motor und sport channel":
        case "HD suisse":
        case "Classica HD":
        case "Puls 4":
        case "QVC":
        case "KidsCo":
        case "TIMM":
        case "Lust Pur":
        case "sonnenklar.tv":
        case "Spreekanal":
        case "Oberpfalz TV":
          return true;

      }
      return false;
    }
  }
}
