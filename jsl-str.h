/*
	jsl-stru.h

	This scource file is part of the jsl- project.

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



#ifndef JSL_STR_H
#define JSL_STR_H


#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace jsl_str
{
	typedef std::vector<std::string> svect_t;

	inline void splitv(svect_t& _dst, const std::string& _str, char _c)
	{
		std::string line;
		std::stringstream stream;
		stream.str(_str);
		while(std::getline(stream, line, _c))
		{
			if(line != "") _dst.push_back(std::move(line));
		}
	}

	inline bool split(const std::string& _src, char _c, std::string& _l, std::string& _r)
	{
		size_t p = _src.find(_c);
		if(p != std::string::npos)
		{
			_r = _src.substr(p+1); // allow _src & _l to the same string
			_l = _src.substr(0,p);
			return true;
		}
		return false;
	}

	inline std::string trim(std::string _str, char _c = ' ')
	{
		size_t b = 0, e = _str.size() - 1;
		while(_str[b] == _c && b <= e) ++b;
		while(_str[e] == _c && e > 0) --e;
		return _str.substr(b,(e + 1) - b);
	}

	std::string escape(const std::string& _str);
	std::string unescape(std::string& _str);

	void utf8_str(uint32_t _char, std::string& _str);

	std::string url_encode(const std::string& _src);
	std::string url_decode(const std::string& _src);

	std::string b64_encode(const std::string& _src);
	std::string b64_decode(const std::string& _src);
}

#endif // #ifndef JSL_STR_H
