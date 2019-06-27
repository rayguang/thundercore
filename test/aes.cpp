#include "crypto++/modes.h"
#include "crypto++/aes.h"
#include "crypto++/filters.h"
#include "crypto++/osrng.h"

#include "crypto++/cryptlib.h"
using CryptoPP::Exception;

#include "crypto++/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "crypto++/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "crypto++/aes.h"
using CryptoPP::AES;

#include "crypto++/ccm.h"
using CryptoPP::CBC_Mode;


#include <iostream>
#include <string>

using namespace std;

byte iv[16];
//string strIv = "162169848599E7C792BF58BFA53D88E6";
//memcpy(iv, strIv.data(), strIv.length());

string Encrypt(string plain, string strkey) {

    byte key[AES::DEFAULT_KEYLENGTH];
    byte* k = (byte*) strkey.c_str();

	for (int i = 0; i < AES::BLOCKSIZE; i++)
        iv[i] = 0;

    for (int i = 0; i < AES::DEFAULT_KEYLENGTH; i++)
        if (i<sizeof (k))
            key[i] = k[i];
        else
            key[i] = 0;
    string ciphertextEncode,ciphertext;
	cout << "\n\n ******** Encrypting plaintext ********" << std::endl;
    cout << "blocksize: " << AES::BLOCKSIZE << " keysize: " << AES::DEFAULT_KEYLENGTH << std::endl;
	cout << "\nplaintext:\t" << plain << std::endl;
    cout << "key(encrypt):\t" <<key << std::endl;
    ciphertextEncode.clear();
    ciphertext.clear();
    CryptoPP::AES::Encryption aesEncryption(key, AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*> (plain.c_str()), plain.length() + 1);
    stfEncryptor.MessageEnd();
    cout << "encrypted: " <<ciphertext << std::endl;
    StringSource ss(ciphertext, true, new HexEncoder(new StringSink(ciphertextEncode)));
    cout << "encoded encrypted(hex): " <<ciphertextEncode << std::endl;
    return ciphertextEncode;
}


string Decrypt(string cipher, string strkey) {

    byte key[AES::DEFAULT_KEYLENGTH];
    byte* k = (byte*) strkey.c_str();
	
	for (int i = 0; i < AES::BLOCKSIZE; i++)
        iv[i] = 0;

    for (int i = 0; i < AES::DEFAULT_KEYLENGTH; i++)
        if (i<sizeof (k))
            key[i] = k[i];
        else
            key[i] = 0;
    string ciphertextDecode,decryptedtext;
    cout << "\n\n ******** Decrypting ciphertext ********" << std::endl;
	cout <<"\nciphertext:\t"<< cipher;
    cout << "\nkey(decrypt):\t" <<key;
    ciphertextDecode.clear();
    decryptedtext.clear();

    StringSource ss(cipher, true, new HexDecoder(new StringSink(ciphertextDecode)));
    cout << "\ncipher decoded: " << ciphertextDecode;
    CryptoPP::AES::Decryption aesDecryption(key, AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
    stfDecryptor.Put(reinterpret_cast<const unsigned char*> (ciphertextDecode.c_str()), ciphertextDecode.size());
    stfDecryptor.MessageEnd();

    cout <<"\ndecrypted text: "<<decryptedtext << std::endl;
    return decryptedtext;
}

int main ()
{
	std::string plaintext, ciphertext, cipherEncode, decryptedText,key;

	key = "yP7BMP850RZiukOHnTqOl1ir2CBL9eDAWv6fP16YNqP";
	plaintext= "zko2u2jOqpp3cprCGtQp3MqwP5xKkxPAJfloSQjAUrh";
	
	cipherEncode = Encrypt( plaintext, key);
	//std::cout << "\n[plaintext]:\t" << plaintext << std::endl;
	//istd::cout << "cipherEncode:\t" << cipherEncode << std::endl;

	decryptedText = Decrypt(cipherEncode, key);
	//std::cout << "plaintext:\t" << decryptedText << std::endl;

	return 0;
}

