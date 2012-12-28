using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFPerson {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    public int Id;
    public bool Used;

    //private XmlNode xmlPerson;
    //private XmlNamespaceManager ns;

    private string uid;
    private string name;    

    /*
    public MXFPerson(XmlNode p, XmlNamespaceManager n) {
      xmlPerson = p;
      ns = n;
      Used = false;
    }
    */

    public MXFPerson(string n) {
      name = n;
      Used = false;
    }

    public MXFPerson(string u, string n) {
      uid = u;
      name = n;
      Used = false;
    }
    
    public string Uid {
      get {
        if (string.IsNullOrEmpty(uid)) {
          string newKey = MXFPerson.CreateUid(Name);
          //log.Debug("Created new person uid: " + "!MCPerson!" + newKey + ": " + Name);
          return "!MCPerson!" + newKey;
        } else {
          return uid;
        }
      }
    }

    public string Name {
      get {
        /*
        if (string.IsNullOrEmpty(name)) {
          XmlNode n = xmlPerson.SelectSingleNode("ns:n[@dflt='1']", ns);
          if (n != null) return n.InnerText;
          return string.Empty;
        } else {
        */ 
          return name;
        //}
      }
    }

    public static string CreateUid(string name) {

      // FIXME NOT REALLY CORRECT, remove only space between first names and surnames
      string[] npart = name.Split(' ');
      string newKey;
      if (npart.Length == 0) {
        newKey = name;
      } else if (npart.Length == 1) {
        newKey = npart[0];
      } else if (npart.Length == 2) {
        newKey = npart[0] + npart[1];
      } else if (npart.Length == 3) {
        if (npart[1].ToLower() == "von" || npart[1].ToLower() == "de" || npart[1].ToLower() == "le" ||
            npart[1].ToLower() == "van" || npart[1].ToLower() == "la" || npart[1].ToLower() == "di" ||
            npart[1].ToLower() == "del" || npart[1].ToLower() == "den" || npart[1].ToLower() == "gimenez" ||
            npart[1].ToLower() == "sosa" || npart[1].ToLower() == "gomez" || npart[1].ToLower() == "bel" ||
            npart[1].ToLower() == "ben" || npart[2].ToLower() == "jr" || npart[2].ToLower() == "jr." ||
            npart[1].ToLower() == "du" || npart[1].ToLower() == "af" || npart[1].ToLower() == "bon" ||
            npart[1].ToLower() == "vom" || npart[1].ToLower() == "san" || npart[2].ToLower() == "ii" ||
            npart[2].ToLower() == "iii" || npart[1].ToLower() == "el" || npart[1].ToLower() == "do" ||
            npart[1].ToLower() == "da" || npart[1].ToLower() == "dos" || npart[1].ToLower() == "st." ||
            npart[2].ToLower() == "sr" || npart[2].ToLower() == "sr.") {
          newKey = npart[0] + npart[1] + " " + npart[2];
        } else {
          newKey = npart[0] + " " + npart[1] + npart[2];
        }
      } else if (npart.Length == 4) {
        string middle = npart[1].ToLower() + " " + npart[2].ToLower();

        if (middle == "von der" || middle == "van den" || middle == "van der" || middle == "van de" || middle == "de la") {
          newKey = npart[0] + npart[1] + " " + npart[2] + " " + npart[3];
        } else if (npart[3].ToLower() == "sr" || npart[3].ToLower() == "sr." || npart[3].ToLower() == "jr" || npart[3].ToLower() == "jr.") {
          newKey = npart[0] + " " + npart[1] + npart[2] + " " + npart[3];
        } else {
          newKey = npart[0] + " " + npart[1] + " " + npart[2] + npart[3];
        }
      } else {
        newKey = name.Replace(npart[0] + " ", npart[0]);
      }
      return newKey;
    }
  }
}
