using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.Xml;
using System.IO;
using System.Data.OleDb;

using Microsoft.MediaCenter.Guide;

namespace TVD2MXF {
  class TVDReader {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    public MXFData Read(string feedDir, OleDbConnection tvmovieConnection, string imageDir, string tvmImageDir, string tvbrowserFile) {

      // Load channels
      XmlDocument xmlChannelDoc = new XmlDocument();
      xmlChannelDoc.Load(feedDir + "\\config\\channels\\channels.xml");

      XmlNamespaceManager ns = new XmlNamespaceManager(xmlChannelDoc.NameTable);
      ns.AddNamespace("ns", "http://www.as-guides.com/schema/channel");

      XmlElement e = xmlChannelDoc.DocumentElement;
      XmlNodeList xmlChannels = e.ChildNodes;
      log.Debug("Reading " + xmlChannels.Count + " Channels");

      MXFData data = new MXFData();
      foreach (XmlNode xmlChannel in xmlChannels) {
        //Get all the channels
        if (xmlChannel.Name == "Ch") {
          MXFChannel ch = new MXFChannel(xmlChannel, ns);          
          data.Channels.Add(ch.Id, ch);

          // Channel image
          MXFGuideImage img = new MXFGuideImage(data.GuideImages.Count, imageDir + "\\images\\logos\\ch" + ch.Id + ".png");
          ch.GuideImage = img;
          data.GuideImages.Add(img);
        }
      }

      //
      // Load keywords
      //

      XmlDocument xmlKeyDoc = new XmlDocument();
      xmlKeyDoc.Load(feedDir + "\\config\\gnr\\gnr1.xml");

      ns = new XmlNamespaceManager(xmlKeyDoc.NameTable);
      ns.AddNamespace("ns", "http://www.as-guides.com/schema/terms");

      // MS Keywords
      /*
      MXFKeyword msKey = new MXFKeyword("MSEPGC_MOVIES", "Film");
      data.Keywords.Add(msKey.RefId, msKey);
      MXFKeywordGroup msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
	    AddKeyword("C100", "Alle", msKeyGrp, data);
	    AddKeyword("C101", "Aktion", msKeyGrp, data);
	    AddKeyword("C102", "Erotik", msKeyGrp, data);
	    AddKeyword("C103", "Animation", msKeyGrp, data);
	    AddKeyword("C104", "Klassiker", msKeyGrp, data);
	    AddKeyword("C105", "Komödie", msKeyGrp, data);
	    AddKeyword("C106", "Krimi", msKeyGrp, data);
	    AddKeyword("C107", "Drama", msKeyGrp, data);
	    AddKeyword("C108", "Film Noir", msKeyGrp, data);
	    AddKeyword("C109", "Horror", msKeyGrp, data);
	    AddKeyword("C110", "Musical", msKeyGrp, data);
	    AddKeyword("C111", "Romantik", msKeyGrp, data);
	    AddKeyword("C112", "Science-Fiction", msKeyGrp, data);
	    AddKeyword("C113", "Kurzfilm", msKeyGrp, data);
	    AddKeyword("C114", "Krieg", msKeyGrp, data);
	    AddKeyword("C115", "Western", msKeyGrp, data);
	    AddKeyword("C116", "Sonstiges", msKeyGrp, data);

      msKey = new MXFKeyword("C2", "Drama");
      data.Keywords.Add(msKey.RefId, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C200", "Alle", msKeyGrp, data);
	    AddKeyword("C201", "Aktion", msKeyGrp, data);
	    AddKeyword("C202", "Krimi", msKeyGrp, data);
	    AddKeyword("C203", "Fantasy", msKeyGrp, data);
	    AddKeyword("C204", "Kurzfilm", msKeyGrp, data);
	    AddKeyword("C205", "Arzt", msKeyGrp, data);
	    AddKeyword("C206", "Science-Fiction", msKeyGrp, data);
	    AddKeyword("C207", "Theaterstück", msKeyGrp, data);
	    AddKeyword("C208", "Western", msKeyGrp, data);
	    AddKeyword("C209", "Sonstiges", msKeyGrp, data);

      //... C3 Sports
      msKey = new MXFKeyword("MSEPGC_SPORTS", "Sport");
      data.Keywords.Add(msKey.RefId, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C300", "Alle", msKeyGrp, data);
			AddKeyword("C301", "Leichtathletik", msKeyGrp, data);
			AddKeyword("C302", "Boxen", msKeyGrp, data);
			AddKeyword("C303", "Radsport", msKeyGrp, data);
			AddKeyword("C304", "Pferdesport", msKeyGrp, data);
			AddKeyword("C305", "Extremsport", msKeyGrp, data);
			AddKeyword("C306", "Fußball", msKeyGrp, data);
			AddKeyword("C307", "Golf", msKeyGrp, data);
			AddKeyword("C308", "Kampfsport", msKeyGrp, data);
			AddKeyword("C309", "Motorsport", msKeyGrp, data);
			AddKeyword("C310", "American Football", msKeyGrp, data);
			AddKeyword("C311", "Teamsport", msKeyGrp, data);
			AddKeyword("C312", "Tennis", msKeyGrp, data);
			AddKeyword("C313", "Wassersport", msKeyGrp, data);
			AddKeyword("C314", "Wintersport", msKeyGrp, data);
			AddKeyword("C315", "Ringen", msKeyGrp, data);
			AddKeyword("C316", "Sonstiges", msKeyGrp, data);

      //... C4 Interests
      msKey = new MXFKeyword("C4", "Interessen");
      data.Keywords.Add(msKey.RefId, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C400", "Alle", msKeyGrp, data);
			AddKeyword("C401", "Antiquitäten", msKeyGrp, data);
			AddKeyword("C402", "Kino", msKeyGrp, data);
			AddKeyword("C403", "Mode", msKeyGrp, data);
			AddKeyword("C404", "Freizeit", msKeyGrp, data);
			AddKeyword("C405", "Einkaufen", msKeyGrp, data);
			AddKeyword("C406", "Konsum", msKeyGrp, data);
			AddKeyword("C407", "Kochen", msKeyGrp, data);
			AddKeyword("C408", "Garten", msKeyGrp, data);
			AddKeyword("C409", "Gesundheit", msKeyGrp, data);
			AddKeyword("C410", "Religion", msKeyGrp, data);
			AddKeyword("C411", "Transport", msKeyGrp, data);
			AddKeyword("C412", "Reisen", msKeyGrp, data);
			AddKeyword("C413", "Sonstiges", msKeyGrp, data);

      //... C5 Music and arts
      msKey = new MXFKeyword("C5", "Musik & Kunst");
      data.Keywords.Add(msKey.RefId, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C500", "Alle", msKeyGrp, data);
			AddKeyword("C501", "Architektur", msKeyGrp, data);
			AddKeyword("C502", "Kunst", msKeyGrp, data);
			AddKeyword("C503", "Ballett", msKeyGrp, data);
			AddKeyword("C504", "Klassische Musik", msKeyGrp, data);
			AddKeyword("C505", "Tanzmusik", msKeyGrp, data);
			AddKeyword("C506", "Tanzen", msKeyGrp, data);
			AddKeyword("C507", "Volkmusik", msKeyGrp, data);
			AddKeyword("C508", "Jazz", msKeyGrp, data);
			AddKeyword("C509", "Oper", msKeyGrp, data);
			AddKeyword("C510", "Pop & Rock", msKeyGrp, data);
			AddKeyword("C511", "Skulptur", msKeyGrp, data);
			AddKeyword("C512", "Theater", msKeyGrp, data);
			AddKeyword("C513", "Sonstiges", msKeyGrp, data);

      //... C6 Documentary
      msKey = new MXFKeyword("C6", "Dokumentation");
      data.Keywords.Add(msKey.RefId, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C600", "Alle", msKeyGrp, data);
			AddKeyword("C601", "Wirtschaft", msKeyGrp, data);
			AddKeyword("C602", "Gesellschaft", msKeyGrp, data);
			AddKeyword("C603", "Aktuelle Themen", msKeyGrp, data);
			AddKeyword("C604", "Bildung", msKeyGrp, data);
			AddKeyword("C605", "Umwelt", msKeyGrp, data);
			AddKeyword("C606", "Geschichte", msKeyGrp, data);
			AddKeyword("C607", "Natur", msKeyGrp, data);
			AddKeyword("C608", "Reality-TV", msKeyGrp, data);
			AddKeyword("C609", "Wissenschaft", msKeyGrp, data);
			AddKeyword("C610", "Krieg", msKeyGrp, data);
			AddKeyword("C611", "Sonstiges", msKeyGrp, data);

      msKey = new MXFKeyword("MSEPGC_KIDS", "Kinder");
      data.Keywords.Add(msKey.RefId, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
			AddKeyword("C700", "Alle", msKeyGrp, data);
			AddKeyword("C701", "Animation", msKeyGrp, data);
			AddKeyword("C702", "Komödie", msKeyGrp, data);
			AddKeyword("C703", "Drama", msKeyGrp, data);
			AddKeyword("C704", "Dramaserie", msKeyGrp, data);
			AddKeyword("C705", "Bildung", msKeyGrp, data);
			AddKeyword("C706", "Unterhaltung", msKeyGrp, data);
			AddKeyword("C707", "Umwelt", msKeyGrp, data);
			AddKeyword("C708", "Film", msKeyGrp, data);
			AddKeyword("C709", "Gameshow", msKeyGrp, data);
			AddKeyword("C710", "Magazin", msKeyGrp, data);
			AddKeyword("C711", "Natur", msKeyGrp, data);
			AddKeyword("C712", "Unter 5", msKeyGrp, data);
			AddKeyword("C713", "Jugendkultur", msKeyGrp, data);
			AddKeyword("C714", "Sonstiges", msKeyGrp, data);

      msKey = new MXFKeyword("C8", "Shows & Serien");
      data.Keywords.Add(msKey.RefId, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
			AddKeyword("C800", "Alle", msKeyGrp, data);
			AddKeyword("C801", "Erotik", msKeyGrp, data);
			AddKeyword("C802", "Zeichentrick", msKeyGrp, data);
			AddKeyword("C803", "Komödie", msKeyGrp, data);
			AddKeyword("C804", "Gameshow", msKeyGrp, data);
			AddKeyword("C805", "Sitcoms", msKeyGrp, data);
			AddKeyword("C806", "Soap", msKeyGrp, data);
			AddKeyword("C807", "Talkshow", msKeyGrp, data);
			AddKeyword("C808", "Sonstiges", msKeyGrp, data);

      msKey = new MXFKeyword("MSEPGC_NEWS", "Nachrichten");
      data.Keywords.Add(msKey.RefId, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C900", "Alle", msKeyGrp, data);
			AddKeyword("C901", "Nachrichten", msKeyGrp, data);
			AddKeyword("C902", "Sonstiges", msKeyGrp, data);

      // MSEPGC_SPECIAL?

      // Keyword mapping
      data.KeywordMapping.Add("100.101", "C116,C209");
      data.KeywordMapping.Add("100.102", "C101,C201");
      data.KeywordMapping.Add("100.103", "C101,C201");
      data.KeywordMapping.Add("100.104", "C116");
      data.KeywordMapping.Add("100.105", "C107");
      data.KeywordMapping.Add("100.106", "C102");
      data.KeywordMapping.Add("100.108", "C112,C203");
      data.KeywordMapping.Add("100.109", "C111");
      data.KeywordMapping.Add("100.110", "C105");
      data.KeywordMapping.Add("100.112", "C106,C202");
      data.KeywordMapping.Add("100.113", "C116");
      data.KeywordMapping.Add("100.114", "C113,C204");
      data.KeywordMapping.Add("100.115", "C110");
      data.KeywordMapping.Add("100.116", "C109,C203");
      data.KeywordMapping.Add("100.117", "C111");
      data.KeywordMapping.Add("100.119", "C112,C206");
      data.KeywordMapping.Add("100.121", "C108,C202");
      data.KeywordMapping.Add("100.122", "C115,C208");
      data.KeywordMapping.Add("100.123", "C103");

      data.KeywordMapping.Add("200.201", "C209,C808");
      data.KeywordMapping.Add("200.202", "C201,C808");
      data.KeywordMapping.Add("200.203", "C201,C808");
      data.KeywordMapping.Add("200.205", "C808,C209");
      data.KeywordMapping.Add("200.206", "C801");
      data.KeywordMapping.Add("200.207", "C806");
      data.KeywordMapping.Add("200.208", "C203");
      data.KeywordMapping.Add("200.210", "C805");
      data.KeywordMapping.Add("200.211", "C205,C806");
      data.KeywordMapping.Add("200.212", "C202,C808");
      data.KeywordMapping.Add("200.214", "C803");
      data.KeywordMapping.Add("200.216", "C203,C808");
      data.KeywordMapping.Add("200.218", "C805");
      data.KeywordMapping.Add("200.219", "C206,C808");
      data.KeywordMapping.Add("200.220", "C806");
      data.KeywordMapping.Add("200.221", "C202,C808");
      data.KeywordMapping.Add("200.222", "C208,C808");
      data.KeywordMapping.Add("200.223", "C802");

      data.KeywordMapping.Add("300.301", "C316");
      data.KeywordMapping.Add("300.331", "C302");
      data.KeywordMapping.Add("300.332", "C311");
      data.KeywordMapping.Add("300.334", "C306");
      data.KeywordMapping.Add("300.335", "C316");
      data.KeywordMapping.Add("300.336", "C307");
      data.KeywordMapping.Add("300.337", "C315");
      data.KeywordMapping.Add("300.338", "C311");
      data.KeywordMapping.Add("300.339", "C309");
      data.KeywordMapping.Add("300.340", "C303");
      data.KeywordMapping.Add("300.341", "C312");
      data.KeywordMapping.Add("300.342", "C313");
      data.KeywordMapping.Add("300.343", "C314");
      data.KeywordMapping.Add("300.344", "C310");
      data.KeywordMapping.Add("300.345", "C301");
      data.KeywordMapping.Add("300.346", "C311");
      data.KeywordMapping.Add("300.347", "C305");
      data.KeywordMapping.Add("300.348", "C316");

      data.KeywordMapping.Add("400.401", "C808");
      data.KeywordMapping.Add("400.406", "C801");
      data.KeywordMapping.Add("400.418", "C608");
      data.KeywordMapping.Add("400.450", "C803");
      data.KeywordMapping.Add("400.451", "C808");
      data.KeywordMapping.Add("400.452", "C804");
      data.KeywordMapping.Add("400.453", "C807");
      data.KeywordMapping.Add("400.454", "C808");
      data.KeywordMapping.Add("400.455", "C406");
      data.KeywordMapping.Add("400.456", "C407");
      data.KeywordMapping.Add("400.457", "C406,C408");

      data.KeywordMapping.Add("500.501", "C413,C611");
      data.KeywordMapping.Add("500.560", "C606");
      data.KeywordMapping.Add("500.561", "C603,C902");
      data.KeywordMapping.Add("500.564", "C409");
      data.KeywordMapping.Add("500.565", "C411");
      data.KeywordMapping.Add("500.566", "C901");
      data.KeywordMapping.Add("500.567", "C607");
      data.KeywordMapping.Add("500.568", "C602");
      data.KeywordMapping.Add("500.569", "C406");
      data.KeywordMapping.Add("500.570", "C412");
      data.KeywordMapping.Add("500.571", "C601");
      data.KeywordMapping.Add("500.572", "C609");
      data.KeywordMapping.Add("500.573", "C604,C611");

      data.KeywordMapping.Add("600.601", "C513");
      data.KeywordMapping.Add("600.680", "C508");
      data.KeywordMapping.Add("600.681", "C504");
      data.KeywordMapping.Add("600.682", "C509");
      data.KeywordMapping.Add("600.683", "C510");
      data.KeywordMapping.Add("600.684", "C507");
      data.KeywordMapping.Add("600.685", "C510");
      data.KeywordMapping.Add("600.686", "C510");
      data.KeywordMapping.Add("600.687", "C510");
      data.KeywordMapping.Add("600.688", "C808");
      data.KeywordMapping.Add("600.689", "C513");
      data.KeywordMapping.Add("600.690", "C207,C512");
      data.KeywordMapping.Add("600.691", "C402");
      data.KeywordMapping.Add("600.692", "C513");

      data.KeywordMapping.Add("700.701", "C714");
      data.KeywordMapping.Add("700.790", "C708");
      data.KeywordMapping.Add("700.791", "C710");
      data.KeywordMapping.Add("700.792", "C704");
      data.KeywordMapping.Add("700.793", "C706");
      data.KeywordMapping.Add("700.795", "C701");
      data.KeywordMapping.Add("700.796", "C701");
      
      /* Old stuff
      foreach (XmlNode xmlKeyGroup in xmlKeyDoc.DocumentElement.ChildNodes) {
        MXFKeyword key = new MXFKeyword(xmlKeyGroup, ns);

          data.Keywords.Add(key.RefId, key);

          MXFKeywordGroup keyGrp = new MXFKeywordGroup(key);
          data.KeywordGroups.Add(keyGrp.Id, keyGrp);

          // "All" keyword
          key = new MXFKeyword(keyGrp.Id + "00", "Alle");
          key.Group = keyGrp;
          keyGrp.Keywords.Add(key);
          data.Keywords.Add(key.RefId, key);

          foreach (XmlNode xmlKey in xmlKeyGroup.SelectNodes("ns:term", ns)) {
            key = new MXFKeyword(xmlKey, ns);
            key.Group = keyGrp;
            keyGrp.Keywords.Add(key);

            data.Keywords.Add(key.RefId, key);
          }
        }
      }
      */

      //
      // Load existing persons from EPG DB
      //

      log.Info("Loading people from EPG DB...");
      MCConnection connection = new MCConnection();

      People people = new People(connection.Store);
      IEnumerator enumerator = people.GetEnumerator();
      while (enumerator.MoveNext()) {
        Person epgperson = enumerator.Current as Person;
        if (!string.IsNullOrEmpty(epgperson.Name) && epgperson.GetUIdValue() != null) {
          if (!epgperson.GetUIdValue().StartsWith("!MCPerson!")) {
            log.Error("*** Invalid person: " + epgperson.GetUIdValue());
          } else {
            //log.Debug("Loading [" + epgperson.GetUIdValue() + "] " + epgperson.Name); 
            MXFPerson pers = new MXFPerson(epgperson.GetUIdValue(), epgperson.Name);
            if (data.Persons.ContainsKey(pers.Name)) {
              data.Persons.Remove(pers.Name);              
              MXFPerson persNeu = DeleteDuplicates(pers.Name, connection);
              data.Persons.Add(persNeu.Name, persNeu);
              persNeu.Id = data.PersonId++;
            } else {
              data.Persons.Add(pers.Name, pers);
              pers.Id = data.PersonId++;
            }
          }
        }
      }      

      //
      // Load programs
      //

      /* OLD STUFF

      XmlDocument xmlTVBrowserDoc = new XmlDocument();
      xmlTVBrowserDoc.Load(tvbrowserFile);

      List<string> cats = new List<string>();
      List<string> chrs = new List<string>();
      //Dictionary<string, List<string>> cats = new Dictionary<string, List<string>>();

      DirectoryInfo di = new DirectoryInfo(feedDir + "\\epg");
      foreach (DirectoryInfo ddi in di.GetDirectories()) {
        log.Info("Reading directory " + ddi.FullName);
        // Reset error counter
        data.tvmovieEntriesNotFound = 0;
        data.tvbrowserEntriesNotFound = 0;
        foreach (FileInfo fi in ddi.GetFiles()) {
          XmlDocument xmlProgDoc = new XmlDocument();
          xmlProgDoc.Load(fi.FullName);

          ns = new XmlNamespaceManager(xmlProgDoc.NameTable);
          ns.AddNamespace("ns", "http://www.as-guides.com/schema/epg");

          foreach (XmlNode xmlProg in xmlProgDoc.DocumentElement.ChildNodes) {
            MXFProgram p = new MXFProgram(xmlProg, ns, data, tvmovieConnection, imageDir, tvmImageDir);
            MXFScheduleEntry se = new MXFScheduleEntry(xmlProg, ns, xmlTVBrowserDoc, data);
            p.DebugInfo = ddi.Name + "\\" + fi.Name;
            if (data.Channels.ContainsKey(se.ChannelId)) {
              if (!data.Programs.ContainsKey(p.Id)) {
                data.Programs.Add(p.Id, p);

                // Series
                if (p.SeriesInfo != null) {
                  if (!data.SeriesInfos.ContainsKey(p.SeriesInfo.Id)) {
                    data.SeriesInfos.Add(p.SeriesInfo.Id, p.SeriesInfo);
                  } else {
                    MXFSeriesInfo si = data.SeriesInfos[p.SeriesInfo.Id];
                    if (!p.Title.Equals(si.Title, StringComparison.CurrentCultureIgnoreCase)) {
                      if (!si.AdditionalTitles.ContainsKey(p.Title)) {
                        si.AdditionalTitles.Add(p.Title, p);
                      }
                    }
                  }
                }
                // Collect cats and chrs for testing
                if (!cats.Contains(p.Cat)) {
                  cats.Add(p.Cat);
                }
                if (!chrs.Contains(p.Chr)) {
                  chrs.Add(p.Chr);
                }
              }

              data.Channels[se.ChannelId].ScheduleEntries.Add(se);
            }
          }
        }
        log.Info("TV Movie entries not found: " + data.tvmovieEntriesNotFound + ", TV Browser: " + data.tvbrowserEntriesNotFound);
      }
      */

      // Collect cats and chrs for testing
      /*
      foreach (string c in cats)
        log.Debug(c);
      log.Debug("-------------------------------------");
      foreach (string c in chrs)
        log.Debug(c);
      */

      return data;
    }

    private void AddKeyword(string id, string name, MXFKeywordGroup group, MXFData data) {

      MXFKeyword msKey = new MXFKeyword(id, name);
      msKey.Group = group;
      group.Keywords.Add(msKey);
      data.Keywords.Add(msKey.Id, msKey);
    }

    private MXFPerson DeleteDuplicates(string name, MCConnection connection) {

      // For debugging 
      bool deleteDuplicate = true;
      //bool deleteDuplicate = false;

      log.Debug("*** Duplicate person: " + name);

      People people = new People(connection.Store);
      People dupl = (People)people.WhereFieldIs("Name", name);
      if (dupl.ToList().Count <= 1) {
        throw new Exception("I lost myself " + name);
      }
      foreach (Person p in dupl.ToList()) {
        log.Debug("    Person [" + p.GetUIdValue() + "]: " + p.Name);
      }

      Person personToRemove = null;
      Person personToStay = null;

      // Try preferred uid
      string bestUid = "!MCPerson!" + MXFPerson.CreateUid(name);
      foreach (Person p in dupl.ToList()) {
        if (p.GetUIdValue() == bestUid) {
          personToStay = p;
        } else {
          personToRemove = p;
        }
      }

      if (personToStay == null || personToRemove == null) {
        // Remove person with the right-most space in uid ("Jamie Lee Curtis" - case)
        log.Debug("Preferred uid not found, using right-most space");
        int pos = -1;
        personToStay = null;
        personToRemove = null;
        foreach (Person p in dupl.ToList()) {
          pos = Math.Max(pos, p.GetUIdValue().LastIndexOf(' '));
        }
        if (pos == -1) throw new Exception("No space!");
        foreach (Person p in dupl.ToList()) {
          if (pos == p.GetUIdValue().LastIndexOf(' ')) {
            personToRemove = p;
          } else {
            personToStay = p;
          }
        }
      }

      if (personToStay == null || personToRemove == null) {
        log.Warn("Cannot distinguish persons!");
        personToStay = dupl.ToList()[0];
        if (personToStay == null) throw new Exception("I lost myself again");
        return new MXFPerson(personToStay.GetUIdValue(), personToStay.Name);
      }

      log.Info("*** REMOVE Person [" + personToRemove.GetUIdValue() + "]: " + personToRemove.Name);
      log.Debug("Keep Person [" + personToStay.GetUIdValue() + "]: " + personToStay.Name);
      
      if (deleteDuplicate) {
        foreach (ActorRole r in personToRemove.ActorRoles.ToList()) {
          log.Debug("ActorRole " + r.Id + " " + r.Rank + " " + r.Character + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          //r.Program = null;
          //r.Character = string.Empty;
          r.Update();
          //log.Debug("Removed");
        }
        foreach (DirectorRole r in personToRemove.DirectorRoles.ToList()) {
          log.Debug("DirectorRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          //r.Program = null;
          r.Update();
          //person.DirectorRoles.RemoveAllMatching(r);
          //log.Debug("Removed");
        }
        foreach (GuestActorRole r in personToRemove.GuestActorRoles.ToList()) {
          log.Debug("GuestActorRole " + r.Id + " " + r.Rank + " " + r.Character + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          //r.Program = null;
          //r.Character = string.Empty;
          r.Update();
          //person.GuestActorRoles.RemoveAllMatching(r);
          //log.Debug("Removed");
        }
        foreach (HostRole r in personToRemove.HostRoles.ToList()) {
          log.Debug("HostRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          //r.Program = null;
          r.Update();
          //person.HostRoles.RemoveAllMatching(r);
          //log.Debug("Removed");
        }
        foreach (ProducerRole r in personToRemove.ProducerRoles.ToList()) {
          log.Debug("ProducerRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          //r.Program = null;
          r.Update();
          //person.ProducerRoles.RemoveAllMatching(r);
          //log.Debug("Removed");
        }
        foreach (WriterRole r in personToRemove.WriterRoles.ToList()) {
          log.Debug("WriterRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          //r.Program = null;
          r.Update();
          //person.WriterRoles.RemoveAllMatching(r);
          //log.Debug("Removed");
        }

        personToRemove.Name = string.Empty;
        personToRemove.Update();

        log.Debug("Removed");
      }
      return new MXFPerson(personToStay.GetUIdValue(), personToStay.Name);
    }
  }
}
