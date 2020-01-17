#include "Flow.h"

Flow::Flow(uint32_t srcIp, uint16_t srcPort, uint32_t dstIp, uint16_t dstPort, uint8_t protocol)
	: srcIp(srcIp), srcPort(srcPort), dstIp(dstIp), dstPort(dstPort), protocol(protocol) {}

Flow Flow::reverse() {
	return Flow{ dstIp, dstPort, srcIp, srcPort, protocol };
}

bool Flow::operator<(const Flow& o) const {
	if (srcIp != o.srcIp)
		return srcIp < o.srcIp;
	if (srcPort != o.srcPort)
		return srcPort < o.srcPort;
	if (dstIp != o.dstIp)
		return dstIp < o.dstIp;
	if (dstPort != o.dstPort)
		return dstPort < o.dstPort;
	if (protocol != o.protocol)
		return protocol < o.protocol;
}

bool Flow::operator==(const Flow& o) const {
	return srcIp == o.srcIp && srcPort == o.srcPort && dstIp == o.dstIp && dstPort == o.dstPort && protocol == o.protocol;
}
