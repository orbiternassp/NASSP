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
        xmlTextWriter.WriteAttributeString("id", si.Id);
        xmlTextWriter.WriteAttributeString("uid", "!Series!tvd2mxf_" + si.Id);
        xmlTextWriter.WriteAttributeString("title", si.Title);
        xmlTextWriter.WriteAttributeString("shortTitle", si.Title);
        xmlTextWriter.WriteEndElement();

        if (si.AdditionalTitles.Count > 100) {
          log.Warn(si.AdditionalTitles.Count + " additional titles of series: " + si.Id + " - " + si.Title);
          /*
          foreach (MXFProgram p in si.AdditionalTitles.Values) {
            log.Warn("    " + p.Title + ", id: " + p.Id + ", " + p.DebugInfo);
          }
          */
        }
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
      xmlTextWriter.WriteAttributeString("uid", "!Program!tvd2mxf_0");
      xmlTextWriter.WriteAttributeString("title", "(Unbekannt)");
      xmlTextWriter.WriteEndElement();
      
      foreach (MXFProgram prog in data.Programs.Values) {
        xmlTextWriter.WriteStartElement("Program");

        xmlTextWriter.WriteAttributeString("id", prog.Id);
        xmlTextWriter.WriteAttributeString("uid", "!Program!tvd2mxf_" + prog.Id);

        xmlTextWriter.WriteAttributeString("title", prog.Title);
        xmlTextWriter.WriteAttributeString("episodeTitle", prog.EpisodeTitle);
        xmlTextWriter.WriteAttributeString("description", prog.Description);
        xmlTextWriter.WriteAttributeString("shortDescription", prog.ShortDescription);

        if (!prog.Year.Equals(string.Empty))
          xmlTextWriter.WriteAttributeString("year", prog.Year);

        if (prog.SeriesInfo != null) {
          xmlTextWriter.WriteAttributeString("series", prog.SeriesInfo.Id);
          xmlTextWriter.WriteAttributeString("seasonNumber", prog.SeasonNumber);          
          xmlTextWriter.WriteAttributeString("episodeNumber", prog.EpisodeNumber);
          xmlTextWriter.WriteAttributeString("isSeries", "true");
        }

        if (prog.Keywords.Count > 0) {
          string kw = "";
          foreach (MXFKeyword k in prog.Keywords.Values) {
            kw += k.Id + ",";
          }
          xmlTextWriter.WriteAttributeString("keywords", kw.Substring(0, kw.Length - 1));
        }

        if (!prog.HalfStars.Equals(string.Empty))
          xmlTextWriter.WriteAttributeString("halfStars", prog.HalfStars);

        if (prog.IsMovie) {
          xmlTextWriter.WriteAttributeString("isMovie", "true");
        } else if (prog.IsNews) {
          xmlTextWriter.WriteAttributeString("isNews", "true");
        } else if (prog.IsSports) {
          xmlTextWriter.WriteAttributeString("isSports", "true");
        } else if (prog.IsKids) {
          xmlTextWriter.WriteAttributeString("isKids", "true");
        } 
        // TODO isSpecial, use "Tagestipps" in the tips directory?  

        // Guide Image
        if (prog.GuideImage != null)
          xmlTextWriter.WriteAttributeString("guideImage", "i" + prog.GuideImage.Id);

        // Write director and actor information
        // Use order in document, rank is bogus
        int rank = 0;
        foreach (MXFRole role in prog.Roles) {
          if (role.RoleType == "1") {
            xmlTextWriter.WriteStartElement("ActorRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else if (role.RoleType == "3" || role.RoleType == "6") {
            xmlTextWriter.WriteStartElement("WriterRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else if (role.RoleType == "5") {
            xmlTextWriter.WriteStartElement("GuestActorRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else if (role.RoleType == "4") {
            xmlTextWriter.WriteStartElement("HostRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else if (role.RoleType == "17" || role.RoleType == "27") {
            xmlTextWriter.WriteStartElement("ProducerRole");
            xmlTextWriter.WriteAttributeString("person", "p" + role.Person.Id);
            xmlTextWriter.WriteAttributeString("rank", rank.ToString());
            xmlTextWriter.WriteEndElement();
          } else  if (role.RoleType == "2") {
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

        for (int i = 0; i < channel.ScheduleEntries.Count; i++) {
          MXFScheduleEntry entry = channel.ScheduleEntries[i];

          // Last entry
          if (i == channel.ScheduleEntries.Count - 1) {
            xmlTextWriter.WriteStartElement("ScheduleEntry");
            if (i == 0) {
              xmlTextWriter.WriteAttributeString("startTime", entry.StartTime);
            }
            WriteScheduleEntryAttributes(xmlTextWriter, entry, int.Parse(entry.Duration));

            // Normal entry
          } else {
            MXFScheduleEntry nextEntry = channel.ScheduleEntries[i + 1];
            TimeSpan duration = entry.ConvertToDateTime(nextEntry.StartTime).Subtract(entry.ConvertToDateTime(entry.StartTime));
            if (duration.TotalSeconds > 0) {
              xmlTextWriter.WriteStartElement("ScheduleEntry");
              if (i == 0) {
                xmlTextWriter.WriteAttributeString("startTime", entry.StartTime);
              }
              int entryDuration = int.Parse(entry.Duration);
              if (duration.TotalSeconds <= entryDuration) {
                WriteScheduleEntryAttributes(xmlTextWriter, entry, Convert.ToInt32(duration.TotalSeconds));
              } else {
                // Gap to next entry
                WriteScheduleEntryAttributes(xmlTextWriter, entry, entryDuration);

                // Add dummy entry
                xmlTextWriter.WriteStartElement("ScheduleEntry");
                xmlTextWriter.WriteAttributeString("program", "0");
                xmlTextWriter.WriteAttributeString("duration", (duration.TotalSeconds - entryDuration).ToString());
                xmlTextWriter.WriteEndElement();
              }
            } else {
              //log.Debug("Skipping entry. Start: " + entry.StartTime + ", program: " + entry.ProgramId + ", channel: " + channel.Id + " " + channel.Name);
            }
          }
        }


        // Old stuff
        /*
        string lastEndTime = "";
        bool isFirst = true;  // we need to know this so that we output the start time for the first entry only
        bool skip = false;
        int shortenDuration = 0; 
        foreach (MXFScheduleEntry entry in channel.ScheduleEntries) {
          if (isFirst) {  // write out the starttime for the first entry only
            xmlTextWriter.WriteStartElement("ScheduleEntry");
            xmlTextWriter.WriteAttributeString("startTime", entry.StartTime);
            isFirst = false;
          } else {
            if (entry.StartTime != lastEndTime) {
              // log.Debug("Start/End mismatch. Start: " + entry.StartTime + ", channel: " + channel.Id + " " + channel.Name);
              // Special case simultaneous entries (Sky Sport subchannels)
              if (entry.ConvertToDateTime(lastEndTime).CompareTo(entry.ConvertToDateTime(entry.EndTime)) >= 0) {
                skip = true;
              } else {
                TimeSpan dur = entry.ConvertToDateTime(entry.StartTime).Subtract(entry.ConvertToDateTime(lastEndTime));
                if (dur.TotalSeconds < 0) {
                  shortenDuration = (int) -dur.TotalSeconds;
                } else {
                  // Add dummy entry
                  xmlTextWriter.WriteStartElement("ScheduleEntry");
                  xmlTextWriter.WriteAttributeString("program", "0");
                  xmlTextWriter.WriteAttributeString("duration", dur.TotalSeconds.ToString());
                  xmlTextWriter.WriteEndElement();
                }
              }
            }
            if (!skip) xmlTextWriter.WriteStartElement("ScheduleEntry");            
          }
          if (!skip) {
            int duration = int.Parse(entry.Duration);
            if (shortenDuration != 0) {
              duration -= shortenDuration;
              shortenDuration = 0;
              if (duration < 0) 
                throw new Exception("Negative dummy duration.");
            }
            xmlTextWriter.WriteAttributeString("program", entry.ProgramId);
            xmlTextWriter.WriteAttributeString("duration", duration.ToString());
            if (entry.IsLive) {
              xmlTextWriter.WriteAttributeString("isLive", "true");
            } else {
              xmlTextWriter.WriteAttributeString("isLive", "false");
            }
            // TODO LiveSports
            if (entry.IsHdtv) {
              xmlTextWriter.WriteAttributeString("isHdtv", "true");
            } else {
              xmlTextWriter.WriteAttributeString("isHdtv", "false");
            }
            xmlTextWriter.WriteEndElement();
          }
          skip = false;
          lastEndTime = entry.EndTime;
        }
        */


        xmlTextWriter.WriteEndElement(); // ScheduleEntries
      }
    }

    private void WriteScheduleEntryAttributes(XmlTextWriter xmlTextWriter, MXFScheduleEntry entry, int duration) {
      xmlTextWriter.WriteAttributeString("program", entry.ProgramId);
      xmlTextWriter.WriteAttributeString("duration", duration.ToString());
      if (entry.IsLive) {
        xmlTextWriter.WriteAttributeString("isLive", "true");
      } else {
        xmlTextWriter.WriteAttributeString("isLive", "false");
      }
      // TODO LiveSports
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

