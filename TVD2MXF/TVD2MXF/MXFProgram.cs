using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

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

    public string DebugInfo;

    public MXFProgram(XmlNode p, XmlNamespaceManager n, MXFData data) {
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

      // GuideImages
      guideImage = null;
      XmlNode xmlImg = xmlProgram.SelectSingleNode("ns:brdcst/ns:media/ns:url", ns);
      if (xmlImg != null) {
        MXFGuideImage img = new MXFGuideImage(data.GuideImages.Count, "\\images\\events\\" + xmlImg.InnerText);
        guideImage = img;
        data.GuideImages.Add(img);
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
        if (n != null) return n.InnerText;

        log.Warn("No title found, id " + this.Id + ", " + this.DebugInfo);              
        return string.Empty;
      }
    }

    public string EpisodeTitle {
      get {
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
        if (n != null) return n.InnerText;
        return string.Empty;
      }
    }

    public string Description {
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

        string desc2 = "";
        n = xmlProgram.SelectSingleNode("ns:chr[@dflt='1']", ns);
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
        if (desc2 != "")
          desc += " (" + desc2 + ")";
        
        return desc;
      }
    }

    public string ShortDescription {
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
          string desc = n.InnerText;
          if (desc.Length > 512) {
            desc = desc.Substring(0, 512-3) + "...";
          }
          return desc;
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
