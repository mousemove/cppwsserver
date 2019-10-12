/*
 * additional.cpp
 *
 *  Created on: 8 окт. 2019 г.
 *      Author: alab
 */


#include "additional.h"

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'};
static int mod_table[] = {0, 2, 1};

/*Названия функций поиска с использованием регулярным выражениям сделаны по аналогии с существующими в других языках */
vector<string> preg_match_all(const std::string & rex,const string & str,unsigned int id)
				{
	vector<string> result;
	std::regex re(rex);
	std::smatch match;
	string::const_iterator searchStart( str.cbegin() );
	while(std::regex_search(searchStart, str.cend(), match, re)) {
		if(id < match.size()) result.push_back(match[id]);
		searchStart = match.suffix().first;
	}

	return result;
				}


string preg_match(const std::string & rex,const string & str,unsigned int id)
{
	string result = "";
	std::regex re(rex);
	std::smatch match;
	if(std::regex_search(str, match, re)) {
		if(id < match.size()) result = match[id];
	}

	return result;
}

bool SHA1_ossl(void* input, unsigned long length, unsigned char* md)
{
	SHA_CTX context;
	if(!SHA1_Init(&context))
		return false;

	if(!SHA1_Update(&context, (unsigned char*)input, length))
		return false;

	if(!SHA1_Final(md, &context))
		return false;

	return true;
}

void base64_encode(const unsigned char *data,
		size_t input_length,
		size_t *output_length,char * encoded_data) {

	*output_length = 4 * ((input_length + 2) / 3);


	for (int i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[*output_length - 1 - i] = '=';

	return ;
}


string sha1stringbase64( const std::string & input)
{
	unsigned char md[20] = {0};
	SHA1_ossl((void*)input.c_str(),input.size(),md);
	char key_out[64] = {0};
	size_t output_length = 64;
	base64_encode(md,20,&output_length,key_out);
	cout << "||" << key_out << endl;
	return string(key_out);
}


