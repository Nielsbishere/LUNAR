#include "../../Headers/Helper/StringUtils.h"

using namespace std;
using namespace osomi;

bool StringUtils::endsWith(string s, string end) {
	if (s.size() < end.size())return false;
	return s.substr(s.size() - end.size()) == end;
}
bool StringUtils::startsWith(string s, string begin) {
	if (s.size() < begin.size())return false;
	return s.substr(0, begin.size()) == begin;
}
string StringUtils::combine(vector<string> arr, string s) {
	if (arr.size() == 0)return "";
	string res = "";
	for (int i = 0; i < (int)arr.size() - 1; i++)
		res = res + arr[i] + s;
	return res + arr[arr.size() - 1];
}
string StringUtils::validatePath(string s) {
	return StringUtils::combine(split(s, '/'), "\\");
}
string StringUtils::fromFile(string file) {
	file = StringUtils::combine(split(file, '\\'), "/");
	ifstream str(file);
	if (!str.good()) {
		printf("Failed to get a file as a string\n");
		return "";
	}
	return string(istreambuf_iterator<char>(str), istreambuf_iterator<char>());
}
string StringUtils::numstring(unsigned int i, string prefix, string postfix) {
	stringstream ss;
	ss << prefix << i << postfix;
	return ss.str();
}
string StringUtils::substring(string s, unsigned int i) {
	return s.substr(i);
}
string StringUtils::substring(string s, unsigned int st, unsigned int en) {
	return s.substr(st, en);
}
vector<string> StringUtils::split(string s, char c) {
	unsigned int i = 0;
	vector<string> res;
	string curr;
	while (i < s.size()) {
		char cur = s[i];
		if (cur == c) {
			res.push_back(curr);
			curr = "";
		}
		else
			curr += cur;
		i++;
	}
	res.push_back(curr);
	return res;
}
string StringUtils::grabPath(string s) {
	s = validatePath(s);
	vector<string> str = split(s, '\\');
	if (str.size() == 1) return "";
	string st;
	for (unsigned int i = 0; i < str.size() - 1; i++)
		st = st + str[i] + "\\";
	return st;
}
string StringUtils::grabFileNoExt(string s) {
	s = validatePath(s);
	vector<string> str = split(s, '.');
	if (str.size() == 1) return s;
	string st;
	for (unsigned int i = 0; i < str.size() - 1; i++)
		st = st + str[i] + (i != str.size() - 2 ? "." : "");
	return st;
}
string StringUtils::grabFile(string s) {
	s = validatePath(s);
	vector<string> str = split(s, '\\');
	return str[str.size() - 1];
}
vector<string> StringUtils::split(string text, string delims) {
	vector<string> tokens;
	size_t off = 0;
	size_t doff;
	string t = text;
	while ((doff = text.find_first_of(delims[0], off)) != string::npos) {
		string subt = text.substr(doff, delims.size());
		if (subt != delims) {
			off = doff + 1;
			continue;
		}
		tokens.push_back(t.substr(0, doff - off));

		t = text.substr(doff + delims.size());
		off = doff + delims.size();
	}
	tokens.push_back(t.substr(0, doff - off));

	return tokens;
}
string StringUtils::parsePath(string s) {
	s = validatePath(s);
			
	unsigned int count = 0;

	while (StringUtils::startsWith(s, "..\\")) {
		count++;
		s = s.substr(3);
	}

	vector<string> vs = split(s, "..\\");
	if (vs.size() == 0)return s;
			
	string res = "";
	for (int i = 0; i < vs.size(); i++) {
		vector<string> vss = split(vs[i], '\\');
		int max = (int)vss.size() - (i != vs.size() - 1 ? 1 : 0);
		for (int j = 0; j < max; j++) {
			res += vss[j] + (j == max - 1 && i == vs.size() - 1 ? "" : "\\");
		}
	}
	for(unsigned int i=0;i<count;i++)s = "..\\" + s;
	return res;
}
string StringUtils::trim(string s, bool dubbleWhitespace) {
	if (s.size() == 0 || s == " ")return "";
	unsigned int UNDEF = 0xFFFFFFFF;
	unsigned int st=UNDEF, e=UNDEF;
	for (unsigned int i = 0; i < s.size(); i++) {
		if (s[i] != ' ' && s[i] != '\t') {
			st = i;
			break;
		}
	}
	for (unsigned int i = s.size(); i > 0; i--) {
		if (s[i - 1] != ' ' && s[i - 1] != '\t') {
			e = i;
			break;
		}
	}
	if (st == UNDEF || e == UNDEF)return "";
	s = s.substr(st, e);
	if (!dubbleWhitespace) {
		unsigned int nd = 0;
		bool b = false;
		for (int i = s.size(); i > 0; i--) {
			if (!b) {
				if (s[i - 1] == ' ' || s[i - 1] == '\t') {
					nd = i;
					b = true;
				}
			}
			else {
				if (!(s[i - 1] == ' ' || s[i - 1] == '\t')) {
					if (i + 1 != nd) 
						s = StringUtils::substring(s, 0, i) + " " + StringUtils::substring(s, nd);
					b = false;
				}
			}
		}
	}
	return s;
}
unsigned int StringUtils::parseInt(string s) {
	int i = stoi(s);
	return *(unsigned int*)(&i);
}
int StringUtils::parseSInt(string s) {
	return stoi(s);
}
vector<string> StringUtils::splitStringNum(string s) {
	vector<string> str = vector<string>();
	string prev = "";
	bool isNum = false;
	for (unsigned int i = 0; i < s.size(); i++) {
		char c = s[i];
		if (c >= 0x30 && c <= 0x39) {
			if (!isNum && prev != "") {
				str.push_back(prev);
				prev = "";
			}
			isNum = true;
		}
		else {
			if (isNum && prev != "") {
				str.push_back(prev);
				prev = "";
			}
			isNum = false;
		}
		prev = prev + s[i];
	}
	if (prev != "")
		str.push_back(prev);
	return str;
}
bool StringUtils::isNumber(string s) {
	for (unsigned int i = 0; i < s.size(); i++) 
		if (s[i] < 0x30 || s[i] > 0x39)return false;
	return true;
}
string StringUtils::toLowerCase(string s) {
	string res = s;
	for (unsigned int i = 0; i < s.size(); i++)
		if (s[i] >= 'A' && s[i] <= 'Z')
			res[i] = s[i] - 'A' + 'a';
	return res;
}
bool StringUtils::equalsIgnoreCase(string s0, string s1) {
	return toLowerCase(s0) == toLowerCase(s1);
}
bool StringUtils::find(vector<string> lines, string a, string b, vector<u32> &locations, bool trim){
	vector<u32> starts, ends;
	find(lines, a, starts, trim);
	find(lines, b, ends, trim);
	if (starts.size() != ends.size() || starts.size() == 0)return false;
	for (u32 i = 0; i < starts.size(); i++){
		locations.push_back(starts[i]);
		locations.push_back(ends[i]);
	}
	return true;
}
bool StringUtils::find(vector<string> lines, string a, vector<u32> &locations, bool tr){
	u32 fsize = locations.size();
	for (u32 i = 0; i < lines.size(); i++) {
		if ((tr && trim(lines[i]) == a) || (!tr && lines[i] == a))
			locations.push_back(i);
	}
	return locations.size() != fsize;
}
vector<string> StringUtils::erase(vector<string> lines, vector<string> toErase, bool tr){
	vector<string> res;
	for (u32 i = 0; i < lines.size(); i++){
		bool contains = false;
		for (u32 j = 0; j < toErase.size(); j++)
			if ((!tr && lines[i] == toErase[j]) || (tr && trim(lines[i]) == toErase[j])){
				contains = true;
				break;
			}
		if (!contains)res.push_back(lines[i]);
	}
	return res;
}