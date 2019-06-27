#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/hmac.h>

std::string hmac_hex(std::string key, std::string msg)
{
	std::stringstream ss;
	unsigned char hash[32];
	unsigned int len = 32;

	if (key.empty())
		throw std::invalid_argument("key is empty");
	if (msg.empty())
		throw std::invalid_argument("msg is empty");
	HMAC_CTX hmac;
	HMAC_CTX_init(&hmac);
	HMAC_Init_ex(&hmac, &key[0], key.length(), EVP_sha256(), NULL);
	HMAC_Update(&hmac, (unsigned char*) &msg[0], msg.length());
	HMAC_Final(&hmac, hash, &len);
	HMAC_CTX_cleanup(&hmac);

	ss.fill('0');
	for (size_t i = 0; i < len; i++) {
		ss << std::hex << std::setw(2) << (unsigned int) hash[i];
	}
	std::string sig = ss.str();
	std::transform(sig.begin(), sig.end(), sig.begin(), ::toupper);
	return sig;
}

std::string hmac_str(std::string key, std::string msg)
{
	std::stringstream ss;
	unsigned char hash[32];
	unsigned int len = 32;


	if (key.empty())
		throw std::invalid_argument("key is empty");
	if (msg.empty())
		throw std::invalid_argument("msg is empty");
	HMAC_CTX hmac;
	HMAC_CTX_init(&hmac);
	HMAC_Init_ex(&hmac, &key[0], key.length(), EVP_sha256(), NULL);
	HMAC_Update(&hmac, (unsigned char*) &msg[0], msg.length());
	HMAC_Final(&hmac, hash, &len);
	HMAC_CTX_cleanup(&hmac);

	ss.fill('0');
	for (size_t i = 0; i < len; i++) {
		ss << hash[i];
	}
	return ss.str();
}
