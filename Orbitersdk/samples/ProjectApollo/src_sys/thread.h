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

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Event
{
public:
    Event() { m_counter = 0; }
    ~Event() = default;
    void Raise() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter++;
            if (m_counter <= 0) {
                return;
            }
        }
        m_cv.notify_all();
    }
    void Wait() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&]() { return m_counter > 0; });
        m_counter--;
    }
private:
    int m_counter;
    std::condition_variable m_cv;
    std::mutex m_mutex;
};

class Runnable
{
public:
    Runnable() { m_stop.store(false); }
    virtual ~Runnable() {}
    void Kill() {
        m_stop.store(true);
        timeStepEvent.Raise();
    }
protected:
    virtual void Run() = 0;
    void Start() {
        m_thread = std::thread(&Runnable::Run, this);
        m_thread.detach();
    }
    std::thread m_thread;
    std::atomic<bool> m_stop;
    Event timeStepEvent;
};

#endif
