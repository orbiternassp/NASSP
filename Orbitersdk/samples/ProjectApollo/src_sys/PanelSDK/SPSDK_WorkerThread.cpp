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
	queuePosition = -1;
	
	numThreads = std::thread::hardware_concurrency() - 2;
	if (numThreads < 1) { 
		numThreads = 1;
	}

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
void ThreadPool::StartWork(const double Setdt, std::vector<void*> *SetQueue, int updateType) {

	calltype = updateType;

	{
		std::unique_lock<std::mutex> lock(readQueueLock);
		dt = Setdt;
		queue = SetQueue;
		queuePosition = queue->size() - 1;
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

	while (!terminate) {

		{
			std::unique_lock<std::mutex> RunningLock(runningLock);
			while (queuePosition < 0) {
				if (++idleThreads > numThreads) { idleThreads = numThreads; }
				cvWork.notify_all();
				cv.wait(RunningLock);
			}
		}

		{
			std::unique_lock<std::mutex> ReadLock(readQueueLock);
			if (queuePosition >= 0) {
				Job = (*queue)[queuePosition];
				--queuePosition;
			}
			else {
				continue;
			}
		}

		switch (calltype) {
			case SPSDKthreadPoolHydraulicRefresh:
				((h_object*)Job)->refresh(dt);
				break;
			case SPSDKthreadPoolHydraulicUpdate:
				((h_object*)Job)->UpdateFlow(dt);
				break;
			case SPSDKthreadPoolElectricRefresh:
				((e_object*)Job)->refresh(dt);
				break;
			case SPSDKthreadPoolElectricUpdate:
				((e_object*)Job)->UpdateFlow(dt);
				break;
			case SPSDKthreadPoolThermal:
				((therm_obj*)Job)->thermic(dt);
				break;
		}
	}
	return;
}
