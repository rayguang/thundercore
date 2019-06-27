#ifndef KRAKCPP_UTILS
#define KRAKCPP_UTILS

#include <iomanip>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <string.h>
#include <sys/time.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>

#include <iostream>
using namespace std;

void split_string( string &s, char delim, vector <string> &result);
bool replace_string( string& str, const char *from, const char *to);
int replace_string_once( string& str, const char *from, const char *to , int offset);


string b2a_hex( char *byte_arr, int n );
time_t get_current_epoch();
unsigned long get_current_ms_epoch();


//--------------------
inline bool file_exists (const std::string& name) {
 	 return ( access( name.c_str(), F_OK ) != -1 );
}

string hmac_sha256( const char *key, const char *data);
string sha256( const char *data );
string hmac_sha512( const char *key, const char *data);
string sha512 ( const char *data );
void string_toupper( string &src);
string string_toupper( const char *cstr );
string string_tolower( const char *cstr );
auto bool_to_string = [](bool b) -> std::string {
	return b? "true" : "false";
};
string to_2decimal(double &value);
vector<unsigned char> b64_decode (const string& data);
string b64_encode (const vector<unsigned char >& data);
vector<unsigned char> hmac_sha512_krak( const vector<unsigned char>& data, const vector<unsigned char>& key );
vector<unsigned char> sha256_krak(const string& data);
#endif
