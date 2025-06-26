#pragma once
#include"pch.h"

bool IsFileGood(string path);
MemoOpStatus NewMemosDb(void);
MemoOpStatus InsertRawMemo(const RawMemo& src);
MemoOpStatus GetRawMemos(vector<RawMemo>& dest, EncInfo& enc);

MemoOpStatus SaveToDb();
