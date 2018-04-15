#pragma once
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

class Database_Conn
{
public:
	Database_Conn(char* db_name);
	~Database_Conn();
	void SaveGameRecord(int winner, char* started, char* finished);


private:
	static int callback(void *NotUsed, int argc, char **argv, char **szColName);
	//Open Database
	void DB_Open(char*db_name);
	//Close Database
	void DB_Close();
	//Execute a SQLite statement 
	void SQL_Statement(const char* stmt);
	sqlite3 *db;
	char *zErrMsg = 0;
	char *sql;
	int rc;
};

