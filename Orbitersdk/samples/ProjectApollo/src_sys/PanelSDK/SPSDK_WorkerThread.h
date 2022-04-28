/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2022

  System & Panel SDK (SPSDK) Multithreading Functionality

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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/COPYING.txt
  for more details.

  **************************************************************************/

#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <functional>
#include <atomic>

namespace SPSDK_ThreadPoolType {
	enum systype { hydraulicRefresh, hydraulicUpdate, electricRefresh, electricUpdate, thermal };
}

class ThreadPool {
public:
	ThreadPool();
	~ThreadPool();
	void StartWork(const double Setdt, std::vector<void*> *SetQueue, SPSDK_ThreadPoolType::systype typ);
private:
	SPSDK_ThreadPoolType::systype calltype;
	double dt;
	//std::atomic<bool> working;
	std::atomic<bool> terminate;
	int idleThreads;
	int numThreads;
	int queuePosition;
	std::vector<std::thread> workerPool;
	std::vector<void*> *queue;
	std::condition_variable cv;
	std::condition_variable cvWork;
	std::mutex readQueueLock;
	std::mutex runningLock;
	std::mutex workLock;
	void workerThreadFunction();
};

