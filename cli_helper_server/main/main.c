#include "tcp_server.h"
#include "tcp_server_internal.h"
#include "error.h"
#include "log.h"
#include "assert.h"

SET_LOG_LEVEL(LOG_LEVEL_DEBUG);

tcp_server_handle_t* server_handle = NULL;

void* function (void* args) {
    LOG_DEBUG("test function");
    return NULL;
}

int main(int argc, char **argv) {
    
    int err = tcp_server_init(&server_handle, 27015, "127.0.0.1");
    assert(server_handle);
    if(err != 0) {
        LOG_FATAL("error in tcp_server: %d,\n%s", err, strerror(err));
    }
    
    char* cmd_base = "cmd_base";
    char* cmd = "cmd";

    tcp_server_cmd_t* founded_node = NULL;
    tcp_server_register_cmd(&server_handle, function, "asdsad", "asdasad");
    tcp_server_register_cmd(&server_handle, function, "|adsasd", "sadqw12qww");
    tcp_server_register_cmd(&server_handle, function, cmd_base, cmd);
    founded_node = tcp_server_find_string_cmd(&server_handle, cmd_base, cmd);

    //tcp_server_start_listen_thread(server_handle);
    

	return 0;
}