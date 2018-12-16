#include "../../Headers/World/Grid3D.h"
#include <iostream>

template<class T> Grid3D<T>::Grid3D(int _w, int _h, int _l, T *_dat) : w(_w), h(_h), l(_l), dat(_dat) {}

template<class T> Grid3D<T> *Grid3D<T>::create(int w, int h, int l) {
	T *dat = new T[w * h * l];
	for (unsigned int i = 0; i < w*h*l; i++)
		dat[i] = 0;
	return new Grid3D<T>(w, h, l, dat);
}
template<class T> Grid3D<T> *Grid3D<T>::copy(unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int endX, unsigned int endY, unsigned int endZ) {
	if (startX >= endX || startY >= endY || startZ >= endZ || startX >= w || startY >= h || startZ >= l)return nullptr;
		
	unsigned int _w = endX >= w ? w - startX : endX - startX;
	unsigned int _h = endY >= h ? h - startY : endY - startY;
	unsigned int _l = endZ >= l ? l - startZ : endZ - startZ;
		
	Grid3D<T> *cpy = Grid3D<T>::create(_w, _h, _l);
	for (unsigned int i = startX; i < endX && i < w; i++)
		for (unsigned int j = startY; j < endY && j < h; j++)
			for (unsigned int k = startZ; k < endZ && k < l; k++)
				cpy->at(i - startX, j - startY, k - startZ) = at(i, j, k);
		
	return cpy;
}
template<class T> void Grid3D<T>::print() {
	for (int k = 0; k < l; k++) {
		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++)
				printf("%f\t", (float)at(i, j, k));	
			printf("\n");
		}	
		printf("\n\n");
	}
}