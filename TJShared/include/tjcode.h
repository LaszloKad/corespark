#ifndef _TJCODE_H
#define _TJCODE_H

#pragma pack(push,1)

namespace tj {
	namespace shared {
		class EXPORTED Code: public virtual Object {
			public:
				Code(const char* code, unsigned int size);
				virtual ~Code();
				unsigned int GetSize();

				template<typename T> T Get(unsigned int& position) {
					unsigned int size = sizeof(T)/sizeof(char);
					if((position+(size-1))>_size) Throw(L"Array index out of bounds in code reader", ExceptionTypeError);

					T* tp = (T*)&(_code[position]);
					position += size;
					return *tp;
				}

			protected:
				char* _code;
				unsigned int _size;
		};

		template<> EXPORTED std::wstring Code::Get(unsigned int& position);
		template<> EXPORTED tj::shared::Vector Code::Get(unsigned int& position);

		class EXPORTED CodeWriter: public virtual Object {
			public:
				CodeWriter(unsigned int initialSize=1024);
				virtual ~CodeWriter();
				unsigned int GetCapacity();
				unsigned int GetSize();

				template<typename T> CodeWriter& Add(const T& x) {
					if(_buffer==0) {
						Throw(L"CodeWriter written to after buffer has been taken over!", ExceptionTypeSevere);
					}

					unsigned int size = sizeof(T)/sizeof(char);
					Grow(size);

					T* tp = (T*)&(_buffer[_pos]);
					*tp = x;
					_pos += size;
					return *this;
				}

				inline const char* GetBuffer() {
					return _buffer;
				}

				inline char* TakeOverBuffer() {
					char* buf = _buffer;
					_buffer = 0;
					_size = 0;
					_pos = 0;
					return buf;
				}
				
			protected:
				void Grow(unsigned int size);

				unsigned int _size;
				unsigned int _pos;

			public:
				char* _buffer;
		};

		template<> EXPORTED CodeWriter& CodeWriter::Add(const std::wstring& x);
	}
}

#pragma pack(pop)
#endif