/**
*/

#include "stdafx.h"
#include "cSaverDB.h"

/**
*/
cSaverDB::cSaverDB()
{
	db = NULL;
	insFileQry = NULL;

	zErr = NULL;
	rc = SQLITE_OK;
}

/**
*/
cSaverDB::~cSaverDB()
{
	close();
}

/**
*/
int cSaverDB::open()
{
	rc = sqlite3_open("myslides.db", &db);
	if (rc) {
		close();
	}
	else if (rc == SQLITE_OK) {
		if (exec("CREATE TABLE IF NOT EXISTS images("
			"id INTEGER PRIMARY KEY,"
			"filebase TEXT,"
			"filename TEXT,"
			"filecreate INTEGER,"
			"filesize INTEGER,"
			"active BOOLEAN,"
			"UNIQUE(filebase,filename)"
			");"
//			"CREATE INDEX IF NOT EXISTS active ON images(active);"
//			"CREATE UNIQUE INDEX IF NOT EXISTS path ON images(filebase,filename);"
			"CREATE TABLE IF NOT EXISTS config("
//			"key TEXT UNIQUE PRIMARY KEY ON CONFLICT REPLACE,"
			"key TEXT UNIQUE PRIMARY KEY,"
			"value TEXT"
			");"
//			"CREATE INDEX IF NOT EXISTS key ON config(key);"
			) == SQLITE_OK
//			&& exec("CREATE INDEX IF NOT EXISTS active ON images(active)") == SQLITE_OK
//			&& exec("CREATE UNIQUE INDEX IF NOT EXISTS path ON images(filebase,filename)") == SQLITE_OK
			) {
			// success!
			exec("PRAGMA synchronous = OFF");
		}
		else {
			sqlite3_errmsg(db);
		}
	}
	return rc;
}

/**
*/
int cSaverDB::close()
{
	if (db) {
		sqlite3_close(db);
		db = NULL;
	}
	return 0;
}

/**
*/
int cSaverDB::exec(const char *sql)
{
	if (!db) return SQLITE_ERROR;
	sqlite3_free(zErr);
#if 1
	return (rc = sqlite3_exec(db, sql, NULL, NULL, &zErr));
#else
	sqlite3_stmt* qry = NULL;
	if ((rc = sqlite3_prepare_v2(db, sql, strlen(sql), &qry, NULL)) == SQLITE_OK) {
		rc = sqlite3_step(qry);
		if (rc==SQLITE_DONE || rc==SQLITE_ROW) {
			rc = sqlite3_finalize( qry );
		}
	}
	else {
		sqlite3_errmsg(db);
	}
	return (rc = sqlite3_errcode(db));
#endif
}

/**
*/
int cSaverDB::texec(const TCHAR *sql)
{
	if (sizeof(TCHAR) == sizeof(char)) {
		return exec((char*)sql);
	}
	if (!db) return SQLITE_ERROR;
	sqlite3_free(zErr);
	sqlite3_stmt* qry = NULL;

	if ((rc = sqlite3_prepare16_v2(db, sql, _tcslen(sql)*sizeof(TCHAR), &qry, NULL)) == SQLITE_OK) {
		rc = sqlite3_step(qry);
		if (rc==SQLITE_DONE || rc==SQLITE_ROW) {
			rc = sqlite3_finalize( qry );
		}
	}
	else {
		sqlite3_errmsg(db);
	}
	return (rc = sqlite3_errcode(db));
}

/**
*/
int cSaverDB::InsertFile(const TCHAR *filebase, const TCHAR *filename)
{
	tstring query;

	query = _T("INSERT OR IGNORE INTO images(filebase,filename) VALUES('");
	query += filebase;
	query += _T("','");
	query += filename;
	query += _T("')");

	texec( query.c_str() );

	return rc;
}

/**
*/
int cSaverDB::InsertFilePrep()
{
	if (!db) return SQLITE_ERROR;
	const TCHAR *query = _T("INSERT OR IGNORE INTO images(filebase,filename,filesize,filecreate) VALUES(?,?,?,?)");

	sqlite3_free(zErr);
	if (insFileQry) {
		rc = sqlite3_finalize( insFileQry );
		insFileQry = 0;
	}

	if ((rc = sqlite3_prepare16_v2(db, query, _tcslen(query)*sizeof(TCHAR), &insFileQry, NULL)) == SQLITE_OK) {
		exec("PRAGMA journal_mode = MEMORY");
//		exec("BEGIN TRANSACTION");
	}
	else {
		sqlite3_errmsg(db);
	}
	return (rc = sqlite3_errcode(db));
}

/**
*/
int cSaverDB::InsertFileExec(const TCHAR *filebase, const WIN32_FIND_DATA *fd)
{
	if (!db) return SQLITE_ERROR;

	sqlite_int64 fd_size = 0;
	sqlite_int64 fd_date = 0;

	fd_size = fd->nFileSizeHigh;
	fd_size <<= 32;
	fd_size |= fd->nFileSizeLow;

	fd_date = fd->ftCreationTime.dwHighDateTime;
	fd_date <<= 32;
	fd_date |= fd->ftCreationTime.dwLowDateTime;

#define WINDOWS_TICK 10000000
#define SEC_TO_UNIX_EPOCH 11644473600LL

	fd_date = (unsigned)(fd_date / WINDOWS_TICK - SEC_TO_UNIX_EPOCH);
// select id,filename,filesize,datetime(filecreate, 'unixepoch') from images;

	// need to check sizeof TCHAR for text16 usage

	if ((rc=sqlite3_bind_text16(insFileQry, 1, filebase, _tcslen(filebase)*sizeof(TCHAR), SQLITE_STATIC)) == SQLITE_OK
		&& (rc=sqlite3_bind_text16(insFileQry, 2, fd->cFileName, _tcslen(fd->cFileName)*sizeof(TCHAR), SQLITE_STATIC)) == SQLITE_OK
		&& (rc=sqlite3_bind_int64(insFileQry, 3, fd_size)) == SQLITE_OK
		&& (rc=sqlite3_bind_int64(insFileQry, 4, fd_date)) == SQLITE_OK
		) {
			// success
			rc = sqlite3_step(insFileQry);
			rc = sqlite3_reset(insFileQry);
	}
	else {
		sqlite3_errmsg(db);
	}
	return (rc = sqlite3_errcode(db));
}

/**
*/
int cSaverDB::InsertFileDone()
{
	rc = sqlite3_finalize( insFileQry );

//	exec("END TRANSACTION");
	exec("PRAGMA journal_mode = DISK");

	insFileQry = 0;
	return 0;
}

/**
*/
int cSaverDB::ConfigRead(const TCHAR *key, TCHAR *val, int val_len)
{
	if (!db) return SQLITE_ERROR;
	const TCHAR *query = _T("INSERT OR UPDATE INTO config(key,value) VALUES(?,?)");
	sqlite3_stmt* cfgQuery = {0};

	sqlite3_free(zErr);

	if ((rc = sqlite3_prepare16_v2(db, query, _tcslen(query)*sizeof(TCHAR), &cfgQuery, NULL)) == SQLITE_OK) {
//		exec("BEGIN TRANSACTION");
	}
	else {
		sqlite3_errmsg(db);
		return (rc = sqlite3_errcode(db));
	}

	if ((rc=sqlite3_bind_text16(insFileQry, 1, key, _tcslen(key)*sizeof(TCHAR), SQLITE_STATIC)) == SQLITE_OK
	&& (rc=sqlite3_bind_text16(insFileQry, 2, val, _tcslen(val)*sizeof(TCHAR), SQLITE_STATIC)) == SQLITE_OK
	) {
		// success
		rc = sqlite3_step(insFileQry);
		rc = sqlite3_reset(insFileQry);
	}
	else {
		sqlite3_errmsg(db);
	}
	return (rc = sqlite3_errcode(db));
}

/**
*/
int cSaverDB::ConfigWrite(int update, const TCHAR *key, TCHAR *val)
{
	if (!db) return SQLITE_ERROR;

	wchar_t query[256] = {0};
	wsprintf( query, L"INSERT %sINTO config (key,value) VALUES('%s','%s');",
		(update ? L"OR REPLACE " : L""), key, val );
#if 0
	wstring query = _T("INSERT OR UPDATE INTO config (key,value) values(");
	tstring query;
	if (update)
		query = _T("INSERT OR UPDATE INTO config (key,value) values(");
	else
		query = _T("INSERT INTO config (key,value) values('");
	query += key;
	query += _T("','");
	query += val;
	query += _T("')");
	return texec( query.c_str() );
#endif
	return texec( query );
}
