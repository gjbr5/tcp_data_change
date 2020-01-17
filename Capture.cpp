#include "Capture.h"
#include <iostream>

Capture::Capture() : handle(WinDivertOpen("true", WINDIVERT_LAYER_NETWORK, 0, 0)), packetBuf(new uint8_t[PACKETBUFSIZE]()) {
	if (handle == INVALID_HANDLE_VALUE) {
		std::cerr << "error: failed to open the WinDivert device (" << GetLastError() << ")\n";
		throw EXIT_FAILURE;
	}
	if (!WinDivertSetParam(handle, WINDIVERT_PARAM_QUEUE_LENGTH, WINDIVERT_PARAM_QUEUE_LENGTH_MAX)) {
		std::cerr << "error: failed to set packet queue length (" << GetLastError() << ")\n";
		throw EXIT_FAILURE;
	}
	if (!WinDivertSetParam(handle, WINDIVERT_PARAM_QUEUE_TIME, WINDIVERT_PARAM_QUEUE_TIME_MAX)) {
		std::cerr << "error: failed to set packet queue time (" << GetLastError() << ")\n";
		throw EXIT_FAILURE;
	}
	if (!WinDivertSetParam(handle, WINDIVERT_PARAM_QUEUE_SIZE, WINDIVERT_PARAM_QUEUE_SIZE_MAX)) {
		std::cerr << "error: failed to set packet queue size (" << GetLastError() << ")\n";
		throw EXIT_FAILURE;
	}
}

Capture::~Capture() {
	delete[] packetBuf;
	WinDivertClose(handle);
}

size_t Capture::get(uint8_t*& pPacket, WINDIVERT_ADDRESS& addr) {
	size_t packetLen;
	if (!WinDivertRecv(handle, packetBuf, PACKETBUFSIZE, &packetLen, &addr)) {
		std::cerr << "warning: failed to read packet (" << GetLastError() << ")\n";
		pPacket = nullptr;
	}
	else
		pPacket = packetBuf;
	return packetLen;
}

bool Capture::set(uint8_t* packet, size_t packetLen, WINDIVERT_ADDRESS& addr) {
	return WinDivertSend(handle, packet, packetLen, nullptr, &addr);
}
