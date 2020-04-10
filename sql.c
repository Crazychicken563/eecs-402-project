#include <my_global.h>
#include <mysql.h>
#include <stdio.h>
#include <string.h>

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

static char *rand_string(char *str, size_t size)
{
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

char* rand_string_alloc(size_t size)
{
     char *s = malloc(size + 1);
     if (s) {
         rand_string(s, size);
     }
     return s;
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

    char *id = malloc(500);
    strcat(id, rand_string_alloc(9));
    strcat(id, "-");
    strcat(id, rand_string_alloc(5));
    strcat(id, "-");
    strcat(id, rand_string_alloc(5));
    strcat(id, "-");
    strcat(id, rand_string_alloc(5));
    strcat(id, "-");
    strcat(id, rand_string_alloc(13));

    return id;
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

    printf("%s\n", getUUIDForIP(con, "127.0.0.1"));
    printf("%s\n", getUUIDForIP(con, "127.0.0.2"));

    mysql_close(con);

    exit(0);
}
