#include "../../Headers/Helper/BinaryHelper.h"
#include "../../Headers/Math/MathDefines.h"

using namespace osomi;

u32 BinaryHelper::getData(char *dat, u32 start, u32 end){
	if (dat == nullptr)return 0;
	u32 res = 0;
	u32 j = 0;
	for (u32 i = end-1; i >= start && i != 0xFFFFFFFF; i--){
		u8 k = uchar(dat[i]);
		res |= k << (j * 8);
		j++;
	}
	return res;
}
u8 BinaryHelper::uchar(char c){
	return *(u8*)(&c);
}
f32 *BinaryHelper::getFloats(char *dat, u32 start, u32 length){
	f32 *fdat = new f32[length];
	for (u32 i = 0; i < length; i++) {
		u32 pure = getData(dat, start + 4 * i, start + 4 + 4 * i);
		fdat[i] = *(f32*)&pure;
	}
	return fdat;
}
void BinaryHelper::writeData(char *dat, u32 offset, u32 i) {
	for (u32 j = 0; j < 4; j++) {
		u32 k = (3 - j) * 8;
		u8 b = (i & (0xFF << k)) >> k;
		dat[offset + j] = *(char*)&b;
	}
}
void BinaryHelper::writeData(char *dat, u32 offset, u64 i) {
	for (u32 j = 0; j < 8; j++) {
		u32 k = (7 - j) * 8;
		u8 b = (i & (0xFF << k)) >> k;
		dat[offset + j] = *(char*)&b;
	}
}
void BinaryHelper::writeData(char *dat, u32 offset, f32 *farr, u32 len) {
	for (u32 i = 0; i < len; i++)
		writeData(dat, offset + 4 * i, *(u32*)&farr[i]);
}
void BinaryHelper::writeData(char *dat, u32 offset, u8 *barr, u32 len) {
	for (u32 i = 0; i < len; i++)
		dat[offset + i] = *(char*)&barr[i];
}

std::string nibble(u8 nibble){
	std::string s = "F";
	if (nibble < 10)s[0] = '0' + nibble;
	else if (nibble < 16)s[0] = 'A' + nibble - 10;
	return s;
}

std::string BinaryHelper::toHex(char *dat, u32 len) {
	std::string h = "";
	bool started = false;
	for (u32 i = 0; i < len; i++){
		u8 nib0 = (dat[i] & 0xF0) >> 4;
		u8 nib1 = dat[i] & 0xF;
		if (nib0 == 0 && nib1 == 0)continue;
		if (!started){
			started = true;
			if (nib0 == 0 && nib1 != 0) {
				h = nibble(nib1); 
				continue;
			}
		}
		h = nibble(nib0) + nibble(nib1) + h;
	}
	if (h == "")return "0x0";
	return "0x" + h;
}
std::string toOct(std::string what){
	if (what == "111")return "7";
	if (what == "110")return "6";
	if (what == "101")return "5";
	if (what == "100")return "4";
	if (what == "011")return "3";
	if (what == "010")return "2";
	if (what == "001")return "1";
	return "0";
}
std::string BinaryHelper::toOctal(char *dat, u32 len) {
	std::string bin = toBinary(dat, len).substr(2);
	std::string oct = "";
	u32 chars = math::ceil(bin.size() / 3);
	for (u32 i = 0; i < chars; i++){
		std::string sub = std::string(bin).substr(math::max<int>(bin.size() - i * 3, 0), math::max<int>(bin.size() - i * 3 + 3, 0));
		oct = toOct(sub) + oct;
	}
	if (oct == "")return "00";
	return "0" + oct;
}
std::string BinaryHelper::toBinary(char *dat, u32 len) {
	std::string bin = "";
	bool started = false;
	for (u32 i = 0; i < len; i++)
		for (u32 j = 7; j != 0xFFFFFFFF; j--) {
			std::string num = dat[i] & (0b1 << j) ? "1" : "0";
			if (num == "0" && !started)continue;
			bin = bin + num;
			started = true;
		}
	if (bin == "")return "0b0";
	return "0b" + bin;
}
std::string BinaryHelper::toDec(char *dat, u32 len) {
	//TODO:
	return "UNDEF";
}