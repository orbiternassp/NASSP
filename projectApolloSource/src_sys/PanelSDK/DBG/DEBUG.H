/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Radu Poenaru

  System & Panel SDK (SPSDK)

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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.2  2006/02/24 02:02:43  dseagrav
  *	Added compiler version check to make builds win on newer VS by "default".
  *	
  *	Revision 1.1  2005/04/22 14:22:38  tschachim
  *	Initial version
  *	
  **************************************************************************/

#ifndef __DEBUG_CLASS_H__
#define __DEBUG_CLASS_H__

#include <stdio.h>
// DS20060223 Try to do the right thing based on which compiler the user has.
// 13xx is VS .Net 2003, 14xx is VS .Net 2005
#if _MSC_VER > 1300 
#include <fstream>
#else
#include <fstream.h>
#endif

class debug_class
{
public:
	// DS20060223 Try to do the right thing based on which compiler the user has.
#if _MSC_VER > 1300 
	std::ofstream ex_file;
#else
	ofstream ex_file;
#endif
	int debug_mode;
	void Open();
	void Close();
	void Print(char* some);

};


#endif