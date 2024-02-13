/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Lunar Module Rendezvous Radar (Header)

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


// Rendezvous Radar
class LEM_RR : public e_object {
public:
	LEM_RR();
	~LEM_RR();
	void Init(LEM *s, e_object *dc_src, e_object *ac_src, h_Radiator *ant, Boiler *anheat, Boiler *stbyanheat, h_HeatLoad *rreh, h_HeatLoad *rrh);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);
	void DefineAnimations(UINT idx);
	double GetAntennaTempF();
	double GetRadarTrunnionVel() { return -trunnionVel; };
	double GetRadarShaftVel() { return shaftVel; };
	double GetRadarTrunnionPos() { return -asin(sin_trunnion); }
	double GetRadarShaftPos() { return -asin(sin_shaft); }
	double GetRadarRange() { return range; };
	double GetRadarRate() { return rate; };
	double GetSignalStrength() { return SignalStrengthRCVD * 4.0; }
	double GetShaftErrorSignal();
	double GetTrunnionErrorSignal();
	double GetTransmitterPower();
	double GetShaftSin() { return sin_shaft; }
	double GetShaftCos() { return cos_shaft; }
	double GetTrunnionSin() { return sin_trunnion; }
	double GetTrunnionCos() { return cos_trunnion; }
	double dBm2SignalStrength(double RecvdRRPower_dBm);
	void SetRCVDrfProp(double freq, double pow, double gain, double phase) { RCVDfreq = freq; RCVDpow = pow; RCVDgain = gain; RCVDPhase = phase; };
	void GetRadarRangeLGC();
	void GetRadarRateLGC();

	bool IsPowered();
	bool IsDCPowered();
	bool IsACPowered();
	bool IsRangeDataGood() { return RangeLock; };
	bool IsFrequencyDataGood() { return FrequencyLock; };
	bool IsRadarDataGood() { return radarDataGood; };
	bool GetNoTrackSignal() { return NoTrackSignal; }
	
private:
	VESSEL *csm;
	LEM * lem;					// Pointer at LEM
	h_Radiator *antenna;		// Antenna (loses heat into space)
	Boiler *antheater;			// Antenna Heater (puts heat back into antenna)
	Boiler *stbyantheater;		// Antenna Standby Heater (puts heat back into antenna)
	h_HeatLoad *rrheat;		// RR Heat Load
	h_HeatLoad *RREHeat;		// RRE Heat Load
	e_object *dc_source;
	e_object *ac_source;
	double RangeLockTimer;
	double tstime;
	int    isTracking;
	bool   radarDataGood;
	bool NoTrackSignal;
	double trunnionAngle;
	double shaftAngle;
	double trunnionVel;
	double shaftVel;
	double range;
	double rate;
	double internalrange;
	double internalrangerate;
	int scratch[2];             // Scratch data
	int mode;					//Mode I = false, Mode II = true
	double hpbw_factor;			//Beamwidth factor
	double SignalStrengthRCVD;
	double SignalStrengthQuadrant[4];
	double AntennaFrequency;
	double RCVDfreq;
	double RCVDpow;
	double RCVDgain;
	double RCVDPhase;
	double AntennaGain;
	double AntennaWavelength;
	double AntennaPower;
	double AntennaPhase;
	double AntennaPolarValue;
	VECTOR3 U_RRL[4];
	bool AutoTrackEnabled;
	bool FrequencyLock;
	bool TrackingModeSwitch;
	bool RangeLock;
	double ShaftErrorSignal;
	double TrunnionErrorSignal;
	VECTOR3 GyroRates;
	double sin_shaft, cos_shaft, sin_trunnion, cos_trunnion;
	// Animations
	UINT anim_RRPitch, anim_RRYaw;
	double rr_proc[2];
	double rr_proc_last[2];
	//connectors
	//LM_RRtoCSM_RRT_Connector lm_rr_to_csm_connector;

	const double SLEW_RATE_FAST = 7.0*RAD;
	const double SLEW_RATE_SLOW = 1.33*RAD;
};