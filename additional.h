
#ifndef ADDITIONAL_H_
#define ADDITIONAL_H_



#include <iostream>
#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/ossl_typ.h>
#include <unistd.h>
#include <openssl/err.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <curl/curl.h>

#include <thread>
#include <memory>
#include <cstring>

#include <vector>
#include <regex>
#include <algorithm>
#include <string>
using namespace std;
vector<string> preg_match_all(const std::string & rex,const string & str,unsigned int id);
string preg_match(const std::string & rex,const string & str,unsigned int id);
bool SHA1_ossl(void* input, unsigned long length, unsigned char* md);
void base64_encode(const unsigned char *data,
		size_t input_length,
		size_t *output_length, char * encoded_data) ;
string sha1stringbase64( const std::string & input);


union RFCHeader{
	struct  {
		union
		{
			struct {
				unsigned char opcode:4;
				unsigned char RSV3:1;
				unsigned char RSV2:1;
				unsigned char RSV1:1;
				unsigned char FIN:1;
			} asBIT;
			unsigned char byte;

		} _firstbyte;
		union
		{
			struct {
				unsigned char length:7;
				unsigned char MASK:1;

			} asBIT;
			unsigned char byte;

		} _secondbyte;

		union additionaldata {
			struct {
				unsigned char mask[4];
				unsigned char messagePart[8];
			} _shortmessage;
			struct {
				unsigned char length[2];
				unsigned char mask[4];
				unsigned char messagePart[6];
			} _middlemessage;
			struct {
				unsigned char length[8];
				unsigned char mask[4];
			} _longmessage;


		} _additionaldata;
	} parted;
	unsigned char buffer[14] = {0};
};


struct RFCMessage
{
	RFCHeader header;
	vector<char> data;
	unsigned int writed;
	void convert()
	{
		unsigned char * mask;
		if (header.parted._secondbyte.asBIT.length <= 125)
		{
			mask = header.parted._additionaldata._shortmessage.mask;
		}
		else if(header.parted._secondbyte.asBIT.length == 126 )
		{
			mask = header.parted._additionaldata._middlemessage.mask;
		}
		else if(header.parted._secondbyte.asBIT.length == 127 )
		{
			mask = header.parted._additionaldata._longmessage.mask;
		}
		for(unsigned int i = 0;i<data.size();i++)
		{
			data[i] = data[i] ^ mask[i % 4];
		}
	}
};


#endif /* ADDITIONAL_H_ */
