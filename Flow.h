#pragma once
#include <cstdint>
#include <unordered_map>

class Flow
{
private:
	uint32_t srcIp;
	uint16_t srcPort;
	uint32_t dstIp;
	uint16_t dstPort;
	uint8_t protocol;

public:
	Flow(uint32_t srcIp, uint16_t srcPort, uint32_t dstIp, uint16_t dstPort, uint8_t protocol);
	Flow reverse();
	bool operator<(const Flow& o) const;
	bool operator==(const Flow& o) const;

	friend std::hash<Flow>;
};

template<>
struct std::hash<Flow>
{
	size_t operator()(const Flow& flow) const {
		size_t hash = std::hash<uint32_t>()(flow.srcIp);
		hash ^= std::hash<uint16_t>()(flow.srcPort) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<uint32_t>()(flow.dstIp) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<uint16_t>()(flow.dstPort) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<uint8_t>()(flow.protocol) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};
