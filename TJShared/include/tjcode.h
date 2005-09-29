#ifndef _TJCODE_H
#define _TJCODE_H

class EXPORTED Code {
	public:
		Code(const char* code, unsigned int size);
		virtual ~Code();
		unsigned int GetSize();

		template<typename T> T Get(unsigned int& position) {
			unsigned int size = sizeof(T)/sizeof(char);
			if(position+size>_size) Throw(L"Array index out of bounds in code reader", ExceptionTypeError);

			T* tp = (T*)&(_code[position]);
			position += size;
			return *tp;
		}

	protected:
		char* _code;
		unsigned int _size;
};

class EXPORTED CodeWriter {
	friend class Stream;

	public:
		CodeWriter(unsigned int initialSize=1024);
		virtual ~CodeWriter();
		unsigned int GetCapacity();
		unsigned int GetSize();

		template<typename T> CodeWriter& Add(T x) {
			unsigned int size = sizeof(T)/sizeof(char);
			if(_pos+size>_size) {
				// TODO: Grow buffer here..
				return *this;
			}

			T* tp = (T*)&(_buffer[_pos]);
			*tp = x;
			_pos += size;
			return *this;
		}

		inline const char* GetBuffer() {
			return _buffer;
		}
		
	protected:
		unsigned int _size;
		unsigned int _pos;
		char* _buffer;
};

template<> CodeWriter& CodeWriter::Add(std::wstring x);

#endif