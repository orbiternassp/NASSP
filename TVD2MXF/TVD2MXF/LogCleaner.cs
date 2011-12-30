using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace LogCleaner {
  class LogCleaner {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    public static void Clean() {
      string file = System.Diagnostics.Process.GetCurrentProcess().Modules[0].FileName;
      int pos = file.LastIndexOf("\\");

      DirectoryInfo di = new DirectoryInfo(file.Substring(0, pos));
      SortedList<string, FileInfo> logfiles = new SortedList<string, FileInfo>();
      foreach (FileInfo fi in di.GetFiles(file.Substring(pos + 1) + ".log_*")) {
        logfiles.Add(fi.Name, fi);
      }
      if (logfiles.Count > 5) {
        for (int i = 0; i < logfiles.Count - 5; i++) {
          log.Debug("Deleting " + logfiles.Values[i].FullName);
          File.Delete(logfiles.Values[i].FullName);
        }
      }
    }
  }
}
