/**
*/

#ifndef _cSAVERDB_H_
#define _cSAVERDB_H_

#include "stdafx.h"
#include "sqlite3.h"

class cSaverDB
{
public:
	cSaverDB();
	~cSaverDB();

	int open();
	int close();
	int exec(const char *sql);
	int wexec(const wchar_t *sql);

	int InsertFile(const TCHAR *filebase, const TCHAR *filename);

	int InsertFilePrep();
	int InsertFileExec(const TCHAR *filebase, const WIN32_FIND_DATA *fd);
	int InsertFileDone();

	int ConfigRead(const wchar_t *key, wchar_t *val, int val_len);
	int ConfigWrite(int update, const wchar_t *key, wchar_t *val);

	int error()				{return rc;}
	const char *error_str()	{return zErr;}

protected:
	int rc;
	char *zErr;

private:
	sqlite3* db;
	sqlite3_stmt* insFileQry;

	void free_error() {sqlite3_free(zErr);}
};

#endif