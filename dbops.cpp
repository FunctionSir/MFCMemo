#include "dbops.h"
#include "pch.h"

bool IsFileGood(string path) {
	ifstream f(path);
	return f.good();
}

MemoOpStatus NewMemosDb(void) {
	if (!IsMemoSelected) {
		return MEMO_OP_FAIL;
	}
	sqlite3* db;
	auto res = sqlite3_open(DbFile.c_str(), &db);
	if (res) {
		return MEMO_OP_FAIL;
	}
	res = sqlite3_exec(db, "CREATE TABLE MEMOS (ID INTEGER PRIMARY KEY AUTOINCREMENT, CREATED INTEGER, DEADLINE INTEGER, DATA BLOB)", nullptr, nullptr,nullptr);
	if (res != SQLITE_OK) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	res = sqlite3_exec(db, "CREATE TABLE ENC (SALT TEXT, CHK BLOB, SHA256 BLOB); INSERT INTO ENC VALUES('','',NULL)", nullptr, nullptr, nullptr);
	if (res != SQLITE_OK) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	sqlite3_close(db);
	return MEMO_OP_OK;
}

MemoOpStatus InsertRawMemo(const RawMemo &src){
	if (!IsMemoSelected) {
		return MEMO_OP_FAIL;
	}
	sqlite3* db;
	sqlite3_stmt* stmt;
	const char* query = "INSERT INTO MEMOS (CREATED, DEADLINE, DATA) VALUES (?,?,?)";
	auto res = sqlite3_open(DbFile.c_str(), &db);
	if (res) {
		return MEMO_OP_FAIL;
	}
	res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
	if (res) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	sqlite3_bind_int64(stmt, 1, src.Created);
	sqlite3_bind_int64(stmt, 2, src.Deadline);
	sqlite3_bind_blob(stmt, 3, src.RawContent.Data, (int)src.RawContent.Size, SQLITE_TRANSIENT);
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		return MEMO_OP_FAIL;
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return MEMO_OP_OK;
}

MemoOpStatus GetRawMemos(vector<RawMemo>& dest,EncInfo &enc) {
	if (!IsMemoSelected || !IsFileGood(DbFile)) {
		return MEMO_OP_FAIL;
	}
	sqlite3* db;
	sqlite3_stmt* stmt;
	const char* query = "SELECT ID, CREATED, DEADLINE, DATA FROM MEMOS";
	const char* query_enc_info = "SELECT SALT, CHK, SHA256 FROM ENC LIMIT 1";
	if (sqlite3_open(DbFile.c_str(), &db) != SQLITE_OK) {
		return MEMO_OP_FAIL;
	}
	if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	RawMemo tmp;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		tmp.Id = sqlite3_column_int64(stmt, 0);
		tmp.Created = sqlite3_column_int64(stmt, 1);
		tmp.Deadline = sqlite3_column_int64(stmt, 2);
		tmp.RawContent.Size = sqlite3_column_bytes(stmt, 3);
		tmp.RawContent.Data = (ByteSlice)malloc(tmp.RawContent.Size);
		if (tmp.RawContent.Data != nullptr) { 
			memcpy(tmp.RawContent.Data, (ByteSlice)sqlite3_column_blob(stmt, 3), tmp.RawContent.Size); 
		} else {
			return MEMO_OP_FAIL;
		}
		
		dest.push_back(tmp);
	}
	sqlite3_finalize(stmt);
	if (sqlite3_prepare_v2(db, query_enc_info, -1, &stmt, nullptr) != SQLITE_OK) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	enc.Key = nullptr;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		enc.Salt = string((char *)sqlite3_column_text(stmt,0));
		enc.ChkLen = sqlite3_column_bytes(stmt, 1);
		if (enc.ChkLen > 0) {
			enc.ChkEnc = (ByteSlice)malloc(enc.ChkLen);
			if (enc.ChkEnc != nullptr) {
				memcpy(enc.ChkEnc, (ByteSlice)sqlite3_column_blob(stmt, 1), enc.ChkLen);
				// === //
				printf("CHK_ENC:");
				for (int i = 0; i < enc.ChkLen; i++) {
					printf("%X", enc.ChkEnc[i]);
				}
				// === //
				puts("");
			}
			else {
				return MEMO_OP_FAIL;
			}
		} else {
			enc.ChkEnc = nullptr;
		}
		if (sqlite3_column_bytes(stmt, 2) == 32) {
			enc.ChkSha256 = (ByteSlice)malloc(32);
			if (enc.ChkSha256 != nullptr) {
				memcpy(enc.ChkSha256, (ByteSlice)sqlite3_column_blob(stmt, 2), 32);
			}
			else {
				return MEMO_OP_FAIL;
			}
		}
		else {
			enc.ChkSha256 = nullptr;
		}
	}
	enc.Enc = (enc.Salt.size() > 0);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return MEMO_OP_OK;
}

MemoOpStatus SaveToDb(){
	if (!IsMemoSelected) {
		return MEMO_OP_FAIL;
	}
	sqlite3* db;
	auto res = sqlite3_open(DbFile.c_str(), &db);
	if (res) {
		return MEMO_OP_FAIL;
	}
	res = sqlite3_exec(db, "DROP TABLE MEMOS", nullptr, nullptr, nullptr);
	if (res != SQLITE_OK) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	res = sqlite3_exec(db, "DROP TABLE ENC", nullptr, nullptr, nullptr);
	if (res != SQLITE_OK) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	res = sqlite3_exec(db, "CREATE TABLE MEMOS (ID INTEGER PRIMARY KEY AUTOINCREMENT, CREATED INTEGER, DEADLINE INTEGER, DATA BLOB)", nullptr, nullptr, nullptr);
	if (res != SQLITE_OK) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	res = sqlite3_exec(db, "CREATE TABLE ENC (SALT TEXT, CHK BLOB, SHA256 BLOB)", nullptr, nullptr, nullptr);
	if (res != SQLITE_OK) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	for (auto &x : AllRaws) {
		InsertRawMemo(x);
	}
	const char* query_enc_info = "INSERT INTO ENC (SALT, CHK, SHA256) VALUES (?,?,?)";
	sqlite3_stmt* stmt;
	res = sqlite3_prepare_v2(db, query_enc_info, -1, &stmt, nullptr);
	if (res) {
		sqlite3_close(db);
		return MEMO_OP_FAIL;
	}
	sqlite3_bind_text(stmt, 1, MemoEnc.Salt.c_str(),-1,SQLITE_TRANSIENT);
	sqlite3_bind_blob(stmt, 2, MemoEnc.ChkEnc,(int)MemoEnc.ChkLen, SQLITE_TRANSIENT);
	sqlite3_bind_blob(stmt, 3, MemoEnc.ChkSha256, 32, SQLITE_TRANSIENT);
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		return MEMO_OP_FAIL;
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return MEMO_OP_OK;
}