#pragma once
#include "../../Helper/StringUtils.h"
#include "../../Math/MathDefines.h"
namespace osomi{
	namespace ss{
		enum ELevel {
			//1-100
			L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11, L12, L13, L14, L15, L16, L17, L18, L19, L20, L21, L22, L23, L24, L25, L26, L27, L28, L29, L30, L31, L32, L33,
			L34, L35, L36, L37, L38, L39, L40, L41, L42, L43, L44, L45, L46, L47, L48, L49, L50, L51, L52, L53, L54, L55, L56, L57, L58, L59, L60, L61, L62, L63, L64, L65, L66,
			L67, L68, L69, L70, L71, L72, L73, L74, L75, L76, L77, L78, L79, L80, L81, L82, L83, L84, L85, L86, L87, L88, L89, L90, L91, L92, L93, L94, L95, L96, L97, L98, L99, L100,
			
			//100A-100Z
			L100A, L100B, L100C, L100D, L100E, L100F, L100G, L100H, L100I, L100J, L100K, L100L, L100M, L100N, L100O, L100P, L100Q, L100R, L100S, L100T, L100U, L100V, L100W, L100X, L100Y, L100Z,
			
			//100ZI-100ZXIVV
			L100ZI, L100ZII, L100ZIII, L100ZIV, L100ZV, L100ZVI, L100ZVII, L100ZVIII, L100ZIX, L100ZX, L100ZXI, L100ZXII, L100ZXIII, L100ZXIV,
			
			//Master level
			L100ZM, L100ZM1, L100ZM2, L100ZM4, L100ZM8, LMAX,
			
			//Entity level only (Player can reach L1->L100ZM using exp and L100ZM->LMAX through defeating bosses and powerful spells). These levels are beyond player's reach and represent bosses that even LMAX can rarely defeat.
			LM1, LM2, LM3, LM4, LM5, LM6, LM7, LM8, LM9, LM10, LMP, LMPP, LMPPP, LMPPPP, LMM
				
			//LMM = +Legendary; these can only be beaten by players with Legendary weapons and +MAX.
		};
		class Level{
		public:
			static std::string getRomanString(u8 b){
				if (b == 0)return "";
				if (b <= 3){
					std::string res = "";
					for (u32 i = 0; i < b; i++)
						res = res + "I";
					return res;
				}
				if (b == 4)return "IV";
				if (b < 10){
					if (b == 9)return "IX";
					return "V" + getRomanString(b - 5);
				}
				if (b < 40){
					std::string pre = "";
					for (u32 i = 0; i < b / 10; i++)
						pre += "X";
					return pre + getRomanString(b % 10);
				}
				if (b < 50)
					return "XL" + getRomanString(b - 40);
				if (b < 100){
					std::string pr = b < 90 ? "L" : "XC";
					return pr + getRomanString(b - b / 10 * 10);
				}
				return "C" + getRomanString(b - 100);
			}
			
			static float getExperience(ELevel l){
				if (l > ELevel::LMAX)
					return getExperience(ELevel::LMAX) * (l - ELevel::LMAX + 1);
				return 100 * pow(2, log(lootModifier(l)) / log(7));
			}
			static float getDamage(ELevel l){
				return pow(getExperience(l), 2 / 3.f) * .5;
			}
			static float getHealth(ELevel l){
				return pow(l / 10.f + 1, .5) * getDamage(l);
			}
			static u32 lootModifier(ELevel l){
				if (l > ELevel::LMAX)return 0;
				if (l < ELevel::L10) return l * 3 + 3;
				if (l < ELevel::L20)return (l - L10) * 7 + 30;
				if (l < ELevel::L30)return (l - L20) * 15 + 100;
				if (l < ELevel::L40)return (l - L30) * 31 + 250;
				if (l < ELevel::L50)return (l - L40) * 63 + 560;
				if (l < ELevel::L60)return (l - L50) * 127 + 1190;
				if (l < ELevel::L70)return (l - L60) * 255 + 2460;
				if (l < ELevel::L80)return (l - L70) * 511 + 5010;
				if (l < ELevel::L90)return (l - L80) * 1023 + 10120;
				if (l < ELevel::L100)return (l - L90) * 2047 + 20350;
				if (l < ELevel::L100I)return (l - L100) * 4095 + 40820;
				if (l < ELevel::L100R)return (l - L100I) * 8191 + 81770;
				if (l < ELevel::L100ZI)return (l - L100R) * 16382 + 163680;
				if (l < ELevel::L100ZX)return (l - L100ZI) * 32767 + 327250;
				if (l < ELevel::L100ZX + 10)return (l - L100ZX) * 65535 + 655200;
				return 16777216;
				
			}
			static std::string getName(ELevel l){
				if (l >= L1 && l <= L100)
					return StringUtils::numstring((unsigned int)l + 1, "+ ", "");
				std::string str = "+100";
				if (l >= L100A && l <= L100Z) {
					str += "A";
					str[str.size() - 1] = (char)('A' + (l - L100A));
					return str;
				}
				if (l >= L100ZI && l <= L100ZXIV) {
					return str + "Z" + getRomanString(l - L100ZI + 1);
				}
				if (l >= L100ZM && l <= L100ZM8){
					str += "ZM";
					if (l == L100ZM)return str;
					u32 i = pow(2, l - L100ZM - 1);
					return StringUtils::numstring(i, str, "");
				}
				if (l == LMAX) return "+MAX";
				if (l >= LM1 && l <= LM10) return StringUtils::numstring(l - LM1 + 1, "+MAX-", "");
				if (l >= LMP && l <= LMPPPP){
					std::string suffix = "";
					for (u32 i = 0; i < l - LMP + 1; i++)
						suffix += "+";
					return "+MAX-10" + suffix;
				}
				return "+Legendary";
			}
		};
	}
}