using System;
using System.Collections.Generic;
using System.Text;

namespace TVD2MXF {
  class TVUtils {

    public static string LeftString(string s, int length) {
      if (string.IsNullOrEmpty(s)) return s;
      if (s.Length > 255) return s.Substring(0, length);
      return s;
    }

    public static object N2N(object o) {
      if (o == DBNull.Value) {
        return null;
      } else {
        return o;
      }
    }

    public static object N2V(object o, object ifnull) {
      if (o == null || o == DBNull.Value) {
        return ifnull;
      } else {
        return o;
      }
    }

    public static bool Empty(object o) {
      if (o == null || o == DBNull.Value || String.Empty.Equals(o))
        return true;

      return false;
    }
  }
}
