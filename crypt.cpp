#include "pch.h"
#include "crypt.h"

ByteSlice Sha256PasswdWithSalt(string passwd, string salt){
	string tot = salt + passwd + salt;
	ByteSlice res = (ByteSlice)malloc(32);
	SHA256((ByteSlice)tot.data(), tot.size(), res);
	return res;
}

int Aes256CbcEnc(const ByteSlice plain,size_t n, ByteSlice enc, const ByteSlice key, const ByteSlice iv) {
	printf("Plain: ");
	for (int i = 0; i < n; i++) {
		printf("%X", plain[i]);
	}
	printf("\n");
	printf("IV: ");
	for (int i = 0; i < 16; i++) {
		printf("%X", iv[i]);
	}
	printf("\n");
	printf("Key: ");
	for (int i = 0; i < 32; i++) {
		printf("%X",key[i]);
	}
	printf("\n");
	
	int encLen=0,tmpLen=0;
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		return -1;
	}
	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	if (EVP_EncryptUpdate(ctx,enc,&encLen,plain,(int)n)!=1){
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	if (EVP_EncryptFinal_ex(ctx, enc + encLen, &tmpLen) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	encLen += tmpLen;
	EVP_CIPHER_CTX_free(ctx);
	printf("Enc: ");
	for (int i = 0; i < encLen; i++) {
		printf("%X", enc[i]);
	}
	printf("\n");
	return encLen;
}

int Aes256CbcDec(const ByteSlice enc, size_t n, ByteSlice plain, const ByteSlice key, const ByteSlice iv) {
	printf("Enc: ");
	for (int i = 0; i < n; i++) {
		printf("%X", enc[i]);
	}
	printf("\n");
	printf("IV: ");
	for (int i = 0; i < 16; i++) {
		printf("%X", iv[i]);
	}
	printf("\n");
	printf("Key: ");
	for (int i = 0; i < 32; i++) {
		printf("%X", key[i]);
	}
	printf("\n");
	int plainLen=0, tmpLen=0;
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		return -1;
	}
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	if (EVP_DecryptUpdate(ctx, plain, &plainLen, enc, (int)n)!=1) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	if (EVP_DecryptFinal_ex(ctx, plain + plainLen, &tmpLen) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	plainLen += tmpLen;
	EVP_CIPHER_CTX_free(ctx);
	printf("Plain: ");
	for (int i = 0; i < plainLen; i++) {
		printf("%X", plain[i]);
	}
	printf("\n");
	return plainLen;
}

void SafeFree(ByteSlice ptr, size_t n) {
	SafeClean(ptr, n);
	free(ptr);
}

void SafeClean(ByteSlice ptr, size_t n){
	memset(ptr, 0xFF, n);
	for (size_t i = 0; i < n; i++) {
		ptr[i] >>= 1;
	}
	memset(ptr, 0x00, n);
	memset(ptr, 0xFF, n);
	for (size_t i = 0; i < n; i++) {
		ptr[i] <<= 1;
	}
	memset(ptr, 0x00, n);
}

string GenSafeRandStr(size_t len) {
	string res;
	res.resize(len);
	if (RAND_bytes(reinterpret_cast<ByteSlice>(&res[0]), (int)len) != 1) {
		return "";
	}
	for (int i = 0; i < res.size();i++) {
		res[i] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklZxcvbnm"[abs(res[i]) % 62];
	}
	return res;
}