/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Utilities (Header)

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

#include <vector>

namespace rtcc
{
	union Numeric
	{
		int i;
		double d;
	};

	struct MEDProcessingOptions
	{
		//0 = Text (up to 7 characters), 1 = Integer, 2 = Double, 3 = Time
		int Format = 0;
		//0 = ignore, 1 = error, 2 = insert default value
		int MissingItemOption = 1;
		//For format 2 only
		double ScaleFactor = 1.0;
		//For format 1-3, check on input
		bool mincheck = false;
		//For format 1-3, check on input
		bool maxcheck = false;
		//Values for formats 1-3
		Numeric MinVal;
		Numeric MaxVal;
		//Default value (all formats) if item is missing
		Numeric DefaultValue;
		//For format 0
		std::vector<std::string> TextTable;
	};

	struct MEDProcessingOutput
	{
		std::vector<bool> Ignored;
		std::vector<Numeric> Values;
		int errorItem = 0;
	};

	void AddTextMEDItem(std::vector<rtcc::MEDProcessingOptions> &opt, int MissingItemOption, std::vector<std::string> TextTable, int DefaultValue = 0)
	{
		MEDProcessingOptions temp;

		temp.Format = 0;
		temp.MissingItemOption = MissingItemOption;
		temp.DefaultValue.i = DefaultValue;
		temp.TextTable = TextTable;

		opt.push_back(temp);
	}

	void AddIntegerMEDItem(std::vector<rtcc::MEDProcessingOptions> &opt, int MissingItemOption, bool mincheck, bool maxcheck, int MinVal = 0, int MaxVal = 0, int DefaultValue = 0)
	{
		MEDProcessingOptions temp;

		temp.Format = 1;
		temp.MissingItemOption = MissingItemOption;
		temp.mincheck = mincheck;
		temp.maxcheck = maxcheck;
		temp.MinVal.i = MinVal;
		temp.MaxVal.i = MaxVal;
		temp.DefaultValue.i = DefaultValue;

		opt.push_back(temp);
	}

	void AddDoubleMEDItem(std::vector<rtcc::MEDProcessingOptions> &opt, int MissingItemOption, bool mincheck, bool maxcheck, double ScaleFactor, double MinVal = 0.0, double MaxVal = 0.0, double DefaultValue = 0.0)
	{
		MEDProcessingOptions temp;

		temp.Format = 2;
		temp.MissingItemOption = MissingItemOption;
		temp.mincheck = mincheck;
		temp.maxcheck = maxcheck;
		temp.MinVal.d = MinVal;
		temp.MaxVal.d = MaxVal;
		temp.DefaultValue.d = DefaultValue;
		temp.ScaleFactor = ScaleFactor;

		opt.push_back(temp);
	}

	void AddTimeMEDItem(std::vector<rtcc::MEDProcessingOptions> &opt, int MissingItemOption, bool mincheck, bool maxcheck, double ScaleFactor = 1.0, double MinVal = 0.0, double MaxVal = 0.0, double DefaultValue = 0.0)
	{
		MEDProcessingOptions temp;

		temp.Format = 3;
		temp.MissingItemOption = MissingItemOption;
		temp.mincheck = mincheck;
		temp.maxcheck = maxcheck;
		temp.MinVal.d = MinVal;
		temp.MaxVal.d = MaxVal;
		temp.DefaultValue.d = DefaultValue;
		temp.ScaleFactor = ScaleFactor;

		opt.push_back(temp);
	}

	int GenericMEDProcessingItem(const MEDProcessingOptions &opt, std::string data, Numeric &out, bool &Ignored)
	{
		bool isMissing;

		isMissing = (data == "");
		Ignored = false;

		if (isMissing)
		{
			//Process missing MED item
			switch (opt.MissingItemOption)
			{
			case 0: //Ignore
				Ignored = true;
				return 0;
			case 1: //Error
				return 2;
			case 2: //Insert default value
				switch (opt.Format)
				{
				case 0:
				case 1:
					out.i = opt.DefaultValue.i;
					return 0;
				case 2:
				case 3:
					out.d = opt.DefaultValue.d;
					return 0;
				}
			}
		}

		switch (opt.Format)
		{
		case 0: //Text
		//Compare to table of inputs
			for (unsigned i = 0; i < opt.TextTable.size(); i++)
			{
				if (data == opt.TextTable[i])
				{
					//Found it
					out.i = i;
					return 0;
				}
			}
			return 2;
		case 1: //Integer
		{
			int valtemp;
			if (sscanf(data.c_str(), "%d", &valtemp) != 1)
			{
				return 2;
			}

			//Limit checking
			if (opt.mincheck)
			{
				if (valtemp < opt.MinVal.i)
				{
					return 2;
				}
			}
			if (opt.maxcheck)
			{
				if (valtemp > opt.MaxVal.i)
				{
					return 2;
				}
			}

			out.i = valtemp;
		}
		return 0;
		case 2: //Double
		case 3: //Time
		{
			double valtemp;
			if (opt.Format == 2)
			{
				if (sscanf(data.c_str(), "%lf", &valtemp) != 1)
				{
					return 2;
				}
			}
			else
			{
				double ss;
				int hh, mm;
				bool pos;

				if (sscanf(data.c_str(), "%d:%d:%lf", &hh, &mm, &ss) != 3)
				{
					return 2;
				}

				pos = true;
				if (data[0] == '-')
				{
					pos = false;
					hh = abs(hh);
				}
				valtemp = 3600.0*(double)hh + 60.0*(double)mm + ss;
				if (pos == false)
				{
					valtemp = -valtemp;
				}
			}

			//Limit checking
			if (opt.mincheck)
			{
				if (valtemp < opt.MinVal.d)
				{
					return 2;
				}
			}
			if (opt.maxcheck)
			{
				if (valtemp > opt.MaxVal.d)
				{
					return 2;
				}
			}

			//Scale
			valtemp *= opt.ScaleFactor;

			out.d = valtemp;
		}
		return 0;
		}
		//Invalid format
		return 4;
	}

	int GenericMEDProcessing(const std::vector<MEDProcessingOptions> &def, std::vector<std::string> data, MEDProcessingOutput &out, unsigned start = 0, unsigned stop = 0)
	{
		int err;
		bool btemp;

		//Adjust size
		data.resize(def.size());
		out.Values.clear();
		out.Values.resize(def.size());
		out.Ignored.clear();
		out.Ignored.resize(def.size());

		//Choose MEDs to process
		if (stop == 0)
		{
			stop = def.size();
		}
		else
		{
			stop++;
			if (def.size() >= stop)
			{
				//Use stop
			}
			else
			{
				stop = def.size();
			}
		}

		for (unsigned i = start; i < stop; i++)
		{
			err = GenericMEDProcessingItem(def[i], data[i], out.Values[i], btemp);
			out.Ignored[i] = btemp;
			if (err)
			{
				out.errorItem = i;
				return err;
			}
		}
		return 0;
	}

	bool MEDTimeInputHHMMSS(std::string data, double &hrs)
	{
		MEDProcessingOptions opt;
		Numeric out;
		int err;
		bool Ignored;

		opt.Format = 3;
		opt.MissingItemOption = 1;
		opt.mincheck = false;
		opt.maxcheck = false;
		opt.ScaleFactor = 1.0;

		err = GenericMEDProcessingItem(opt, data, out, Ignored);
		hrs = out.d / 3600.0;

		return (err != 0);
	}
}