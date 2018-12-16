#include "../../Headers/World/Grid2D.h"
#include <iostream>

template<class T> Grid2D<T>::Grid2D(int _w, int _h, T *_dat) : w(_w), h(_h), dat(_dat) {}

template<class T> Grid2D<T> *Grid2D<T>::create(int w, int h) {
	T *dat = new T[w * h];
	for (unsigned int i = 0; i < w*h; i++)
		dat[i] = 0;
	return new Grid2D<T>(w, h, dat);
}
template<class T> Grid2D<T> *Grid2D<T>::copy(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY) {
	if (startX >= endX || startY >= endY || startX >= w || startY >= h)return nullptr;
		
	unsigned int _w = endX >= w ? w - startX : endX - startX;
	unsigned int _h = endY >= h ? h - startY : endY - startY;
		
	Grid2D<T> *cpy = Grid2D<T>::create(_w, _h);
	for (unsigned int i = startX; i < endX && i < w; i++)
		for (unsigned int j = startY; j < endY && j < h; j++)
				cpy->at(i - startX, j - startY) = at(i, j);
		
	return cpy;
}
template<class T> void Grid2D<T>::print() {
		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++)
				printf("%f\t", (float)at(i, j));	
			printf("\n");
		printf("\n\n");
	}
}