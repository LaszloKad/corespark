#ifndef _TJDMXENTTECPRODEVICE_H
#define _TJDMXENTTECPRODEVICE_H

#include "../tjdmxinternal.h"
#include "../tjdmxdevice.h"
#include "enttecdmx.h"

#ifdef TJ_OS_WIN

namespace tj {
	namespace dmx {
		namespace devices {
			using namespace tj::shared;
			using namespace tj::dmx;

			class DMX_EXPORTED DMXEnttecProDevice: public DMXDevice {
				friend class DMXController;

				public:
					DMXEnttecProDevice();
					virtual ~DMXEnttecProDevice();
					virtual std::wstring GetPort();
					virtual std::wstring GetDeviceID();
					virtual std::wstring GetDeviceInfo();
					virtual std::wstring GetDeviceSerial();
					virtual std::wstring GetDeviceName() const;
					virtual unsigned int GetSupportedUniversesCount();

					#ifdef TJ_DMX_HAS_TJSHAREDUI
						virtual ref<PropertySet> GetProperties();
					#endif

					virtual void Load(TiXmlElement* you);
					virtual void Save(TiXmlElement* you);
					
				protected:
					virtual void Connect();
					virtual void OnTransmit(ref<DMXController> controller);
					virtual std::wstring AskDeviceSerial();
					virtual std::wstring AskDeviceInfo();

					std::wstring FindDevice();
					void SetCommParameters();
					int SendData(int label, unsigned char const* data, int length);
					int ReceiveData(int label, unsigned char *data, unsigned int expected_length);

					HANDLE _file;
					enttec::DMXUSBPROParamsType _parameters;
					std::wstring _port;
					std::wstring _serial;
					std::wstring _info;
					int _universe;
					unsigned char* _transmitBuffer;
			};

			class DMXEnttecProDeviceClass: public DMXDeviceClass {
				public:
					DMXEnttecProDeviceClass();
					virtual ~DMXEnttecProDeviceClass();
					virtual ref< std::vector< ref<DMXDevice> > > GetAvailableDevices();

				protected:
					ref<DMXEnttecProDevice> _device;
			};
		}
	}
}

#endif

#endif