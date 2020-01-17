#pragma once
#include "Flow.h"
#include "Capture.h"
#include <string>
#include <unordered_map>

class Modifier
{
private:
	Capture cap;
	std::string from_string, to_string;
	int offset;
	std::unordered_map<Flow, int> flowMap;

private:
	int compare(uint8_t* data, size_t len);
	bool modifyData(const uint8_t* packet, size_t& packetLen);
	bool modifySeqNum(const uint8_t* packet, const size_t packetLen);
	void calcChksum(uint8_t* packet, size_t packetLen, WINDIVERT_ADDRESS& addr);
public:
	Modifier(std::string from_string, std::string to_string);
	void modify();
};
