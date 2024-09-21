/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Display Formatting

Project Apollo is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Project Apollo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Project Apollo; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

See http://nassp.sourceforge.net/license/ for more details.

**************************************************************************/

#include "RTCCDisplayFormatting.h"

namespace rtcc
{
	void RTCCDisplayPrint::Print(oapi::Sketchpad *skp, DWORD W, DWORD H, unsigned dispnum) const
	{
		for (unsigned i = 0; i < displays.size(); i++)
		{
			if (dispnum == displays[i].MSKNumber)
			{
				const rtcc::RTCCDisplay *disp = &displays[i];

				oapi::Sketchpad::TAlign_horizontal halign = oapi::Sketchpad::LEFT;
				skp->SetTextAlign(halign);

				for (unsigned i = 0; i < disp->Data.size(); i++)
				{
					if (halign != disp->Data[i].align)
					{
						skp->SetTextAlign(disp->Data[i].align);
						halign = disp->Data[i].align;
					}
					skp->Text(disp->Data[i].x * W / 1024, (1024 - disp->Data[i].y)*H / 1024, disp->Data[i].Text.c_str(), disp->Data[i].Text.size());
				}
			}
		}		
	}

	RTCCBackgroundSlides::RTCCBackgroundSlides()
	{
		displays.clear();

		RTCCDisplay temp;

		//Coelliptic ARM
		temp.MSKNumber = 232;
		AddTitle(temp, "Ascent Rendezvous Monitor");
		AddMSK(temp, "0232");

		AddText(temp, "WT", 64, 950);
		AddText(temp, "E", 320, 950);
		AddText(temp, "CSI", 512, 950);
		AddText(temp, "CDH", 672, 950);
		AddText(temp, "TPI", 64, 918);
		AddText(temp, "MIN PL", 320, 918);
		AddText(temp, "INS", 512, 918);

		AddText(temp, "SOURCE", 64, 854);
		AddText(temp, "MSFN", 320, 854);
		AddText(temp, "PGNS", 576, 854);
		AddText(temp, "AGS", 832, 854);

		AddText(temp, "RECOMMEND", 64, 790);

		AddText(temp, "GET TWEAK", 64, 726);
		AddText(temp, "DVX TWEAK", 64, 694);
		AddText(temp, "HP", 64, 662);

		AddText(temp, "GET CSI", 64, 598);
		AddText(temp, "DV CSI", 64, 566);
		AddText(temp, "GET CDH", 64, 534);
		AddText(temp, "DV CDH", 64, 502);

		AddText(temp, "GET KICK", 64, 438);
		AddText(temp, "DVX KICK", 64, 406);

		displays.push_back(temp);

		//Short ARM
		temp.Data.clear();
		temp.MSKNumber = 233;
		AddTitle(temp, "Short Ascent Rendezvous Monitor");
		AddMSK(temp, "0233");

		AddText(temp, "WT", 64, 950);
		AddText(temp, "DPH", 320, 950);
		AddText(temp, "TPI", 64, 918);
		AddText(temp, "INS", 320, 918);
		AddText(temp, "REF", 640, 918);

		AddText(temp, "SOURCE", 64, 854);
		AddText(temp, "MSFN", 320, 854);
		AddText(temp, "PGNS", 576, 854);
		AddText(temp, "AGS", 832, 854);

		AddText(temp, "RECOMMEND", 64, 790);

		AddText(temp, "GET TWEAK", 64, 726);
		AddText(temp, "DVX", 64, 694);
		AddText(temp, "DVY", 64, 662);
		AddText(temp, "DVZ", 64, 630);
		AddText(temp, "HP", 64, 598);

		AddText(temp, "R(M)", 64, 566);
		AddText(temp, "P(I)", 64, 534);
		AddText(temp, "Y(O)", 64, 502);

		AddText(temp, "GETTPI", 64, 438);
		AddText(temp, "DV", 64, 406);

		displays.push_back(temp);
	}

	void RTCCBackgroundSlides::AddTitle(struct RTCCDisplay &disp, std::string title) const
	{
		DisplayFormatData temp;

		temp.Text = title;
		temp.x = 508;
		temp.y = 1004;
		temp.align = oapi::Sketchpad::TAlign_horizontal::CENTER;

		disp.Data.push_back(temp);
	}

	void RTCCBackgroundSlides::AddMSK(struct RTCCDisplay &disp, std::string msk) const
	{
		DisplayFormatData temp;

		temp.Text = msk;
		temp.x = 940;
		temp.y = 1004;
		temp.align = oapi::Sketchpad::TAlign_horizontal::LEFT;

		disp.Data.push_back(temp);
	}

	void RTCCBackgroundSlides::AddText(struct RTCCDisplay &disp, std::string Text, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		DisplayFormatData temp;

		temp.Text = Text;
		temp.x = x;
		temp.y = y;
		temp.align = align;

		disp.Data.push_back(temp);
	}

	void RTCCDynamicDisplayData::UpdateDisplay(const RTCCDisplay &disp)
	{
		//Search through vector
		for (unsigned i = 0; i < displays.size(); i++)
		{
			if (disp.MSKNumber == displays[i].MSKNumber)
			{
				displays[i] = disp;
				return;
			}
		}

		//Did not find it, create it
		displays.push_back(disp);
	}

	void RTCCDynamicDisplayData::DisplayFormatting(RTCCDisplay &disp, std::string Text, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		DisplayFormatData temp;

		temp.Text = Text;
		temp.x = x;
		temp.y = y;
		temp.align = align;

		disp.Data.push_back(temp);
	}

	void RTCCDynamicDisplayData::DFLDouble(RTCCDisplay &disp, double val, const char* format, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		char Buff[64];
		sprintf_s(Buff, format, val);

		DisplayFormatting(disp, Buff, x, y, align);
	}

	void RTCCDynamicDisplayData::DFLInteger(RTCCDisplay &disp, int val, const char* format, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		char Buff[64];
		sprintf_s(Buff, format, val);

		DisplayFormatting(disp, Buff, x, y, align);
	}

	void RTCCDynamicDisplayData::DFLTime(RTCCDisplay &disp, double val, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		char Buff[64];
		format_time_rtcc(Buff, val);

		DisplayFormatting(disp, Buff, x, y, align);
	}

	void RTCCDynamicDisplayData::format_time_rtcc(char *buf, double time) const
	{
		buf[0] = 0; // Clobber
		int hours, minutes, seconds;
		if (time < 0) { return; } // don't do that
		hours = (int)(time / 3600);
		minutes = (int)((time / 60) - (hours * 60));
		seconds = (int)((time - (hours * 3600)) - (minutes * 60));
		sprintf_s(buf, 64, "%03d:%02d:%02d", hours, minutes, seconds);
	}
}