using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Data.OleDb;
using System.Data.SqlClient;

using Microsoft.MediaCenter.Guide;

namespace TVD2MXF {
  class Program {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    public static readonly string WorkingDir = @"C:\Program Files (x86)\TVTools\TVD2MXF";

    static void Main(string[] args) {
      log.Info("*** START ***");

      LogEpgDBFileSize();

      // Immer Samstag DB warten
      if (DateTime.Now.DayOfWeek == DayOfWeek.Saturday) {
        EpgDbMaintenance();
        LogEpgDBFileSize();

        log.Info("*** FINISHED ***");
        return;
      }

      MXFData data = null;
      // Media Center connection
      MCConnection mcconnection = new MCConnection();

      try {
        string tvmovieDb = "C:\\Program Files (x86)\\TV Movie\\TV Movie ClickFinder\\tvdaten.mdb";
        string tvmovieImageDir = "C:\\Program Files (x86)\\TV Movie\\TV Movie ClickFinder\\Hyperlinks";
 
        // TVMovie database connection
        string connectionString = "Provider=Microsoft.ACE.OLEDB.12.0; Data Source=" + tvmovieDb + ";";
        OleDbConnection tvmovieConnection = new OleDbConnection(connectionString);
        tvmovieConnection.Open();


        // TODO SQL Server connection
        // Development
        // connectionString = @"Initial Catalog=TVTools; Data Source=WILES\SQLExpress; user=sa; MultipleActiveResultSets=True";
        // Release
        connectionString = @"Initial Catalog=TVTools; Data Source=HTPC\MYMOVIES; user=tvtools; Password=tvtools; MultipleActiveResultSets=True";


        SqlConnection connection = new SqlConnection(connectionString);
        connection.Open();

        // Read data
        TVBReader tvbReader = new TVBReader();
        data = tvbReader.Read(connection, tvmovieConnection, tvmovieImageDir, mcconnection);

        // Create MXF File
        MXFWriter writer = new MXFWriter(data);
        writer.Write(WorkingDir + "\\tvd.mxf.xml");
        log.Info("MXF File written.");

        tvmovieConnection.Close();
        connection.Close();

      } catch (Exception e) {
        log.Error("Error while preparing data, aborting...");
        log.Error(e.Message, e);
        return;
      }


      // Stop before import
      /*
      log.Warn("Actual import disabled.");
      return;
      */


      try {
        // Start actual import
        Dictionary<string, Dictionary<string, string>> backupRolesForProgram = BeforeImport(mcconnection);

        // Wait 60s
        System.Threading.Thread.Sleep(60 * 1000);

        // Call loadmxf
        Process p = new Process();
        p.StartInfo.FileName = @"C:\Windows\eHome\loadmxf.exe";
        p.StartInfo.Arguments = " -i \"" + WorkingDir + "\\tvd.mxf.xml\"";
        p.StartInfo.WindowStyle = ProcessWindowStyle.Normal;
        p.Start();
        p.WaitForExit();
        log.Info("MXF File loaded.");
        
        // Wait 60s
        System.Threading.Thread.Sleep(60 * 1000);
       
        AfterImport(mcconnection, data, backupRolesForProgram);

        // Wait 60s
        System.Threading.Thread.Sleep(60 * 1000);

        // Reindex Search Root 
        string schtask = @"C:\Windows\system32\schtasks.exe";
        Process process = new Process();
        ProcessStartInfo info = new ProcessStartInfo(schtask);
        info.Arguments = " /run /tn \"\\Microsoft\\Windows\\Media Center\\ReindexSearchRoot\"";
        info.CreateNoWindow = true;
        info.WindowStyle = ProcessWindowStyle.Hidden;
        process.StartInfo = info;
        process.Start();
        process.WaitForExit();
        log.Info("Reindex Search Root done.");

        LogEpgDBFileSize();
        
        // Delete old log files
        LogCleaner.LogCleaner.Clean();        

        log.Info("*** FINISHED ***");
      } catch (Exception e) {
        log.Error("Error while importing data.");
        log.Error(e.Message, e);
      }
    }

    private static Dictionary<string, Dictionary<string, string>> BeforeImport(MCConnection connection) {

      // For debugging, is normally true
      bool clearCharacter = true;
      //bool clearCharacter = false;

      // Remove duplicate roles and all characters
      Dictionary<string, Dictionary<string, string>> backup = new Dictionary<string, Dictionary<string, string>>();
      Programs progs = new Programs(connection.Store);
      foreach (Microsoft.MediaCenter.Guide.Program p in progs) {
        //log.Debug("Handling program " + p.GetUIdValue());
        if (p.GetUIdValue() != null) {
          if (p.GetUIdValue().StartsWith("!Program!tvtools_")) {
            Dictionary<string, string> backupRole = new Dictionary<string, string>();
            List<string> check = new List<string>();
            foreach (ActorRole r in p.ActorRoles) {
              if (check.Contains(r.Person.Name)) {
                log.Warn("Removing Duplicate ActorRole " + r.Id + " " + r.Rank + " " + r.Character + " " + r.Person.GetUIdValue());
                r.Person = null;
                r.Program = null;
                r.Character = string.Empty;
                r.Update();
              } else {
                //log.Debug(r.Character + "|" + (r.Character == null ? "NULL" : "NN") + "|" + (r.Character == string.Empty ? "EMPTY" : "NE"));
                if (!string.IsNullOrEmpty(r.Character)) {
                  string oldChar = r.Character;
                  backupRole.Add(r.Person.Name + "#1", r.Character);
                  if (clearCharacter) {
                    r.Character = string.Empty;
                    try {
                      r.Update();
                    } catch (Exception e) {
                      log.Error("ERROR " + r.Person.Name + ":" + oldChar + "|" + (oldChar == null ? "NULL" : "NN") + "|" + (oldChar == string.Empty ? "EMPTY" : "NE"));
                      log.Error(e.Message, e);
                      // terminate...
                      throw e;

                      // backupRole.Remove(r.Person.Name + "#1");
                    }
                  }
                }
                check.Add(r.Person.Name);
              }
            }

            check = new List<string>();
            foreach (GuestActorRole r in p.GuestActorRoles) {
              if (check.Contains(r.Person.Name)) {
                log.Warn("Removing Duplicate GuestActorRole " + r.Id + " " + r.Rank + " " + r.Character + " " + r.Person.GetUIdValue());
                r.Person = null;
                r.Program = null;
                r.Character = string.Empty;
                r.Update();
              } else {
                //log.Debug(r.Character + "|" + (r.Character == null ? "NULL" : "NN") + "|" + (r.Character == string.Empty ? "EMPTY" : "NE"));
                if (!string.IsNullOrEmpty(r.Character)) {
                  backupRole.Add(r.Person.Name + "#5", r.Character);
                  if (clearCharacter) {
                    r.Character = string.Empty;
                    r.Update();
                  }
                }
                check.Add(r.Person.Name);
              }
            }
            backup.Add(p.GetUIdValue(), backupRole);
          }
        }
      }
      log.Info("BeforeImport done.");
      return backup;
    }

    private static void AfterImport(MCConnection connection, MXFData data, Dictionary<string, Dictionary<string, string>> backupRolesForProgram) {

      Programs progs = new Programs(connection.Store);
      List<string> servicedProgs = new List<string>();
      foreach (Microsoft.MediaCenter.Guide.Program p in progs) {
        if (p.GetUIdValue() != null) {
          string uid = p.GetUIdValue();
          //log.Debug("Servicing " + uid);
          if (uid.StartsWith("!Program!tvtools_")) {
            uid = uid.Replace("!Program!tvtools_", "");
            long luid = long.Parse(uid);
            if (data.Programs.ContainsKey(luid)) {
              //log.Debug("TVD program found");
              MXFProgram mxfProg = data.Programs[luid];
              foreach (ActorRole r in p.ActorRoles.ToList()) {
                foreach (MXFRole role in mxfProg.Roles) {
                  if (role.RoleType == MXFRole.TYPE_ActorRole && r.Person.Name == role.Person.Name && r.Character != role.Character && !string.IsNullOrEmpty(role.Character)) {
                    //log.Debug("Character update: " + r.Person.Name + " (" + role.Character + ") in " + p.Title);
                    r.Character = role.Character;
                    r.Update();
                  }
                }
              }
              foreach (GuestActorRole r in p.GuestActorRoles.ToList()) {
                foreach (MXFRole role in mxfProg.Roles) {
                  if (role.RoleType == MXFRole.TYPE_GuestActorRole && r.Person.Name == role.Person.Name && r.Character != role.Character && !string.IsNullOrEmpty(role.Character)) {
                    //log.Debug("Character update: " + r.Person.Name + " (" + role.Character + ") in " + p.Title);
                    r.Character = role.Character;
                    r.Update();
                  }
                }
              }
            } else if (backupRolesForProgram.ContainsKey(p.GetUIdValue())) {
              //log.Debug("Program found in backup");
              Dictionary<string, string> backupRoles = backupRolesForProgram[p.GetUIdValue()];
              foreach (ActorRole r in p.ActorRoles.ToList()) {
                if (backupRoles.ContainsKey(r.Person.Name + "#1")) {
                  string chara = backupRoles[r.Person.Name + "#1"];
                  if (r.Character != chara) {
                    //log.Debug("Backup character update: " + r.Person.Name + " (" + chara + ") in " + p.Title);
                    r.Character = chara;
                    r.Update();
                  }
                }
              }
              foreach (GuestActorRole r in p.GuestActorRoles.ToList()) {
                if (backupRoles.ContainsKey(r.Person.Name + "#5")) {
                  string chara = backupRoles[r.Person.Name + "#5"];
                  if (r.Character != chara) {
                    //log.Debug("Backup character update: " + r.Person.Name + " (" + chara + ") in " + p.Title);
                    r.Character = chara;
                    r.Update();
                  }
                }
              }
            } else {
              log.Error("Program not found: " + p.GetUIdValue() + " " + p.Title);
            }
            servicedProgs.Add(p.GetUIdValue());
          }
        }
      }

      // Check if we found all MXFPrograms
      foreach (MXFProgram prog in data.Programs.Values) {
        if (!servicedProgs.Contains("!Program!tvtools_" + prog.Id)) {
          log.Error("Not serviced program: " + "!Program!tvtools_" + prog.Id + " " + prog.Title + " " + prog.DebugInfo);
        }
      }
      log.Info("AfterImport done.");
    }

    private static void EpgDbMaintenance() {
      log.Info("Starting EPG DB maintenance...");

      // Logging on
      Microsoft.Win32.Registry.SetValue(@"HKEY_LOCAL_MACHINE\SOFTWARE\Debug\mcstore", "TraceLevel", 3);

      // Call mcupdate
      Process p = new Process();
      p.StartInfo.FileName = @"C:\Windows\eHome\mcupdate.exe";
      p.StartInfo.Arguments = " -dbgc";
      p.StartInfo.WindowStyle = ProcessWindowStyle.Normal;
      p.Start();
      p.WaitForExit();
      log.Info("MCUpdate done");

      // Logging off
      Microsoft.Win32.Registry.SetValue(@"HKEY_LOCAL_MACHINE\SOFTWARE\Debug\mcstore", "TraceLevel", 0);
    }

    private static void LogEpgDBFileSize() {
      int epgDbNo = (int)Microsoft.Win32.Registry.GetValue(@"HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Media Center\Service\EPG", "EPG.instance", 0);
      string epgDbFile = @"C:\ProgramData\Microsoft\eHome\mcepg2-" + epgDbNo + ".db";
      FileInfo epgFile = new FileInfo(epgDbFile);
      log.Warn("EPG DB file size: " + epgFile.Length.ToString("n0") + " byte");
    }
  }
}
