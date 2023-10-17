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

    //tcp_server_start_listen_thread(server_handle);
    

	return 0;
}