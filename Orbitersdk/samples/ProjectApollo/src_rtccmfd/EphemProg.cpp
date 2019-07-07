// =================================================================================================================================
// The MIT Licence:
//
// Copyright (C) 2016 Jarmo Nikkanen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
// files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
// modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// =================================================================================================================================

#include "Orbitersdk.h"
#include "ARCore.h"

inline void agcSwap(VECTOR3 *a)
{
	double b = a->z; a->z=a->y; a->y=b;
}

inline double angle(VECTOR3 a, VECTOR3 b)
{
	return acos(dotp(unit(a), unit(b)));
}

void agcPolar2Cartesian_LH(VECTOR3 *_pv, VECTOR3 *_vv)
{
	double cx = cos((*_pv).x), sx = sin((*_pv).x);
	double cy = sin((*_pv).y), sy = cos((*_pv).y);
	double r = (*_pv).z * AU;

	VECTOR3 _r = _V(sy*cx, cy, sy*sx);

	*_pv = _r * r;

	if (_vv == NULL) return;

	VECTOR3 _y = _V(-cy * cx, sy, -cy * sx);
	VECTOR3 _x = _V(-sx, 0, cx);

	*_vv = _r * ((*_vv).z*AU) + _y * (r*(*_vv).y) + _x * (r*(*_vv).x*sy);
}

// -------------------------------------------------------------------------------
//
int ARCore::agcCelBody_RH(CELBODY *Cel, double mjd, int Flags, VECTOR3 *Pos, VECTOR3 *Vel)
{
	int ret = agcCelBody_LH(Cel, mjd, Flags, Pos, Vel);
	if (Pos) agcSwap(Pos);
	if (Vel) agcSwap(Vel);
	return ret;
}

// -------------------------------------------------------------------------------
//
int ARCore::agcCelBody_LH(CELBODY *Cel, double mjd, int Flags, VECTOR3 *Pos, VECTOR3 *Vel)
{

	//if (Cel==NULL) assert(false);

	//
	// Local Variables and structures
	//
	struct s_ret {
		VECTOR3 TrPos;
		VECTOR3 TrVel;
		VECTOR3 BCPos;
		VECTOR3 BCVel;
	} ret;
	

	int info = Cel->clbkEphemeris(mjd, Flags, (double *)&ret);
	//
	// Data Conversions
	//
	if (info&EPHEM_POLAR) {	
		if (info&EPHEM_TRUEVEL && info&EPHEM_TRUEPOS) agcPolar2Cartesian_LH(&ret.TrPos, &ret.TrVel);
		else if (info&EPHEM_TRUEPOS)				  agcPolar2Cartesian_LH(&ret.TrPos, NULL);

		if (info&EPHEM_BARYVEL && info&EPHEM_BARYPOS) agcPolar2Cartesian_LH(&ret.BCPos, &ret.BCVel);
		else if (info&EPHEM_TRUEPOS)                  agcPolar2Cartesian_LH(&ret.BCPos, NULL);
	}
	

	if (Flags&EPHEM_TRUEPOS) {

		if (info&EPHEM_TRUEPOS) {
			if (Pos) *Pos = ret.TrPos;
			if (Vel) *Vel = ret.TrVel;
			return info&(EPHEM_TRUEPOS|EPHEM_TRUEVEL);
		}
		else {
			if (Pos) *Pos = ret.BCPos;
			if (Vel) *Vel = ret.BCVel;
			return info&(EPHEM_BARYPOS|EPHEM_BARYVEL);	
		}
	}

	if (Flags&EPHEM_BARYPOS) {

		if (info&EPHEM_BARYPOS) {
			if (Pos) *Pos = ret.BCPos;
			if (Vel) *Vel = ret.BCVel;
			return info&(EPHEM_BARYPOS|EPHEM_BARYVEL);		
		}
		else {
			if (Pos) *Pos = ret.TrPos;
			if (Vel) *Vel = ret.TrVel;
			return info&(EPHEM_TRUEPOS|EPHEM_TRUEVEL);
		}
	}

	if (info&EPHEM_BARYPOS) {
		if (Pos) *Pos = ret.BCPos;
		if (Vel) *Vel = ret.BCVel;
		return info&(EPHEM_BARYPOS|EPHEM_BARYVEL);	
	}
	
	if (Pos) *Pos = ret.TrPos;
	if (Vel) *Vel = ret.TrVel;
	return info&(EPHEM_TRUEPOS|EPHEM_TRUEVEL);	
}


// ================================================================================================
//
int DoubleToBuffer(double x, double q, int m)
{
	int c=0, out=0, f=1;
	
	x = x * (268435456.0 / pow(2.0, fabs(q)));
	
	if (m) c = 0x3FFF & (((int)fabs(x))>>14);	// High word
	else   c = 0x3FFF & ((int)fabs(x));		    // Low word
	
	if (x<0.0) c = 0x7FFF & (~c); // Polarity change
	
	while (c!=0) {
		out += (c&7) * f;
		f*=10;	c = c>>3;
	}
	return out;
}

// =================================================================================================================================
//

void ARCore::AGCEphemeris(double T0, double Epoch, double TEphem0)
{
	double dt  = 0.25;
	double Tm0 = T0 - dt*28.0;

	double x[57];	
	double y[57];
	double xx[10];	
	double yy[10];
	double zz[10];

	VECTOR3 pos[57];
	
	double dscale = 1.0/pow(2.0, 31.0);
	double tscale = 8.64e6/pow(2.0, 26.0);

	MATRIX3 Mat = OrbMech::J2000EclToBRCS(Epoch);//J2000EclToEqu(Epoch);
	
	// ----- Solar Ephemeris -----

	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	CELBODY *cEarth = oapiGetCelbodyInterface(hEarth);
	
	VECTOR3 _SPos1, _SPos2;

	agcCelBody_RH(cEarth, T0, EPHEM_TRUEPOS, &_SPos1);
	agcCelBody_RH(cEarth, T0+0.1, EPHEM_TRUEPOS, &_SPos2);

	double avel = (pow(2.0,26.0)*angle(_SPos2,_SPos1)*10.0) / (PI2 * 86400.0 * 100.0);

	_SPos1 = mul(Mat, -_SPos1);
	_SPos2 = mul(Mat, -_SPos2);

	VECTOR3 _SVel = (_SPos2 - _SPos1) * (10.0/86400.0) * 0.01;
	
	// ----- Lunar Ephemeris -----

	OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	CELBODY *cMoon = oapiGetCelbodyInterface(hMoon);

	VECTOR3 _LPos;

	agcCelBody_RH(cMoon, T0, EPHEM_TRUEPOS, &_LPos);

	for (int i=0;i<57;i++) {
		x[i] = (Tm0 - T0) * tscale;	
		VECTOR3 _x;
		agcCelBody_RH(cMoon, Tm0, EPHEM_TRUEPOS, &_x);
		pos[i] = mul(Mat, _x) * dscale;
		Tm0+=dt;
	}

	for (int i=0;i<57;i++) y[i]=pos[i].x;
	OrbMech::SolveSeries(x, y, 57, xx, 10);
	for (int i=0;i<57;i++) y[i]=pos[i].y;
	OrbMech::SolveSeries(x, y, 57, yy, 10);
	for (int i=0;i<57;i++) y[i]=pos[i].z;
	OrbMech::SolveSeries(x, y, 57, zz, 10);
	
	int mem = 02033;

	FILE *file = fopen("EphemData.txt","w");

	fprintf(file,"Epoch   = %6.6f (MJD) Epoch of Basic Reference Coordinate System (40221.525=Colossus249)\n",Epoch);
	fprintf(file,"TEphem0 = %6.6f (MJD) Ephemeris Time Zero (40038=Colossus249)\n",TEphem0);
	fprintf(file,"TIMEM0  = %6.6f (MJD) Mission mid-range time. (Mission Specific value. 40214.5=Apollo8)\n\n",T0);


	fprintf(file,"------- TIMEM0 -------\n");

	double t = (T0 - TEphem0) * 8640000.0;

	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(t, 56.0, 0)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(t, 42.0, 0)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(t, 28.0, 0)); mem++;


	fprintf(file,"------- Lunar Ephemeris -------\n");

	for (int i=9;i>=0;i--) {

		fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(xx[i], 0.0, 1)); mem++;
		fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(xx[i], 0.0, 0)); mem++;
		fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(yy[i], 0.0, 1)); mem++;
		fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(yy[i], 0.0, 0)); mem++;
		fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(zz[i], 0.0, 1)); mem++;
		fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(zz[i], 0.0, 0)); mem++;
	}
	fprintf(file,"------- Solar Ephemeris -------\n");

	mem = 02132;

	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SPos1.x, 38.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SPos1.x, 38.0, 0)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SPos1.y, 38.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SPos1.y, 38.0, 0)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SPos1.z, 38.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SPos1.z, 38.0, 0)); mem++;

	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SVel.x, 9.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SVel.x, 9.0, 0)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SVel.y, 9.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SVel.y, 9.0, 0)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SVel.z, 9.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(_SVel.z, 9.0, 0)); mem++;

	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(avel, 0.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(avel, 0.0, 0)); mem++;

	/*
	fprintf(file,"\n------- Earth Rotation Parameters %2.4f in J2000 -------\n\n",Epoch);

	VECTOR3 _EI = tmul(Mat, _V(1,0,0));
	VECTOR3 _EJ = tmul(Mat, _V(0,1,0));
	VECTOR3 _EK = tmul(Mat, _V(0,0,1));

	double Sid = 86164.09890413;
	double Pre = -9412846.0;
	double azo = 0.7753206164;	//Revolutions not rad
	
	double a = limit(PI2*azo + PI2*(J2000-lnEpoch) / (Sid*OPDAY));

	VECTOR3 _J, _K;
	VECTOR3 _I = _EI*cos(a) + _EJ*sin(a);
	VECTOR3 _L = crossp(_EK, _K_ECL);

	double lan = Longitude(_L, _I_ECL, _K_ECL);
	double obl = angle(_EK, _K_ECL);
	double rot = Longitude(_I, _L, _EK);

	mem = 01711;

	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(azo, 0.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(azo, 0.0, 0)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(0, 0.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(0, 0.0, 0)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(0, 0.0, 1)); mem++;
	fprintf(file,"EMEM%o %d\n", mem, DoubleToBuffer(0, 0.0, 0)); mem++;

	fprintf(file,"AX0          = %2.8f(deg)\n", 0);
	fprintf(file,"AY0          = %2.8f(deg)\n", 0);
	fprintf(file,"AZ0          = %2.8f(deg)\n", azo*360.0);
	fprintf(file,"LAN          = %2.6f(rad) (no precession in the Orbiter)\n", limit(lan));
	fprintf(file,"SidRotOffset = %2.6f(rad) (no precession in the Orbiter)\n", limit(rot));
	fprintf(file,"SidRotPeriod = %2.6f(sec)\n", Sid);
	fprintf(file,"Obliquity    = %2.6f(rad)\n", obl);

	fprintf(file,"\n------- Moon Rotation Parameters %2.4f in J2000 -------\n\n",T0);

	EclipticOfDate(Epoch, &_EI, &_EJ, &_EK);

	double t  = (T0 - lnEpoch) * 86400.0;
	double rt = limit(lnRotOff + PI2 * t / lnRotPer);
	double ob = lnObliquity;
	double ln = limit(lnLanOff + PI2 * t / lnLanPer); 

	VECTOR3 _i = _EI*cos(ln) + _EJ*sin(ln);
	VECTOR3 _j = unit(crossp(_EK, _i));

	_K = _EK*cos(ob) - _j*sin(ob);
	_I = _i*cos(rt) + unit(crossp(_K, _i))*sin(rt);
	_J = unit(crossp(_K, _I));
	_L = crossp(_K, _K_ECL);

	lan = Longitude(_L, _I_ECL, _K_ECL);
	rot = Longitude(_I, _L, _K);
	obl = angle(_K, _K_ECL);
	Sid = 27.321662;
	Pre = -6792.9076;

	rot += PI2 * (J2000-T0) / Sid;

	fprintf(file,"LAN          = %2.6f(rad) (no precession in the Orbiter)\n", limit(lan));
	fprintf(file,"SidRotOffset = %2.6f(rad) (no precession in the Orbiter)\n", limit(rot));
	fprintf(file,"SidRotPeriod = %2.6f(sec)\n", Sid*86400.0);	
	fprintf(file,"Obliquity    = %2.6f(rad)\n", obl);
	*/
	
	//
	// Lunar Ephem Analysis ----------------------------------------------------
	//

	fputs("\n\nDeviation analysis\n\n", file);
	
	for (int i=0;i<57;i++) {
		double a = xx[0]; double b = yy[0]; 
		double c = zz[0]; double h = x[i];		
		
		for (int k=1;k<10;k++) { a += xx[k]*h; b += yy[k]*h; c += zz[k]*h; h *= x[i];	}

		VECTOR3 _g = _V(a,b,c);
		double m = T0 + x[i]/tscale;

		fprintf(file,"%.3f = %g (arcsec)\n",m,DEG*angle(pos[i], _g)*3600.0);
	}

	fprintf(file,"\n\n");

	fprintf(file,"------- State Vectors at TIMEM0 -------\n");

	fprintf(file,"Moon_x = %6.6f\n",_LPos.x);
	fprintf(file,"Moon_y = %6.6f\n",_LPos.y);
	fprintf(file,"Moon_z = %6.6f\n",_LPos.z);

	fprintf(file,"EMBC_x = %6.6f\n",_SPos1.x);
	fprintf(file,"EMBC_y = %6.6f\n",_SPos1.y);
	fprintf(file,"EMBC_z = %6.6f\n\n",_SPos1.z);

	if (file) fclose(file);
}