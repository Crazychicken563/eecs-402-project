#include <my_global.h>
#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <uuid/uuid.h>

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
    printf("No UUID found, must generate\n");

    // typedef unsigned char uuid_t[16];
    uuid_t uuid;

    // generate
    uuid_generate_time_safe(uuid);

    // unparse (to string)
    char uuid_str[37];      // ex. "1b4e28ba-2fa1-11d2-883f-0016d3cca427" + "\0"
    uuid_unparse_upper(uuid, uuid_str);

    return uuid_str;
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

    printf("%s", getUUIDForIP(con, "127.0.0.1"));
    printf("%s", getUUIDForIP(con, "127.0.0.2"));

    mysql_close(con);

    exit(0);
}
