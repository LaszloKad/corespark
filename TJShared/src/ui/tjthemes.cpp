#include "../../include/tjshared.h"
using namespace tj::shared;
using namespace tj::shared::graphics;

namespace tj {
	namespace shared {
		namespace themes {
			class BrightTheme: public Theme {
				public:
					BrightTheme(float dpi=1.0f);
					virtual ~BrightTheme();
					virtual std::wstring GetName() const;
					virtual graphics::Color GetBackgroundColor() const;
					virtual graphics::Color GetTimeBackgroundColor() const;
					virtual graphics::Color GetPropertyBackgroundColor() const;
					virtual graphics::Color GetLineColor() const;
					virtual graphics::Color GetTextColor() const;
					virtual graphics::Color GetActiveTrackColor() const;
					virtual graphics::Color GetEditBackgroundColor() const;

					virtual graphics::Color GetActiveStartColor() const;
					virtual graphics::Color GetActiveEndColor() const;
					virtual graphics::Color GetFocusColor() const;

					virtual graphics::Color GetSplitterStartColor() const;
					virtual graphics::Color GetSplitterEndColor() const;
					virtual graphics::Color GetCurrentPositionColor() const;
					virtual graphics::Color GetTrackDescriptionTextColor() const;
					virtual graphics::Color GetFaderColor() const;

					virtual graphics::Color GetTimeSelectionColorEnd() const;
					virtual graphics::Color GetTimeSelectionColorStart() const;

					virtual graphics::Color GetTabButtonColorStart() const;
					virtual graphics::Color GetTabButtonColorEnd() const;

					virtual graphics::Color GetDisabledOverlayColor() const;
					virtual graphics::Brush* GetApplicationBackgroundBrush(HWND root, HWND child) const;

					virtual graphics::Color GetHighlightColorStart() const;
					virtual graphics::Color GetHighlightColorEnd() const;

					
					virtual graphics::Color GetGlassColorStart() const;
					virtual graphics::Color GetGlassColorEnd() const;

					virtual graphics::Color GetToolbarColorStart() const;
					virtual graphics::Color GetToolbarColorEnd() const;

					// ProgressWnd
					virtual graphics::Color GetProgressBackStart() const;
					virtual graphics::Color GetProgressBackEnd() const;
					virtual graphics::Color GetProgressGlassStart() const;
					virtual graphics::Color GetProgressGlassEnd() const;
					virtual graphics::Color GetProgressColor() const;
			};

			class VistaTheme: public Theme {
				public:
					VistaTheme(float dpi): Theme(dpi) {
					}

					virtual ~VistaTheme() {
					}

					virtual std::wstring GetName() const {
						if(_dpi==1.0f) {
							return TL(theme_vista);
						}
						else {
							return TL(theme_vista) + std::wstring(L"(")+Stringify(_dpi*Theme::KDefaultDPI)+L" dpi)";
						}
					}

					virtual graphics::Color GetBackgroundColor() const {
						return Color(255,255,255);
					}

					virtual graphics::Color GetTabButtonBackgroundColor() const {
						return Color(130,255,255,255);
					}

					virtual graphics::Color GetTimeBackgroundColor() const {
						return Color(241,251,254);
					}

					virtual graphics::Color GetPropertyBackgroundColor() const {
						return GetTimeBackgroundColor();
					}

					virtual graphics::Color GetLineColor() const {
						return GetActiveEndColor();
					}

					virtual graphics::Color GetTextColor() const {
						return Color(0,0,0);
					}

					virtual graphics::Color GetActiveTrackColor() const {
						return GetBackgroundColor();
					}

					virtual graphics::Color GetEditBackgroundColor() const {
						return Color(255,255,255);
					}

					virtual graphics::Color GetActiveStartColor() const {
						//return Color(242,251,254);
						return Color(200,145,175,194);
					}

					virtual graphics::Color GetActiveEndColor() const {
						return Color(130,158,175);
					}

					virtual graphics::Color GetFocusColor() const {
						return Color::DarkBlue;
					}

					virtual graphics::Color GetSplitterStartColor() const {
						return Color(255,255,255);
					}

					virtual graphics::Color GetSplitterEndColor() const {
						return Color(255,255,255);
					}

					virtual graphics::Color GetCurrentPositionColor() const {
						return Color(0,0,0);
					}

					virtual graphics::Color GetTrackDescriptionTextColor() const {
						return Color(0,0,0);
					}

					virtual graphics::Color GetFaderColor() const {
						return Color(0,0,0);
					}

					virtual graphics::Color GetTimeSelectionColorEnd() const {
						return Color(100,0,0,100);
					}

					virtual graphics::Color GetTimeSelectionColorStart() const {
						return Color(50, 0, 0, 100);
					}

					virtual graphics::Color GetTabButtonColorStart() const {
						return Color(0,0,0,0);
					}

					virtual graphics::Color GetTabButtonColorEnd() const {
						return Color(0,0,0,0);
					}

					virtual graphics::Color GetDisabledOverlayColor() const {
						return Color(200,255,255,255);
					}

					virtual graphics::Brush* GetApplicationBackgroundBrush(HWND root, HWND child) const {
						RECT rootrc, childrc;
						GetWindowRect(root, &rootrc);
						GetWindowRect(child, &childrc);

						graphics::LinearGradientBrush* lbr = new graphics::LinearGradientBrush(PointF(0.0f, -float(childrc.top-rootrc.top)), PointF(0.0f,float(rootrc.bottom-rootrc.top)), Color(242,251,254), GetActiveEndColor());
						lbr->SetWrapMode(WrapModeClamp);
						REAL factors[3] = {1.0f, 0.0f, 0.0f};
						REAL positions[3] = {0.0f, 0.25f ,1.0f};
						lbr->SetBlend(factors,positions, 3);

						return lbr;
					}

					virtual graphics::Color GetHighlightColorStart() const {
						return Color(255, 145,175,194);
					}

					virtual graphics::Color GetHighlightColorEnd() const {
						return Color(100, 145,175,194);
					}

					virtual graphics::Color GetToolbarColorStart() const {
						return Color(24,106,119);
					}

					virtual graphics::Color GetToolbarColorEnd() const {
						return Color(87,164,171);
					}

					virtual Color GetGlassColorStart() const {
						return Color(50,255,255,255);
					}

					virtual Color GetGlassColorEnd() const {
						return Color(100,255,255,255);
					}
			};
		}
	}
}

using namespace tj::shared::themes;

// Bright theme
BrightTheme::BrightTheme(float dpi): Theme(dpi) {
}

BrightTheme::~BrightTheme() {
}

Color BrightTheme::GetBackgroundColor() const {
	return Color(255,255,255);
}

Color BrightTheme::GetTimeBackgroundColor() const {
	return Color(245,245,245);
}

Color BrightTheme::GetEditBackgroundColor() const {
	return GetBackgroundColor();
}

Color BrightTheme::GetPropertyBackgroundColor() const {
	return Color(255,255,255);
}

Color BrightTheme::GetLineColor() const {
	return Color(150,150,150);
}

Color BrightTheme::GetTextColor() const {
	return Color(0,0,0);
}

Color BrightTheme::GetActiveTrackColor() const {
	return Color(240,240,240);
}

Color BrightTheme::GetActiveStartColor() const {
	//return Color(139,139,139);
	return Color(149,149,149);
}

Color BrightTheme::GetActiveEndColor() const {
	//return Color(166,166,166);
	return Color(176,176,176);
}

Color BrightTheme::GetSplitterStartColor() const {
	return Color(204,204,204);
}

Color BrightTheme::GetSplitterEndColor() const {
	return Color(255,255,255);
}

Color BrightTheme::GetCurrentPositionColor() const {
	return Color(0,0,0);
}

Color BrightTheme::GetTrackDescriptionTextColor() const {
	return Color(120,120,120);
}

Color BrightTheme::GetFaderColor() const {
	return Color(0,0,0);
}

Color BrightTheme::GetFocusColor() const {
	return Color(200, 0,0,255);
}

graphics::Color BrightTheme::GetTimeSelectionColorEnd() const {
	return Color(100,0,0,100);
}

graphics::Color BrightTheme::GetTimeSelectionColorStart() const {
	return Color(50, 0, 0, 100);
}

Color BrightTheme::GetDisabledOverlayColor() const {
	return Color(200,255,255,255);
}

std::wstring BrightTheme::GetName() const { 
	if(_dpi==1.0f) {
		return TL(theme_bright);
	}
	else {
		return TL(theme_bright) + std::wstring(L" (") + Stringify(_dpi*Theme::KDefaultDPI)+L" dpi)"; 
	}
}

Color BrightTheme::GetTabButtonColorStart() const {
	return Color(100,145,145,145);
}

Color BrightTheme::GetTabButtonColorEnd() const {
	return Color(100,185,185,185);
}

Color BrightTheme::GetHighlightColorStart() const {
	return Color(255, 255,100,0);
}

Color BrightTheme::GetHighlightColorEnd() const {
	return Color(100, 255, 100, 0);
}

Color BrightTheme::GetToolbarColorStart() const {
	return GetActiveStartColor();
}

Color BrightTheme::GetToolbarColorEnd() const {
	return GetActiveEndColor();
}

Color BrightTheme::GetGlassColorStart() const {
	return Color(50,255,255,255);
}

Color BrightTheme::GetGlassColorEnd() const {
	return Color(100,255,255,255);
}

Color BrightTheme::GetProgressBackStart() const {
	return Color(201,201,201);
}

Color BrightTheme::GetProgressBackEnd() const {
	return Color(255,255,255);
}

Color BrightTheme::GetProgressGlassStart() const {
	return Color(211,255,255,255);
}

Color BrightTheme::GetProgressGlassEnd() const {
	return Color(43,255,255,255);
}

Color BrightTheme::GetProgressColor() const {
	return Color(0,211,40);
}
	

Brush* BrightTheme::GetApplicationBackgroundBrush(HWND root, HWND child) const {
	RECT rootrc, childrc;
	GetWindowRect(root, &rootrc);
	GetWindowRect(child, &childrc);

	graphics::LinearGradientBrush* lbr = new graphics::LinearGradientBrush(PointF(0.0f, -float(childrc.top-rootrc.top)), PointF(0.0f,float(rootrc.bottom-rootrc.top)), Color(255,255,255), Color(204,204,204));
	lbr->SetWrapMode(WrapModeClamp);
	REAL factors[3] = {1.0f, 0.0f, 0.0f};
	REAL positions[3] = {0.0f, 0.25f ,1.0f};
	lbr->SetBlend(factors,positions, 3);

	return lbr;
}

namespace tj {
	namespace shared {
		namespace themes {
			// Order is important (needed when saving theme choice to settings)!
			ThemeSubscription _theme_subscription(GC::Hold(new Theme(Theme::KDefaultDPI)));
			ThemeSubscription _subscription_bright_theme(ref<Theme>(GC::Hold(new BrightTheme(Theme::KDefaultDPI))));
			ThemeSubscription _subscription_vista_theme(ref<Theme>(GC::Hold(new VistaTheme(Theme::KDefaultDPI))));

			// All weird DPI themes
			ThemeSubscription _theme_subscription_large(GC::Hold(new Theme(Theme::KDefaultDPI*1.5f)));
			ThemeSubscription _subscription_bright_theme_large(ref<Theme>(GC::Hold(new BrightTheme(Theme::KDefaultDPI*1.5f))));
			ThemeSubscription _theme_subscription_small(GC::Hold(new Theme(Theme::KDefaultDPI/1.5f)));
			ThemeSubscription _subscription_vista_theme_large(ref<Theme>(GC::Hold(new VistaTheme(Theme::KDefaultDPI*1.5f))));
		}
	}
}