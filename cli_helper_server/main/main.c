#include "tcp_server.h"
#include "tcp_server_internal.h"
#include "error.h"
#include "log.h"
#include "assert.h"

SET_LOG_LEVEL(LOG_LEVEL_DEBUG);

tcp_server_handle_t* server_handle = NULL;

int main(int argc, char **argv) {
    
    int err = tcp_server_init(&server_handle, 27015, "127.0.0.1");
    assert(server_handle);
    if(err != 0) {
        LOG_FATAL("error in tcp_server: %d,\n%s", err, strerror(err));
    }

    
    err = tcp_server_start_listen_thread(server_handle);
    if(err != 0) {
        LOG_FATAL("error when starting listen thread: %d,\n%s", err, err_to_name(err));
    }

	return 0;
}