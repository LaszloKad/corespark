#ifndef _TJMIXED_H
#define _TJMIXED_H

namespace tj {
	namespace shared {
		/** Mixed is a value type that is meant for easy handling of values that can be
		'mixed' from different sources. For example, an audio volume in TJMedia is mixed
		by a track-volume and a manual ('live') volume.
		
		Important: users of this class should provide their own locking if this mixer is
		updated from multiple threads!
		**/
		template<typename T> class Mixed {
			public:
				inline Mixed() {
				}

				~Mixed() {
				}

				inline bool HasMixValues() const {
					return _mixValues.size() > 0;
				}

				inline void SetMixValue(const String& ident, T value) {
					_mixValues[ident] = value;
				}

				inline T GetMixValue(const String& ident, T defaultValue) const {
					typename std::map< const String, T>::const_iterator it = _mixValues.find(ident);
					if(it==_mixValues.end()) {
						return defaultValue;
					}

					return it->second;
				}

				/** This returns the value for the specified source value when using 'multiply mixing',
				in which all mixing values are simply multiplied **/
				inline T GetMultiplyMixValue(T source) const {
					typename std::map< const String, T>::const_iterator it = _mixValues.begin();
					while(it!=_mixValues.end()) {
						source *= it->second;
						++it;
					}

					return source;
				}

				inline void RemoveMixValue(const String& ident) {
					typename std::map<const String, T>::iterator it = _mixValues.find(ident);
					if(it!=_mixValues.end()) {
						_mixValues.erase(it);
					}
				}

			protected:
				std::map< const String, T> _mixValues;
		};
	}
}

#endif