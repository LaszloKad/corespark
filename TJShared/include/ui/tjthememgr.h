#ifndef _TJTHEMEMGR_H
#define _TJTHEMEMGR_H

namespace tj {
	namespace shared {
		class EXPORTED ThemeManager: public virtual Object {
			public:
				static strong<Theme> GetTheme();
				static strong<SettingsStorage> GetLayoutSettings();
				static void AddTheme(strong<Theme> thm);
				static void RemoveTheme(ref<Theme> thm);
				static void SelectTheme(strong<Theme> thm);
				static void SelectTheme(int n);
				static void ListThemes(std::vector< ref<Theme> >& lst);
				static int GetThemeId();

				struct ThemeChangeNotification {
					public:
						ThemeChangeNotification(ref<Theme> newTheme);
						ref<Theme> GetNewTheme();

					private:
						ref<Theme> _newTheme;
				};
				static Listenable<ThemeChangeNotification> EventThemeChanged;

				// friendly time
				static bool IsFriendlyTime();
				static void SetFriendlyTime(bool t);

			protected:
				static std::vector< strong<Theme> > _themes;
				static ref<Theme> _theme;
				static ref<SettingsStorage> _layoutSettings;
				static bool _friendlyTime;
		};

		class EXPORTED ThemeSubscription {
			public:
				ThemeSubscription(ref<Theme> thm);
				virtual ~ThemeSubscription();

			protected:
				ref<Theme> _theme;
		};
	}
}

#endif