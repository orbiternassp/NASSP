using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;


namespace TVD2MXF {
  class MXFWriter {
    private static log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

    private MXFData data;

    public MXFWriter(MXFData dta) {
      data = dta;
    }

    public void Write(string fileName) {
      XmlTextWriter xmlTextWriter = new XmlTextWriter(fileName, Encoding.Unicode);
      xmlTextWriter.Formatting = Formatting.Indented;
      xmlTextWriter.WriteStartDocument();
      xmlTextWriter.WriteStartElement("MXF");

      WriteBaseXML(xmlTextWriter); //Write the base XML content

      xmlTextWriter.WriteStartElement("With");
      xmlTextWriter.WriteAttributeString("provider", "provider1");

      WriteKeywords(xmlTextWriter); //Write Keywords
      WriteImages(xmlTextWriter); //Write Channel images.
      WritePeople(xmlTextWriter); //Write all people objects            
      WriteSeries(xmlTextWriter); //Write all series information
      WritePrograms(xmlTextWriter); //Write Programmes
      WriteAffiliates(xmlTextWriter); //Write all affiliate data
      WriteServices(xmlTextWriter); //Write all service data
      WriteScheduleEntries(xmlTextWriter);
      WriteLineups(xmlTextWriter);
      xmlTextWriter.WriteEndDocument();
      xmlTextWriter.Close();
    }

    private void WriteBaseXML(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("Assembly");
      xmlTextWriter.WriteAttributeString("name", "mcepg");
      xmlTextWriter.WriteAttributeString("version", "6.1.0.0");
      xmlTextWriter.WriteAttributeString("cultureInfo", "");
      xmlTextWriter.WriteAttributeString("publicKey", "0024000004800000940000000602000000240000525341310004000001000100B5FC90E7027F67871E773A8FDE8938C81DD402BA65B9201D60593E96C492651E889CC13F1415EBB53FAC1131AE0BD333C5EE6021672D9718EA31A8AEBD0DA0072F25D87DBA6FC90FFD598ED4DA35E44C398C454307E8E33B8426143DAEC9F596836F97C8F74750E5975C64E2189F45DEF46B2A2B1247ADC3652BF5C308055DA9");

      xmlTextWriter.WriteStartElement("NameSpace");
      xmlTextWriter.WriteAttributeString("name", "Microsoft.MediaCenter.Guide");

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Lineup");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Channel");
      xmlTextWriter.WriteAttributeString("parentFieldName", "lineup");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Service");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "ScheduleEntry");
      xmlTextWriter.WriteAttributeString("groupName", "ScheduleEntries");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Program");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Keyword");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "KeywordGroup");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Person");
      xmlTextWriter.WriteAttributeString("groupName", "People");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "ActorRole");
      xmlTextWriter.WriteAttributeString("parentFieldName", "program");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "DirectorRole");
      xmlTextWriter.WriteAttributeString("parentFieldName", "program");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "WriterRole");
      xmlTextWriter.WriteAttributeString("parentFieldName", "program");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "HostRole");
      xmlTextWriter.WriteAttributeString("parentFieldName", "program");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "GuestActorRole");
      xmlTextWriter.WriteAttributeString("parentFieldName", "program");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "ProducerRole");
      xmlTextWriter.WriteAttributeString("parentFieldName", "program");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "GuideImage");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Affiliate");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "SeriesInfo");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Season");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteEndElement();
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Assembly");
      xmlTextWriter.WriteAttributeString("name", "mcstore");
      xmlTextWriter.WriteAttributeString("version", "6.1.0.0");
      xmlTextWriter.WriteAttributeString("cultureInfo", "");
      xmlTextWriter.WriteAttributeString("publicKey", "0024000004800000940000000602000000240000525341310004000001000100B5FC90E7027F67871E773A8FDE8938C81DD402BA65B9201D60593E96C492651E889CC13F1415EBB53FAC1131AE0BD333C5EE6021672D9718EA31A8AEBD0DA0072F25D87DBA6FC90FFD598ED4DA35E44C398C454307E8E33B8426143DAEC9F596836F97C8F74750E5975C64E2189F45DEF46B2A2B1247ADC3652BF5C308055DA9");

      xmlTextWriter.WriteStartElement("NameSpace");
      xmlTextWriter.WriteAttributeString("name", "Microsoft.MediaCenter.Store");

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "Provider");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Type");
      xmlTextWriter.WriteAttributeString("name", "UId");
      xmlTextWriter.WriteAttributeString("parentFieldName", "target");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteEndElement();
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("Providers");

      xmlTextWriter.WriteStartElement("Provider");
      xmlTextWriter.WriteAttributeString("id", "provider1");
      xmlTextWriter.WriteAttributeString("name", "TVD2MXF");
      xmlTextWriter.WriteAttributeString("displayName", "TVD2MXF converter");
      xmlTextWriter.WriteAttributeString("copyright", "Copyright text here.");
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteEndElement();

    }

    private void WriteKeywords(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("Keywords");

      foreach (MXFKeyword k in data.Keywords.Values) {
        xmlTextWriter.WriteStartElement("Keyword");
        xmlTextWriter.WriteAttributeString("id", k.Id);
        xmlTextWriter.WriteAttributeString("word", k.Name);
        xmlTextWriter.WriteEndElement();
      }
      xmlTextWriter.WriteEndElement();

      xmlTextWriter.WriteStartElement("KeywordGroups");
      foreach (MXFKeywordGroup kg in data.KeywordGroups.Values) {
        xmlTextWriter.WriteStartElement("KeywordGroup");
        xmlTextWriter.WriteAttributeString("uid", "!KeywordGroup!" + kg.Id);
        xmlTextWriter.WriteAttributeString("groupName", kg.Id);
        string kw = "";
        foreach (MXFKeyword k in kg.Keywords) {
          kw += k.Id + ",";
        }
        xmlTextWriter.WriteAttributeString("keywords", kw.Substring(0, kw.Length - 1));
        xmlTextWriter.WriteEndElement();
      }
      xmlTextWriter.WriteEndElement();
    }

    private void WriteImages(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("GuideImages");

      foreach (MXFGuideImage img in data.GuideImages) {
        xmlTextWriter.WriteStartElement("GuideImage");
        xmlTextWriter.WriteAttributeString("id", "i" + img.Id);
        xmlTextWriter.WriteAttributeString("imageUrl", "file://" + img.FileName);
        xmlTextWriter.WriteEndElement(); // Guide Image
      }
      xmlTextWriter.WriteEndElement(); // Guide Images
    }

    private void WritePeople(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("People");

      foreach (MXFPerson p in data.Persons.Values) {
        if (p.Used) {
          xmlTextWriter.WriteStartElement("Person");
          xmlTextWriter.WriteAttributeString("id", "p" + p.Id);
          xmlTextWriter.WriteAttributeString("name", p.Name);
          xmlTextWriter.WriteAttributeString("uid", p.Uid);
          xmlTextWriter.WriteEndElement();
        }
      }

      xmlTextWriter.WriteEndElement();
    }
    private void WriteSeries(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("SeriesInfos");

      foreach (MXFSeriesInfo si in data.SeriesInfos.Values) {
        xmlTextWriter.WriteStartElement("SeriesInfo");
        xmlTextWriter.WriteAttributeString("id", si.XmlId);
        xmlTextWriter.WriteAttributeString("uid", "!Series!tvtools_" + si.XmlId);
        xmlTextWriter.WriteAttributeString("title", si.Title);
        xmlTextWriter.WriteAttributeString("shortTitle", si.Title);
        xmlTextWriter.WriteEndElement();
      }
      xmlTextWriter.WriteEndElement(); // Series Infos
    }
    private void WriteAffiliates(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("Affiliates");
      //xmlTextWriter.WriteStartElement("Affiliate");
      //xmlTextWriter.WriteAttributeString("name", ConfigurationSettings.AppSettings["providerString"]);
      //xmlTextWriter.WriteAttributeString("uid", "!Affiliate!" + ConfigurationSettings.AppSettings["providerString"]);
      ////theWriter.WriteAttributeString("logoImage", "i100");
      //xmlTextWriter.WriteEndElement(); // Affiliate
      xmlTextWriter.WriteEndElement();
    }

    private void WriteServices(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("Services");

      foreach (MXFChannel ch in data.Channels.Values) {
        xmlTextWriter.WriteStartElement("Service");
        xmlTextWriter.WriteAttributeString("id", "s" + ch.Id);
        xmlTextWriter.WriteAttributeString("uid", "!Service!" + ch.Id + ".tvd2mxf.org");
        xmlTextWriter.WriteAttributeString("name", ch.Name);
        xmlTextWriter.WriteAttributeString("callSign", ch.Name);
        if (ch.GuideImage != null)
          xmlTextWriter.WriteAttributeString("logoImage", "i" + ch.GuideImage.Id);
        xmlTextWriter.WriteEndElement();
      }
      xmlTextWriter.WriteEndElement();
    }
    private void WritePrograms(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("Programs");

      // Dummy program to fill schedule holes
      xmlTextWriter.WriteStartElement("Program");
      xmlTextWriter.WriteAttributeString("id", "0");
      xmlTextWriter.WriteAttributeString("uid", "!Program!tvtools_0");
      xmlTextWriter.WriteAttributeString("title", "(Unbekannt)");
      xmlTextWriter.WriteEndElement();
      
      foreach (MXFProgram prog in data.Programs.Values) {
        xmlTextWriter.WriteStartElement("Program");

        xmlTextWriter.WriteAttributeString("id", prog.Id.ToString());
        xmlTextWriter.WriteAttributeString("uid", "!Program!tvtools_" + prog.Id);

        xmlTextWriter.WriteAttributeString("title", prog.Title);
        MXFEpisodeInfo ei = prog.EpisodeInfo;
        if (ei != null)
          xmlTextWriter.WriteAttributeString("episodeTitle",ei.Episode);
        else
          xmlTextWriter.WriteAttributeString("episodeTitle", "");

        xmlTextWriter.WriteAttributeString("description", prog.Description);
        xmlTextWriter.WriteAttributeString("shortDescription", prog.ShortDescription);

        if (prog.ProductionYear != null)
          xmlTextWriter.WriteAttributeString("year", prog.ProductionYear.ToString());

        if (ei != null) {
          xmlTextWriter.WriteAttributeString("series", ei.SeriesInfo.XmlId);
          xmlTextWriter.WriteAttributeString("seasonNumber", TVUtils.N2V(ei.SeasonNo, "0").ToString());
          xmlTextWriter.WriteAttributeString("episodeNumber", TVUtils.N2V(ei.EpisodeNo, "0").ToString());
          xmlTextWriter.WriteAttributeString("isSeries", "true");
        }

        if (prog.Keywords.Count > 0) {
          string kw = "";
          foreach (MXFKeyword k in prog.Keywords.Values) {
            kw += k.Id + ",";
          }
          xmlTextWriter.WriteAttributeString("keywords", kw.Substring(0, kw.Length - 1));
        }

        if (!string.IsNullOrEmpty(prog.HalfStars))
          xmlTextWriter.WriteAttributeString("halfStars", prog.HalfStars);

        if (prog.IsMovie) {
          xmlTextWriter.WriteAttributeString("isMovie", "true");
        } else {
          xmlTextWriter.WriteAttributeString("isMovie", "false");
        }
        if (prog.IsNews) {
          xmlTextWriter.WriteAttributeString("isNews", "true");
        } else {
          xmlTextWriter.WriteAttributeString("isNews", "false");
        }
        if (prog.IsSports) {
          xmlTextWriter.WriteAttributeString("isSports", "true");
        } else {
          xmlTextWriter.WriteAttributeString("isSports", "false");
        }
        if (prog.IsKids) {
          xmlTextWriter.WriteAttributeString("isKids", "true");
        } else {
          xmlTextWriter.WriteAttributeString("isKids", "false");
        }
        if (prog.IsSpecial) {
          xmlTextWriter.WriteAttributeString("isSpecial", "true");
        } else {
          xmlTextWriter.WriteAttributeString("isSpecial", "false");
        } 

        // Guide Image
        if (prog.GuideImage != null)
          xmlTextWriter.WriteAttributeString("guideImage", "i" + prog.GuideImage.Id);

        // Write role information
        // Use order in list as rank
        int rank = 0;
        foreach (MXFRole role in prog.Roles) {
          if (role.RoleType == MXFRole.TYPE_ActorRole) {
            xmlTextWriter.WriteStartElement("ActorRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else if (role.RoleType == MXFRole.TYPE_WriterRole) {
            xmlTextWriter.WriteStartElement("WriterRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else if (role.RoleType == MXFRole.TYPE_GuestActorRole) {
            xmlTextWriter.WriteStartElement("GuestActorRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else if (role.RoleType == MXFRole.TYPE_HostRole) {
            xmlTextWriter.WriteStartElement("HostRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else if (role.RoleType == MXFRole.TYPE_ProducerRole) {
            xmlTextWriter.WriteStartElement("ProducerRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else  if (role.RoleType == MXFRole.TYPE_DirectorRole) {
            xmlTextWriter.WriteStartElement("DirectorRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          }
          rank++;
        }
        xmlTextWriter.WriteEndElement();
      } 
      xmlTextWriter.WriteEndElement(); // Programs
    }

    private void WriteScheduleEntries(XmlTextWriter xmlTextWriter) {

      foreach (MXFChannel channel in data.Channels.Values) {
        xmlTextWriter.WriteStartElement("ScheduleEntries");
        xmlTextWriter.WriteAttributeString("service", "s" + channel.Id);

        bool startTimeWritten = false;
        for (int i = 0; i < channel.ScheduleEntries.Count; i++) {
          MXFScheduleEntry entry = channel.ScheduleEntries[i];

          // Last entry
          if (i == channel.ScheduleEntries.Count - 1) {
            // not writable as duration can't be calculated

          // Normal entry
          } else {
            MXFScheduleEntry nextEntry = channel.ScheduleEntries[i + 1];
            TimeSpan duration = nextEntry.StartTime.ToUniversalTime().Subtract(entry.StartTime.ToUniversalTime());
            if (duration.TotalSeconds > 0) {
              xmlTextWriter.WriteStartElement("ScheduleEntry");
              if (!startTimeWritten) {
                xmlTextWriter.WriteAttributeString("startTime", entry.XmlStartTime);
                startTimeWritten = true;
              }
              WriteScheduleEntryAttributes(xmlTextWriter, entry, Convert.ToInt32(duration.TotalSeconds));
            } else {
              //log.Debug("Skipping entry. Start: " + entry.StartTime + ", program: " + entry.ProgramId + ", channel: " + channel.Id + " " + channel.Name);
            }
          }
        }
        xmlTextWriter.WriteEndElement(); // ScheduleEntries
      }
    }

    private void WriteScheduleEntryAttributes(XmlTextWriter xmlTextWriter, MXFScheduleEntry entry, int duration) {
      xmlTextWriter.WriteAttributeString("program", entry.Program.Id.ToString());
      xmlTextWriter.WriteAttributeString("duration", duration.ToString());
      xmlTextWriter.WriteAttributeString("audioFormat", entry.AudioFormat);
      if (entry.IsLive) {
        xmlTextWriter.WriteAttributeString("isLive", "true");
      } else {
        xmlTextWriter.WriteAttributeString("isLive", "false");
      }

      // isLiveSports?
      
      if (entry.IsHdtv) {
        xmlTextWriter.WriteAttributeString("isHdtv", "true");
      } else {
        xmlTextWriter.WriteAttributeString("isHdtv", "false");
      }
      xmlTextWriter.WriteEndElement(); 
    }

    private void WriteLineups(XmlTextWriter xmlTextWriter) {
      xmlTextWriter.WriteStartElement("Lineups");

      xmlTextWriter.WriteStartElement("Lineup");
      xmlTextWriter.WriteAttributeString("id", "l1");
      xmlTextWriter.WriteAttributeString("name", "TVD2MXF");
      xmlTextWriter.WriteAttributeString("uid", "!Lineup!TVD2MXF");
      xmlTextWriter.WriteAttributeString("primaryProvider", "!MCLineup!MainLineup");

      xmlTextWriter.WriteStartElement("channels");
      foreach (MXFChannel channel in data.Channels.Values) {
        xmlTextWriter.WriteStartElement("Channel");
        xmlTextWriter.WriteAttributeString("uid", "!Channel!TVD2MXF!" + channel.Id);
        xmlTextWriter.WriteAttributeString("lineup", "l1");
        xmlTextWriter.WriteAttributeString("service", "s" + channel.Id);
        xmlTextWriter.WriteAttributeString("number", "-1");
        xmlTextWriter.WriteEndElement(); // Channel
      }
      xmlTextWriter.WriteEndElement(); // Channels
      xmlTextWriter.WriteEndElement(); // Line Up
      xmlTextWriter.WriteEndElement(); // Line Ups
    }

  }
}

