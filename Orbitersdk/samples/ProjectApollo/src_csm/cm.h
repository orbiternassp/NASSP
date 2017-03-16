/****************************************************************************
This file is part of Project Apollo - NASSP

Command Module vessel -- header file (Orbiter 2016 edition)

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

//	KT20170309
/***************************************************************************
Taking advantage of more nuanced staging and attachment controls in
Orbiter 2016, I have started this project to begin to make all stages and
major components of the Apollo-Saturn system independent, as opposed to the
pre-2016 method of time-consuming vessel spawns and de-spawns.

The plan is for the CM to contain systems (methods) associated only with
that vehicle (batteries, AGC and optics, sequencers, etc.), and to communicate
with other vessels via connectors, much like real-life umbilicals. In this
way, the CM would have direct connections with the LES, docking probe, and SM;
control of the IU and other Saturn stages would be through chained connectors.

Validation of the CM split-off would be performed through "pad test" scenarios,
verifying independent operation of systems and pad abort LES firings, in
particular. GTE could be coded in to simulate necessary external stimuli.
****************************************************************************/