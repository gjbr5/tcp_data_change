#pragma once
#include <cstdint>
#include "windivert.h"

class Capture
{
public:
	static constexpr size_t PACKETBUFSIZE = 0xffff;
private:
	HANDLE handle;
	uint8_t* packetBuf;

public:
	Capture();
	~Capture();
	size_t get(uint8_t*& pPacket, WINDIVERT_ADDRESS& addr);
	bool set(uint8_t* packet, size_t packetLen, WINDIVERT_ADDRESS& addr);
};
