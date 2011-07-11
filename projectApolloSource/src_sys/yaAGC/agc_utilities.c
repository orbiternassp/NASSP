/*
  Copyright 2003-2005 Ronald S. Burkey <info@sandroid.org>
  
  This file is part of yaAGC.

  yaAGC is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  yaAGC is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with yaAGC; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  In addition, as a special exception, Ronald S. Burkey gives permission to
  link the code of this program with the Orbiter SDK library (or with 
  modified versions of the Orbiter SDK library that use the same license as 
  the Orbiter SDK library), and distribute linked combinations including 
  the two. You must obey the GNU General Public License in all respects for 
  all of the code used other than the Orbiter SDK library. If you modify 
  this file, you may extend this exception to your version of the file, 
  but you are not obligated to do so. If you do not wish to do so, delete 
  this exception statement from your version. 
 
  Filename:	agc_utilities.c
  Purpose:	Miscellaneous functions, useful for agc_engine or for other
  		yaAGC-family functions.
  Compiler:	GNU gcc.
  Contact:	Ron Burkey <info@sandroid.org>
  Reference:	http://www.ibiblio.org/apollo/index.html
  Mods:		04/21/03 RSB.	Began.
  		08/20/03 RSB.	Added uBit to ParseIoPacket.
		05/30/04 RSB	Various.
		07/12/04 RSB	Q is now 16 bits.
		01/31/05 RSB	Added the setsockopt call to 
				EstablishSocket.
		02/27/05 RSB	Added the license exception, as required by
				the GPL, for linking to Orbiter SDK libraries.
		04/30/05 RSB	Added workaround for lack of Win32 close().
		05/14/05 RSB	Corrected website references.
		05/29/05 RSB	Added a couple of AGS equivalents for packet
				function.
*/

// ... and the project's includes.
#include <stdio.h>
#include <string.h>
#include "yaAGC.h"
#include "agc_engine.h"

// Used for socket-operation error codes.
int ErrorCodes = 0;


//////////////////////////////////////////////////////////////////////////////////
// Functions for working with the data packets used for yaAGC "channel i/o".

//--------------------------------------------------------------------------------
// This function can take an i/o channel number and a 15-bit value for it, and
// constructs a 4-byte packet suitable for transmission to yaAGC via a socket.
// Space for the packet must have been allocated by the calling program.  
// Refer to the Virtual AGC Technical Manual, "I/O Specifics" subheading of the 
// "Developer Details" chapter.  Briefly, the 4 bytes are:
//      00pppppp 01pppddd 10dddddd 11dddddd
// where ppppppppp is the 9-bit channel number and ddddddddddddddd is the 15-bit
// value.  Finally, it transmits the packet. Returns 0 on success.
//
// ... Later, the 9-bit "Channel" is actually the u-bit plus an 8-bit channel
// number, but the function works the same.

int
FormIoPacket (int Channel, int Value, unsigned char *Packet)
{
  if (Channel < 0 || Channel > 0x1ff)
    return (1);
  if (Value < 0 || Value > 0x7fff)
    return (1);
  if (Packet == NULL)
    return (1);
  Packet[0] = Channel >> 3;
  Packet[1] = 0x40 | ((Channel << 3) & 0x38) | ((Value >> 12) & 0x07);
  Packet[2] = 0x80 | ((Value >> 6) & 0x3F);
  Packet[3] = 0xc0 | (Value & 0x3F);
  // All done.
  return (0);
}


// Same as FormIoPacket(), but for AGS-style packets rather than AGC-style.

int
FormIoPacketAGS (int Type, int Data, unsigned char *Packet)
{
  if (Type < 0 || Type > 077)
    return (1);
  Data &= 0777777;
  Packet[0] = Type;
  Packet[1] = 0xc0 | ((Data >> 12) & 077);
  Packet[2] = 0x80 | ((Data >> 6) & 077);
  Packet[3] = 0x40 | (Data & 077);
  return (0);
}

//--------------------------------------------------------------------------------
// This function is the opposite of FormIoPacket:  A 4-byte packet representing
// yaAGC channel i/o can be converted to an integer channel-number and value.
// Returns 0 on success.

int
ParseIoPacket (unsigned char *Packet, int *Channel, int *Value, int *uBit)
{
  // Pick the channel number and value from the packet.
  if (0x00 != (0xc0 & Packet[0]))
    return (1);
  if (0x40 != (0xc0 & Packet[1]))
    return (1);
  if (0x80 != (0xc0 & Packet[2]))
    return (1);
  if (0xc0 != (0xc0 & Packet[3]))
    return (1);
  *Channel = ((Packet[0] & 0x1F) << 3) | ((Packet[1] >> 3) & 7);
  *Value = ((Packet[1] << 12) & 0x7000) | ((Packet[2] << 6) & 0x0FC0) |
    (Packet[3] & 0x003F);
  *uBit = (0x20 & Packet[0]);
  return (0);
}

// Same, but for AGS.

int
ParseIoPacketAGS (unsigned char *Packet, int *Type, int *Data)
{
  // Pick the packet type and data from the packet, if the signature matches.
  if (0xc0 != (0xc0 & Packet[1]))
    return (1);
  if (0x80 != (0xc0 & Packet[2]))
    return (1);
  if (0x40 != (0xc0 & Packet[3]))
    return (1);
  *Type = Packet[0];
  *Data = ((Packet[1] & 077) << 12) | ((Packet[2] & 077) << 6) | (Packet[3] & 077);
  return (0);
}

