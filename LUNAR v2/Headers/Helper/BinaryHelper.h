#pragma once
#include "../Math/Types.h"
#include <string>
namespace osomi{
	class BinaryHelper{
	public:
		static u8 uchar(char c);
		static u32 getData(char *dat, u32 s, u32 e);
		static f32 *getFloats(char *dat, u32 start, u32 length);
		
		static void writeData(char *dat, u32 offset, u32 i);
		static void writeData(char *dat, u32 offset, u64 i);
		static void writeData(char *dat, u32 offset, f32 *farr, u32 len);
		static void writeData(char *dat, u32 offset, u8 *barr, u32 len);
		
		static std::string toHex(char *dat, u32 len);
		static std::string toOctal(char *dat, u32 len);
		static std::string toBinary(char *dat, u32 len);
		static std::string toDec(char *dat, u32 len);
	};
}