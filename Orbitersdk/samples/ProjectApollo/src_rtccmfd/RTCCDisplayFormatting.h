/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Display Formatting (Header)

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

#pragma once

#include "Orbitersdk.h"

namespace rtcc
{
	//DISPLAY FORMATTING LANGUAGE
	struct DisplayFormatData
	{
		std::string Text;
		int x = 0, y = 0;
		oapi::Sketchpad::TAlign_horizontal align = oapi::Sketchpad::RIGHT;
	};

	struct RTCCDisplay
	{
		int MSKNumber;
		std::vector<DisplayFormatData> Data;
	};

	class RTCCDisplayPrint
	{
	public:
		void Print(oapi::Sketchpad *skp, DWORD W, DWORD H, unsigned dispnum) const;
	protected:
		std::vector<RTCCDisplay> displays;
	};

	class RTCCBackgroundSlides : public RTCCDisplayPrint
	{
	public:
		RTCCBackgroundSlides();
	protected:

		void AddTitle(struct RTCCDisplay &disp, std::string title) const;
		void AddMSK(struct RTCCDisplay &disp, std::string msk) const;
		void AddText(struct RTCCDisplay &disp, std::string Text, int x, int y, oapi::Sketchpad::TAlign_horizontal align = oapi::Sketchpad::LEFT) const;
	};

	class RTCCDynamicDisplayData : public RTCCDisplayPrint
	{
	public:
		void UpdateDisplay(const RTCCDisplay &disp);

		void DisplayFormatting(RTCCDisplay &disp, std::string Text, int x, int y, oapi::Sketchpad::TAlign_horizontal align = oapi::Sketchpad::LEFT) const;
		void DFLDouble(RTCCDisplay &disp, double val, const char* format, int x, int y, oapi::Sketchpad::TAlign_horizontal align = oapi::Sketchpad::RIGHT) const;
		void DFLInteger(RTCCDisplay &disp, int val, const char* format, int x, int y, oapi::Sketchpad::TAlign_horizontal align = oapi::Sketchpad::RIGHT) const;
		void DFLTime(RTCCDisplay &display, double val, int x, int y, oapi::Sketchpad::TAlign_horizontal align = oapi::Sketchpad::RIGHT) const;
		void format_time_rtcc(char *buf, double time) const;
	protected:
	};
}