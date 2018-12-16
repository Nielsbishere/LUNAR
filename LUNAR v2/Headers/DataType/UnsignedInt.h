#pragma once
#include "../Math/MathDefines.h"
#include "../Helper/BinaryHelper.h"
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
namespace osomi{
	template<unsigned int len, unsigned int bytes> class UnsignedInt {
	private:
		char dat[bytes];
		u32 length;
	public:
		//Constructors and important functions
		UnsignedInt(){
			if (bytes != math::max((unsigned int)math::ceil(len / 8.f), 1U)){
				throw std::invalid_argument("Byte count != actual length in bytes!\n");
			}
			length = math::max((unsigned int)math::ceil(len / 8.f), 1U);
			memset(dat, 0, length);
		}
		UnsignedInt(u64 someNum){	
			if (bytes != math::max((unsigned int)math::ceil(len / 8.f), 1U)) {
				throw std::invalid_argument("Byte count != actual length in bytes!\n");
			}
			length = math::max((unsigned int)math::ceil(len / 8.f), 1U);
			memset(dat, 0, length);
			char *dat = new char[8];
			BinaryHelper::writeData(dat, 0, someNum);
			std::string bin = BinaryHelper::toBinary(dat, 8).substr(2);
			delete[] dat;
			if (bin.size() > len){
				for (u32 i = 0; i < bytes; i++)
					this->dat[i] = 0;
			} else{
				for (u32 i = 0; i < len; i++) 
					(*this).set(i, i >= bin.size() ? false : bin[bin.size() - 1 - i] == '1');
			}
		}
		UnsignedInt(std::string str){
			if (bytes != math::max((unsigned int)math::ceil(len / 8.f), 1U)) {
				throw std::invalid_argument("Byte count != actual length in bytes!\n");
			}
			length = math::max((unsigned int)math::ceil(len / 8.f), 1U);
			memset(dat, 0, length);
			//TODO: Convert string to osomi::UnsignedInt
		}
		//Args; 0 = decimal, 1 = hex, 2 = octal, 3 = binary
		std::string toString(u8 args = 0) {
			switch (args){
			case 1:
				return BinaryHelper::toHex(dat, bytes);
			case 2:
				return BinaryHelper::toOctal(dat, bytes);
			case 3:
				return BinaryHelper::toBinary(dat, bytes);
			default:
				return BinaryHelper::toDec(dat, bytes);
			}
		}
		bool operator[](unsigned int index){
			if (index >= len)return false;
			return (dat[index / 8] & (0b1 << (index % 8))) != 0;
		}
		void set(unsigned int index, bool b){
			if (index >= len)return;
			if (b)dat[index / 8] |= (0b1 << (index % 8));
			else dat[index / 8] &= 0xFF - (0b1 << (index % 8));
		}
		
		//Equals
		void operator=(UnsignedInt<len, bytes> dt){
			for (u32 i = 0; i < len; i++)
				set(i, dt[i]);
		}
		
		//Bitwise operators
		void operator|=(UnsignedInt<len, bytes> dt){
			for (u32 i = 0; i < len; i++)
				set(i, (*this)[i] || dt[i]);
		}
		void operator&=(UnsignedInt<len, bytes> dt) {
			for (u32 i = 0; i < len; i++) {
				if (dt[i] && (*this)[i])set(i, true);
				else set(i, false);
			}
		}
		void operator^=(UnsignedInt<len, bytes> dt) {
			for (u32 i = 0; i < len; i++) {
				if (dt[i] != (*this)[i] && !(dt[i] && (*this)[i]))set(i, true);
				else set(i, false);
			}
		}
		UnsignedInt<len, bytes> operator~() {
			UnsignedInt<len, bytes> cpy = *this;
			for (u32 i = 0; i < len; i++)
				cpy.set(i, !cpy[i]);
			return cpy;
		}
		UnsignedInt<len, bytes> operator|(UnsignedInt<len, bytes> dt) {
			UnsignedInt<len, bytes> cpy = *this;
			cpy |= dt;
			return cpy;
		}
		UnsignedInt<len, bytes> operator&(UnsignedInt<len, bytes> dt) {
			UnsignedInt<len, bytes> cpy = *this;
			cpy &= dt;
			return cpy;
		}
		UnsignedInt<len, bytes> operator^(UnsignedInt<len, bytes> dt) {
			UnsignedInt<len, bytes> cpy = *this;
			cpy ^= dt;
			return cpy;
		}
		UnsignedInt<len, bytes> operator<<(u32 in){
			UnsignedInt<len, bytes> cpy = UnsignedInt<len, bytes>(0);
			for (u32 i = 0; i < len; i++)
				cpy.set(i + in, (*this)[i]);
			return cpy;
		}
		UnsignedInt<len, bytes> operator>>(u32 in) {
			UnsignedInt<len, bytes> cpy = UnsignedInt<len, bytes>(0);
			for (u32 i = 0; i < len; i++)
				cpy.set(i - in, (*this)[i]);
			return cpy;
		}
		void operator>>=(u32 in){
			*this = (*this) >> in;
		}
		void operator<<=(u32 in) {
			*this = (*this) << in;
		}
		//Logical operators
		bool operator==(UnsignedInt<len, bytes> dt) {
			for (u32 i = 0; i < len; i++)
				if (dt[i] != (*this)[i])
					return false;
			return true;
		}
		bool operator!=(UnsignedInt<len, bytes> dt) {
			return !(dt == *this);
		}
		bool operator>(UnsignedInt<len, bytes> dt) {
			for (u32 i = len - 1; i != 0xFFFFFFFF; i--){
				if ((*this)[i] && !dt[i])return true;
				if (!(*this)[i] && dt[i])return false;
			}
			return false;
		}
		bool operator>=(UnsignedInt<len, bytes> dt) {
			for (u32 i = len - 1; i != 0xFFFFFFFF; i--) {
				if ((*this)[i] && !dt[i])return true;
				if (!(*this)[i] && dt[i])return false;
			}
			return true;
		}
		bool operator<(UnsignedInt<len, bytes> dt) {
			return !(*this >= dt);
		}
		bool operator<=(UnsignedInt<len, bytes> dt) {
			return !(*this > dt);
		}
		//Boolean operators
		bool isZero(){
			for (u32 i = 0; i < length; i++)
				if (dat[i] != 0)return false;
			return true;
		}
		bool operator!() {
			return isZero();
		}
		bool operator||(UnsignedInt<len, bytes> dt) {
			return !isZero() || !dt.isZero();
		}
		bool operator&&(UnsignedInt<len, bytes> dt) {
			return !isZero() && !dt.isZero();
		}
		//Arithmetic operators
		//+
		void operator+=(UnsignedInt<len, bytes> dt) {
			for (u32 i = len - 1; i != 0xFFFFFFFF; i--)
				if ((*this)[i] && dt[i] && i == len - 1) {
					(*this) = UnsignedInt<len, bytes>::max();
					break;
				}
				else{
					//TODO: Calculate shit
				}
		}
		UnsignedInt<len, bytes> operator+(UnsignedInt<len, bytes> dt) {
			UnsignedInt<len, bytes> cpy = *this;
			cpy += dt;
			return cpy;
		}
		void operator++(){
			*this = *this + UnsignedInt<len, bytes>(1);
		}
		//-
		void operator-=(UnsignedInt<len, bytes> dt) {
			if (*this < dt){
				*this = min();
				return;
			}
			for (u32 i = len - 1; i != 0xFFFFFFFF; i--)
				;
				//TODO: Calculate shit
		}
		UnsignedInt<len, bytes> operator-(UnsignedInt<len, bytes> dt) {
			UnsignedInt<len, bytes> cpy = *this;
			cpy -= dt;
			return cpy;
		}
		void operator--() {
			*this = *this - UnsignedInt<len, bytes>(1);
		}
		//*
		void operator*=(UnsignedInt<len, bytes> dt) {
			UnsignedInt<len, bytes> cpy = *this;
			for (u32 i = len - 1; i != 0xFFFFFFFF; i--) {
				if ((*this)[i]) {
					cpy += dt << i;
				}
			}
			*this = cpy;
		}
		UnsignedInt<len, bytes> operator*(UnsignedInt<len, bytes> dt) {
			UnsignedInt<len, bytes> cpy = *this;
			cpy *= dt;
			return cpy;
		}
		// /
		
		// %
		
		//Limits
		static UnsignedInt<len, bytes> max() {
			UnsignedInt<len, bytes> dt;
			for (u32 i = 0; i < len; i++)
				dt.set(i, true);
			return dt;
		}
		static UnsignedInt<len, bytes> min() {
			return ~max();
		}
	};
}

template class osomi::UnsignedInt<4, 1>;
template class osomi::UnsignedInt<8, 1>;
template class osomi::UnsignedInt<16, 2>;
template class osomi::UnsignedInt<24, 3>;
template class osomi::UnsignedInt<32, 4>;
template class osomi::UnsignedInt<64, 8>;
template class osomi::UnsignedInt<128, 16>;
template class osomi::UnsignedInt<256, 32>;
template class osomi::UnsignedInt<512, 64>;
template class osomi::UnsignedInt<1024, 128>;
template class osomi::UnsignedInt<2048, 256>;
template class osomi::UnsignedInt<4096, 512>;
template class osomi::UnsignedInt<8192, 1024>;

typedef osomi::UnsignedInt<4, 1> NIBBLE;
typedef osomi::UnsignedInt<8, 1> OIU1;
typedef osomi::UnsignedInt<16, 2> OIU2;
typedef osomi::UnsignedInt<24, 3> RGB;
typedef osomi::UnsignedInt<32, 4> OIU4;
typedef osomi::UnsignedInt<64, 8> OIU8;
typedef osomi::UnsignedInt<128, 16> OIU16;
typedef osomi::UnsignedInt<256, 32> OIU32;
typedef osomi::UnsignedInt<512, 64> OIU64;
typedef osomi::UnsignedInt<1024, 128> OIU128;
typedef osomi::UnsignedInt<2048, 256> OIU256;
typedef osomi::UnsignedInt<4096, 512> OIU512;
typedef osomi::UnsignedInt<8192, 1024> OIU1024;