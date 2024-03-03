#include "RemoteSiteProcessor.h"
/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2024

  Remote Site Telemetry/Command Processor

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

#include "RemoteSiteProcessor.h"

PCMTelemetryProcessor::PCMTelemetryProcessor(std::vector<uint8_t>* buffer, std::vector<uint8_t>* output, std::mutex *mtex) {
	TelemetryBuffer = buffer;
	TelemetryOutput = output;
	BufferMutex = mtex;

	TelemetryOutput->resize(6400);
	std::fill(TelemetryOutput->begin(), TelemetryOutput->end(), 0);
	runProcessor = true;
	ProcessorThread = std::thread{ &PCMTelemetryProcessor::ProcessTelemetry, this };
}

PCMTelemetryProcessor::~PCMTelemetryProcessor()
{
}

void PCMTelemetryProcessor::ProcessTelemetry() {
	int i = 0;
	while (runProcessor) {
		if (TelemetryBuffer->size() > 0) {
			BufferMutex->lock();
			TelemetryOutput[i] = TelemetryBuffer[TelemetryBuffer->front()];
			TelemetryBuffer->erase(TelemetryBuffer->end());
			BufferMutex->unlock();
		}
	}
}
