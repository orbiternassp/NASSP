/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC for Mission C (Header)

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

// MISSION STATES: MISSION C
#define MST_C_INSERTION		10
// Ends at specified time, goes to
#define MST_C_SEPARATION	11
// Ends at 1ST RDZ PHASING BURN, goes to
#define MST_C_COAST1		12
// Ends at 1ST SPS BURN (NCC BURN), goes to
#define MST_C_COAST2		13
// Ends at 2ND SPS BURN (NSR BURN), goes to
#define MST_C_COAST3		14
// Ends at TERMINAL PHASE INITIATION BURN, goes to
#define MST_C_COAST4		15
// Ends at MCC, goes to
#define MST_C_COAST5		16
// Ends at TERMINAL PHASE FINALIZE BURN, goes to
#define MST_C_COAST6		17
// Ends at SEPARATION MANEUVER, goes to
#define MST_C_COAST7		18
// Ends at 3RD SPS BURN, goes to
#define MST_C_COAST8		19
// Ends at 4TH SPS BURN (MIN IMP TEST), goes to
#define MST_C_COAST9		20
// Ends at 5TH SPS BURN, goes to
#define MST_C_COAST10		21
// Ends at 6TH SPS BURN (MIN IMP TEST 2), goes to
#define MST_C_COAST11		22
// Ends at 7TH SPS BURN (TIME ANOMALY ADJ FOR DEORBIT), goes to
#define MST_C_COAST12		23
// Ends at DEORBIT BURN, goes to
#define MST_C_COAST13		24
// Ends at entry interface, goes to entry.
#define MST_C_COAST14		25
#define MST_C_COAST15		26
#define MST_C_COAST16		27
#define MST_C_COAST17		28
#define MST_C_COAST18		29
#define MST_C_COAST19		30
#define MST_C_COAST20		31
#define MST_C_COAST21		32
#define MST_C_COAST22		33
#define MST_C_COAST23		34
#define MST_C_COAST24		35
#define MST_C_COAST25		36
#define MST_C_COAST26		37
#define MST_C_COAST27		38
#define MST_C_COAST28		39
#define MST_C_COAST29		40
#define MST_C_COAST30		41
#define MST_C_COAST31		42
#define MST_C_COAST32		43
#define MST_C_COAST33		44
#define MST_C_COAST34		45
#define MST_C_COAST35		46
#define MST_C_COAST36		47
#define MST_C_COAST37		48
#define MST_C_COAST38		49
#define MST_C_COAST39		50
#define MST_C_COAST40		51
#define MST_C_COAST41		52
#define MST_C_COAST42		53
#define MST_C_COAST43		54
#define MST_C_COAST44		55
#define MST_C_COAST45		56
#define MST_C_COAST46		57
#define MST_C_COAST47		58
#define MST_C_COAST48		59
#define MST_C_COAST49		60
#define MST_C_COAST50		61
#define MST_C_COAST51		62
#define MST_C_COAST52		63
#define MST_C_COAST53		64
#define MST_C_COAST54		65
#define MST_C_COAST55		66
#define MST_C_COAST56		67
#define MST_C_COAST57		68
#define MST_C_COAST58		69
#define MST_C_COAST59		70
#define MST_C_COAST60		71
#define MST_C_ABORT			75
// Entered on abort from orbit, works like COAST13, goes to MST_ORBIT_ENTRY