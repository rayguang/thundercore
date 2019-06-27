#include "krakcpp_utils.h"

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

//------------------------------
string to_2decimal( double &value) {
	stringstream stream;
	stream << fixed << setprecision(2) << value;
	return stream.str();
}

//------------------------------
// helper function to decode a base64 string to a vector of bytes:
std::vector<unsigned char> b64_decode(const std::string& data) 
{
   BIO* b64 = BIO_new(BIO_f_base64());
   BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

   BIO* bmem = BIO_new_mem_buf((void*)data.c_str(),data.length());
   bmem = BIO_push(b64, bmem);
   
   std::vector<unsigned char> output(data.length());
   int decoded_size = BIO_read(bmem, output.data(), output.size());
   BIO_free_all(bmem);

   if (decoded_size < 0)
      throw std::runtime_error("failed while decoding base64.");
   
   return output;
}

//------------------------------
// helper function to encode a vector of bytes to a base64 string:
std::string b64_encode(const std::vector<unsigned char>& data) 
{
   BIO* b64 = BIO_new(BIO_f_base64());
   BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

   BIO* bmem = BIO_new(BIO_s_mem());
   b64 = BIO_push(b64, bmem);
   
   BIO_write(b64, data.data(), data.size());
   BIO_flush(b64);

   BUF_MEM* bptr = NULL;
   BIO_get_mem_ptr(b64, &bptr);
   
   std::string output(bptr->data, bptr->length);
   BIO_free_all(b64);

   return output;
}

std::vector<unsigned char> 
hmac_sha512_krak(const std::vector<unsigned char>& data, 
	    const std::vector<unsigned char>& key)
{   
   unsigned int len = EVP_MAX_MD_SIZE;
   std::vector<unsigned char> digest(len);

   HMAC_CTX *ctx = HMAC_CTX_new();
   //HMAC_CTX_init(&ctx);

   HMAC_Init_ex(ctx, key.data(), key.size(), EVP_sha512(), NULL);
   HMAC_Update(ctx, data.data(), data.size());
   HMAC_Final(ctx, digest.data(), &len);
   
   HMAC_CTX_free(ctx);
   //HMAC_CTX_cleanup(&ctx);
   
   return digest;
}

std::vector<unsigned char> sha256_krak(const std::string& data)
{
   std::vector<unsigned char> digest(SHA256_DIGEST_LENGTH);

   SHA256_CTX ctx;
   SHA256_Init(&ctx);
   SHA256_Update(&ctx, data.c_str(), data.length());
   SHA256_Final(digest.data(), &ctx);

   return digest;
}

//------------------------------
// Build http queries by parsing xyz=xxx&abc=xxx&
static std::string params_string(Params const &params)
{
  if(params.empty()) return "";
  Params::const_iterator pb= params.cbegin(), pe= params.cend();
  std::string data= pb-> first+ "="+ pb-> second;
  ++ pb; if(pb== pe) return data;
  for(; pb!= pe; ++ pb)
    data+= "&"+ pb-> first+ "="+ pb-> second;
  return data;
}
