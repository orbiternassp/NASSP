using System;
using System.Collections.Generic;
using System.Data.OleDb;
using System.Data.SqlClient;
using System.Text.RegularExpressions;
using System.Xml;


namespace TVD2MXF {
  class MXFProgram {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    private XmlNode xmlProgram;

    private long? programId = null;
    private long? seriesId = null;
    private MXFEpisodeInfo episodeInfo = null;

    private string tvmTitel;
    private string tvmOriginaltitel;
    private string tvmKurzBeschreibung;
    private string tvmBilddateiname;
    private string tvmKurzkritik;
    private string tvmBewertungen;
    private string tvmBeschreibung;
    private string tvmRegie;
    private string tvmDarsteller;
    private string tvmGenre;
    private string tvmKategorie;
    private string tvmHerstellungsjahr;
    private string tvmHerstellungsland;
    private int tvmBewertung;

    private SqlConnection connection;

    private Dictionary<string, MXFKeyword> keywords = new Dictionary<string, MXFKeyword>();
    private List<string> inputKeywords = new List<string>();
    private List<MXFRole> roles = new List<MXFRole>();
    private MXFGuideImage guideImage;

    // TODO Set to channel + starttime
    public string DebugInfo;


    public MXFProgram(MXFChannel ch, XmlNode p, SqlConnection con, OleDbConnection tvmConnection, MXFData data, string tvmImageDir) {
      xmlProgram = p;
      connection = con;

      //log.Debug("Reading program " + xmlProgram.Attributes.GetNamedItem("starttime").Value + ": " + this.Title);

      if (string.IsNullOrEmpty(this.Title)) {
        //log.Error("Program without title");
        throw new Exception("Program without title");
      }


      //
      // TV Movie data
      //

      if (ch.TVMovieName != "") {
        string sqlTime = "#" + this.StartTime.ToString("MM\\/dd\\/yyyy HH:mm:ss") + "#";

        OleDbCommand command = new OleDbCommand("Select * from Sendungen where SenderKennung = '" + ch.TVMovieName + "' and Beginn = " + sqlTime, tvmConnection);
        OleDbDataReader reader = command.ExecuteReader();
        if (reader.Read()) {
          string tvmNT = NormalizeTitle((string)reader["Titel"]);
          string nt = NormalizedTitle;

          bool similar = false;
          double match = -1;
          if (nt == tvmNT || nt.StartsWith(tvmNT) || nt.EndsWith(tvmNT) || tvmNT.StartsWith(nt) || tvmNT.EndsWith(nt)) {
            similar = true;
          } else {
            SimMetricsApi.AbstractStringMetric comp = new SimMetricsMetricUtilities.SmithWaterman();
            match = comp.GetSimilarity(nt, tvmNT);
            if (match > 0.5) {
              similar = true;
            }
          }
          if (similar) {
            tvmTitel = DecodeEntities((string)reader["Titel"]);
            // Originaltitle is episode title sometimes...
            // tvmOriginaltitel = DecodeEntities((string)reader["Originaltitel"]);
            tvmOriginaltitel = null; 
            tvmKurzBeschreibung = DecodeEntities((string)reader["KurzBeschreibung"]);
            tvmBilddateiname = (string)reader["Bilddateiname"];
            tvmKurzkritik = DecodeEntities((string)reader["Kurzkritik"]);
            tvmBewertungen = (string)reader["Bewertungen"];
            tvmRegie = DecodeEntities((string)reader["Regie"]);
            tvmGenre = (string)reader["Genre"];
            tvmKategorie = (string)reader["Kategorietext"];
            tvmHerstellungsjahr = (string)reader["Herstellungsjahr"];
            tvmHerstellungsland = (string)reader["Herstellungsland"];
            tvmBewertung = (int)TVUtils.N2V(reader["Bewertung"], 0);

            OleDbCommand commandDetail = new OleDbCommand("Select * from SendungenDetails where Pos = " + reader["Pos"].ToString(), tvmConnection);
            OleDbDataReader readerDetail = commandDetail.ExecuteReader();
            if (readerDetail.Read()) {
              tvmBeschreibung = (string)readerDetail["Beschreibung"];
              // \n und <br> ersetzen
              tvmBeschreibung = tvmBeschreibung.Replace("\n\n\n", Environment.NewLine);
              tvmBeschreibung = tvmBeschreibung.Replace("\n\n", Environment.NewLine);
              tvmBeschreibung = tvmBeschreibung.Replace("\n", Environment.NewLine);
              tvmBeschreibung = tvmBeschreibung.Replace("<br><br><br>", Environment.NewLine);
              tvmBeschreibung = tvmBeschreibung.Replace("<br><br>", Environment.NewLine);
              tvmBeschreibung = tvmBeschreibung.Replace("<br>", Environment.NewLine);
              tvmBeschreibung = DecodeEntities(tvmBeschreibung);

              tvmDarsteller = DecodeEntities((string)readerDetail["Darsteller"]);
            }
            readerDetail.Close();

          } else {
            log.Debug("TVMovie title mismatch: " + ch.Name + " at " + StartTime.ToString() + ": " + Title + " # " + reader["Titel"] + " [" + match + "]");
          }

        } else {
          log.Debug("TVMovie Sendung not found: " + ch.Name + " at " + StartTime.ToString() + ": " + Title);
          data.tvmovieEntriesNotFound++;
        }
        reader.Close();

      // Nicht mehr nötig...
      // } else {
      //  log.Error("TVMovie channel doesn't exist: " + ch.Name);
      }


      //
      // GuideImages
      //
      
      guideImage = null;
      if (!string.IsNullOrEmpty(tvmBilddateiname)) {
        if (System.IO.File.Exists(tvmImageDir + "\\" + tvmBilddateiname)) {
          MXFGuideImage img = new MXFGuideImage(data.GuideImages.Count, tvmImageDir + "\\" + tvmBilddateiname);
          guideImage = img;
          data.GuideImages.Add(img);
        } else {
          log.Error("GuideImage not found: " + tvmImageDir + "\\" + tvmBilddateiname);
        }
      }

      //
      // Roles / Persons
      //

      if (!string.IsNullOrEmpty(tvmRegie) && tvmRegie != "N. N. N. N.") {
        AddRole(tvmRegie.Trim(), MXFRole.TYPE_DirectorRole, null, data);
      } else {
        AddTVBRole("director", MXFRole.TYPE_DirectorRole, data);
      }

      // TV Movie with character names
      if (!string.IsNullOrEmpty(tvmDarsteller) && tvmDarsteller.Contains("(")) {
        List<string[]> specialRoles = new List<string[]>();
        string[] drs = tvmDarsteller.Split(';');
        foreach (string dr in drs) {
          string[] dar = dr.Split('(');
          string d = dar[0].Trim();
          string r = "";
          if (dar.Length > 1) {
            if (dar[1].Length > 0) {
              r = dar[1].Substring(0, dar[1].Length - 1).Trim();
            } else {
              log.Warn("Malformed TV Movie Darsteller: " + dr);
            }
            // For testing
            /*
            SqlCommand insertRoles = new SqlCommand("Insert into Role(Name) Values (@a)", connection);
            insertRoles.Parameters.Add(new SqlParameter("@a", r));
            insertRoles.ExecuteNonQuery();
            */
          }
          string rt = TVMCharakterToRoleType(r);
          string[] dds = d.Split(',');
          foreach (string dd in dds) {
            if (rt == MXFRole.TYPE_Special) {
              specialRoles.Add(new string[] { dd.Trim(), r });
            } else if (rt == MXFRole.TYPE_ActorRole || rt == MXFRole.TYPE_GuestActorRole) {
              AddRole(dd.Trim(), rt, r, data);
            } else {
              AddRole(dd.Trim(), rt, null, data);
            }
          }
        }
        foreach (string[] dr in specialRoles) {
          AddRole(dr[0], MXFRole.TYPE_ActorRole, dr[1], data);
        }

      // TV Browser with character names
      } else if (!string.IsNullOrEmpty(GetTVBField("actor list")) && GetTVBField("actor list").Contains("\t\t-\t\t")) {
        string[] drs = GetTVBField("actor list").Split(new string[] {",\n", "\n", ","}, StringSplitOptions.RemoveEmptyEntries);
        foreach (string dr in drs) {
          string[] dar = dr.Split(new string[] {"\t\t-\t\t"}, StringSplitOptions.RemoveEmptyEntries);
          string d = dar[0].Trim();
          string r = "";
          if (dar.Length > 1) {
            r = dar[1].Trim();
          }
          AddRole(d, MXFRole.TYPE_ActorRole, r, data);
        }
        AddTVBRoles(data);

      // TV Browser with character names 2
      } else if (!string.IsNullOrEmpty(GetTVBField("actor list")) && GetTVBField("actor list").Contains("(")) {
        string[] drs = GetTVBField("actor list").Split(new string[] {",\n", "\n", ","}, StringSplitOptions.RemoveEmptyEntries);
        foreach (string dr in drs) {
          string[] dar = dr.Split(new string[] { "(", ")" }, StringSplitOptions.RemoveEmptyEntries);
          if (dar.Length > 1) {
            string d1 = dar[0].Trim();
            string d2 = dar[1].Trim(); ;
            if (d1.Length > 0 && d2.Length > 0) {
              // Sometimes d1 is the actor, sometimes d2...
              if (data.Persons.ContainsKey(d1)) {
                AddRole(d1, MXFRole.TYPE_ActorRole, d2, data);
              } else if (data.Persons.ContainsKey(d2)) {
                AddRole(d2, MXFRole.TYPE_ActorRole, d1, data);
              } else {
                AddRole(d1, MXFRole.TYPE_ActorRole, d2, data);
              }
            } else {
              log.Warn("Malformed TV Browser Darsteller: " + dr);
            }
          }
        }
        AddTVBRoles(data);

      } else if (!string.IsNullOrEmpty(tvmDarsteller)) {
        string[] drs = tvmDarsteller.Split(';');
        foreach (string dr in drs) {
          AddRole(dr.Trim(), MXFRole.TYPE_ActorRole, null, data);
        }
      } else if (!string.IsNullOrEmpty(GetTVBField("actor list"))) {
        string[] drs = GetTVBField("actor list").Split(new string[] {",\n", "\n", ","}, StringSplitOptions.RemoveEmptyEntries);
        foreach (string dr in drs) {
          AddRole(dr.Trim(), MXFRole.TYPE_ActorRole, null, data);
        }
        AddTVBRoles(data);
      }


      //
      // Determine Program and Series IDs
      //

      DetermineIds(ch.Name, data);


      //
      // Keywords
      //

      ProcessKeyword(tvmGenre, data);
      ProcessKeyword(tvmKategorie, data);
      ProcessKeyword(GetTVBField("genre"), data);

      // Spezial handling "Thriller" etc.
      // they can be both series and movie

      if (keywords.Count == 0) {
        if (inputKeywords.Contains("Thriller")) 
          AddMovieOrSeriesKeyword("C102", "C205", data);

        if (inputKeywords.Contains("Fantasy/Action") || inputKeywords.Contains("Horror") || inputKeywords.Contains("Fantasyabenteuer") || inputKeywords.Contains("Fantasykomödie") ||
            inputKeywords.Contains("Fantasy-Action") || inputKeywords.Contains("Mysterythriller"))
          AddMovieOrSeriesKeyword("C104", "C203", data);

        if (inputKeywords.Contains("Abenteuer"))
          AddMovieOrSeriesKeyword("C103", "C205", data);

        if (inputKeywords.Contains("Familienkomödie") || inputKeywords.Contains("Komoedie"))
          AddMovieOrSeriesKeyword("C105", "C209", data);
      }

      if (keywords.Count == 0 && inputKeywords.Count != 0) {
        log.Warn("No Keywords: " + ch.Name + " at " + StartTime.ToString() + ": " + Title + "(" + tvmGenre + ", " + tvmKategorie + ", " + GetTVBField("genre") + ")");
      }
    }

    private void AddTVBRoles(MXFData data) {
      AddTVBRole("script", MXFRole.TYPE_WriterRole, data);
      AddTVBRole("moderation", MXFRole.TYPE_HostRole, data);
      AddTVBRole("producer", MXFRole.TYPE_ProducerRole, data);
      // Additional persons?
    }

    private void AddTVBRole(string fieldName, string roleType, MXFData data) {
      string f = GetTVBField(fieldName);
      if (!string.IsNullOrEmpty(f)) {
        string[] ps = f.Split(',');
        foreach (string p in ps) {
          AddRole(p.Trim(), roleType, null, data);
        }
      }
    }

    private void AddRole(String name, String roleType, string character, MXFData data) {
      // Check for duplicates
      foreach (MXFRole r in roles) {
        if (r.Person.Name == name && r.RoleType == roleType)
          return;
      }
      MXFRole ro;
      if (data.Persons.ContainsKey(name)) {
        ro = new MXFRole(data.Persons[name], roleType, character);
      } else {
        MXFPerson pers = new MXFPerson(name);
        ro = new MXFRole(pers, roleType, character);
        data.Persons.Add(pers.Name, pers);
        pers.Id = data.PersonId++;
      }
      ro.Person.Used = true;
      roles.Add(ro);
    }

    private void DetermineIds(string chName, MXFData data) {

      /*
       * OT / OE
       * OT / E
       * OT / SN / EN
       * OT / AEN
       * T / OE
       * T / E
       * T / SN / EN
       * T / AEN
      */

      List<long> pids = new List<long>();
      bool isSeries = false;
      long? sid = null;
      long pid;

      if (!string.IsNullOrEmpty(this.NormalizedOriginalTitle)) {
        if (!string.IsNullOrEmpty(this.NormalizedOriginalEpisode)) {
          pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' and NormalizedOriginalEpisode='" + this.NormalizedOriginalEpisode + "'");
          isSeries = true;
        }
        if (pids.Count == 0 && !string.IsNullOrEmpty(this.NormalizedEpisode)) {
          pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' and NormalizedEpisode='" + this.NormalizedEpisode + "'");
          isSeries = true;
          // NormalizedEpisode not unique
          if (pids.Count > 1) {
            SqlCommand fix = new SqlCommand("Update ProgramDetail set NormalizedEpisode = Null where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' and NormalizedEpisode='" + this.NormalizedEpisode + "'", connection);
            fix.ExecuteNonQuery();
            pids = new List<long>();
          }
        }
        if (pids.Count == 0 && this.SeasonNo != null && this.EpisodeNo != null) {
          pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' and SeasonNo=" + this.SeasonNo + " and EpisodeNo=" + this.EpisodeNo);
          isSeries = true;
          // SeasonNo/EpisodeNo not unique
          if (pids.Count > 1) {
            SqlCommand fix = new SqlCommand("Update ProgramDetail set SeasonNo = Null, EpisodeNo = Null where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' and SeasonNo=" + this.SeasonNo + " and EpisodeNo=" + this.EpisodeNo, connection);
            fix.ExecuteNonQuery();
            pids = new List<long>();
          }
        }
        if (pids.Count == 0 && this.AbsoluteEpisodeNo != null) {
          pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' and AbsoluteEpisodeNo=" + this.AbsoluteEpisodeNo);
          isSeries = true;
          // AbsoluteEpisodeNo not unique
          if (pids.Count > 1) {
            SqlCommand fix = new SqlCommand("Update ProgramDetail set AbsoluteEpisodeNo = Null where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' and AbsoluteEpisodeNo=" + this.AbsoluteEpisodeNo, connection);
            fix.ExecuteNonQuery();
            pids = new List<long>();
          }
        }
      }
      if (pids.Count == 0 && !string.IsNullOrEmpty(this.NormalizedOriginalEpisode)) {
        pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedTitle='" + this.NormalizedTitle + "' and NormalizedOriginalEpisode='" + this.NormalizedOriginalEpisode + "'");
        isSeries = true;
      }
      if (pids.Count == 0 && !string.IsNullOrEmpty(this.NormalizedEpisode)) {
        pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedTitle='" + this.NormalizedTitle + "' and NormalizedEpisode='" + this.NormalizedEpisode + "'");
        isSeries = true;
        // NormalizedEpisode not unique
        if (pids.Count > 1) {
          SqlCommand fix = new SqlCommand("Update ProgramDetail set NormalizedEpisode = Null where NormalizedTitle='" + this.NormalizedTitle + "' and NormalizedEpisode='" + this.NormalizedEpisode + "'", connection);
          fix.ExecuteNonQuery();
          pids = new List<long>();
        }
      }
      if (pids.Count == 0 && this.SeasonNo != null && this.EpisodeNo != null) {
        pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedTitle='" + this.NormalizedTitle + "' and SeasonNo=" + this.SeasonNo + " and EpisodeNo=" + this.EpisodeNo);
        isSeries = true;
        // SeasonNo/EpisodeNo not unique
        if (pids.Count > 1) {
          SqlCommand fix = new SqlCommand("Update ProgramDetail set SeasonNo = Null, EpisodeNo = Null where NormalizedTitle='" + this.NormalizedTitle + "' and SeasonNo=" + this.SeasonNo + " and EpisodeNo=" + this.EpisodeNo, connection);
          fix.ExecuteNonQuery();
          pids = new List<long>();
        }
      }
      if (pids.Count == 0 && this.AbsoluteEpisodeNo != null) {
        pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedTitle='" + this.NormalizedTitle + "' and AbsoluteEpisodeNo=" + this.AbsoluteEpisodeNo);
        isSeries = true;
        // AbsoluteEpisodeNo not unique
        if (pids.Count > 1) {
          SqlCommand fix = new SqlCommand("Update ProgramDetail set AbsoluteEpisodeNo = Null where NormalizedTitle='" + this.NormalizedTitle + "' and AbsoluteEpisodeNo=" + this.AbsoluteEpisodeNo, connection);
          fix.ExecuteNonQuery();
          pids = new List<long>();
        }
      }

      string desc = TVUtils.LeftString(this.TVBDescription, 255);
      string desc2 = TVUtils.LeftString(tvmBeschreibung, 255);
      // Special case ignore description when equal to title
      if (this.TVBDescription == this.Title) {
        desc = null;
      }
      if (tvmBeschreibung == this.Title) {
        desc2 = null;
      }

      // wenn bislang gefunden, 1. pid nehmen ohne desc Vergleich
      if (pids.Count > 0) {
        if (pids.Count > 1) {
          //log.Error("Program/episode not unique: " + pids[0]);
          throw new Exception("Program/episode not unique: " + pids[0] + ", " + pids[1] + "...");
        }

        pid = pids[0];
        //log.Debug("Program/episode " + pid + " found");

        sid = GetSidOfPid(pid);
        if (sid == null) {
          throw new Exception("Program/episode has no sid: " + pid);
        }

        if (FindSeriesDetails((long)sid) == null) {
          InsertSeriesDetails((long)sid, chName);
        }

        if (!string.IsNullOrEmpty(desc)) {
          if (FindProgramDetails(pid, desc) == null) {
            InsertProgramDetails(pid, desc, chName);
          }
        }
        if (!string.IsNullOrEmpty(desc2)) {
          if (FindProgramDetails(pid, desc2) == null) {
            InsertProgramDetails(pid, desc2, chName);
          }
        }
      } else {
        pids = FindProgramsForTitle();

        List<long> pidEquals1 = new List<long>();
        List<long> pidEquals2 = new List<long>();
        long? pidSimilar = null;
        String similarDesc = "";
        String similarDBDesc = "";
        double similarMatch = 0;

        foreach (long pi in pids) {
          //log.Debug("Program for title found: " + pi);

          String dbDesc = "";
          SqlCommand command = new SqlCommand("SELECT * from ProgramDetailView where IDProgram = " + pi, connection);
          SqlDataReader reader = command.ExecuteReader();
          while (reader.Read()) {

            // Serienfelder dürfen nicht beide gesetzt sein, weil sie dann ungleich sein müssen, sonst wären sie oben schon gefunden worden
            if ((TVUtils.Empty(this.NormalizedOriginalEpisode) || TVUtils.Empty(GetMaxFieldForPid(pi, "NormalizedOriginalEpisode"))) &&
                (TVUtils.Empty(this.NormalizedEpisode) || TVUtils.Empty(GetMaxFieldForPid(pi, "NormalizedEpisode"))) &&
                (TVUtils.Empty(this.EpisodeNo) || TVUtils.Empty(GetMaxFieldForPid(pi, "EpisodeNo"))) &&
                (TVUtils.Empty(this.AbsoluteEpisodeNo) || TVUtils.Empty(GetMaxFieldForPid(pi, "AbsoluteEpisodeNo")))) {

              dbDesc = TVUtils.LeftString((string)TVUtils.N2N(reader["Description"]), 255);

              if (string.IsNullOrEmpty(dbDesc) && string.IsNullOrEmpty(desc) && string.IsNullOrEmpty(desc2)) {
                if (!pidEquals1.Contains(pi))
                  pidEquals1.Add(pi);
              } else {
                if (!string.IsNullOrEmpty(desc) && desc == dbDesc) {
                  if (!pidEquals1.Contains(pi))
                    pidEquals1.Add(pi);
                } else if (!string.IsNullOrEmpty(desc2) && desc2 == dbDesc) {
                  if (!pidEquals2.Contains(pi))
                    pidEquals2.Add(pi);
                } else {
                  const double minMatch = 0.7; 
                  SimMetricsApi.AbstractStringMetric comp = new SimMetricsMetricUtilities.SmithWaterman();
                  if (!string.IsNullOrEmpty(desc)) {
                    double match = comp.GetSimilarity(desc, dbDesc);
                    if (match > minMatch) {
                      if (match > similarMatch) {
                        pidSimilar = pi;
                        similarMatch = match;
                        similarDesc = desc;
                        similarDBDesc = dbDesc;
                      }
                    }
                  }
                  if (!string.IsNullOrEmpty(desc2)) {
                    double match = comp.GetSimilarity(desc2, dbDesc);
                    if (match > minMatch) {
                      if (match > similarMatch) {
                        pidSimilar = pi;
                        similarMatch = match;
                        similarDesc = desc2;
                        similarDBDesc = dbDesc;
                      }
                    }
                  }
                }
              }
            }
          }
          reader.Close();
        }

        // Serienerkennung, wenn
        // - gefundenes Program ist Serie
        // - mehrere Programs gefunden, egal ob passendes dabei 
        //   - dann werden alle Serie, wenn sie noch nicht sein sollten (bereits geladene werden neu geladen)

        // Kein Program gefunden -> Neuanlage
        if (pidEquals1.Count == 0 && pidEquals2.Count == 0 && pidSimilar == null) {
          // Automatische Serienerkennung
          if (pids.Count > 0) {
            isSeries = true;
          }

          if (isSeries) {
            sid = DetermineSeriesId(chName);

            // Wegen automatischer Serienerkennung
            CheckPidsForSid(pids, (long)sid, data);

            SqlCommand insert = new SqlCommand("Insert into Program(ProgramIDSeries) Values(" + sid + ")", connection);
            insert.ExecuteNonQuery();
          } else {
            SqlCommand insert = new SqlCommand("Insert into Program default values", connection);
            insert.ExecuteNonQuery();
          }
          SqlCommand cmdNewID = new SqlCommand("Select @@IDENTITY", connection);
          pid = Decimal.ToInt64((decimal)cmdNewID.ExecuteScalar());

          if (string.IsNullOrEmpty(desc) && string.IsNullOrEmpty(desc2)) {
            InsertProgramDetails(pid, null, chName);
          } else {
            if (!string.IsNullOrEmpty(desc)) {
              InsertProgramDetails(pid, desc, chName);
            }
            if (!string.IsNullOrEmpty(desc2) && desc != desc2) {
              InsertProgramDetails(pid, desc2, chName);
            }
          }

        // Program gefunden...
        } else {
          // ... über gleiche Beschreibung
          if (pidEquals1.Count > 0 || pidEquals2.Count > 0) {
            // Die Beschreibung mit weniger Matches ist signifikanter
            List<long> pidEquals = pidEquals1;
            if (pidEquals1.Count == 0 || (pidEquals2.Count < pidEquals1.Count && pidEquals2.Count > 0))
              pidEquals = pidEquals2;

            pid = pidEquals[0];

            // Mehrere Programme mit gleicher Beschreibung
            /*
             * Es gibt keinen Grund anzunehmen, dass die vorher als ungleich erachtete Programme jetzt gleich sein sollen,
             * nur weil die aktuelle benutzte Beschreibung nicht eindeutig ist (z.B. Serien- gegen Episodenbeschreibung),
             * daher ist dies deaktiviert
              
            if (pidEquals.Count > 1) {
              string pidWhere = "";
              for (int i = 1; i < pidEquals.Count; i++) {
                pidWhere += pidEquals[i] + ", ";
              }
              pidWhere = pidWhere.Substring(0, pidWhere.Length - 2);

              // nur OK, wenn keine Serie aufgrund von Episode etc., alle anderen umhängen
              SqlCommand checkP = new SqlCommand("Update ProgramDetail set ProgramDetailIDProgram = " + pid + " where ProgramDetailIDProgram in (" + pidWhere + ") and OriginalEpisode is null and Episode is null and SeasonNo is null and EpisodeNo is null and AbsoluteEpisodeNo is null", connection);
              log.Debug("Duplicate programs - ProgramDetails attached to Program " + pid + ": " + checkP.ExecuteNonQuery());
              // Programs ohne ProgramDetails löschen
              checkP = new SqlCommand("Delete from Program where IDProgram not in (Select ProgramDetailIDProgram from ProgramDetail)", connection);
              log.Debug("Duplicate programs - Programs deleted: " + checkP.ExecuteNonQuery());
              
              // Wenn keine Serie aufgrund von Episode etc. ...
              if (!isSeries) {
                long? sidCheck = GetSidOfPid(pid);
                // ... aber Serie in Datenbank (wegen automatischer Serienerkennung) ...
                if (sidCheck != null) {
                  // ... und wenn kein Program in Datenbank Serie aufgrund von Episode etc. ...
                  checkP = new SqlCommand("Select Count(*) from ProgramDetailView where ProgramIDSeries = " + sidCheck + " and (OriginalEpisode is not null or Episode is not null or SeasonNo is not null or EpisodeNo is not null or AbsoluteEpisodeNo is not null)", connection);
                  if (((int)checkP.ExecuteScalar()) == 0) {
                    // ... und Serie nur ein Program hat
                    checkP = new SqlCommand("Select Count(*) from Program where ProgramIDSeries = " + sidCheck, connection);
                    if (((int)checkP.ExecuteScalar()) <= 1) {
                      // Serie löschen
                      checkP = new SqlCommand("Update Program set ProgramIDSeries = null where ProgramIDSeries = " + sidCheck, connection);
                      checkP.ExecuteNonQuery();
                      checkP = new SqlCommand("Delete from Series where IDSeries = " + sidCheck, connection);
                      checkP.ExecuteNonQuery();
                      log.Debug("Duplicate programs - Series deleted: " + sidCheck);
                    }
                  }
                }
              }
              // pids neu bestimmen wegen Löschungen etc.
              pids = FindProgramsForTitle();
            }
             */

            //log.Debug("Description of pid " + pid + " equal");

          // ... über ähnliche Beschreibung
          } else {
            pid = (long)pidSimilar;

            log.Debug("Description of pid " + pid + " similar: " + similarMatch);
            log.Debug("--------------------------------------");
            log.Debug(similarDesc);
            log.Debug("--------------------------------------");
            log.Debug(similarDBDesc);
            log.Debug("--------------------------------------");
          }

          // Ist gefundenes Programm Serie?
          sid = GetSidOfPid(pid);
          if (sid != null) {
            isSeries = true;
          } else {
            // Automatische Serienerkennung
            if (pids.Count > 1) {
              isSeries = true;
            }
            if (isSeries) {
              sid = DetermineSeriesId(chName);
            }
          }

          if (isSeries) {
            // Wegen automatischer Serienerkennung
            CheckPidsForSid(pids, (long)sid, data);
          }

          if (!string.IsNullOrEmpty(desc)) {
            if (FindProgramDetails(pid, desc) == null) {
              InsertProgramDetails(pid, desc, chName);
            }
          }
          if (!string.IsNullOrEmpty(desc2)) {
            if (FindProgramDetails(pid, desc2) == null) {
              InsertProgramDetails(pid, desc2, chName);
            }
          }
        }
      }
      programId = pid;
      seriesId = sid;
    }

    public void SetSeriesId(long sid) {
      seriesId = sid;
    }

    public MXFEpisodeInfo EpisodeInfo {
      get {
        if (seriesId == null)
          return null;

        if (episodeInfo != null && seriesId == episodeInfo.SeriesInfo.Id)
          return episodeInfo;

        MXFSeriesInfo si = new MXFSeriesInfo((long)seriesId, this.Title);
        episodeInfo = new MXFEpisodeInfo(si);
        episodeInfo.Episode = this.Episode;
        episodeInfo.EpisodeNo = this.EpisodeNo;
        episodeInfo.SeasonNo = this.SeasonNo;
        string oe = this.OriginalEpisode;

        SqlCommand command = new SqlCommand("Select * from ProgramDetail where ProgramDetailIDProgram = " + programId, connection);
        SqlDataReader reader = command.ExecuteReader();
        while (reader.Read()) {
          if (string.IsNullOrEmpty(episodeInfo.Episode)) {
            episodeInfo.Episode = (string)TVUtils.N2N(reader["Episode"]);
          }
          if (episodeInfo.EpisodeNo == null) {
            episodeInfo.EpisodeNo = (int?)TVUtils.N2N(reader["EpisodeNo"]);
            episodeInfo.SeasonNo = (int?)TVUtils.N2N(reader["SeasonNo"]);
          }
          if (string.IsNullOrEmpty(oe)) {
            oe = (string)TVUtils.N2N(reader["OriginalEpisode"]);
          }
        }
        reader.Close();

        if (string.IsNullOrEmpty(episodeInfo.Episode))
          episodeInfo.Episode = oe;

        return episodeInfo;
      }
    }

    private long DetermineSeriesId(string chName) {
      List<long> sids = new List<long>();
      long sid;

      if (!string.IsNullOrEmpty(this.NormalizedOriginalTitle)) {
        sids = FindSeries("SELECT * from SeriesDetailView where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' order by IDSeries");
      }
      if (sids.Count == 0) {
        sids = FindSeries("SELECT * from SeriesDetailView where NormalizedTitle='" + this.NormalizedTitle + "' order by IDSeries");
      }

      if (sids.Count > 0) {
        sid = sids[0];
      } else {
        SqlCommand insert = new SqlCommand("Insert into Series default values", connection);
        insert.ExecuteNonQuery();

        SqlCommand cmdNewID = new SqlCommand("Select @@IDENTITY", connection);
        sid = Decimal.ToInt64((decimal)cmdNewID.ExecuteScalar());
      }

      if (FindSeriesDetails(sid) == null) {
        InsertSeriesDetails(sid, chName);
      }
      return sid;
    }

    private void CheckPidsForSid(List<long> pids, long sid, MXFData data) {
      foreach (long pi in pids) {
        long? si = GetSidOfPid(pi);
        if (si == null) {
          UpdateSidOfPid(pi, sid, data);

        } else if (si != sid) {
          log.Info("Duplicate series " + sid + ", " + si);
          UpdateSidOfPid(pi, sid, data);

          SqlCommand command = new SqlCommand("Select * from Program where ProgramIDSeries = " + si, connection);
          SqlDataReader reader = command.ExecuteReader();
          while (reader.Read()) {
            UpdateSidOfPid((long)reader["IDProgram"], sid, data);
          }
          reader.Close();

          command = new SqlCommand("Delete from Series where IDSeries = " + si, connection);
          log.Debug("Series deleted: " + command.ExecuteNonQuery());
        }
      }
    }

    private void UpdateSidOfPid(long pi, long sid, MXFData data) {
      SqlCommand u = new SqlCommand("Update Program set ProgramIDSeries = " + sid + " where IDProgram = " + pi, connection);
      u.ExecuteNonQuery();

      // Bereits geladene Programs aktualisieren
      if (data.Programs.ContainsKey(pi)) {
        data.Programs[pi].SetSeriesId(sid);
      }

      // InsertSeriesDetails for pi if not there
      SqlCommand command = new SqlCommand("Select * from ProgramDetailView where IDProgram = " + pi, connection);
      SqlDataReader reader = command.ExecuteReader();
      while (reader.Read()) {
        string ot = (string)TVUtils.N2N(reader["OriginalTitle"]);
        string not = (string)TVUtils.N2N(reader["NormalizedOriginalTitle"]);
        string t = (string)TVUtils.N2N(reader["Title"]);
        string nt = (string)TVUtils.N2N(reader["NormalizedTitle"]);
        if (FindSeriesDetails(sid, ot, not, t, nt) == null) {
          InsertSeriesDetails(sid, "(CheckPidsForSid for pid " + pi + ")", ot, not, t, nt);
        }
      }
      reader.Close();
    }

    private long? GetSidOfPid(long pid) {
      SqlCommand command = new SqlCommand("Select ProgramIDSeries from Program where IDProgram = " + pid, connection);
      SqlDataReader reader = command.ExecuteReader();
      reader.Read();
      long? sid = (long?)TVUtils.N2N(reader["ProgramIDSeries"]);
      reader.Close();
      return sid;
    }

    private object GetMaxFieldForPid(long pid, string fieldName) {
      SqlCommand command = new SqlCommand("Select Max(" + fieldName + ") from ProgramDetailView where IDProgram = " + pid, connection);
      return command.ExecuteScalar();
    }

    private List<long> FindProgramsForTitle() {
      List<long> pids = new List<long>();
      if (!string.IsNullOrEmpty(this.NormalizedOriginalTitle)) {
        if (this.ProductionYear == null) {
          pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' order by IDProgram");
        } else {
          pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedOriginalTitle='" + this.NormalizedOriginalTitle + "' and (ProductionYear = " + this.ProductionYear + " or ProductionYear is null) order by IDProgram");
        }
      }
      if (pids.Count == 0) {
        if (this.ProductionYear == null) {
          pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedTitle='" + this.NormalizedTitle + "' order by IDProgram");
        } else {
          pids = FindPrograms("SELECT * from ProgramDetailView where NormalizedTitle='" + this.NormalizedTitle + "' and (ProductionYear = " + this.ProductionYear + " or ProductionYear is null) order by IDProgram");
        }
      }
      return pids;
    }

    private List<long> FindPrograms(string sql) {
      List<long> ids = new List<long>();

      SqlCommand command = new SqlCommand(sql, connection);
      SqlDataReader reader = command.ExecuteReader();
      while (reader.Read()) {
        long id = (long)reader["IDProgram"];
        if (!ids.Contains(id)) {
          ids.Add(id);
        }
      }
      reader.Close();
      return ids;
    }

    private List<long> FindSeries(string sql) {
      List<long> ids = new List<long>();

      SqlCommand command = new SqlCommand(sql, connection);
      SqlDataReader reader = command.ExecuteReader();
      while (reader.Read()) {
        long id = (long)reader["IDSeries"];
        if (!ids.Contains(id)) {
          ids.Add(id);
        }
      }
      reader.Close();
      return ids;
    }

    private long? FindProgramDetails(long pid, string description) {
      List<SqlParameter> paras = new List<SqlParameter>();
      string sql = "Select * from ProgramDetail where " +
          GetSqlCondition("ProgramDetailIDProgram", pid, paras) + "and" +
          GetSqlCondition("OriginalTitle", this.OriginalTitle, paras) + "and" +
          GetSqlCondition("NormalizedOriginalTitle", this.NormalizedOriginalTitle, paras) + "and" +
          GetSqlCondition("Title", this.Title, paras) + "and" +
          GetSqlCondition("NormalizedTitle", this.NormalizedTitle, paras) + "and" +
          GetSqlCondition("OriginalEpisode", this.OriginalEpisode, paras) + "and" +
          GetSqlCondition("NormalizedOriginalEpisode", this.NormalizedOriginalEpisode, paras) + "and" +
          GetSqlCondition("Episode", this.Episode, paras) + "and" +
          GetSqlCondition("NormalizedEpisode", this.NormalizedEpisode, paras) + "and" +
          GetSqlCondition("SeasonNo", this.SeasonNo, paras) + "and" +
          GetSqlCondition("EpisodeNo", this.EpisodeNo, paras) + "and" +
          GetSqlCondition("AbsoluteEpisodeNo", this.AbsoluteEpisodeNo, paras) + "and" +
          GetSqlCondition("ProductionYear", this.ProductionYear, paras) + "and" +
          GetSqlCondition("Description", TVUtils.LeftString(description, 255), paras);

      SqlCommand select = new SqlCommand(sql, connection);
      select.Parameters.AddRange(paras.ToArray());
      SqlDataReader reader = select.ExecuteReader();
      if (reader.Read()) {
        long? id = (long?)reader["IDProgramDetail"];
        reader.Close();
        return id;
      }
      reader.Close();
      return null;
    }

    private long? FindSeriesDetails(long sid) {
      return FindSeriesDetails(sid, this.OriginalTitle, this.NormalizedOriginalTitle, this.Title, this.NormalizedTitle);
    }

    private long? FindSeriesDetails(long sid, string orgTit, string normOrgTit, string tit, string normTit) {
      List<SqlParameter> paras = new List<SqlParameter>();
      string sql = "Select * from SeriesDetail where " +
          GetSqlCondition("SeriesDetailIDSeries", sid, paras) + "and" +
          GetSqlCondition("OriginalTitle", orgTit, paras) + "and" +
          GetSqlCondition("NormalizedOriginalTitle", normOrgTit, paras) + "and" +
          GetSqlCondition("Title", tit, paras) + "and" +
          GetSqlCondition("NormalizedTitle", normTit, paras);

      SqlCommand select = new SqlCommand(sql, connection);
      select.Parameters.AddRange(paras.ToArray());
      SqlDataReader reader = select.ExecuteReader();
      if (reader.Read()) {
        long? id = (long?)reader["IDSeriesDetail"];
        reader.Close();
        return id;
      }
      reader.Close();
      return null;
    }

    private string GetSqlCondition(string field, object value, List<SqlParameter> paras) {
      if (value == null) {
        return " " + field + " is null ";
      } else {
        paras.Add(new SqlParameter("@" + field, value));
        return " " + field + " = @" + field + " ";
      }
    }

    private void InsertProgramDetails(long pid, string description, string channel) {
      SqlCommand insert = new SqlCommand("Insert into ProgramDetail(ProgramDetailIDProgram, OriginalTitle, NormalizedOriginalTitle, " +
                                                                   "Title, NormalizedTitle, OriginalEpisode, NormalizedOriginalEpisode, " +
                                                                   "Episode, NormalizedEpisode, SeasonNo, EpisodeNo, AbsoluteEpisodeNo, " +
                                                                   "ProductionYear, Description, CreatedByChannel) Values (@a, @b, @c, @d, @e, @f, @g, @h, @i, @j, @k, @l, @m, @n, @o)", connection);
      AddProgramDetailParameters(insert, pid, description, channel);
      insert.ExecuteNonQuery();
    }

    private void AddProgramDetailParameters(SqlCommand sqlCmd, long pid, string description, string channel) {
      sqlCmd.Parameters.Add(new SqlParameter("@a", pid));
      AddSqlParameter(sqlCmd, "@b", this.OriginalTitle);
      AddSqlParameter(sqlCmd, "@c", this.NormalizedOriginalTitle);
      sqlCmd.Parameters.Add(new SqlParameter("@d", this.Title));
      sqlCmd.Parameters.Add(new SqlParameter("@e", this.NormalizedTitle));
      AddSqlParameter(sqlCmd, "@f", this.OriginalEpisode);
      AddSqlParameter(sqlCmd, "@g", this.NormalizedOriginalEpisode);
      AddSqlParameter(sqlCmd, "@h", this.Episode);
      AddSqlParameter(sqlCmd, "@i", this.NormalizedEpisode);
      AddSqlParameter(sqlCmd, "@j", this.SeasonNo);
      AddSqlParameter(sqlCmd, "@k", this.EpisodeNo);
      AddSqlParameter(sqlCmd, "@l", this.AbsoluteEpisodeNo);
      AddSqlParameter(sqlCmd, "@m", this.ProductionYear);
      AddSqlParameter(sqlCmd, "@n", TVUtils.LeftString(description, 255));
      AddSqlParameter(sqlCmd, "@o", channel);
    }

    private void InsertSeriesDetails(long sid, string channel) {
      InsertSeriesDetails(sid, channel, this.OriginalTitle, this.NormalizedOriginalTitle, this.Title, this.NormalizedTitle);
    }

    private void InsertSeriesDetails(long sid, string channel, string orgTit, string normOrgTit, string tit, string normTit) {
      SqlCommand insert = new SqlCommand("Insert into SeriesDetail(SeriesDetailIDSeries, OriginalTitle, NormalizedOriginalTitle, Title, NormalizedTitle, CreatedByChannel) " +
                                                                  "Values (@a, @b, @c, @d, @e, @f)", connection);
      insert.Parameters.Add(new SqlParameter("@a", sid));
      AddSqlParameter(insert, "@b", orgTit);
      AddSqlParameter(insert, "@c", normOrgTit);
      insert.Parameters.Add(new SqlParameter("@d", tit));
      insert.Parameters.Add(new SqlParameter("@e", normTit));
      insert.Parameters.Add(new SqlParameter("@f", channel));

      insert.ExecuteNonQuery();
    }

    private void AddSqlParameter(SqlCommand sqlCmd, string name, object value) {
      if (value == null) {
        sqlCmd.Parameters.Add(new SqlParameter(name, DBNull.Value));
      } else {
        sqlCmd.Parameters.Add(new SqlParameter(name, value));
      }
    }

    public string Title {
      get {
        XmlNode xmlT = xmlProgram.SelectSingleNode("fields/field[@name='title']");
        if (xmlT != null) {
          return xmlT.InnerText;
        }
        return null;
      }
    }

    public string NormalizedTitle {
      get {
        return NormalizeTitle(Title);
      }
    }

    public string OriginalTitle {
      get {
        string t = null;
        XmlNode xmlOT = xmlProgram.SelectSingleNode("fields/field[@name='original title']");
        if (xmlOT != null) {
          t = xmlOT.InnerText;

          // Das Erste: "Vom 8.12.2012"
          if (Regex.IsMatch(t, @"^Vom \d{1,2}\.\d{1,2}\.\d{4}$")) {
            t = null;
          }

          // KIKA: "2012"
          else if (Regex.IsMatch(t, @"^\d{4}$")) {
            t = null;
          }

        }
        if (string.IsNullOrEmpty(t)) {
          t = tvmOriginaltitel;
        }
        return t;
      }
    }

    public string NormalizedOriginalTitle {
      get {
        return NormalizeTitle(OriginalTitle);
      }
    }

    public string Episode {
      get {
        XmlNode xmlT = xmlProgram.SelectSingleNode("fields/field[@name='episode']");
        if (xmlT != null) {
          return xmlT.InnerText;
        }
        return null;
      }
    }

    public string NormalizedEpisode {
      get {
        return NormalizeTitle(Episode);
      }
    }

    public string OriginalEpisode {
      get {
        XmlNode xmlT = xmlProgram.SelectSingleNode("fields/field[@name='original episode']");
        if (xmlT != null) {
          return xmlT.InnerText;
        }
        return null;
      }
    }

    public string NormalizedOriginalEpisode {
      get {
        return NormalizeTitle(OriginalEpisode);
      }
    }

    public int? SeasonNo {
      get {
        XmlNode x = xmlProgram.SelectSingleNode("fields/field[@name='number of season']");
        if (x != null) {
          return int.Parse(x.InnerText);
        }
        return null;
      }
    }

    public int? EpisodeNo {
      get {
        XmlNode x = xmlProgram.SelectSingleNode("fields/field[@name='number of episode']");
        if (x != null) {
          if (this.SeasonNo != null) {
            return int.Parse(x.InnerText);
          }
        }
        return null;
      }
    }

    public int? AbsoluteEpisodeNo {
      get {
        XmlNode x = xmlProgram.SelectSingleNode("fields/field[@name='number of episode']");
        if (x != null) {
          if (this.SeasonNo == null) {
            return int.Parse(x.InnerText);
          }
        }
        return null;
      }
    }

    public int? ProductionYear {
      get {
        int? y = null;
        XmlNode xmlpy = xmlProgram.SelectSingleNode("fields/field[@name='production year']");
        if (xmlpy != null) {
          if (!string.IsNullOrEmpty(xmlpy.InnerText)) {
            y = int.Parse(xmlpy.InnerText);
          }
        }
        if (y == null) {
          if (!string.IsNullOrEmpty(tvmHerstellungsjahr)) {
            y = int.Parse(tvmHerstellungsjahr);
          }
        }
        return y;
      }
    }

    public string Genre {
      get {
        XmlNode xmlG = xmlProgram.SelectSingleNode("fields/field[@name='genre']");
        if (xmlG != null) {
          return xmlG.InnerText;
        }
        return null;
      }
    }

    public string Country {
      get {
        string c = null;
        XmlNode xmlG = xmlProgram.SelectSingleNode("fields/field[@name='origin']");
        if (xmlG != null) {
          c = xmlG.InnerText;
        }
        if (string.IsNullOrEmpty(c)) {
          c = tvmHerstellungsland;
        }
        if (!string.IsNullOrEmpty(c)) {
          return c;
        }
        return null;
      }
    }

    public string TVBDescription {
      get {
        XmlNode xmlT = xmlProgram.SelectSingleNode("fields/field[@name='description']");
        if (xmlT != null && !xmlT.InnerText.StartsWith("Diese Sendung hat noch keine Beschreibung")) {
          return xmlT.InnerText;
        } else {
          xmlT = xmlProgram.SelectSingleNode("fields/field[@name='short description']");
          if (xmlT != null && !xmlT.InnerText.StartsWith("Diese Sendung hat noch keine Beschreibung")) {
            return xmlT.InnerText;
          }
        }
        return null;
      }
    }

    private string GetTVBField(string name) {
      XmlNode xmlG = xmlProgram.SelectSingleNode("fields/field[@name='" + name + "']");
      if (xmlG != null) {
        return xmlG.InnerText;
      }
      return null;
    }

    private string NormalizeTitle(string title) {
      if (string.IsNullOrEmpty(title)) return title;

      string ot = "";
      string nt = title.ToLower();
      foreach (char c in nt) {
        if (Char.IsLetterOrDigit(c) || c.Equals(' ')) {
          ot += c;
        }
      }
      // Doppelspaces raus
      ot = Regex.Replace(ot, @"\s+", " ");
      return ot.Trim();
    }

    private DateTime StartTime {
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

    public long Id {
      get { return (long)programId; }
    }

    public string Description {
      get {
        string desc = "";
        string descTVB = TVUtils.N2V(this.TVBDescription, "").ToString();
        string descTVM = TVUtils.N2V(tvmBeschreibung, "").ToString();

        int countTVB = GetDescriptionCount(descTVB);
        int countTVM = GetDescriptionCount(descTVM);

        if (string.IsNullOrEmpty(descTVM)) {
          desc = descTVB;
        } else if (string.IsNullOrEmpty(descTVB)) {
          desc = descTVM;
        } else if (countTVB == countTVM) {
          if (descTVM.Length > descTVB.Length) {
            desc = descTVM;
          } else {
            desc = descTVB;
          }
        } else if (countTVB < countTVM) {
          desc = descTVB;
        } else {
          desc = descTVM;
        }

        if (!String.IsNullOrEmpty(tvmKurzkritik)) {
          if (desc != "") {
            desc += Environment.NewLine;
          }
          desc += "\"" + tvmKurzkritik + "\"";
        }

        string desc2 = "";
        if (this.Genre != null) {
          desc2 += this.Genre;
        }
        if (this.Country != null || this.ProductionYear != null) {
          if (desc2 != "") desc2 += " - ";

          string desc3 = "";
          if (this.Country != null)
            desc3 = this.Country;
          if (this.ProductionYear != null) {
            if (desc3 != "") desc3 += " ";
            desc3 += this.ProductionYear;
          }
          desc2 += desc3;
        }
        if (this.OriginalTitle != null) {
          if (desc2 != "") desc2 += " - ";
          desc2 += "OT: " + this.OriginalTitle;
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

    private int GetDescriptionCount(string desc) {
      int count = 0;
      if (this.seriesId != null) {
        if (!string.IsNullOrEmpty(desc)) {
          List<SqlParameter> paras = new List<SqlParameter>();
          SqlCommand command = new SqlCommand("select COUNT(IDProgram) from ProgramDetailView where " + GetSqlCondition("ProgramIDSeries", seriesId, paras) + " and " +
                                                                                                        GetSqlCondition("Description", TVUtils.LeftString(desc, 255), paras), connection);
          command.Parameters.AddRange(paras.ToArray());
          SqlDataReader reader = command.ExecuteReader();
          if (reader.Read()) {
            count = (int)reader[0];
          }
          reader.Close();
        }
      }
      return count;
    }

    public string ShortDescription {
      get {
        string sd = this.TVBDescription;
        if (string.IsNullOrEmpty(sd)) {
          sd = tvmBeschreibung;
        }
        if (string.IsNullOrEmpty(sd)) {
          sd = tvmKurzBeschreibung;
        }

        if (string.IsNullOrEmpty(sd)) {
          return string.Empty;
        }
        if (sd.Length > 512) {
          sd = sd.Substring(0, 512 - 3) + "...";
        }
        return sd;
      }
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

    public string HalfStars {
      get {
        /* 0-6
         * 0 unbewertet
         * 1 grau
         * 2 rosa
         * 3 rot
         * 4 rot, Tagestipp
         * 5 rot/gelb
         * 6 ???
         */

        switch (tvmBewertung) {
          case 0:
          case 6:
            return string.Empty;
          case 1:
            return "2";
          case 2:
            return "4";
          case 3:
            return "6";
          case 4:
            return "7";
          case 5:
            return "8";
        }
        return string.Empty;
      }
    }

    public bool IsMovie {
      // Nicht TVBrowser isMovie Flag auswerten, ist auch bei Serien gesetzt
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

    public bool IsSpecial {
      get { return IsKeyword("MSEPGC_SPECIAL"); }
    }

    private bool IsKeyword(string id) {
      if (keywords.ContainsKey(id)) {
        return true;
      }
      return false;
    }

    private string TVMCharakterToRoleType(string charakter) {
      switch (charakter) {
        case "Drehbuch":
        case "Musik":
        case "Autor":
        case "Buch":
          return MXFRole.TYPE_WriterRole;

        case "Spezialpersonen":
        case "Gast":
        case "Gaststar":
          return MXFRole.TYPE_GuestActorRole;

        case "Moderator":
        case "Verantwortlich":
          return MXFRole.TYPE_HostRole;

        case "Produzent":
        case "Produktionsfirma":
        case "Produktionsleitung":
        case "Herstellungsleitung":
          return MXFRole.TYPE_ProducerRole;

        case "Kamera":
        case "Schnitt":
        case "Spezialeffekte":
        case "Kostüm":
        case "Stimme":
        case "Ton":
        case "Casting":
        case "Regie-Assistenz":
        case "Ausstattung":
        case "Komponist":
        case "Sprecher":
        case "Animation":
        case "Stunts":
        case "Szenen-/Bühnenbild":
        case "Maske":
        case "Vorlage":
        case "Bauten":
          return MXFRole.TYPE_Special;
      }
      return MXFRole.TYPE_ActorRole;
    }

    private string DecodeEntities(string input) {
      if (string.IsNullOrEmpty(input))
        return input;

      if (!input.Contains("&#"))
        return input;

      string output = null;
      string[] parts = input.Split(new string[] {"&#"}, StringSplitOptions.None);
      foreach (string part in parts) {
        if (output == null) {
          output = part;
        } else if (part.Contains(";")) {
          int pos = part.IndexOf(';'); 
          string ent = part.Substring(0, pos + 1);
          string rest = part.Substring(pos + 1, part.Length - pos - 1);
          XmlDocument xd = new XmlDocument();
          xd.LoadXml("<test>&#" + ent + "</test>");
          output += xd.InnerText;
          output += rest;
        } else {
          log.Warn("'&#' in field without being an entity: " + input);
          output += part;
        }
      }
      return output;
    }

    private void ProcessKeyword(string keywrds, MXFData data) {
      if (string.IsNullOrEmpty(keywrds)) return;
      if (keywrds == "NN") return;
      if (keywrds == "AA") return;

      string[] kws = keywrds.Split(new string[] { ",", "(", ")", " - " }, StringSplitOptions.RemoveEmptyEntries);
      foreach (string kw in kws) {
        string keywrd = kw.Trim();
        SqlCommand select = new SqlCommand("Select * from Keyword where Name = @a", connection);
        select.Parameters.Add(new SqlParameter("@a", keywrd));
        SqlDataReader reader = select.ExecuteReader();
        if (reader.Read()) {
          string kwid = (string)TVUtils.N2N(reader["KeywordId"]);
          reader.Close();
          if (!string.IsNullOrEmpty(kwid)) {
            AddKeyword(kwid, data);
          }
          SqlCommand update = new SqlCommand("update Keyword set ProgramCount = ProgramCount + 1 where Name = @a", connection);
          update.Parameters.Add(new SqlParameter("@a", keywrd));
          update.ExecuteNonQuery();
        } else {
          reader.Close();
          SqlCommand insert = new SqlCommand("Insert into Keyword(Name, ProgramCount) Values (@a, 1)", connection);
          insert.Parameters.Add(new SqlParameter("@a", keywrd));
          insert.ExecuteNonQuery();
        }

        if (!inputKeywords.Contains(keywrd)) {
          inputKeywords.Add(keywrd);
        }

        /*
        string testKeyword = "Fantasy";
        if (keywrd == testKeyword) {
          log.Info("############ " + this.Title);
        }
        */
        
        /*
        if (this.Title.StartsWith("Emergency Room") ||
          this.Title.Contains("Mord ist ihr Hobby")) {
          log.Info("############# " + keywrd + ": " + this.Title);
        }
        */
      }
    }

    private void AddKeyword(string kwid, MXFData data) {

      if (data.Keywords.ContainsKey(kwid)) {
        MXFKeyword k = data.Keywords[kwid];
        if (!keywords.ContainsKey(k.Id)) {
          keywords.Add(k.Id, k);
          // 1st Level keywords are needed, too
          if (!keywords.ContainsKey(k.Group.Group.Id)) {
            keywords.Add(k.Group.Group.Id, k.Group.Group);
          }
        }
      } else {
        throw new Exception("*** ERROR Keyword not found: " + kwid);
      }
    }

    private void AddMovieOrSeriesKeyword(string movieKw, string seriesKw, MXFData data) {
      if (this.EpisodeInfo == null) {
        AddKeyword(movieKw, data);
      } else {
        AddKeyword(seriesKw, data);
      }
    }
  }
}
