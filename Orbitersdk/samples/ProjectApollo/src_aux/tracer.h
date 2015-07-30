/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Tracing class. This is defined as a class so that it can be allocated on the
  stack in each function, then the constructor and destructor message will
  always be produced automatically when it's entered and exited. It also makes
  keeping track of the function name easy, so that you don't need to put it
  in every trace message manually.

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
  *	Revision 1.2  2005/12/30 03:27:54  movieman523
  *	Hopefully fixed some compilation errors with VC++ 2005
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/
//############################################################################//
class Tracer{
public:
	Tracer(char *s);
	~Tracer();
	void print(char *s);

private:
	char *ThisFunc;
};
//############################################################################//
// You'll need to define trace_file as a string holding the name of the file
// that you want to write to.  
//############################################################################//
extern char trace_file[];
//############################################################################//
// Macros so we can remove tracing from release builds. TRACESETUP should always
// be the first line in the function, so that the constructor is called before
// those of any other objects you allocate on the stack. Otherwise you may not get
// to see the "Starting" message if one of those constructors crashes.
//
// Unfortunately in certain circumstances the destructor seems to get called when
// an error occurs, so you can't definitely rely on the "Done" message to tell
// you that the function completely successfully... if you're not certain, then
// put in another TRACE message just before it should exit.
//############################################################################//
#ifdef _DEBUG
#define TRACESETUP(s) Tracer traceobj(s);
#define TRACE(s) {traceobj.print(s);}
#else
#define TRACESETUP(s)
#define TRACE(s) {}
#endif  
//############################################################################//