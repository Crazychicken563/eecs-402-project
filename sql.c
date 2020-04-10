#include <my_global.h>
#include <mysql.h>
#include <string.h>

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

char* getUUIDForIP(MYSQL* con, char* ipAddr) {
    char q[1000] = "SELECT * FROM ipmap WHERE ip='";
    strcat(q, ipAddr);
    strcat(q, "'");
    printf("Query: %s\n", q);

    if (mysql_query(con, q))
    {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL) 
    {
        finish_with_error(con);
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) 
    {
        mysql_free_result(result);
	return row[1];
    }

    mysql_free_result(result);
    printf("No UUID found\n");
    return "GENERATE_ME";
}

int main(int argc, char **argv)
{
    printf("MySQL client version: %s\n", mysql_get_client_info());
    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }  

    if (mysql_real_connect(
        con, "localhost", "root", "root", 
        "eecs402proj", 0, NULL, 0
    ) == NULL)
    {
        finish_with_error(con);
    }

    getUUIDForIP(con, "127.0.0.1");
    getUUIDForIP(con, "127.0.0.2");

    mysql_close(con);

    exit(0);
}
