#ifndef _TJICON_H
#define _TJICON_H

namespace tj {
	namespace shared {
		class EXPORTED Icon {
			public:
				Icon(const ResourceIdentifier& rid);
				virtual ~Icon();
				Icon(graphics::Image* bmp); // Icon will own and delete bmp!
				operator graphics::Image*();
				graphics::Image* GetBitmap();
				bool IsLoaded() const;
				virtual void Paint(graphics::Graphics& g, const Area& rc, bool enabled = true);
				virtual void Paint(graphics::Graphics& g, const Area& rc, float alpha);
				virtual void Paint(graphics::Graphics& g, const Area& rc, bool enabled, float alpha);

			protected:
				graphics::Image* _bitmap;
		};
	}
}

#endif