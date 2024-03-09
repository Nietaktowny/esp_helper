

struct http_s_handle_obj;
typedef struct http_s_handle_obj* http_s_handle_t;

typedef enum {
    HTTP_S_GET,
    HTTP_S_POST,
    HTTP_S_HEAD,
    HTTP_S_PUT,
    HTTP_S_DELETE,
    HTTP_S_CONNECT,
    HTTP_S_OPTIONS,
    HTTP_S_TRACE,
    HTTP_S_PATCH,
} http_server_method_t;

struct http_s_uri_t {
    char uri[64];
    http_server_method_t method;
};