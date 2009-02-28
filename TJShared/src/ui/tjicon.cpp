#include "../../include/ui/tjui.h" 
using namespace tj::shared;
using namespace tj::shared::graphics;

Icon::Icon(const ResourceIdentifier& rid): _bitmap(0) {
	if(rid!=L"") {
		std::wstring path;
		if(ResourceManager::Instance()->GetPathToLocalResource(rid, path)) {
			_bitmap = Image::FromFile(path.c_str(), TRUE);
		}

		if(_bitmap==0) {
			Log::Write(L"TJShared/Icon", L"Could not load Icon: " + rid);
		}
	}
}

Icon::Icon(graphics::Image* bmp) {
	_bitmap = bmp;
}

Icon::~Icon() {
	delete _bitmap;
}

bool Icon::IsLoaded() const {
	return _bitmap!=0;
}

graphics::Image* Icon::GetBitmap() {
	return _bitmap;
}

Icon::operator graphics::Image*() {
	return _bitmap;
}

// alpha
void Icon::Paint(graphics::Graphics& g, const Area& rc, float alpha) {
	if(_bitmap==0) return;

	// Set up color matrix
	ColorMatrix cm = {
		1.0f,   0.0f,   0.0f, 0.0f, 0.0f,
		0.0f,   1.0f,   0.0f, 0.0f, 0.0f,
		0.0f,   0.0f,   1.0f, 0.0f, 0.0f,
		0.0f,	0.0f,	0.0f, alpha, 0.0f,
		0.0f,	0.0f,	0.0f, 0.0f, 1.0f,
	};

	ImageAttributes attr;
	attr.SetColorMatrix(&cm);

	g.DrawImage(_bitmap, rc, &attr);
}

// disabled + alpha
void Icon::Paint(graphics::Graphics& g, const Area& rc, bool enabled, float alpha) {
	if(_bitmap==0) return;

	if(enabled) {
		Paint(g, rc, alpha);
	}
	else {
		graphics::ColorMatrix cm = {
			0.299f, 0.299f, 0.299f, 0.0f, 0.0f,
			0.587f, 0.587f, 0.587f, 0.0f, 0.0f,
			0.114f, 0.114f, 0.114f, 0.0f, 0.0f,
			0.0f,	0.0f,	0.0f,	alpha, 0.0f,
			0.0f,	0.0f,	0.0f,	0.0f, 1.0f,
		};

		ImageAttributes attr;
		attr.SetColorMatrix(&cm);

		g.DrawImage(_bitmap, rc, &attr);
	}
}

// Disabled or normal
void Icon::Paint(graphics::Graphics& g, const Area& rc, bool enabled) {
	if(_bitmap==0) return;

	if(enabled) {
		g.DrawImage(_bitmap, rc);
	}
	else {
		graphics::ColorMatrix cm = {
			0.299f, 0.299f, 0.299f, 0.0f, 0.0f,
			0.587f, 0.587f, 0.587f, 0.0f, 0.0f,
			0.114f, 0.114f, 0.114f, 0.0f, 0.0f,
			0.0f,	0.0f,	0.0f,	1.0f, 0.0f,
			0.0f,	0.0f,	0.0f,	0.0f, 1.0f,
		};

		ImageAttributes attr;
		attr.SetColorMatrix(&cm);

		g.DrawImage(_bitmap, rc, &attr);
	}
}