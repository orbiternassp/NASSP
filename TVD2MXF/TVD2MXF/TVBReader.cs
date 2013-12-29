using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.Xml;
using System.Data.SqlClient;
using System.Data.OleDb;
using Microsoft.MediaCenter.Guide;
using Microsoft.MediaCenter.Store;

namespace TVD2MXF {
  class TVBReader {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    private SqlConnection connection;
    private OleDbConnection tvmovieConnection;

    public MXFData Read(SqlConnection con, OleDbConnection tvmCon, string tvmImageDir, MCConnection mcconnection) {
      connection = con;
      tvmovieConnection = tvmCon;

      // Load channels
      XmlDocument xmlChannelDoc = new XmlDocument();
      xmlChannelDoc.Load(Program.WorkingDir + "\\Channels\\Channels.xml");

      XmlNamespaceManager ns = new XmlNamespaceManager(xmlChannelDoc.NameTable);
      ns.AddNamespace("ns", "http://www.as-guides.com/schema/channel");

      XmlElement e = xmlChannelDoc.DocumentElement;
      XmlNodeList xmlChannels = e.ChildNodes;
      log.Info("Reading " + xmlChannels.Count + " Channels");

      MXFData data = new MXFData();
      foreach (XmlNode xmlChannel in xmlChannels) {
        //Get all the channels
        if (xmlChannel.Name == "Ch") {
          MXFChannel ch = new MXFChannel(xmlChannel, ns);
          data.Channels.Add(ch.Id, ch);

          // Channel image
          MXFGuideImage img = new MXFGuideImage(data.GuideImages.Count, Program.WorkingDir + "\\Channels\\Logos\\ch" + ch.Id + ".png");
          ch.GuideImage = img;
          data.GuideImages.Add(img);
        }
      }


      //
      // Keywords
      //

      log.Info("Loading keywords and fixing programs in EPG DB...");

      LoadKeywords(data);

      SqlCommand sql = new SqlCommand("Update Keyword set ProgramCount = 0", connection);
      sql.ExecuteNonQuery();


      /*
      KeywordGroups kwgs = new KeywordGroups(mcconnection.Store);
      foreach (KeywordGroup kwg in kwgs) {
        log.Debug(kwg.Name + " [" + kwg.GetUIdValue() + "]");
        foreach (Keyword kw in kwg.Keywords) {
          log.Debug("    " + kw.Word);
        }
      }
      */

  
      // Removing wrong keywords     
      CleanupPrograms(mcconnection);

      //
      // Load existing persons from EPG DB
      //

      log.Info("Loading people from EPG DB...");
      People people = new People(mcconnection.Store);
      IEnumerator enumerator = people.GetEnumerator();
      while (enumerator.MoveNext()) {
        Person epgperson = enumerator.Current as Person;
        if (!string.IsNullOrEmpty(epgperson.Name) && epgperson.GetUIdValue() != null) {
          if (!epgperson.GetUIdValue().StartsWith("!MCPerson!")) {
            log.Error("*** Invalid person: " + epgperson.GetUIdValue());
          } else {
            //log.Debug("Loading [" + epgperson.GetUIdValue() + "] " + epgperson.Name); 
            MXFPerson pers = new MXFPerson(epgperson.GetUIdValue(), epgperson.Name);
            // Delete invalid persons
            if ((pers.Name.Contains("(") || pers.Name.Contains(")")) && !(pers.Name.Contains("(I)") || pers.Name.Contains("(II)") || pers.Name.Contains("(III)"))) {
              DeletePerson(epgperson, mcconnection);
            } else {
              if (data.Persons.ContainsKey(pers.Name)) {
                data.Persons.Remove(pers.Name);
                MXFPerson persNeu = DeleteDuplicates(pers.Name, mcconnection);
                data.Persons.Add(persNeu.Name, persNeu);
                persNeu.Id = data.PersonId++;
              } else {
                data.Persons.Add(pers.Name, pers);
                pers.Id = data.PersonId++;
              }
            }
          }
        }
      }

      string tvbrowserFile = Program.WorkingDir + "\\TVBData.xml";
      XmlDocument xmlTvb = new XmlDocument();
      xmlTvb.Load(tvbrowserFile);

      foreach (MXFChannel ch in data.Channels.Values) {
        if (!string.IsNullOrEmpty(ch.TVBrowserName)) {
          XmlNode xmlChannel = xmlTvb.DocumentElement.SelectSingleNode("channel[name='" + ch.TVBrowserName + "']");
          if (xmlChannel != null) {
            XmlNode n = xmlChannel.SelectSingleNode("name");
            log.Info("Reading channel " + n.InnerText);
            ch.VerifyTVMovie(tvmovieConnection);

            XmlNodeList xmlProgs = xmlChannel.SelectNodes("programs/program");

            // Special Hack Sky Hits -> Sky Christmas
            if (ch.TVBrowserName == "Sky Hits" && xmlProgs.Count == 0) {
              log.Info("Using Sky Christmas instead of Sky Hits");
              xmlChannel = xmlTvb.DocumentElement.SelectSingleNode("channel[name='Sky Christmas']");
              if (xmlChannel != null) {
                xmlProgs = xmlChannel.SelectNodes("programs/program");
              }
            }

            foreach (XmlNode xmlProg in xmlProgs) {
              MXFProgram prog = new MXFProgram(ch, xmlProg, connection, tvmovieConnection, data, tvmImageDir);
              if (!data.Programs.ContainsKey(prog.Id)) {
                data.Programs.Add(prog.Id, prog);
                // Series
                if (prog.EpisodeInfo != null) {
                  MXFSeriesInfo si = prog.EpisodeInfo.SeriesInfo;
                  if (!data.SeriesInfos.ContainsKey(si.Id)) {
                    data.SeriesInfos.Add(si.Id, si);
                  }
                }
              } else {
                prog = data.Programs[prog.Id];
              }
              MXFScheduleEntry se = new MXFScheduleEntry(ch, prog, xmlProg);
              data.Channels[ch.Id].ScheduleEntries.Add(se);
            }
            log.Info("TV Movie entries not found: " + data.tvmovieEntriesNotFound);
            data.tvmovieEntriesNotFound = 0;
          } else {
            log.Error("TV Browser channel " + ch.TVBrowserName + " not found.");
          }

        } else {
          // München TV etc. gibt es wirklich nicht
          if (ch.Name == "München TV" || ch.Name == "VH1 Classic") {
            log.Info("TV Browser channel doesn't exist: " + ch.Name);
          } else {
            log.Error("TV Browser channel not mapped: " + ch.Name);
          }
        }
      }
      return data;
    }

    /*
    private SimMetricsApi.AbstractStringMetric[] GetComparers() {

      SimMetricsApi.AbstractStringMetric[] cmps = new SimMetricsApi.AbstractStringMetric[9];
      //cmps[0] = new SimMetricsMetricUtilities.Levenstein();
      cmps[0] = new SimMetricsMetricUtilities.BlockDistance();
      //cmps[2] = new SimMetricsMetricUtilities.ChapmanLengthDeviation();
      //cmps[3] = new SimMetricsMetricUtilities.ChapmanMeanLength();
      cmps[1] = new SimMetricsMetricUtilities.CosineSimilarity();
      cmps[2] = new SimMetricsMetricUtilities.DiceSimilarity();
      //cmps[3] = new SimMetricsMetricUtilities.EuclideanDistance();
      //cmps[4] = new SimMetricsMetricUtilities.JaccardSimilarity();
      //cmps[8] = new SimMetricsMetricUtilities.Jaro();
      //cmps[9] = new SimMetricsMetricUtilities.JaroWinkler();
      cmps[3] = new SimMetricsMetricUtilities.MatchingCoefficient();
      //cmps[11] = new SimMetricsMetricUtilities.MongeElkan();
      //cmps[12] = new SimMetricsMetricUtilities.NeedlemanWunch();
      cmps[4] = new SimMetricsMetricUtilities.OverlapCoefficient();
      cmps[5] = new SimMetricsMetricUtilities.QGramsDistance();
      cmps[6] = new SimMetricsMetricUtilities.SmithWaterman();
      cmps[7] = new SimMetricsMetricUtilities.SmithWatermanGotoh();
      cmps[8] = new SimMetricsMetricUtilities.SmithWatermanGotohWindowedAffine();
      return cmps;
    }
    */

    private void DeletePerson(Person p, MCConnection connection) {
      log.Warn("Deleting Person [" + p.GetUIdValue() + "]: " + p.Name);

      foreach (ActorRole r in p.ActorRoles.ToList()) {
        log.Debug("ActorRole " + r.Id + " " + r.Rank + " " + r.Character + " " + r.Program.Title + " " + r.Program.GetUIdValue());
        r.Person = null;
        r.Program = null;
        r.Character = string.Empty;
        r.Update();
      }      
      foreach (DirectorRole r in p.DirectorRoles.ToList()) {
        log.Debug("DirectorRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
        r.Person = null;
        r.Program = null;
        r.Update();
      }      
      foreach (GuestActorRole r in p.GuestActorRoles.ToList()) {
        log.Debug("GuestActorRole " + r.Id + " " + r.Rank + " " + r.Character + " " + r.Program.Title + " " + r.Program.GetUIdValue());
        r.Person = null;
        r.Program = null;
        r.Character = string.Empty;
        r.Update();
      }      
      foreach (HostRole r in p.HostRoles.ToList()) {
        log.Debug("HostRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
        r.Person = null;
        r.Program = null;
        r.Update();
      }
      foreach (ProducerRole r in p.ProducerRoles.ToList()) {
        log.Debug("ProducerRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
        r.Person = null;
        r.Program = null;
        r.Update();
      }
      foreach (WriterRole r in p.WriterRoles.ToList()) {
        log.Debug("WriterRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
        r.Person = null;
        r.Program = null;
        r.Update();
      }
      
      p.Name = string.Empty;
      p.Update();
    }

    private MXFPerson DeleteDuplicates(string name, MCConnection connection) {

      bool deleteDuplicate = true;
      // For debugging 
      // bool deleteDuplicate = false;

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
        log.Warn("Cannot distinguish persons, keeping first person...");
        personToStay = dupl.ToList()[0];
        personToRemove = dupl.ToList()[1];
        if (personToStay == null) throw new Exception("I lost myself again (stay)");
        if (personToRemove == null) throw new Exception("I lost myself again (remove)");

        //return new MXFPerson(personToStay.GetUIdValue(), personToStay.Name);
      }

      log.Info("*** REMOVE Person [" + personToRemove.GetUIdValue() + "]: " + personToRemove.Name);
      log.Info("Keep Person [" + personToStay.GetUIdValue() + "]: " + personToStay.Name);

      if (deleteDuplicate) {
        foreach (ActorRole r in personToRemove.ActorRoles.ToList()) {
          log.Debug("ActorRole " + r.Id + " " + r.Rank + " " + r.Character + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          r.Update();
        }
        foreach (DirectorRole r in personToRemove.DirectorRoles.ToList()) {
          log.Debug("DirectorRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          r.Update();
        }
        foreach (GuestActorRole r in personToRemove.GuestActorRoles.ToList()) {
          log.Debug("GuestActorRole " + r.Id + " " + r.Rank + " " + r.Character + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          r.Update();
        }
        foreach (HostRole r in personToRemove.HostRoles.ToList()) {
          log.Debug("HostRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          r.Update();
        }
        foreach (ProducerRole r in personToRemove.ProducerRoles.ToList()) {
          log.Debug("ProducerRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          r.Update();
        }
        foreach (WriterRole r in personToRemove.WriterRoles.ToList()) {
          log.Debug("WriterRole " + r.Id + " " + r.Rank + " " + r.Program.Title + " " + r.Program.GetUIdValue());
          r.Person = personToStay;
          r.Update();
       }

        personToRemove.Name = string.Empty;
        personToRemove.Update();

        log.Debug("Removed");
      }
      return new MXFPerson(personToStay.GetUIdValue(), personToStay.Name);
    }

    private void LoadKeywords(MXFData data) {

      // Stream EPG Keywords
      MXFKeyword msKey = new MXFKeyword("MSEPGC_MOVIES", "Spielfilm");
      data.Keywords.Add(msKey.Id, msKey);
      MXFKeywordGroup msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C100", "Alle", msKeyGrp, data);
      AddKeyword("C101", "Sonstiges", msKeyGrp, data);
      AddKeyword("C102", "Thriller", msKeyGrp, data);
      AddKeyword("C103", "Abenteuer-Western", msKeyGrp, data);
      AddKeyword("C104", "Sci-Fi-Horror", msKeyGrp, data);
      AddKeyword("C105", "Comedy", msKeyGrp, data);
      AddKeyword("C106", "Soap-Melodrama", msKeyGrp, data);
      AddKeyword("C107", "Romanze", msKeyGrp, data);
      AddKeyword("C108", "Classisch-Historisch", msKeyGrp, data);

      msKey = new MXFKeyword("C2", "Shows & Serien");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C200", "Alle", msKeyGrp, data);
      AddKeyword("C201", "Sonstiges", msKeyGrp, data);
      AddKeyword("C202", "Science-Fiction", msKeyGrp, data);
      AddKeyword("C203", "Fantasy", msKeyGrp, data);
      AddKeyword("C204", "Krimi", msKeyGrp, data);
      AddKeyword("C205", "Aktion", msKeyGrp, data);
      AddKeyword("C206", "Sitcoms", msKeyGrp, data);
      AddKeyword("C207", "Soap", msKeyGrp, data);
      AddKeyword("C208", "Zeichentrick", msKeyGrp, data);
      AddKeyword("C209", "Komödie", msKeyGrp, data);
      AddKeyword("C210", "Talkshow", msKeyGrp, data);
      AddKeyword("C211", "Spielshow", msKeyGrp, data);

      msKey = new MXFKeyword("C3", "Bildung & Wissenschaft");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C300", "Alle", msKeyGrp, data);
      AddKeyword("C301", "Sonstiges", msKeyGrp, data);
      AddKeyword("C302", "Tiere & Natur", msKeyGrp, data);
      AddKeyword("C303", "Technologie-Naturwissenschaft", msKeyGrp, data);
      AddKeyword("C304", "Medizin-Psychologie", msKeyGrp, data);
      AddKeyword("C305", "Expeditionen & andere Länder", msKeyGrp, data);
      AddKeyword("C306", "Soziales & Spirituelles", msKeyGrp, data);
      AddKeyword("C307", "Sprachen", msKeyGrp, data);
      AddKeyword("C308", "Weiterbildung", msKeyGrp, data);

      msKey = new MXFKeyword("C4", "Freizeit & Hobbies");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C400", "Alle", msKeyGrp, data);
      AddKeyword("C401", "Sonstiges", msKeyGrp, data);
      AddKeyword("C402", "Tourismus & Reisen", msKeyGrp, data);
      AddKeyword("C403", "Motorisierung", msKeyGrp, data);
      AddKeyword("C404", "Fitness & Gesundheit", msKeyGrp, data);
      AddKeyword("C405", "Kochen", msKeyGrp, data);
      AddKeyword("C406", "Einkaufen & Werbung", msKeyGrp, data);
      AddKeyword("C407", "Im Garten", msKeyGrp, data);
      AddKeyword("C408", "Handarbeit", msKeyGrp, data);

      msKey = new MXFKeyword("MSEPGC_KIDS", "Kinder & Jugendliche");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C500", "Alle", msKeyGrp, data);
      AddKeyword("C501", "Sonstiges", msKeyGrp, data);
      AddKeyword("C502", "Vorschulsendung", msKeyGrp, data);
      AddKeyword("C503", "Unterhaltung von 6 bis 14", msKeyGrp, data);
      AddKeyword("C504", "Unterhaltung von 10 bis 16", msKeyGrp, data);
      AddKeyword("C505", "Information-Lernen", msKeyGrp, data);
      AddKeyword("C506", "Trickfilm", msKeyGrp, data);

      msKey = new MXFKeyword("C6", "Kunst & Kultur");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C600", "Alle", msKeyGrp, data);
      AddKeyword("C601", "Sonstiges", msKeyGrp, data);
      AddKeyword("C602", "Darstellende Kunst", msKeyGrp, data);
      AddKeyword("C603", "Religion", msKeyGrp, data);
      AddKeyword("C604", "Literatur", msKeyGrp, data);
      AddKeyword("C605", "Kino", msKeyGrp, data);
      AddKeyword("C606", "Presse", msKeyGrp, data);
      AddKeyword("C607", "Kunstmagazin", msKeyGrp, data);
      AddKeyword("C608", "Neue Medien", msKeyGrp, data);
      AddKeyword("C609", "Mode & Kleidung", msKeyGrp, data);

      msKey = new MXFKeyword("C7", "Musik-Tanzen");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C700", "Alle", msKeyGrp, data);
      AddKeyword("C701", "Sonstiges", msKeyGrp, data);
      AddKeyword("C702", "Rock-Pop", msKeyGrp, data);
      AddKeyword("C703", "Classische Musik", msKeyGrp, data);
      AddKeyword("C704", "Folksmusik", msKeyGrp, data);
      AddKeyword("C705", "Jazz", msKeyGrp, data);
      AddKeyword("C706", "Musical-Oper", msKeyGrp, data);
      AddKeyword("C707", "Ballet", msKeyGrp, data);

      msKey = new MXFKeyword("MSEPGC_NEWS", "Nachrichten");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C800", "Alle", msKeyGrp, data);
      AddKeyword("C801", "Sonstiges", msKeyGrp, data);
      AddKeyword("C802", "Nachrichten", msKeyGrp, data);
      AddKeyword("C803", "Wetterbericht", msKeyGrp, data);
      AddKeyword("C804", "Unterhaltungsmagazin", msKeyGrp, data);
      AddKeyword("C805", "Dokumentarfilm", msKeyGrp, data);
      AddKeyword("C806", "Diskussion-Interview", msKeyGrp, data);

      msKey = new MXFKeyword("C9", "Soziales & Politik");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C900", "Alle", msKeyGrp, data);
      AddKeyword("C901", "Sonstiges", msKeyGrp, data);
      AddKeyword("C902", "Reportage & Dokumentation", msKeyGrp, data);
      AddKeyword("C903", "Wirtschaft", msKeyGrp, data);
      AddKeyword("C904", "Bemerkenswerte Leute", msKeyGrp, data);

      msKey = new MXFKeyword("MSEPGC_SPECIAL", "Spezial");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C1000", "Alle", msKeyGrp, data);
      AddKeyword("C1001", "Sonstiges", msKeyGrp, data);
      AddKeyword("C1002", "Ursprachen", msKeyGrp, data);
      AddKeyword("C1003", "Schwarz & Weiss", msKeyGrp, data);
      AddKeyword("C1004", "Unveröffentlicht", msKeyGrp, data);
      AddKeyword("C1005", "Live Sendung", msKeyGrp, data);

      msKey = new MXFKeyword("MSEPGC_SPORTS", "Sport");
      data.Keywords.Add(msKey.Id, msKey);
      msKeyGrp = new MXFKeywordGroup(msKey);
      data.KeywordGroups.Add(msKeyGrp.Id, msKeyGrp);
      AddKeyword("C1100", "Alle", msKeyGrp, data);
      AddKeyword("C1101", "Sonstiges", msKeyGrp, data);
      AddKeyword("C1102", "Olympische Spiele-Weltcup", msKeyGrp, data);
      AddKeyword("C1103", "Sport Magazin", msKeyGrp, data);
      AddKeyword("C1104", "Fussball", msKeyGrp, data);
      AddKeyword("C1105", "Tennis", msKeyGrp, data);
      AddKeyword("C1106", "Gruppensport", msKeyGrp, data);
      AddKeyword("C1107", "Athletik", msKeyGrp, data);
      AddKeyword("C1108", "Motorsport", msKeyGrp, data);
      AddKeyword("C1109", "Wassersport", msKeyGrp, data);
      AddKeyword("C1110", "Wintersport", msKeyGrp, data);
    }

    private void AddKeyword(string id, string name, MXFKeywordGroup group, MXFData data) {
      MXFKeyword msKey = new MXFKeyword(id, name);
      msKey.Group = group;
      group.Keywords.Add(msKey);
      data.Keywords.Add(msKey.Id, msKey);
    }

    private void CleanupPrograms(MCConnection mcconnection) {
      bool finished = false;
      do {
        finished = true;
        Programs progs = new Programs(mcconnection.Store);
        foreach (Microsoft.MediaCenter.Guide.Program p in progs) {
          /*
          log.Debug("Handling program " + p.Title + ", " + p.GetUIdValue());
          foreach (ScheduleEntry se in p.ScheduleEntries) {
            log.Debug("    SE " + se.StartTime.ToString());
          }
          */
          bool clear = false;
          if (p.GetUIdValue() == null) {
            clear = true;
          } else if (!p.GetUIdValue().StartsWith("!Loaders!Glid!Programs!") && !p.GetUIdValue().StartsWith("!Program!tvtools")) {
            clear = true;
          }
          if (clear) {
            if (p.Keywords.First != null) {
              p.Keywords.Clear();
              p.Update();
              finished = false;
            }
          }
        }
      } while (finished == false);

      /*
      Services ss = new Services(mcconnection.Store);
      foreach (Service service in ss) {
        if (service.GetUIdValue() != null) {
          if (service.GetUIdValue().EndsWith(".tvd2mxf.org")) {
            log.Debug(service.Name + " - " + service.ScheduleEntries.ToArray().Length);

            //DateTime dtEnd = DateTime.Now.Subtract(new TimeSpan(TimeSpan.TicksPerDay)).ToUniversalTime();
            //DateTime dtEnd = DateTime.MaxValue;
            DateTime dtEnd = DateTime.Now.Add(new TimeSpan(2 * TimeSpan.TicksPerDay)).ToUniversalTime();

            ScheduleEntry[] scheduleEntriesBetween = service.GetScheduleEntriesBetween(DateTime.MinValue, dtEnd);
            log.Debug("SEs: " + scheduleEntriesBetween.Length);
            List<ScheduleEntry> entries = new List<ScheduleEntry>();
            foreach (ScheduleEntry entry in scheduleEntriesBetween) {
              if (entry.EndTime <= dtEnd) {
                entries.Add(entry);
              }
            }
            CleanupService(service, entries);

            return;
          }
        }
      }
      */

    }

    /*
    private void CleanupService(Service channel_service, List<ScheduleEntry> entries) {
      using (List<ScheduleEntry>.Enumerator enumerator = entries.GetEnumerator()) {
        while (enumerator.MoveNext()) {
          ScheduleEntry entry = enumerator.Current;
          if ((entry.ProgramContent == null) && entry.Program.ProgramContents.Empty) {
            ClearProgram(entry.Program);
          }
          
          UpdateDelegate operation = delegate
          {
            entry.Service = null;
            if (entry.ProgramContent == null) {
              entry.Program = null;
            }
            entry.Unlock();
          };
          entry.Update(operation);
          
          //entry.Service = null;
          //entry.Program = null;
          //entry.Update();

        }
        channel_service.Update();
      }
    }

    private void ClearProgram(Microsoft.MediaCenter.Guide.Program program) {

      log.Debug("Program: " + program.Title);

      program.Title = string.Empty;
      program.ShortDescription = string.Empty;
      program.Description = string.Empty;
      program.EpisodeTitle = string.Empty;
      program.Actors = string.Empty;
      IEnumerator enumerator = program.ActorRoles.GetEnumerator();
      if (enumerator != null) {
        while (enumerator.MoveNext()) {
          ActorRole role = enumerator.Current as ActorRole;
          UpdateDelegate operation = delegate
          {
            role.Program = null;
          };
          role.Update(operation);
        }
      }
      program.Directors = string.Empty;
      enumerator = program.DirectorRoles.GetEnumerator();
      if (enumerator != null) {
        while (enumerator.MoveNext()) {
          DirectorRole role = enumerator.Current as DirectorRole;
          UpdateDelegate delegate3 = delegate
          {
            role.Program = null;
          };
          role.Update(delegate3);
        }
      }
      program.GuestActors = string.Empty;
      enumerator = program.GuestActorRoles.GetEnumerator();
      if (enumerator != null) {
        while (enumerator.MoveNext()) {
          GuestActorRole role = enumerator.Current as GuestActorRole;
          UpdateDelegate delegate4 = delegate
          {
            role.Program = null;
          };
          role.Update(delegate4);
        }
      }
      program.Producers = string.Empty;
      enumerator = program.ProducerRoles.GetEnumerator();
      if (enumerator != null) {
        while (enumerator.MoveNext()) {
          ProducerRole role = enumerator.Current as ProducerRole;
          UpdateDelegate delegate5 = delegate
          {
            role.Program = null;
          };
          role.Update(delegate5);
        }
      }
      program.Writers = string.Empty;
      enumerator = program.WriterRoles.GetEnumerator();
      if (enumerator != null) {
        while (enumerator.MoveNext()) {
          WriterRole role = enumerator.Current as WriterRole;
          UpdateDelegate delegate6 = delegate
          {
            role.Program = null;
          };
          role.Update(delegate6);
        }
      }
      program.Keywords.Clear();
      program.Language = string.Empty;
      program.GuideImage = null;
      program.Year = 0;
      program.EpisodeNumber = 0;
      program.SeasonNumber = 0;
      program.HalfStars = 0;
      program.Series = null;
      program.IsSeries = false;
      program.OriginalAirdate = new DateTime();
      program.IsAction = false;
      program.HasAdult = false;
      program.IsComedy = false;
      program.IsDocumentary = false;
      program.IsDrama = false;
      program.IsEducational = false;
      program.IsHorror = false;
      program.IsKids = false;
      program.IsMovie = false;
      program.IsMusic = false;
      program.IsNews = false;
      program.IsReality = false;
      program.IsRomance = false;
      program.IsSerial = false;
      program.IsScienceFiction = false;
      program.IsSoap = false;
      program.IsSpecial = false;
      program.IsSports = false;
      program.IsThriller = false;
      program.Update();
    }
    */
  }
}
