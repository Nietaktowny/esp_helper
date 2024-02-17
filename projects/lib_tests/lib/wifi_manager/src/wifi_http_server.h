


struct wifi_s_handle_obj;
typedef struct wifi_s_handle_obj* wifi_s_handle_t;


struct wifi_s_urls_obj {
    char* http_root_url;
    char* http_redirect_url;
    char* http_js_url;
    char* http_css_url;
    char* http_connect_url;
    char* http_ap_url;
    char* http_state_url;
};
typedef struct wifi_s_urls_obj* wifi_s_urls_t;


int wifi_manager_server_init(wifi_s_handle_t* out_handle);
