/*
	js-str.cpp

	This scource file is part of the jsl-esp32 project.

	Author: Lorenzo Pastrana
	Copyright © 2019 Lorenzo Pastrana

	This program is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation, either version 3 of the License, or (at your
	option) any later version.

	This program is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
	or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
	for more details.

	You should have received a copy of the GNU General Public License along
	with this program. If not, see http://www.gnu.org/licenses/.

*/



#include <iomanip>

#include "jsl-str.h"


std::string jsl_str::escape(const std::string& _str)
{
	std::string str;
	str.reserve(_str.size());
	for(auto i = _str.begin(); i != _str.end(); ++ i)
	{
		switch(*i)
		{
		case '\t':
			str.push_back('\\');
			str.push_back('t');
			break;
		case '\n':
			str.push_back('\\');
			str.push_back('n');
			break;
		case '\r':
			str.push_back('\\');
			str.push_back('r');
			break;
		case '\f':
			str.push_back('\\');
			str.push_back('f');
			break;
		case '\b':
			str.push_back('\\');
			str.push_back('b');
			break;
		case '\0':
			str.push_back('\\');
			str.push_back('0');
			break;
		case '\'':
			str.push_back('\\');
			str.push_back('\'');
			break;
		case '\"':
			str.push_back('\\');
			str.push_back('"');
			break;
		case '\\':
			str.push_back('\\');
			str.push_back('\\');
			break;
		// case '/':
		// 	str.push_back('\\');
		// 	str.push_back('/');
		// 	break;
		default:
			str.push_back(*i);
		}
	}
	return str;
}

std::string jsl_str::unescape(std::string& _str)
{
	std::string str;
	str.reserve(_str.size());
	for(auto i = _str.begin(); i != _str.end(); ++ i)
	{
		switch(*i)
		{
		case '"':
		case '\\':
		case '/':
			_str.push_back(*i++);
			break;
		case 'f':
			_str.push_back('\f');
			++i;
			break;
		case 'b':
			_str.push_back('\b');
			++i;
			break;
		case 'n':
			_str.push_back('\n');
			++i;
			break;
		case 'r':
			_str.push_back('\r');
			++i;
			break;
		case 't':
			_str.push_back('\t');
			++i;
			break;
		case 'u':
			++i;
			std::string hex(i,i+4);
			std::string::value_type ch = std::strtol(hex.c_str(),NULL,16);
			utf8_str(ch,_str);
			i+=4;
			break;
		}
	}
	return str;
}

void jsl_str::utf8_str(uint32_t _char, std::string& _str)
{
	char buf[5] = {'\0','\0','\0','\0','\0'};
	if (_char <= 0x7F)
	{
		buf[0] = static_cast<char>(_char);
	}
	else if (_char <= 0x7FF)
	{
		buf[1] = 0x80 | (_char & 0x3F);
		_char >>= 6;
		buf[0] = 0xC0 | _char;
	}
	else if (_char <= 0xFFFF)
	{
		buf[2] = 0x80 | (_char & 0x3F);
		_char >>= 6;
		buf[1] = 0x80 | (_char & 0x3F);
		_char >>= 6;
		buf[0] = 0xE0 | _char;
	}
	else
	{
		buf[3] = 0x80 | (_char & 0x3F);
		_char >>= 6;
		buf[2] = 0x80 | (_char & 0x3F);
		_char >>= 6;
		buf[1] = 0x80 | (_char & 0x3F);
		_char >>= 6;
		buf[0] = 0xF0 | _char;
	}
	_str += buf;
}

std::string jsl_str::url_encode(const std::string& _src)
{
	std::ostringstream ret;

	auto i = _src.begin();
	auto e = _src.end();
	while(i != e)
	{
		auto c = *i++;

        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
            c == '*' || c == '\'' || c == '(' || c == ')')
        {
			ret << c;
		}
        else if (c == ' ')
        {
            ret << '+';
        }
		else
		{
			// Any other characters are percent-encoded
			ret << std::uppercase;
			ret << '%' << std::setw(2) << (int)((unsigned char) c);
			ret << std::nouppercase;
		}
	}
	return ret.str();
}

std::string jsl_str::url_decode(const std::string& _src)
{
	std::string ret;
	ret.reserve(_src.size());

	auto i = _src.begin();
	auto e = _src.end();
	while(i != e)
	{
		if(*i == '%')
		{
			std::string hex(i+1,i+2);
			std::string::value_type ch = std::strtol(hex.c_str(),NULL,16);
			ret.push_back(ch);
			i += 3;
		}
		else if(*i == '+')
		{
			ret.push_back(' ');
			++i;
		}
		else
		{
			ret.push_back(*i);
			++i;
		}
	}
	return ret;
}

std::string jsl_str::b64_encode(const std::string& _src)
{
	std::string ret;

	constexpr const char* tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	int val=0, valb=-6;
	for(auto c : _src)
	{
		val = (val << 8) + c;
		valb += 8;
		while(valb >= 0)
		{
			ret.push_back(tbl[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}

	if (valb > -6)
	{
		ret.push_back(tbl[((val << 8) >> (valb + 8)) & 0x3F]);
	}

	while(ret.size() % 4) ret.push_back('=');

	return ret;
}

std::string jsl_str::b64_decode(const std::string& _src)
{
	std::string ret;

	constexpr const char* tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::vector<int> T(256,-1);

	for (int i = 0; i < 64; ++i) T[tbl[i]] = i;

	int val = 0, valb = -8;
	for(auto c : _src)
	{
		if (T[c] == -1) break;

		val = (val << 6) + T[c];
		valb += 6;

		if (valb >= 0)
		{
			ret.push_back(char((val >> valb) & 0xFF));
			valb-=8;
		}
	}

	return ret;
}
