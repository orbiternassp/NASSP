using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Data.OleDb;


namespace TVD2MXF {
  class MXFProgram {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    private XmlNode xmlProgram;
    private XmlNamespaceManager ns;

    private Dictionary<string, MXFKeyword> keywords;
    private MXFKeywordGroup keywordGroup;
    private List<MXFRole> roles;
    private MXFSeriesInfo seriesInfo;
    private MXFGuideImage guideImage;

    private string tvmTitel = "";
    private string tvmKurzBeschreibung = "";
    private string tvmBilddateiname = "";
    private string tvmKurzkritik = "";
    private string tvmBewertungen = "";
    private string tvmBeschreibung = "";

    public string DebugInfo;

    public MXFProgram(XmlNode p, XmlNamespaceManager n, MXFData data, OleDbConnection tvmovieConnection, string imageDir, string tvmImageDir) {
      xmlProgram = p;
      ns = n;

      // Keywords
      keywordGroup = null;
      keywords = new Dictionary<string, MXFKeyword>();
      XmlNodeList xmlkeys = xmlProgram.SelectNodes("ns:mgnr[@t='1'] ", ns);
      foreach (XmlNode xmlkey in xmlkeys) {
        string kws = xmlkey.InnerText;
        // Mapping (to n keywords)
        if (data.KeywordMapping.ContainsKey(kws))
          kws = data.KeywordMapping[kws];
        string[] kwsa = kws.Split(',');
        foreach (string kw in kwsa) {
          MXFKeyword k;
          if (data.Keywords.TryGetValue(kw, out k)) {
            if (!keywords.ContainsKey(k.Id)) {
              if (keywordGroup == null) {
                keywordGroup = k.Group;
              }
              keywords.Add(k.Id, k);

              // 1st Level keywords are needed, too
              if (!keywords.ContainsKey(k.Group.Group.Id)) {
                keywords.Add(k.Group.Group.Id, k.Group.Group);
              }
            }
          } else {
            throw new Exception("*** ERROR Keyword " + xmlkey.InnerText);
          }
        }
      }

      // Spezial keyword mapping
      if (IsMovie) {
        // Kriegsfilm
        if (HasGnrKeywordStartingWith("51")) {
          MXFKeyword k = data.Keywords["C114"];
          keywords.Add(k.Id, k);
        }
        // Klassiker
        if (HasGnrKeywordStartingWith("47")) {
          MXFKeyword k = data.Keywords["C104"];
          keywords.Add(k.Id, k);
        }
      }

      // Roles /Persons
      roles = new List<MXFRole>();
      List<string> rolesCheck = new List<string>();

      XmlNodeList xmlroles = xmlProgram.SelectNodes("ns:mprsn", ns);
      foreach (XmlNode xmlrole in xmlroles) {
        MXFPerson pers = new MXFPerson(xmlrole, ns);
        MXFRole ro;

        if (string.IsNullOrEmpty(pers.Name.Trim())) {
          log.Error("*** Person without name, program " + Id + " " + Title);
        }

        if (data.Persons.ContainsKey(pers.Name)) {
          ro = new MXFRole(data.Persons[pers.Name], xmlrole, ns);
        } else {
          ro = new MXFRole(pers, xmlrole, ns);
          data.Persons.Add(pers.Name, pers);
          pers.Id = data.PersonId++;
        }
        ro.Person.Used = true;

        // Check duplicate roles
        if (!rolesCheck.Contains(pers.Name + "#" + ro.RoleType)) {
          roles.Add(ro);
          rolesCheck.Add(pers.Name + "#" + ro.RoleType);
        }
        /*
        else {
          log.Debug("Duplicate Role: " + pers.Name);
        }
        */
      }

      // Series
      XmlNode ser = xmlProgram.SelectSingleNode("ns:ser", ns);
      if (ser != null) {
        string sid = ser.Attributes.GetNamedItem("serid").Value;
        // Series 1 is buggy
        if (sid != "1") {
          seriesInfo = new MXFSeriesInfo(sid, this.Title);
        }
      }

      // TVMovie Data
      string chid = xmlProgram.Attributes.GetNamedItem("chid").Value;
      if (data.Channels.ContainsKey(chid)) {
        string chName = data.Channels[chid].TVMovieName;
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
            startTime = start.ToString("MM\\/dd\\/yyyy HH:mm:ss");
            string sqlTime = "#" + startTime + "#";

            OleDbCommand command = new OleDbCommand("Select * from Sendungen where SenderKennung = '" + chName + "' and Beginn = " + sqlTime, tvmovieConnection);
            OleDbDataReader reader = command.ExecuteReader();
            if (reader.Read()) {
              tvmTitel = (string) reader["Titel"];
              tvmKurzBeschreibung = (string) reader["KurzBeschreibung"];
              tvmBilddateiname = (string) reader["Bilddateiname"];
              tvmKurzkritik = (string) reader["Kurzkritik"];
              tvmBewertungen = (string) reader["Bewertungen"];

              //tvmBeschreibung = (string)reader["Titel"];
              OleDbCommand commandDetail = new OleDbCommand("Select * from SendungenDetails where Pos = " + reader["Pos"].ToString(), tvmovieConnection);
              OleDbDataReader readerDetail = commandDetail.ExecuteReader();
              if (readerDetail.Read()) {
                tvmBeschreibung = (string)readerDetail["Beschreibung"];
                // \n und <br> ersetzen
                tvmBeschreibung = tvmBeschreibung.Replace("\n", Environment.NewLine);
                tvmBeschreibung = tvmBeschreibung.Replace("<br><br>", Environment.NewLine);
                tvmBeschreibung = tvmBeschreibung.Replace("<br>", Environment.NewLine); 
              }
              readerDetail.Close();
            } else {
              data.tvmovieEntriesNotFound++;
              //log.Debug("TVMovie Sendung not found: " + chName + " at " + startTime + ": " + Title);
            }
            reader.Close();
          }
        }
      }

      // GuideImages
      guideImage = null;
      if (tvmBilddateiname != "") {
        MXFGuideImage img = new MXFGuideImage(data.GuideImages.Count, tvmImageDir + "\\" + tvmBilddateiname);
        guideImage = img;
        data.GuideImages.Add(img);

      } else {
        XmlNode xmlImg = xmlProgram.SelectSingleNode("ns:brdcst/ns:media/ns:url", ns);
        if (xmlImg != null) {
          MXFGuideImage img = new MXFGuideImage(data.GuideImages.Count, imageDir + "\\images\\events\\" + xmlImg.InnerText);
          guideImage = img;
          data.GuideImages.Add(img);
        }
      }
    }

    private bool HasGnrKeywordStartingWith(string gnrk) {
      XmlNodeList xmlkeys = xmlProgram.SelectNodes("ns:mgnr", ns);
      foreach (XmlNode xmlkey in xmlkeys) {
        if (xmlkey.Attributes.GetNamedItem("t") == null) {
          string kw = xmlkey.InnerText;
          if (kw.StartsWith(gnrk))
            return true;
        }
      }
      return false;
    }

    /*
    public List<MXFPerson> GetPersons() {
      List<MXFPerson> ps = new List<MXFPerson>();
      foreach (MXFRole r in roles) {
        ps.Add(r.Person);
      }
      return ps;
    }
    */

    public MXFSeriesInfo SeriesInfo {
      get { return seriesInfo; }
    }

    public Dictionary<string, MXFKeyword> Keywords {
      get { return keywords; }
    }

    public List<MXFRole> Roles {
      get { return roles; }
    }

    public MXFGuideImage GuideImage {
      get { return guideImage; }
    }

    public string Id {
      get { return xmlProgram.Attributes.GetNamedItem("pid").Value; }
    }

    public string Title {
      get {
        string tit = string.Empty;
        XmlNode n = xmlProgram.SelectSingleNode("ns:tit[@dflt='1']", ns);
        if (n != null) {
          if (n.Attributes.GetNamedItem("lang").Value != "deu") {
            // Is there a German title?
            n = xmlProgram.SelectSingleNode("ns:tit[@lang='deu']", ns);
            if (n == null) {
              // Fallback to foreign language
              n = xmlProgram.SelectSingleNode("ns:tit[@dflt='1']", ns);
            }
          }
        } else {
          // Is there a German title?
          n = xmlProgram.SelectSingleNode("ns:tit[@lang='deu']", ns);
          if (n == null) {
            // Is there any title?
            n = xmlProgram.SelectSingleNode("ns:tit", ns);
          }
        }
        if (n != null) 
          tit = n.InnerText;

        if (tit == "")
          tit = tvmTitel;

        if (tit == "")
          log.Warn("No title found, id " + this.Id + ", " + this.DebugInfo);              
        
        return tit;
      }
    }

    public string EpisodeTitle {
      get {
        string st = string.Empty;
        XmlNode n = xmlProgram.SelectSingleNode("ns:stit[@dflt='1']", ns);
        if (n != null) {
          if (n.Attributes.GetNamedItem("lang").Value != "deu") {
            // Is there a German subtitle?
            n = xmlProgram.SelectSingleNode("ns:stit[@lang='deu']", ns);
            if (n == null) {
              // Fallback to foreign language
              n = xmlProgram.SelectSingleNode("ns:stit[@dflt='1']", ns);
            }
          }
        } else {
          // Is there a German subtitle?
          n = xmlProgram.SelectSingleNode("ns:stit[@lang='deu']", ns);
          if (n == null) {
            // Is there any subtitle?
            n = xmlProgram.SelectSingleNode("ns:stit", ns);
          }
        }
        if (n != null) 
          st = n.InnerText;

        // TVMovie EpisodeTitle?
      
        return st;
      }
    }

    public string Description {
      get {
        string desc = TVDDescription;

        if (desc.Length < tvmBeschreibung.Length)
          desc = tvmBeschreibung;

        if (tvmKurzkritik != "") {
          if (desc != "") {
            desc += Environment.NewLine;
          }
          desc += "\"" + tvmKurzkritik + "\"";
        }

        string desc2 = "";
        XmlNode n = xmlProgram.SelectSingleNode("ns:chr[@dflt='1']", ns);
        if (n != null) {
          desc2 += n.InnerText;
        }
        n = xmlProgram.SelectSingleNode("ns:prdct", ns);
        if (n != null && n.Attributes.GetNamedItem("cntr") != null) {
          if (desc2 != "") desc2 += " - ";
          desc2 += n.Attributes.GetNamedItem("cntr").Value.Replace("|", ", ");
          if (Year != string.Empty) desc2 += " " + Year;
        }
        n = xmlProgram.SelectSingleNode("ns:tit[@orig='1']", ns);
        if (n != null && n.InnerText != Title) {
          if (desc2 != "") desc2 += " - ";
          desc2 += "OT: " + n.InnerText;
        }
        if (desc2 != "") {
          if (desc != "") {
            desc += Environment.NewLine;
          }        
          desc += "(" + desc2 + ")";
        }
        return desc;
      }
    }

    private string TVDDescription {
      get {
        string desc = string.Empty;
        XmlNode n = xmlProgram.SelectSingleNode("ns:losyn[@dflt='1']", ns);
        if (n != null) {
          if (!IgnoreForeignLanguage() && n.Attributes.GetNamedItem("lang").Value != "deu") {
            //log.Debug("Default description not German, id " + this.Id + ", " + this.DebugInfo);
            // Is there a german losyn?
            n = xmlProgram.SelectSingleNode("ns:losyn[@lang='deu']", ns);
            if (n == null) {
              log.Warn("No German description found, id " + this.Id + ", " + this.DebugInfo);              
              // Is there a german shsyn?
              n = xmlProgram.SelectSingleNode("ns:shsyn[@lang='deu']", ns);
              if (n == null) {
                // Fallback to foreign language
                n = xmlProgram.SelectSingleNode("ns:losyn[@dflt='1']", ns);
              }
            }
          }
        } else {
          n = xmlProgram.SelectSingleNode("ns:losyn[@lang='deu']", ns);
          if (n == null) {
            //log.Warn("No default and no German description found, id " + this.Id + ", " + this.DebugInfo);
            // Is there a german shsyn?
            n = xmlProgram.SelectSingleNode("ns:shsyn[@lang='deu']", ns);
            if (n == null) {
              // Is there any losyn?
              n = xmlProgram.SelectSingleNode("ns:losyn", ns);
              if (n == null) {
                // Is there any shsyn?
                n = xmlProgram.SelectSingleNode("ns:shsyn", ns);
              }
            }
          }
        }
        if (n != null) {
          desc = n.InnerText;
        }                
        return desc;
      }
    }

    public string ShortDescription {
      get {
        string sd = TVDShortDescription;
        if (sd == "") {
          sd = tvmBeschreibung;
        }
        if (sd == "") {
          sd = tvmKurzBeschreibung;
        }
        if (sd.Length > 512) {
          sd = sd.Substring(0, 512 - 3) + "...";
        }
        return sd;
      }
    }

    private string TVDShortDescription {
      get {
        XmlNode n = xmlProgram.SelectSingleNode("ns:shsyn[@dflt='1']", ns);
        if (n != null) {
          if (!IgnoreForeignLanguage() && n.Attributes.GetNamedItem("lang").Value != "deu") {
            //log.Debug("Default short description not German, id " + this.Id + ", " + this.DebugInfo);
            // Is there a german shsyn?
            n = xmlProgram.SelectSingleNode("ns:shsyn[@lang='deu']", ns);
            if (n == null) {
              // Is there a german losyn?
              n = xmlProgram.SelectSingleNode("ns:losyn[@lang='deu']", ns);
              if (n == null) {
                log.Warn("No German short description found, id " + this.Id + ", " + this.DebugInfo);
                // Fallback to foreign language
                n = xmlProgram.SelectSingleNode("ns:shsyn[@dflt='1']", ns);
              }
            }
          }
        } else {
          // Is there a german shsyn?
          n = xmlProgram.SelectSingleNode("ns:shsyn[@lang='deu']", ns);
          if (n == null) {
            // Is there a german losyn?
            n = xmlProgram.SelectSingleNode("ns:losyn[@lang='deu']", ns);
            if (n == null) {
              // Is there any shsyn?
              n = xmlProgram.SelectSingleNode("ns:shsyn", ns);
              if (n == null) {
                // Is there any losyn?
                n = xmlProgram.SelectSingleNode("ns:losyn", ns);
              }
            }
          }
        }
        if (n != null) {
          return n.InnerText;
        }
        return string.Empty;
      }
    }

    public string Year {
      get {
        XmlNode n = xmlProgram.SelectSingleNode("ns:prdct", ns);
        if (n != null) {
          if (n.Attributes.GetNamedItem("ylst") != null && !String.IsNullOrEmpty(n.Attributes.GetNamedItem("ylst").Value)) {
            return n.Attributes.GetNamedItem("ylst").Value;
          } else if (n.Attributes.GetNamedItem("yfst") != null) {
            return n.Attributes.GetNamedItem("yfst").Value;
          }
        }
        return string.Empty;
      }
    }

    public string EpisodeNumber {
      get {
        XmlNode ser = xmlProgram.SelectSingleNode("ns:ser", ns);
        if (ser != null) {
          return ser.Attributes.GetNamedItem("ep").Value;
        }
        return string.Empty;
      }
    }

    public string SeasonNumber {
      get {
        XmlNode ser = xmlProgram.SelectSingleNode("ns:ser", ns);
        if (ser != null) {
          return ser.Attributes.GetNamedItem("seas").Value;
        }
        return string.Empty;
      }
    }

    public string HalfStars {
      get {
        XmlNode ser = xmlProgram.SelectSingleNode("ns:rtng[@brnd='tvd' and @type='tot']", ns);
        if (ser != null) {
          String r = ser.Attributes.GetNamedItem("val").Value;
          return (Int32.Parse(r) * 2).ToString();
        }
        return string.Empty;
      }
    }

    public bool IsMovie {
      get { return IsKeyword("MSEPGC_MOVIES"); }
    }

    public bool IsNews {
      get { return IsKeyword("MSEPGC_NEWS"); }
    }

    public bool IsSports {
      get { return IsKeyword("MSEPGC_SPORTS"); }
    }

    public bool IsKids {
      get { return IsKeyword("MSEPGC_KIDS"); }
    }

    private bool IsKeyword(string refid) {
      if (keywordGroup != null) {
        if (keywordGroup.Group.RefId.Equals(refid))
          return true;
      }
      return false;
    }

    public string Cat {
      get {
        XmlNode n = xmlProgram.SelectSingleNode("ns:cat[@dflt='1']", ns);
        if (n != null) return n.InnerText;
        return string.Empty;
      }
    }

    public string Chr {
      get {
        XmlNode n = xmlProgram.SelectSingleNode("ns:chr[@dflt='1']", ns);
        if (n != null) return n.InnerText;
        return string.Empty;
      }
    }

    private bool IgnoreForeignLanguage() {
      string ch = xmlProgram.Attributes.GetNamedItem("chid").Value;

      /*
      296		VH1 Classic		
	    300		Deutsche Welle TV
	    454		E! Entertainment
	    455		Fashion TV
	    461		Motors TV
	    465		MTV Dance
	    466		MTV Hits UK
	    553		MTV Entertainment
	    591		ESPN America
	    601		MTV HD
	    662		BBC Entertainment
      463   Nautical Channel / Sailing
      693   HD suisse
      */

      if (ch == "296" || ch == "300" || ch == "454" || ch == "455" || ch == "461" || ch == "465" || ch == "466" ||
          ch == "553" || ch == "591" || ch == "601" || ch == "662" || ch == "463" || ch == "693") {
        return true;
      }
      return false;
    }

  }
}
