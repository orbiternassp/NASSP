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
#include "OrbMech.h"

namespace rtcc
{
	void RTCCDisplayPrint::Print(oapi::Sketchpad* skp, DWORD CW, DWORD WOFF, DWORD CH, DWORD HOFF, unsigned dispnum) const
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

					if (disp->Data[i].x >= 0 && disp->Data[i].y >= 0)
					{
						skp->Text(disp->Data[i].x * CW + WOFF, disp->Data[i].y * CH + HOFF, disp->Data[i].Text.c_str(), disp->Data[i].Text.size());
					}
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
		AddText(temp, "Ascent Rendezvous Monitor", 8, 0);
		AddText(temp, "0232", 38, 0);

		AddText(temp, "WT", 1, 2);
		AddText(temp, "E", 14, 2);
		AddText(temp, "CSI", 24, 2);
		AddText(temp, "CDH", 35, 2);
		AddText(temp, "TPI", 1, 3);
		AddText(temp, "MIN PL", 16, 3);
		AddText(temp, "INS", 29, 3);

		AddText(temp, "SOURCE", 1, 5);
		AddText(temp, "MSFN", 16, 5);
		AddText(temp, "PGNS", 26, 5);
		AddText(temp, "AGS", 36, 5);

		AddText(temp, "RECOMMEND", 1, 7);

		AddText(temp, "GET TWEAK", 1, 9);
		AddText(temp, "DVX TWEAK", 1, 10);
		AddText(temp, "HP", 1, 11);

		AddText(temp, "GET CSI", 1, 13);
		AddText(temp, "DV CSI", 1, 14);
		AddText(temp, "GET CDH", 1, 15);
		AddText(temp, "DV CDH", 1, 16);

		AddText(temp, "GET KICK", 1, 18);
		AddText(temp, "DVX KICK", 1, 19);

		displays.push_back(temp);

		//Short ARM
		temp.Data.clear();
		temp.MSKNumber = 233;
		AddText(temp, "Short Ascent Rendezvous Monitor", 4, 0);
		AddText(temp, "0233", 38, 0);

		AddText(temp, "WT", 1, 2);
		AddText(temp, "DPH", 16, 2);
		AddText(temp, "TPI", 1, 3);
		AddText(temp, "INS", 16, 3);
		AddText(temp, "REF", 32, 3);

		AddText(temp, "SOURCE", 1, 5);
		AddText(temp, "MSFN", 16, 5);
		AddText(temp, "PGNS", 26, 5);
		AddText(temp, "AGS", 36, 5);

		AddText(temp, "RECOMMEND", 1, 7);

		AddText(temp, "GET TWEAK", 1, 9);
		AddText(temp, "DVX", 1, 10);
		AddText(temp, "DVY", 1, 11);
		AddText(temp, "DVZ", 1, 12);
		AddText(temp, "HP", 1, 13);

		AddText(temp, "R(M)", 1, 14);
		AddText(temp, "P(I)", 1, 15);
		AddText(temp, "Y(O)", 1, 16);

		AddText(temp, "GETTPI", 1, 18);
		AddText(temp, "DV", 1, 19);

		displays.push_back(temp);
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

	int RTCCDynamicDisplayData::GetDisplayData(int MSK, RTCCDisplay& disp)
	{
		for (unsigned i = 0; i < displays.size(); i++)
		{
			if (MSK == displays[i].MSKNumber)
			{
				disp = displays[i];
				return 0;
			}
		}
		return 1;
	}

	void RTCCDynamicDisplayData::DisplayFormatting(RTCCDisplay &disp, unsigned num, std::string Text, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		if (disp.Data.size() <= num) return;

		DisplayFormatData temp;

		temp.Text = Text;
		temp.x = x;
		temp.y = y;
		temp.align = align;

		disp.Data[num] = temp;
	}

	void RTCCDynamicDisplayData::DFLDouble(RTCCDisplay &disp, unsigned num, double val, const char* format, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		char Buff[64];
		sprintf_s(Buff, format, val);

		DisplayFormatting(disp, num, Buff, x, y, align);
	}

	void RTCCDynamicDisplayData::DFLInteger(RTCCDisplay &disp, unsigned num, int val, const char* format, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		char Buff[64];
		sprintf_s(Buff, format, val);

		DisplayFormatting(disp, num, Buff, x, y, align);
	}

	void RTCCDynamicDisplayData::DFLTime(RTCCDisplay &disp, unsigned num, double val, int x, int y, oapi::Sketchpad::TAlign_horizontal align) const
	{
		char Buff[64];
		OrbMech::format_time_HHHMMSS(Buff, val);

		DisplayFormatting(disp, num, Buff, x, y, align);
	}
}