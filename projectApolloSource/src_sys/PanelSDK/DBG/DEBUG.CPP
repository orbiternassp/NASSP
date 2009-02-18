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
  *	Revision 1.1  2005/04/22 14:22:38  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "debug.h"


void debug_class::Open()
{ex_file.open("modules\\lc_debug.txt");
debug_mode=1;
};
void debug_class::Close()
{if (debug_mode==1)
	ex_file.close();
debug_mode=0;
}

void debug_class::Print(char* some)
{if (debug_mode==1)
	ex_file<<some;
}
