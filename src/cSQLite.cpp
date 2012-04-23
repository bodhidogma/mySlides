/**
*/

#include "stdafx.h"
#include "cSQLite.h"

/**
*/
SQLite::SQLite()
{
	db = NULL;
	zErr = NULL;
	rc = SQLITE_OK;
}

/**
*/
SQLite::~SQLite()
{
	close();
}

/**
*/
int SQLite::open(char *db_file)
{
	rc = sqlite3_open(db_file, &db);
	if (rc) {
		close();
	}
	return rc;
}

/**
*/
int SQLite::close()
{
	if (db) {
		sqlite3_close(db);
		db = NULL;
	}
	return 0;
}

/**
*/
int SQLite::exec(char *sql)
{
	if (!db) return SQLITE_ERROR;

	sqlite3_free(zErr);
	rc = sqlite3_exec(db, sql, NULL, NULL, &zErr);
	return rc;
}


