#pragma once
#include"pch.h"
ByteSlice Sha256PasswdWithSalt(string passwd, string salt);
int Aes256CbcEnc(const ByteSlice plain, size_t n, ByteSlice enc, const ByteSlice key, const ByteSlice iv);
int Aes256CbcDec(const ByteSlice enc, size_t n, ByteSlice plain, const ByteSlice key, const ByteSlice iv);
void SafeFree(ByteSlice ptr, size_t n);
void SafeClean(ByteSlice ptr, size_t n);
string GenSafeRandStr(size_t len);
