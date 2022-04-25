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

#include "orbitersdk.h"
#include "PanelSDK.h"
#include "Internals/Thermal.h"
#include "Internals/Hsystems.h"
#include "Internals/Esystems.h"
#include "SPSDK_WorkerThread.h"


  //create the threadpool and start it
ThreadPool::ThreadPool()
{
	dt = 0;
	terminate = false;
	
	numThreads = std::thread::hardware_concurrency() - 2;
	if (numThreads < 1) { 
		numThreads = 1;
	}

	numThreads = 4;

	idleThreads = numThreads;
	for (int i = 0; i < numThreads; i++) {
		workerPool.push_back(std::thread(&ThreadPool::workerThreadFunction, this));
		workerPool[i].detach();
	}
}

ThreadPool::~ThreadPool()
{
	terminate = true;
	cv.notify_all();
}

//copies a predetermined queue vector of jobs to be run
void ThreadPool::StartWork(const double Setdt, const std::vector<void*> SetQueue, SPSDK_ThreadPoolType::systype typ) {

	calltype = typ;

	{
		std::unique_lock<std::mutex> lock(readQueueLock);
		dt = Setdt;
		queue = SetQueue;
	}
	idleThreads = 0;

	std::unique_lock<std::mutex> RunningLock(runningLock);
	cv.notify_all();
	cvWork.wait(RunningLock, [this]() { return (idleThreads == numThreads); });
}

//always runs waiting for work
void ThreadPool::workerThreadFunction()
{
	void* Job;				//pointer to the system that needs updating
	h_object* h_ptr;		//pointer to hydraulic objects so that their members can be used
	e_object* e_ptr;		//pointer to electrical objects so that their members can be used
	therm_obj* therm_ptr;	//pointer to thermal objects so that their members can be used

	while (!terminate) {

		{
			std::unique_lock<std::mutex> RunningLock(runningLock);
			while (queue.empty()) {
				if (++idleThreads > numThreads) { idleThreads = numThreads; }
				cvWork.notify_all();
				cv.wait(RunningLock);
			}
		}

		{
			std::unique_lock<std::mutex> lock(readQueueLock);
			if (!queue.empty()) {
				Job = queue.back();
				queue.pop_back();
			}
			else {
				continue;
			}
		}

		switch (calltype) {
			case SPSDK_ThreadPoolType::systype::hydraulicRefresh:
				h_ptr = (h_object*)Job;
				h_ptr->refresh(dt);
				break;
			case SPSDK_ThreadPoolType::systype::hydraulicUpdate:
				h_ptr = (h_object*)Job;
				h_ptr->UpdateFlow(dt);
				break;
			case SPSDK_ThreadPoolType::systype::electricRefresh:
				e_ptr = (e_object*)Job;
				e_ptr->refresh(dt);
				break;
			case SPSDK_ThreadPoolType::systype::electricUpdate:
				e_ptr = (e_object*)Job;
				e_ptr->UpdateFlow(dt);
				break;
			case SPSDK_ThreadPoolType::systype::thermal:
				therm_ptr = (therm_obj*)Job;
				therm_ptr->thermic(dt);
				break;
		}
	}
	return;
}
