#include "bitscpp_utils.h"

//--------------------------------
void split_string( string &s, char delim, vector <string> &result) {

    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    
}



//--------------------------------
int replace_string_once( string& str, const char *from, const char *to, int offset) {

    size_t start_pos = str.find(from, offset);
    if( start_pos == std::string::npos ) {
        return 0;
    }
    str.replace(start_pos, strlen(from), to);
    return start_pos + strlen(to);
}


//--------------------------------
bool replace_string( string& str, const char *from, const char *to) {

    bool found = false;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, strlen( from ), to);
        found = true;
        start_pos += strlen(to);
    }
    return found;
}


//-----------------------
void string_toupper( string &src) {
    for ( int i = 0 ; i < src.size() ; i++ ) {
        src[i] = toupper(src[i]);
    }
}

//------------------
string string_toupper( const char *cstr ) {
    string ret;
    for ( int i = 0 ; i < strlen( cstr ) ; i++ ) {
        ret.push_back( toupper(cstr[i]) );
    }
    return ret;
}

//-----------------
string string_tolower( const char *cstr ) {
	string ret;
	for ( int i = 0 ; i < strlen ( cstr) ; i++ ) {
		ret.push_back ( tolower(cstr[i]) );
	}
	return ret;
}


//--------------------------------------
string b2a_hex( char *byte_arr, int n ) {

    const static std::string HexCodes = "0123456789abcdef";
    string HexString;
    for ( int i = 0; i < n ; ++i ) {
        unsigned char BinValue = byte_arr[i];
        HexString += HexCodes[( BinValue >> 4 ) & 0x0F];
        HexString += HexCodes[BinValue & 0x0F];
    }
    return HexString;
}



//---------------------------------
time_t get_current_epoch( ) {

    struct timeval tv;
    gettimeofday(&tv, NULL); 

    return tv.tv_sec ;
}

//---------------------------------
unsigned long get_current_ms_epoch( ) {

    struct timeval tv;
    gettimeofday(&tv, NULL); 

    return tv.tv_sec * 1000 + tv.tv_usec / 1000 ;

}

//---------------------------
string hmac_sha256( const char *key, const char *data) {

    unsigned char* digest;
    digest = HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)data, strlen(data), NULL, NULL);    
    return b2a_hex( (char *)digest, 32 );
}   

//------------------------------
string sha256( const char *data ) {

    unsigned char digest[32];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, strlen(data) );
    SHA256_Final(digest, &sha256);
    return b2a_hex( (char *)digest, 32 );
    
}

//---------------------------
string hmac_sha512( const char *key, const char *data) {

    unsigned char* digest;
    digest = HMAC(EVP_sha512(), key, strlen(key), (unsigned char*)data, strlen(data), NULL, NULL);
	/*char sha512_str[HMAC_MAX_MD_CBLOCK];
    for (int i = 0; i < 64; i++)
        sprintf(&sha512_str[i * 2], "%02x", (unsigned int) digest[i]);

    return std::string(sha512_str);*/
    return b2a_hex( (char *)digest, 64 );
}

//------------------------------
string sha512( const char *data ) {

    unsigned char digest[64];
    SHA256_CTX sha512;
    SHA256_Init(&sha512);
    SHA256_Update(&sha512, data, strlen(data) );
    SHA256_Final(digest, &sha512);
    return b2a_hex( (char *)digest, 64 );

}

//--------------------
string to_2decimal( double &value) {
	stringstream stream;
	stream << fixed << setprecision(2) << value;
	return stream.str();
}

