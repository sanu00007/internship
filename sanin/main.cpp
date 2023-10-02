#include "common.h"
#include "report_generate.h"
#include "wkhtmltox/pdf.h"
#include <execinfo.h>
#include <signal.h>

const char *success_json = "{\"result\":true}";
const char *error_json = "{\"result\":false, \"error\":1}";
const char *invalid_json = "{\"result\":false, \"error\":2}";
const char *failed_json = "{\"result\":false, \"error\":5}";

event_base *g_event_base;
static int listen_fd;
char *instance = NULL;

static int setnonblock(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  if (flags < 0)
    return flags;
  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) < 0)
    return -1;

  return 0;
}

void write_response(const char *resp, struct client *client, bool close)
{
  int len = strlen(resp);
  client->write_buf = (char *)malloc(len + sizeof(int));
  memcpy(client->write_buf, &len, sizeof(int));
  memcpy(client->write_buf + sizeof(int), resp, len);
  if(!close){
    client->next_step = CLOSE_CONN; 
  } else {
    client->next_step = PROCESS_DATA; 
  }
  client->write_len = len + sizeof(int);
  event_add(client->ev_write, NULL);
}

static void close_client(struct client *client)
{
  close(client->fd);
  event_del(client->ev_read);
  event_free(client->ev_read);
  event_del(client->ev_write);
  event_free(client->ev_write);
  free(client->write_buf);
  free(client->read_buf);
  free(client->session_id);
  free(client);
}

static void handle_request(struct client *client)
{
  json_t *root = NULL;
  json_t *method = NULL;
  json_error_t error;

  do
  {

    root = json_loads(client->read_buf, 0, &error);
    if (!root)
      break;

    method = json_object_get(root, "method");
    if (!method)
      break;

    int method_id = json_integer_value(method);

    switch (method_id)
    {
      case 138:
        handle_get_html(root, client);
        break;
      case 139:
        handle_get_pdf(root, client);
        break;
      case 1001:
        write_response(success_json, client, true);
        break;
      case 1002:
        write_response(success_json, client, true);
        event_base_loopexit(g_event_base, NULL);
      default:
        write_response(error_json, client);
    }
  } while (0);

  if (root)
  {
    json_decref(root);
  }
}

static void on_write(int fd, short ev, void *arg)
{
  struct client *client = (struct client *)arg;
  int len = write(fd, client->write_buf + client->data_wrote, client->write_len - client->data_wrote);
  if (len < 0)
  {
    close_client(client);
    return;
  }
  client->data_wrote += len;
  if (client->write_len > client->data_wrote)
  {
    event_add(client->ev_write, NULL);
    return;
  }
  if (client->next_step == CLOSE_CONN)
  {
    close_client(client);
  } else {
    //reset all write buffers here
    client->data_wrote = 0;
    free(client->write_buf);
    client->write_buf = NULL;
    client->write_len = 0;
    client->data_wrote = 0;
  }
}

static void on_read(int fd, short ev, void *arg)
{
  struct client *client = (struct client *)arg;
  int len;
  int total_read = 0;
  int to_read;
  char *tmp_ptr;

  if (!client->read_len)
  {
    to_read = sizeof(len);
    tmp_ptr = (char *)&len;
    do
    {
      int data_read = read(fd, tmp_ptr + total_read, to_read - total_read);
      if (data_read <= 0)
      {
        close_client(client);
        return;
      }
      total_read += data_read;
    } while (total_read < to_read);

    if (len <= 0 || len > MAX_PACK_SIZE)
    {
      if (client->next_step == PROCESS_DATA)
      {
        client->next_step = CLOSE_CONN;
        write_response(invalid_json, client);
      }
      else
      {
        client->next_step = CLOSE_CONN;
        write_response(error_json, client);
      }
      return;
    }
    free(client->read_buf);
    client->read_buf = (char *)calloc(len + 1, 1);
    client->read_len = len;
  }
  else
  {
    to_read = client->read_len - client->data_read;
    tmp_ptr = client->read_buf + client->data_read;
    int data_read = read(fd, tmp_ptr, to_read);
    if (data_read <= 0)
    {
      close_client(client);
      return;
    }
    client->data_read += data_read;
    if (client->data_read >= client->read_len)
    {
      if (client->next_step == PROCESS_DATA)
      {
        handle_request(client);
        client->read_len = 0;
        client->data_read = 0;
      }
      else
      {
        ERROR_LOG("Invalid state.. from read()");
        client->next_step = CLOSE_CONN;
        write_response(invalid_json, client);
      }
    }
  }
}

static void on_accept(int fd, short ev, void *arg)
{
  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  struct client *client;

  client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd == -1)
  {
    ERROR_LOG("accept failed");
    return;
  }

  if (setnonblock(client_fd) < 0)
    ERROR_LOG("failed to set client socket non-blocking");

  client = (struct client *)calloc(1, sizeof(*client));
  if (client == NULL)
  {
    ERROR_LOG("malloc failed");
    return;
  }

  client->ev_read = event_new(g_event_base, client_fd, EV_READ | EV_PERSIST, on_read, client);
  client->ev_write = event_new(g_event_base, client_fd, EV_WRITE, on_write, client);

  client->fd = client_fd;

  event_add(client->ev_read, NULL);
}

static int setup_listen_socket(int port)
{
  struct sockaddr_in listen_addr;
  int reuseaddr_on = 1;
  struct event *ev_accept;

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0)
  {
    ERROR_LOG("socket() failed");
    return -1;
  }
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on,
                 sizeof(reuseaddr_on)) == -1)
  {
    ERROR_LOG("setsockopt failed");
    return -2;
  }
  memset(&listen_addr, 0, sizeof(listen_addr));
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  listen_addr.sin_port = htons(port);

  if (bind(listen_fd, (struct sockaddr *)&listen_addr,
           sizeof(listen_addr)) < 0)
  {
    ERROR_LOG("bind failed");
    return -3;
  }

  if (listen(listen_fd, 5) < 0)
  {
    ERROR_LOG("listen failed");
  }

  if (setnonblock(listen_fd) < 0)
  {
    ERROR_LOG("failed to set server socket to non-blocking");
    return -4;
  }

  ev_accept = event_new(g_event_base, listen_fd, EV_READ | EV_PERSIST, on_accept, NULL);
  event_add(ev_accept, NULL);
  return 0;
}

static void syntax(const char *app_name)
{
  fprintf(stdout, "Syntax: %s -p <port> \n", app_name);
}

static void crash_handler(int sig) {
  void *array[10];
  size_t size;
  size = backtrace(array, 10);
  char **strings;

  strings = backtrace_symbols(array, size);
  if (strings != NULL) {
    for (unsigned int j = 0; j < size; j++){
      ERROR_LOG("%s\n", strings[j]);
    }
  }
  free(strings);
  exit(-1);
}

int main(int argc, char **argv)
{
  int port = -1;
  int c;
  int ret;

  if (argc < 2)
  {
    syntax(argv[0]);
    return 1;
  }
  opterr = 0;
  while ((c = getopt(argc, argv, "p:")) != -1)
  {
    switch (c)
    {
      case 'p':
        port = atoi(optarg);
        break;
      return 1;
    }
  }
  if (argc < 1)
  {
    syntax(argv[0]);
    return 1;
  }
  g_event_base = event_base_new();

  init_log(argv[0]);
  do
  {
    ret = setup_listen_socket(port);
    if (ret){
      break;
    }
  } while (0);
  if(wkhtmltopdf_init(0) == 0){
    ERROR_LOG("Some issue in initializing wkhtmltopdf.. exiting\n");
    close_log();
    ret = -1;
  }
  INFO_LOG("%s starting.. all seems well\n",argv[0]);
  set_log_level(1);
  signal(SIGSEGV, crash_handler);
  //if (!ret)
  //  event_base_dispatch(g_event_base);
  if(getchar() == 'a'){
          handle_get_html(NULL, NULL);
  }
  return 0;
}
