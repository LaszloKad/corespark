#include "../include/tjprotocol.h"
#include <memory>

using namespace tj::np;
using namespace tj::shared;

/* Packet */
Packet::Packet(const PacketHeader& ph, const char* message, unsigned int size) {
	_size = size;
	_header = reinterpret_cast<PacketHeader*>(new char[size+sizeof(PacketHeader)]);
	if(size > 0 && message != 0) {
		_message = reinterpret_cast<char*>(_header + sizeof(PacketHeader));
		memcpy(_message, message, size);
	}
	*_header = ph;
}

Packet::Packet(char* data, unsigned int size) {
	_size = size;
	_header = reinterpret_cast<PacketHeader*>(data);
	_message = data + sizeof(PacketHeader);
}

Packet::~Packet() {
	delete[] _header;
	_size = 0;
}

unsigned int Packet::GetSize() {
	return _size;
}

PacketHeader Packet::GetHeader() {
	return *_header;
}

const char* Packet::GetMessage() {
	return _message;
}

PacketHeader::PacketHeader() {
	_action = ActionNothing;
	_channel = 0;
	_group = 0;
	_size = 0;
	_rpid = 0;
	_version[0] = 'T';
	_version[1] = '4';
	_flags = 0;
	_transaction = 0;
	_from = 0;
	_plugin = 0;
}