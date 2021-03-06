#include "crypto++/cryptlib.h"
#include "crypto++/modes.h"
#include "crypto++/aes.h"
#include "crypto++/filters.h"
#include "crypto++/sha.h"
#include "crypto++/hkdf.h"
#include <iostream>

using namespace std; 
int main(int argc, char* argv[]) {

    //Key and IV setup
    //AES encryption uses a secret key of a variable length (128-bit, 196-bit or 256-   
    //bit). This key is secretly exchanged between two parties before communication   
    //begins. DEFAULT_KEYLENGTH= 16 bytes
    byte passwordBytes[ CryptoPP::AES::DEFAULT_KEYLENGTH ];
    byte ivBytes[ CryptoPP::AES::BLOCKSIZE ];
	std::string plaintext("Please encrypt me :)");
	std::string aesKey("aespassword");
	std::string ivText("ivtext");

    // memset( key, 'xyzxyz', CryptoPP::AES::DEFAULT_KEYLENGTH );
    // memset( iv, 'abcedfabcdefabcd', CryptoPP::AES::BLOCKSIZE );
	memset(passwordBytes, 0, sizeof(passwordBytes));
	memcpy(passwordBytes, aesKey.data(), aesKey.size());
	memcpy(ivBytes, ivText.data(), CryptoPP::AES::BLOCKSIZE);

	std::cout  << "KEY: " << key << std::endl;
	std::cout << "IV: :" << iv << std::endl;

    //
    // String and Sink setup
    //
    std::string plaintext = "Now a test for AES cipher...";
    std::string ciphertext;
    std::string decryptedtext;

    //
    // Dump Plain Text
    //
    std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
    std::cout << plaintext;
    std::cout << std::endl << std::endl;

    //
    // Create Cipher Text
    //
    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() + 1 );
    stfEncryptor.MessageEnd();

    //
    // Dump Cipher Text
    //
    std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

    for( int i = 0; i < ciphertext.size(); i++ ) {

        std::cout << "0x" << std::hex << (0xFF & static_cast<byte>(ciphertext[i])) << " ";
    }

    std::cout << std::endl << std::endl;

    //
    // Decrypt
    //
    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
    stfDecryptor.MessageEnd();

    //
    // Dump Decrypted Text
    //
    std::cout << "Decrypted Text: " << std::endl;
    std::cout << decryptedtext;
    std::cout << std::endl << std::endl;

    return 0;
}

