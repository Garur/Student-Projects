#include "database_conn.h"



Database_Conn::Database_Conn(char* db_name)
{
	DB_Open(db_name);	
}


Database_Conn::~Database_Conn()
{
	DB_Close();
}

void Database_Conn::SaveGameRecord(int winner, char * started, char * finished)
{	
	sqlite3_stmt *stmt;
	// Create SQL statement 
	sql = "INSERT INTO Games (Winner, TimeStarted, TimeFinished)" \
		"VALUES(?, ?, ?);";

	if (sqlite3_prepare(db,	sql, -1,&stmt, 0) != SQLITE_OK) {
		printf("\nCould not prepare statement.");
		return;
	}
	printf("\nThe statement has %d wildcards\n", sqlite3_bind_parameter_count(stmt));

	if (sqlite3_bind_int(stmt,1, winner)
		!= SQLITE_OK) {
		printf("\nCould not bind double.\n");
		return;
	}
	if (sqlite3_bind_text(stmt,2, started, strlen(started), SQLITE_STATIC)!= SQLITE_OK) {
		printf("\nCould not bind text.\n");
		return;
	}

	if (sqlite3_bind_text(stmt, 3, finished, strlen(finished), SQLITE_STATIC) != SQLITE_OK) {
		printf("\nCould not bind int.\n");
		return;
	}

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		printf("\nCould not step (execute) stmt.\n");
		return;
	}

	sqlite3_reset(stmt);	
	
	
	
	/*
	// Create SQL statement 
	sql = "INSERT INTO Games (Winner, TimeStarted, TimeFinished)" \
		"VALUES('1', 'yep', 'today');";

	// Execute SQL statement
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Table created successfully\n");
	}*/
}

// This is the callback function to display the select data in the table
int Database_Conn::callback(void * NotUsed, int argc, char ** argv, char ** szColName)
{
	for (int i = 0; i < argc; i++)
	{
		std::cout << szColName[i] << " = " << argv[i] << std::endl;
	}

	std::cout << "\n";

	return 0;
}

void Database_Conn::DB_Open(char* db_name)
{
	int db_result;
	// open database
	if (db_result = sqlite3_open(db_name, &db) == SQLITE_OK)
	{
		std::cout << "Open database successfully\n";
		//Path to the database
		//printf("Dabatase Path is: %s\n", sqlite3_db_filename(db, "main"));

	}
	else {
		std::cout << "Failed to open database " << sqlite3_errmsg(db) << ", code:" << db_result <<'\n';
	}
}

void Database_Conn::DB_Close()
{
	// close database
	if (db)
	{
		sqlite3_close(db);
	}
}

void Database_Conn::SQL_Statement(const char* stmt) {
	char *errmsg;
	int   result;

	result = sqlite3_exec(db, stmt, 0, 0, &errmsg);

	if (result != SQLITE_OK) {
		printf("Error in statement: %s [%s].\n", stmt, errmsg);
	}
}
