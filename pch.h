// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include "openssl/evp.h"
#include "openssl/sha.h"
#include "openssl/rand.h"
#include "sqlite3.h"
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>

using namespace std;

typedef __int64 Int64;
typedef unsigned char Byte;
typedef Byte* ByteSlice;

enum MemoOpStatus {MEMO_OP_FAIL=0, MEMO_OP_OK=1};

struct SizedBlob {
	ByteSlice Data{};
	size_t Size{};
};

struct RawMemo {
	Int64 Id{};
	Int64 Created{};
	Int64 Deadline{};
	SizedBlob RawContent{};
};

struct PlainMemo {
	Int64 Id{};
	Int64 Created{};
	Int64 Deadline{};
	string Content{};
};

struct EncInfo {
	bool Enc{};
	string Salt{};
	ByteSlice Key{};
	ByteSlice ChkEnc{};
	size_t ChkLen{};
	ByteSlice ChkSha256{};
};

extern string DbFile;
extern vector<RawMemo> AllRaws;
extern vector<PlainMemo> AllPlain;
extern vector<bool> Show;
extern EncInfo MemoEnc;
extern bool IsMemoSelected;
extern bool IsMemoEncrypted;
extern bool IsMemoUnlocked;
extern ByteSlice IV;

#endif //PCH_H
