#include "tcp_client.h"
#include "tcp_client_cmds.h"
#include "error.h"
#include "log.h"
#include "assert.h"
#include <unistd.h>

SET_LOG_LEVEL(LOG_LEVEL_DEBUG);

static int get_user_input(tcp_client_data_t* data);

tcp_client_handle_t* client_handle = NULL;

tcp_client_data_t data = {
    .data = "data from client",
    .data_len = 16,
    .sync = 0,
    .cmd_base = 49,
    .cmd = 255,
};

int main(int argc, char **argv) {
    int err = 0;

    if(argc < 2) {
        LOG_WARN("no arguments, connecting to localhost on default port 27015");
        err = tcp_client_init(&client_handle, "127.0.0.1", 27015);
    } else {
        int port = 0;
        if(sscanf(argv[2], "%d", &port) == 1) {
            LOG_INFO("connecting to address:%s:%d", argv[1], port);
            err = tcp_client_init(&client_handle, argv[1], port);
        } else {
            LOG_ERROR("entered wrong port number: %s", argv[2]);
            exit(EXIT_FAILURE);
        }
    }
    
    assert(client_handle);
    if(err != 0) {
        LOG_FATAL("error in tcp_client: %d,\n%s", err, err_to_name(err));
        exit(EXIT_FAILURE);
    }

    tcp_client_sync_commands(client_handle, 10);

    while(1) {

        get_user_input(&data);

        err = tcp_client_send_data(client_handle, &data);
        if(err != 0) {
            break;
        }

        LOG_INFO("sent: %s", data.data);
        
        tcp_client_recv_data(client_handle, &data);
        if(err != 0) {
            break;
        }
        LOG_INFO("received:%s", data.data);
        
        sleep(3);
    }

    tcp_client_close_connection(client_handle);
	return 0;
}

static int get_user_input(tcp_client_data_t* data) {
    int i = 0;
    char line[256];
    char* cmd_base = (char*) malloc(sizeof(char)*255);
    char* cmd = (char*) malloc(sizeof(char)*255);
    char* args = (char*) malloc(sizeof(char)*255);
    if (fgets(line, sizeof(line), stdin)) {
        i = sscanf(&(line[0]), "%s%s%s", cmd_base, cmd, args);
        if(i == 3) {
            memset(&(data->data[0]), 0, sizeof(data->data));
            LOG_DEBUG("readed cmd base: %s", cmd_base);
            tcp_client_hash_string(cmd_base);
            LOG_DEBUG("readed cmd: %s", cmd);
            tcp_client_hash_string(cmd);
            LOG_DEBUG("readed args: %s", args);
            strncpy(&(data->data[0]), cmd_base, strlen(cmd_base));
            strncat(&(data->data[0]), cmd, strlen(cmd));
            strncat(&(data->data[0]), args, strlen(args));
            free(cmd_base);
            free(cmd);
            free(args);
        } else if (i == 2)
        {
            memset(&(data->data[0]), 0, sizeof(data->data));
            LOG_DEBUG("readed cmd base: %s", cmd_base);
            tcp_client_hash_string(cmd_base);
            LOG_DEBUG("readed cmd: %s", cmd);
            tcp_client_hash_string(cmd);
            strncpy(&(data->data[0]), cmd_base, strlen(cmd_base));
            strncat(&(data->data[0]), cmd, strlen(cmd));
            free(cmd_base);
            free(cmd);
        } else if (i == 1) {
            memset(&(data->data[0]), 0, sizeof(data->data));
            LOG_DEBUG("readed cmd base: %s", cmd_base);
            tcp_client_hash_string(cmd_base);
            strncpy(&(data->data[0]), cmd_base, strlen(cmd_base));
            free(cmd_base);
        } else {
            LOG_WARN("no user input");
        }  
    }
    return i;
}