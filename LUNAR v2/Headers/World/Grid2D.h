#pragma once

template<class T> class Grid2D {
private:
	int w, h;
	T *dat;
	Grid2D(int _w, int _h, T *_dat);
public:
	static Grid2D<T> *create(int w, int h);
	
	~Grid2D() { if (dat != nullptr) delete[] dat; }
	int getWidth() { return w; }
	int getHeight() { return h; }
	T &at(unsigned int i, unsigned int j) {
		if (i >= w || j >= h)return dat[0];
		return dat[j * w + i];
	}
	Grid2D<T> *copy(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY);
	void print();
};
template class Grid2D<float>;
template class Grid2D<bool>;
template class Grid2D<int>;

typedef Grid2D<float> Grid2Df;
typedef Grid2D<bool> Grid2Db;
typedef Grid2D<int> Grid2Di;