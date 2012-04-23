/**
*/

#ifndef _cSQLITE_H_
#define _cSQLITE_H_

#include "sqlite3.h"

class SQLite
{
public:
	SQLite();
	~SQLite();

	int open(char *db_file);
	int close();
	int exec(char *sql);

	int error()			{return rc;}
	char *error_str()	{return zErr;}

protected:
	int rc;
	char *zErr;

private:
	sqlite3* db;

	void free_error() {sqlite3_free(zErr);}
};

#endif