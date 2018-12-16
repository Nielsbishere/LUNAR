#pragma once

template<class T> class Grid3D {
private:
	int w, h, l;
	T *dat;
	Grid3D(int _w, int _h, int _l, T *_dat);
public:
	static Grid3D<T> *create(int w, int h, int l);
	
	~Grid3D() { if (dat != nullptr) delete[] dat; }
	int getWidth() { return w; }
	int getHeight() { return h; }
	int getLength() { return l; }
	T &at(unsigned int i, unsigned int j, unsigned int k) {
		if (i >= w || j >= h || k >= l)return dat[0];
		return dat[k * h * w + j * w + i];
	}
	Grid3D<T> *copy(unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int endX, unsigned int endY, unsigned int endZ);
	void print();
};
template class Grid3D<float>;
template class Grid3D<bool>;
template class Grid3D<int>;

typedef Grid3D<float> Grid3Df;
typedef Grid3D<bool> Grid3Db;
typedef Grid3D<int> Grid3Di;