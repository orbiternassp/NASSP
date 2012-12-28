using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace TVD2MXF {
  class MXFRole {

    public static readonly string TYPE_ActorRole = "ActorRole";
    public static readonly string TYPE_WriterRole = "WriterRole";
    public static readonly string TYPE_GuestActorRole = "GuestActorRole";
    public static readonly string TYPE_HostRole = "HostRole";
    public static readonly string TYPE_ProducerRole = "ProducerRole";
    public static readonly string TYPE_DirectorRole = "DirectorRole";
    public static readonly string TYPE_Special = "(Special)";
  
    private MXFPerson person;
    private string roleType;
    private string character;

    public MXFRole(MXFPerson p, string rt, string c) {
      person = p;
      roleType = rt;
      character = c;
    }

    public MXFPerson Person {
      get { return person; }
    }

    public string RoleType {
      get { return roleType; }
    }

    public string Character {
      get { return character; }
    }
  }
}
