#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "../Math/Types.h"
namespace osomi {
	class StringUtils {
	public:
		//Checks whether or not a ends with b
		static bool endsWith(std::string s, std::string end);
		//Checks whether or not a starts with b
		static bool startsWith(std::string s, std::string begin);
		//Combines the array by putting 's' in between strings
		static std::string combine(std::vector<std::string> arr, std::string s);
		//Splits a string by a character
		static std::vector<std::string> split(std::string s, char c);
		//Splits a by b
		static std::vector<std::string> split(std::string text, std::string delims);
		
		//Returns locations where the string is located
		static bool find(std::vector<std::string> lines, std::string a, std::vector<u32> &locations, bool trim=false);
		//Returns locations starting at the index of string a and ending at the index of string b
		static bool find(std::vector<std::string> lines, std::string a, std::string b, std::vector<u32> &locations, bool trim=false);
		//Returns lines without referenced lines
		static std::vector<std::string> erase(std::vector<std::string> lines, std::vector<std::string> toErase, bool trim=false);
		
		//Returns prefix + i + postfix; example: 5, "Enchantment", "testing"; "Enchantment 5 testing"
		static std::string numstring(unsigned int i, std::string prefix, std::string postfix);
		//Gets the substring starting at i
		static std::string substring(std::string s, unsigned int i);
		//Gets the string at [st,en>
		static std::string substring(std::string s, unsigned int st, unsigned int en);
		
		//Validates path (replaces / with backslash)
		static std::string validatePath(std::string s);
		//Gets the string contents of a file
		static std::string fromFile(std::string file);		
		
		//Grabs the path without file+extension
		static std::string grabPath(std::string s);
		//Grabs the entire file path without an extension
		static std::string grabFileNoExt(std::string s);
		//Grabs the file name with extension
		static std::string grabFile(std::string s);
		//Parses path; gets rid of the ../
		static std::string parsePath(std::string s);
		//Gets rid of whitespace before and after the string; turn on 'dubbleWhitespace' to get rid of double whitespace
		static std::string trim(std::string s, bool dubbleWhitespace = false);
		
		//Parses an unsigned int from string (If string<0 it will see it as UINT_MAX)
		static unsigned int parseInt(std::string s);
		//Parses a signed int
		static int parseSInt(std::string s);
		//Splits the words from numbers
		static std::vector<std::string> splitStringNum(std::string s);
		//Checks whether or not the string is a number
		static bool isNumber(std::string s);
		
		//Gets the string in lowercase
		static std::string toLowerCase(std::string s);
		//Checks if a==b when lowercase
		static bool equalsIgnoreCase(std::string s0, std::string s1);
	};
}