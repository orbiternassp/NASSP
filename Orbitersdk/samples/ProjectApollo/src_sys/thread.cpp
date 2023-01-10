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

#include "thread.h"
#ifdef _WIN32
#include <windows.h>
#endif

KillableWorker::~KillableWorker() {
    Kill();
}

void KillableWorker::Start(std::function<void()>f) {
    Kill();
    m_running = true;
    m_thread = std::thread([this, f] {
                    f();
                    m_running = false;
               });
}

// Some RTCC algorithms can loop forever if there is a convergence problem,
// so we need a way to kill the worker thread.
// Unfortunately there's no standard way to do that so we have to revert to
// native thread API to do it
void KillableWorker::Kill() {
    if (m_running) {
#ifdef _WIN32
        DWORD exitcode = 0;
        TerminateThread(m_thread.native_handle(), exitcode);
#else
#error Need to implement KillableWorker::Kill for this platform
#endif
        m_running = false;
    }
    if(m_thread.joinable())
        m_thread.detach();
}
