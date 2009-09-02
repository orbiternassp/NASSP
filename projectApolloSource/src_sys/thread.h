/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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
#if !defined(_THREAD_H)
#define _THREAD_H
#include <windows.h>



class Thread
{
public:
    Thread ( DWORD (WINAPI * callback) (void* arg), void* arg)
    {
        handle = CreateThread (0,0,callback,arg,CREATE_SUSPENDED,&threadId);
    }
    ~Thread ()           { CloseHandle (handle); }
    void Resume ()       { ResumeThread (handle); }
    void WaitForDeath () { WaitForSingleObject (handle, INFINITE); }
private:
    HANDLE handle;
    DWORD  threadId;
};

class Mutex
{
public:
    Mutex () { InitializeCriticalSection (& critSection); }
    ~Mutex () { DeleteCriticalSection (& critSection); }
    inline void Acquire () { EnterCriticalSection (& critSection); }
    inline void Release () { LeaveCriticalSection (& critSection); }
private:
    CRITICAL_SECTION critSection;
};

class Lock
{
public:
    Lock ( Mutex & mutex ): _mutex(mutex) { _mutex.Acquire(); }
    ~Lock () { _mutex.Release(); }
private:
    Mutex & _mutex;
};

class Event
{
public:
    Event ()  { handle = CreateEvent (0, FALSE, FALSE, 0); }
    ~Event () { CloseHandle (handle); }

    void Raise ()  { SetEvent (handle); }
    void Wait ()   { WaitForSingleObject (handle, INFINITE); }
private:
    HANDLE handle;
};


class Runnable
{
public:
    Runnable ();
    virtual ~Runnable () {}
    void Kill ();
protected:
    virtual void Run () = 0;
    static DWORD WINAPI ThreadEntry (void *pArg);
    Thread     thread;
};

#endif