#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <ctype.h>
#include <sys/time.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <map>
#include <list>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <jansson.h>
#include <math.h>
#include "logger.h"
#define MAX_PACK_SIZE 8196000

enum conn_action
{
    PROCESS_DATA,
    READ_DATA,
    CLOSE_CONN
};

enum type_t 
{
    INTEGER,
    BOOLEAN,
    STRING,
    NONE
};

struct client
{
    client() : ev_read(NULL),
               ev_write(NULL),
               write_buf(NULL),
               read_buf(NULL),
               session_id(NULL){};

    struct event *ev_read;
    struct event *ev_write;
    char *write_buf;
    char *read_buf;
    char *session_id;
    int write_len;
    int data_wrote;
    int read_len;
    int data_read;
    int cursor;
    int fd;
    int user_id;
    int profile_id;
    conn_action next_step;
};
extern const char *success_json;
extern const char *error_json;
extern const char *invalid_json;
extern const char *failed_json;

void write_response(const char *resp, struct client *client, bool close = false);

#endif //_COMMON_H_
