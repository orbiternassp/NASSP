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
  *	Revision 1.1  2005/04/22 14:22:37  tschachim
  *	Initial version
  *	
  **************************************************************************/

#ifndef _VECTORS_
#define _VECTORS_


#include "matrix.h"
class matrix;
class vector3
{ public:
  double x;
  double y;
  double z;

  //constructors
      vector3(double _x,double _y,double _z) {set(_x,_y,_z);}
	  vector3()	{set(0.0,0.0,0.0);}
	  vector3(vector3 &v) {set(v.x,v.y,v.z);}
  //operators
	//vector to vector
  vector3 operator+ (vector3);
  void	  operator+= (vector3);
  vector3 operator- (vector3);
  void    operator-=(vector3);
  vector3 operator* (vector3);
  vector3 operator/ (vector3);
  double operator% (vector3);   //dot product
  vector3 operator! ();
	//vector to double
  vector3 operator+ (double);
  void    operator+=(double);
  vector3 operator- (double);
  void    operator-=(double);
  vector3 operator* (double);
  void    operator*=(double);
  vector3 operator/ (double);
  void    operator/=(double);
    //vector to matrix
  vector3 operator* (matrix &m);
  void operator*=(matrix &m);
  //functions
  void set(double _x,double _y,double _z);
  double selfdot();
  vector3 normalize();
  void selfnormalize();
  double length();
  double mod();
  double sqmod();
  double distance(vector3 &v);
  double angle(vector3 &v);
  vector3 inplane(vector3 &v1,vector3 &v2); //return the projection of this onto plane v1,v2

};

vector3 _vector3(double _x, double _y, double _z);
#endif