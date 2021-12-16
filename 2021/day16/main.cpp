#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class Packet {
	public:
		static const int HEADER_LENGTH = 6;

	public:
		Packet() {
			this->type    = 0;
			this->version = 0;
		}

		virtual ~Packet() {

		}

		virtual uint32_t parse(const std::string &bitstream, int offset) {
			this->version = getU8(bitstream, offset + 0, 3);
			this->type    = getU8(bitstream, offset + 3, 3);

			return HEADER_LENGTH;
		}

		uint8_t getType() const {
			return this->type;
		}

		uint8_t getVersion() const {
			return this->version;
		}

		virtual uint32_t getLength() const {
			return HEADER_LENGTH;
		}

		virtual uint32_t getVersionSum() const {
			return this->version;
		}

		virtual uint64_t getValue() const {
			DBG(("Packet::%s(): CALL", __func__));
			return 0;
		}

	protected:
		static uint8_t getU8(const std::string &bitstream, int offset, int bitsNo) {
			return getU16(bitstream, offset, bitsNo);
		}

		static uint16_t getU16(const std::string &bitstream, int offset, int bitsNo) {
			uint16_t ret = 0;

			for (int bitNo = 0; bitNo < bitsNo; bitNo++) {
				ret <<= 1;
				ret  |= (bitstream[offset + bitNo] - '0');
			}

			return ret;
		}

	private:
		uint8_t type;
		uint8_t version;
};


static Packet *parsePacket(const std::string &bitstream, int offset);


class LiteralValuePacket : public Packet {
	public:
		static const int TYPE = 4;

	public:
		LiteralValuePacket(const Packet &base) : Packet(base) {
			this->length = 0;
		}

		virtual uint32_t parse(const std::string &bitstream, int offset) {
			this->length = offset;

			offset += HEADER_LENGTH;

			{
				uint8_t value;

				do {
					value = getU8(bitstream, offset, 5);

					this->numbers.push_back(value & 0x0f);

					offset += 5;
				} while ((value & 0x10) != 0);
			}

			this->length = offset - this->length;

			DBG(("LiteralValuePacket::parse() => %u (values: %zd)", this->length, this->numbers.size()));

			return getLength();
		}

		virtual uint32_t getLength() const {
			return this->length;
		}

		virtual uint64_t getValue() const {
			uint64_t ret = 0;

			DBG(("LiteralValuePacket::%s(): CALL", __func__));

			for (auto nibble : numbers) {
				ret <<= 4;
				ret  |= nibble;
			}

			return ret;
		}

	private:
		std::vector<uint8_t> numbers;
		uint32_t length;
};


class OperatorPacket : public Packet {
	public:
		OperatorPacket(const Packet &base) : Packet(base) {
			this->length = 0;
		}

		virtual uint32_t parse(const std::string &bitstream, int offset) {
			this->length = offset;

			offset += Packet::HEADER_LENGTH;

			{
				uint8_t lengthTypeId = getU8(bitstream, offset, 1);

				offset++;

				if (lengthTypeId == 1) {
					int pktNumber = getU16(bitstream, offset, 11);

					offset += 11;

					DBG(("OperatorPacket::parse(): subpackets count: %d", pktNumber));

					while (pktNumber-- > 0) {
						Packet *pkt = parsePacket(bitstream, offset);

						this->subPackets.push_back(pkt);

						offset += pkt->getLength();
					}

				} else {
					int pktOffset = 0;
					int pktLength = getU16(bitstream, offset, 15); offset += 15;

					DBG(("OperatorPacket::parse(): subpackets length: %d", pktLength));

					do {
						Packet *pkt = parsePacket(bitstream, offset + pktOffset);

						this->subPackets.push_back(pkt);

						pktOffset += pkt->getLength();
					} while (pktOffset < pktLength);

					offset += pktLength;
				}
			}

			this->length = offset - this->length;

			DBG(("OperatorPacket::parse() => %u", this->length));

			return getLength();
		}

		virtual uint32_t getLength() const {
			return this->length;
		}

		virtual uint32_t getVersionSum() const {
			uint32_t ret = this->getVersion();

			for (auto pkt : this->subPackets) {
				ret += pkt->getVersionSum();
			}

			return ret;
		}

		const std::vector<Packet *> &getSubpackets() const {
			return this->subPackets;
		}

	private:
		std::vector<Packet *> subPackets;
		uint32_t length;
};


class SumPacket : public OperatorPacket {
	public:
		static const int TYPE = 0;

	public:
		SumPacket(const Packet &base) : OperatorPacket(base) {
		}

		virtual uint64_t getValue() const {
			uint64_t ret = 0;

			DBG(("SumPacket::%s(): CALL", __func__));

			for (auto pkt : this->getSubpackets()) {
				ret += pkt->getValue();
			}

			return ret;
		}
};


class ProductPacket : public OperatorPacket {
	public:
		static const int TYPE = 1;

	public:
		ProductPacket(const Packet &base) : OperatorPacket(base) {
		}

		virtual uint64_t getValue() const {
			uint64_t ret = 1;

			DBG(("ProductPacket::%s(): CALL", __func__));

			for (auto pkt : this->getSubpackets()) {
				ret *= pkt->getValue();
			}

			return ret;
		}
};


class MinimumPacket : public OperatorPacket {
	public:
		static const int TYPE = 2;

	public:
		MinimumPacket(const Packet &base) : OperatorPacket(base) {
		}

		virtual uint64_t getValue() const {
			uint64_t ret = UINT64_MAX;

			DBG(("MinimumPacket::%s(): CALL", __func__));

			for (auto pkt : this->getSubpackets()) {
				uint64_t pktValue = pkt->getValue();

				if (ret > pktValue) {
					ret = pktValue;
				}
			}

			return ret;
		}
};


class MaximumPacket : public OperatorPacket {
	public:
		static const int TYPE = 3;

	public:
		MaximumPacket(const Packet &base) : OperatorPacket(base) {
		}

		virtual uint64_t getValue() const {
			uint64_t ret = 0;

			DBG(("MaximumPacket::%s(): CALL", __func__));

			for (auto pkt : this->getSubpackets()) {
				uint64_t pktValue = pkt->getValue();

				if (ret < pktValue) {
					ret = pktValue;
				}
			}

			return ret;
		}
};


class GreaterThanPacket : public OperatorPacket {
	public:
		static const int TYPE = 5;

	public:
		GreaterThanPacket(const Packet &base) : OperatorPacket(base) {
		}

		virtual uint64_t getValue() const {
			uint64_t ret = this->getSubpackets()[0]->getValue() > this->getSubpackets()[1]->getValue();

			DBG(("GreaterThanPacket::%s(): CALL", __func__));

			return ret;
		}
};


class LessThanPacket : public OperatorPacket {
	public:
		static const int TYPE = 6;

	public:
		LessThanPacket(const Packet &base) : OperatorPacket(base) {
		}

		virtual uint64_t getValue() const {
			uint64_t ret = this->getSubpackets()[0]->getValue() < this->getSubpackets()[1]->getValue();

			DBG(("LessThanPacket::%s(): CALL", __func__));

			return ret;
		}
};


class EqualPacket : public OperatorPacket {
	public:
		static const int TYPE = 7;

	public:
		EqualPacket(const Packet &base) : OperatorPacket(base) {
		}

		virtual uint64_t getValue() const {
			uint64_t ret = this->getSubpackets()[0]->getValue() == this->getSubpackets()[1]->getValue();

			DBG(("EqualPacket::%s(): CALL", __func__));

			return ret;
		}
};


static Packet *parsePacket(const std::string &bitstream, int offset) {
	Packet *ret = nullptr;

	{
		Packet base;

		base.parse(bitstream, offset);

		switch (base.getType()) {
			case LiteralValuePacket::TYPE:
				ret = new LiteralValuePacket(base);
				break;

			case SumPacket::TYPE:
				ret = new SumPacket(base);
				break;

			case ProductPacket::TYPE:
				ret = new ProductPacket(base);
				break;

			case MinimumPacket::TYPE:
				ret = new MinimumPacket(base);
				break;

			case MaximumPacket::TYPE:
				ret = new MaximumPacket(base);
				break;

			case GreaterThanPacket::TYPE:
				ret = new GreaterThanPacket(base);
				break;

			case LessThanPacket::TYPE:
				ret = new LessThanPacket(base);
				break;

			case EqualPacket::TYPE:
				ret = new EqualPacket(base);
				break;

			default:
				ret = new OperatorPacket(base);
				break;
		}

		ret->parse(bitstream, offset);
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::string bitstream;

		for (auto c : lines[0]) {
			switch (::tolower(c)) {
				case '0': bitstream.append("0000"); break;
				case '1': bitstream.append("0001"); break;
				case '2': bitstream.append("0010"); break;
				case '3': bitstream.append("0011"); break;
				case '4': bitstream.append("0100"); break;
				case '5': bitstream.append("0101"); break;
				case '6': bitstream.append("0110"); break;
				case '7': bitstream.append("0111"); break;
				case '8': bitstream.append("1000"); break;
				case '9': bitstream.append("1001"); break;
				case 'a': bitstream.append("1010"); break;
				case 'b': bitstream.append("1011"); break;
				case 'c': bitstream.append("1100"); break;
				case 'd': bitstream.append("1101"); break;
				case 'e': bitstream.append("1110"); break;
				case 'f': bitstream.append("1111"); break;
			}
		}

		{
			std::vector<Packet *> packets;

			{
				int offset = 0;

				do {
					Packet *pkt = parsePacket(bitstream, offset);

					packets.push_back(pkt);

					offset += pkt->getLength();
				} while (offset + Packet::HEADER_LENGTH + 5 < bitstream.size());
			}

			{
				uint32_t partA = 0;

				for (auto pkt : packets) {
					partA += pkt->getVersionSum();
				}

				PRINTF(("PART_A: %u", partA));
			}

			PRINTF(("PART_B: %lu", packets[0]->getValue()));
		}
	}

	return 0;
}
