#ifndef _TJLANGUAGE_H
#define _TJLANGUAGE_H

#pragma warning(push)
#pragma warning(disable:4251)

class EXPORTED Language {
	public:
		static const wchar_t* Get(std::wstring id);
		static void Load(std::wstring file);

		/** Translates menus for a window to the language **/
		static void Translate(HWND wnd);
		static void Translate(HMENU menu);
		virtual ~Language();
		static void Clear();
		Language();

	protected:
		std::map<std::wstring, wchar_t*> _strings;
};

#define TL(id) (Language::Get(std::wstring(L#id)))

#pragma warning(pop)

#endif