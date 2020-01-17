#include "Modifier.h"
#include <iostream>

Modifier::Modifier(std::string from_string, std::string to_string)
	: from_string(from_string), to_string(to_string), offset(to_string.size() - from_string.size()) {}

void Modifier::modify() {
	uint8_t* packet;
	size_t packetLen;
	WINDIVERT_ADDRESS addr;
	packetLen = cap.get(packet, addr);
	if (packet) {
		if (modifySeqNum(packet, packetLen) | modifyData(packet, packetLen))
			calcChksum(packet, packetLen, addr);
		cap.set(packet, packetLen, addr);
	}
}

bool Modifier::modifyData(const uint8_t* packet, size_t& packetLen) {
	size_t modified = 0;
	if (!packet)
		return modified;
	PWINDIVERT_IPHDR ipHdr;
	PWINDIVERT_TCPHDR tcpHdr;
	uint8_t* data;
	size_t dataLen;
	WinDivertHelperParsePacket(packet, packetLen,
		&ipHdr, nullptr, nullptr,
		nullptr, nullptr, &tcpHdr, nullptr,
		reinterpret_cast<PVOID*>(&data), &dataLen, nullptr, nullptr);
	if (!ipHdr || dataLen < from_string.size())
		return modified;
	int cnt = 0;
	for (size_t i = 0; i <= dataLen - from_string.size(); i++) {
		if (compare(data + i, dataLen - i))
			continue;
		if (offset < 0)
			for (size_t j = i + to_string.size(); j < dataLen; j++)
				data[j] = data[j - offset];
		else if (offset > 0)
			for (size_t j = dataLen; j >= i + to_string.size(); j--)
				data[j] = data[j - offset];
		for (size_t j = 0; j < to_string.size(); j++)
			data[i + j] = to_string[j];
		modified++;
		dataLen += offset;
		i += to_string.size() - 1;
	}
	if (modified) {
		Flow flow{ ipHdr->SrcAddr, tcpHdr->SrcPort, ipHdr->DstAddr, tcpHdr->DstPort, ipHdr->Protocol };
		flowMap.insert({ flow, 0 }).first->second += modified * offset;
		packetLen += modified * offset;
		ipHdr->Length = htons(ntohs(ipHdr->Length) + modified * offset);
		char ipBuf[16] = { 0 };
		WinDivertHelperFormatIPv4Address(ntohl(ipHdr->SrcAddr), ipBuf, 16);
		std::cout << ipBuf << ":" << ntohs(tcpHdr->SrcPort) << " -> ";
		WinDivertHelperFormatIPv4Address(ntohl(ipHdr->DstAddr), ipBuf, 16);
		std::cout << ipBuf << ":" << ntohs(tcpHdr->DstPort) << " : ";
		std::cout << modified << " Data Modified.\n";
	}
	return modified;
}

bool Modifier::modifySeqNum(const uint8_t* packet, const size_t packetLen) {
	PWINDIVERT_IPHDR ipHdr = nullptr;
	PWINDIVERT_TCPHDR tcpHdr = nullptr;
	WinDivertHelperParsePacket(packet, packetLen,
		&ipHdr, nullptr, nullptr,
		nullptr, nullptr, &tcpHdr, nullptr,
		nullptr, nullptr, nullptr, nullptr);
	bool modified = false;
	if (tcpHdr) {
		Flow flow{ ipHdr->SrcAddr, tcpHdr->SrcPort, ipHdr->DstAddr, tcpHdr->DstPort, ipHdr->Protocol };
		auto it = flowMap.find(flow);
		if (it != flowMap.end() && it->second != 0) {
			tcpHdr->SeqNum = htonl(ntohl(tcpHdr->SeqNum) + it->second);
			modified = true;
		}
		it = flowMap.find(flow.reverse());
		if (it != flowMap.end() && it->second != 0) {
			tcpHdr->AckNum = htonl(ntohl(tcpHdr->AckNum) + it->second);
			modified = true;
		}
	}
	return modified;
}

int Modifier::compare(uint8_t* data, size_t len) {
	if (len < from_string.size())
		return -1;
	for (size_t i = 0; i < from_string.size(); i++)
		if (data[i] != from_string[i])
			return data[i] - from_string[i];
	return 0;
}

void Modifier::calcChksum(uint8_t* packet, size_t packetLen, WINDIVERT_ADDRESS& addr) {
	if (!WinDivertHelperCalcChecksums(packet, packetLen, &addr, 0))
		std::cout << "Checksum Error\n";
}
