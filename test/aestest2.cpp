// g++ -g3 -ggdb -O0 -DDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread
// g++ -g -O2 -DNDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread
#include "crypto++/modes.h"
#include "crypto++/aes.h"
#include "crypto++/filters.h"
#include "crypto++/osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <cstdlib>
using std::exit;

#include "crypto++/cryptlib.h"
using CryptoPP::Exception;

#include "crypto++/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "crypto++/base64.h"
using CryptoPP::Base64Encoder;
using CryptoPP::Base64Decoder;

#include "crypto++/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "crypto++/aes.h"
using CryptoPP::AES;

#include "crypto++/ccm.h"
using CryptoPP::CBC_Mode;

#include "assert.h"

int main(int argc, char* argv[])
{
	AutoSeededRandomPool prng;

	byte key[AES::DEFAULT_KEYLENGTH];
	//prng.GenerateBlock(key, sizeof(key));

	byte iv[AES::BLOCKSIZE];
	prng.GenerateBlock(iv, sizeof(iv));

	string keytext("yP7BMP850RZiukOHnTqOl1ir2CBL9eDAWv6fP16YNqP");
	memset(key, 0, sizeof(key));
    memcpy(key, keytext.data(), sizeof(key));

	string plain = "zko2u2jOqpp3cprCGtQp3MqwP5xKkxPAJfloSQjAUrh";
	string cipher, encoded, decoded, recovered, cipher_encoded, cipher_decoded, iv_encoded, iv_decoded, key_encoded, key_decoded;

	/*********************************\
	\*********************************/

	// Pretty print key
	// HexEncoder, Base64Encoder 
	encoded.clear();
	StringSource(key, sizeof(key), true,
		new Base64Encoder( 
			new StringSink(key_encoded)
		) // Base64Encoder
	); // StringSource
	cout << "key: " << key_encoded << " key size: " << sizeof(key) << endl;

	// Pretty print iv
	encoded.clear();
	StringSource(iv, sizeof(iv), true,
		new Base64Encoder(
			new StringSink(iv_encoded)
		) // Base64Encoder
	); // StringSource
	cout << "iv: " << iv_encoded << " iv size: " << sizeof(iv) << endl;
	size_t len = sizeof(iv);
	string iv_string( reinterpret_cast<char const*>(iv), len);
	cout << "iv_string: " << iv_string << endl;

	
	
	//cout << "iv (non-encoded): " << iv << endl;
	/*********************************\
	\*********************************/

	try
	{
		cout << "plain text:\t" << plain << endl;

		CBC_Mode< AES >::Encryption e;
		e.SetKeyWithIV(key, sizeof(key), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(plain, true, 
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) // StreamTransformationFilter
		); // StringSource

#if 0
		StreamTransformationFilter filter(e);
		filter.Put((const byte*)plain.data(), plain.size());
		filter.MessageEnd();

		const size_t ret = filter.MaxRetrievable();
		cipher.resize(ret);
		filter.Get((byte*)cipher.data(), cipher.size());
#endif
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	/*********************************\
	\*********************************/

	// Pretty print
	encoded.clear();
	StringSource(cipher, true,
		new Base64Encoder(
			new StringSink(cipher_encoded)
		) //
	); // StringSource
	cout << "cipher text: " << cipher_encoded << endl;
	

	/*********************************\
	\*********************************/

	try
	{
		CBC_Mode< AES >::Decryption d;
		d.SetKeyWithIV(key, sizeof(key), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(cipher, true, 
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) // StreamTransformationFilter
		); // StringSource

#if 0
		StreamTransformationFilter filter(d);
		filter.Put((const byte*)cipher.data(), cipher.size());
		filter.MessageEnd();

		const size_t ret = filter.MaxRetrievable();
		recovered.resize(ret);
		filter.Get((byte*)recovered.data(), recovered.size());
#endif

		cout << "recovered text:\t" << recovered << endl;
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	/*********************************\
	\*********************************/

	return 0;
}

