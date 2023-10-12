#include "tcp_controller.h"
#include "tcp_c_events.h"
#include "err_controller.h"
#include "wifi_controller.h"

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "freertos/task.h"
#include "inttypes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


static const char* TAG = "tcp_controller";                              ///< Tcp controller log tag.


static void tcp_c_prepare_sock_addr(struct sockaddr_storage *dest_addr);
static int tcp_c_prepare_listen_socket(void);
static int tcp_c_bind_socket(int socket, struct sockaddr_storage *dest_addr);
static int tcp_c_listen_on_socket(int socket);
static int tcp_c_accept_socket(int socket);
static void tcp_c_handler_task(void* args);
static void tcp_c_listen_task(void* args);
//static int tcp_c_close_socket(int socket);

static EventGroupHandle_t tcp_event_group;                               ///< Freertos event group used to communicate wit tcp controller.
static TaskHandle_t tcp_listen_task_handle;                              ///< Freertos task handle of tcp controller loop task.
static TaskHandle_t tcp_handler_task_handle;                                 
static SemaphoreHandle_t tcp_send_mutex;
static SemaphoreHandle_t tcp_receive_mutex;
static SemaphoreHandle_t tcp_conn_socket_mutex;

static int listen_socket = 0;                                            ///< Socket to listen on.
static int connected_socket = 0;                                         ///< Socket on which accepted sockets are stored.

/**
 * @brief Prepare destination address.
 * 
 * @param dest_addr Destination address where to store result.
 */
static void tcp_c_prepare_sock_addr(struct sockaddr_storage *dest_addr) {
    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(TCP_C_PORT);
}

/**
 * @brief Create socket to listen on.
 * 
 * @return int Socket descriptor.
 */
static int tcp_c_prepare_listen_socket(void) {
    int listen_sock = socket(TCP_C_ADDR_FAMILY, SOCK_STREAM, TCP_C_IP_PROTOCOL);
    volatile err_c_t err = ERR_C_OK;
    Try {
        if (listen_sock < 0) {
            ERR_C_SET_AND_THROW_ERR(err, errno);
        }
        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    } Catch(err) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", err);
        TCP_C_LOG_ERRNO(err)
        Throw(err);
    }

    return listen_sock;
}

/**
 * @brief Bin socket to specified destination address.
 * 
 * @param socket Socket descriptor. 
 * @param dest_addr Destination address.
 * @return int error code
 */
static int tcp_c_bind_socket(int socket, struct sockaddr_storage *dest_addr) {
    volatile err_c_t err = ERR_C_OK;
    Try {
        err = bind(socket, (struct sockaddr *)dest_addr, sizeof(struct sockaddr_storage));
        ERR_C_CHECK_AND_THROW_ERR(err);
    } Catch(err) {
        err = errno;
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", err);
        TCP_C_LOG_ERRNO(err)
    }

    return err;
}

/**
 * @brief Listen on passed socket.
 * 
 * @param socket Socket descriptor.
 * @return int Error code.
 */
static int tcp_c_listen_on_socket(int socket) {
    volatile err_c_t err = ERR_C_OK;
    Try {
        err = listen(socket, TCP_C_BACKLOG_NUM);
        ERR_C_CHECK_AND_THROW_ERR(err);
    } Catch(err) {
        err = errno;
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", err);
        TCP_C_LOG_ERRNO(err)
    }

    return err;
}

/**
 * @brief Accept connection on socket.
 * 
 * @param socket Socket on which listening was performed.
 * @return int Socket on which connection was accepted.
 */
static int tcp_c_accept_socket(int socket) {
    volatile err_c_t err = ERR_C_OK;
    volatile int sock;
    char addr_str[128];
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    socklen_t addr_len = sizeof(source_addr);
    int keep_alive = TCP_C_KEEPALIVE;
    int keep_idle = TCP_C_KEEPALIVE_IDLE;
    int keep_interval = TCP_C_KEEPALIVE_INTERVAL;
    int keep_count = TCP_C_KEEPALIVE_COUNT;

    Try {
        sock = accept(socket, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ERR_C_SET_AND_THROW_ERR(err, errno);
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(int));
        
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }

        ESP_LOGD(TAG, "Socket accepted ip address: %s", addr_str);
    } Catch (err) {
        ESP_LOGE(TAG, "Unable to accept connection: errno %d", err);
        TCP_C_LOG_ERRNO(err)
    }

    return sock;
}

/**
 * @brief Close the socket.
 * 
 * @param socket Socket descriptor.
 * @return int 
 */
int tcp_c_close_socket(void) {
    //Close the connection.
    volatile err_c_t err = ERR_C_OK;
    Try {
        ERR_C_CHECK_AND_THROW_ERR(close(connected_socket));
        connected_socket = 0;
    } Catch(err) {
        err = errno;
        ESP_LOGE(TAG, "Error during closing socket: %d", err);
        TCP_C_LOG_ERRNO(err);
    }
    return err;
}

int tcp_c_deinit(void) {
    vEventGroupDelete(tcp_event_group);
    tcp_c_close_socket();
    vTaskDelete(tcp_listen_task_handle);
    return ERR_C_OK;
} 

int tcp_c_send(const char tx_buffer[], int buflen) {
    volatile err_c_t err = ERR_C_OK;
    volatile int to_write = buflen;
    assert(tx_buffer);
    Try {
        if(tcp_send_mutex == NULL) {
            ERR_C_SET_AND_THROW_ERR(err, TCP_C_ERR_NULL_MUTEX);
        }
        if(xSemaphoreTake(tcp_send_mutex, pdTICKS_TO_MS(TCP_C_HANDLER_WAIT_TIME)) == pdFALSE) {
            ERR_C_SET_AND_THROW_ERR(err, TCP_C_ERR_UNABLE_TO_GET_MUTEX);
        }
        while (to_write > 0) {
        int written = send(connected_socket, tx_buffer+(buflen-to_write), to_write, 0);
        ESP_LOGD(TAG, "sended: %s", tx_buffer);
        if (written < 0) {
            ERR_C_SET_AND_THROW_ERR(err, errno);
        }
        to_write -= written;
        }
        xSemaphoreGive(tcp_send_mutex);
        xEventGroupSetBits(tcp_event_group, TCP_C_SENDED_DATA_BIT);
    } Catch(err) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        TCP_C_LOG_ERRNO(err)
        tcp_c_close_socket();
        xSemaphoreGive(tcp_send_mutex);
        xEventGroupSetBits(tcp_event_group, TCP_C_FINISHED_TRANSMISSION); //there was error, finish current transmission
    }
    return err;
}

int tcp_c_receive(char rx_buffer[]) {
    volatile err_c_t err = ERR_C_OK;
    int len;
    assert(rx_buffer);
    memset(rx_buffer, 0, TCP_C_RECEIVE_BUFLEN);
    Try {
        if(tcp_receive_mutex == NULL) {
            ERR_C_SET_AND_THROW_ERR(err, TCP_C_ERR_NULL_MUTEX);
        }
        if(xSemaphoreTake(tcp_receive_mutex, pdTICKS_TO_MS(TCP_C_HANDLER_WAIT_TIME)) == pdFALSE) {
            ERR_C_SET_AND_THROW_ERR(err, TCP_C_ERR_UNABLE_TO_GET_MUTEX);
        }

        do {
            len = recv(connected_socket, rx_buffer, TCP_C_RECEIVE_BUFLEN-1, 0);
            if(len < 0) {
                ERR_C_SET_AND_THROW_ERR(err, errno);
            } else if (len == 0) {
                ESP_LOGD(TAG, "Connection closed.");
            } else {
                rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
                ESP_LOGD(TAG, "Received %d bytes.", len);
                ESP_LOGD(TAG, "Received data: %s", rx_buffer);
                break;
            }
        } while (len > 0);

        xSemaphoreGive(tcp_receive_mutex);

    } Catch(err) {
        err = errno;
        ESP_LOGE(TAG, "Error during receiving data, errno: %d", err);
        TCP_C_LOG_ERRNO(err)
        xSemaphoreGive(tcp_receive_mutex);
        if(err == 104) {
            tcp_c_close_socket();
            vTaskDelete(tcp_listen_task_handle);
            tcp_c_start_listen_task(3);
        }
    }
    xEventGroupSetBits(tcp_event_group, TCP_C_RECEIVED_DATA_BIT);
    return err;
}

int tcp_c_start_listen_task(uint16_t task_priority) {
    volatile err_c_t err = ERR_C_OK;
    ESP_LOGI(TAG, "Starting listen task");
    Try {
        if(tcp_listen_task_handle == NULL) {
            xTaskCreate(tcp_c_listen_task, "tcp_listen_task", 4096, NULL, task_priority, &tcp_listen_task_handle);
        } else {
            ESP_LOGE(TAG, "Listen task already running");
            ERR_C_SET_AND_THROW_ERR(err, 1);
        }
    } Catch(err) {
        ESP_LOGE(TAG, "Unable to create listen task");
    }
    return err;
}

int tcp_c_start_handler_task(uint16_t task_priority) {
    volatile err_c_t err = ERR_C_OK;
    ESP_LOGI(TAG, "Starting handler task");
    Try {
        if(tcp_handler_task_handle == NULL) {
            xTaskCreate(tcp_c_handler_task, "tcp_handler_task", 4096, NULL, task_priority, &tcp_handler_task_handle);
        } else {
            ESP_LOGE(TAG, "Handler task already running");
            ERR_C_SET_AND_THROW_ERR(err, 1);
        }
    } Catch(err) {
        ESP_LOGE(TAG, "Unable to create handler task");
    }
    return err;
}

static void tcp_c_handler_task(void* args) {
    volatile err_c_t err = ERR_C_OK;
    char receive_checksum[TCP_C_RECEIVE_BUFLEN];
    Try {
        while (1) {
            tcp_c_receive(receive_checksum);
            ESP_LOGI(TAG, "Received: %s", receive_checksum);
            tcp_c_send(receive_checksum, strlen(receive_checksum));
            ESP_LOGI(TAG, "Sended: %s", receive_checksum);
            //tcp_c_close_socket();
        }
        //xTaskCreate(tcp_c_handler_task, "tcp_handler_task", 4096, NULL, 3, &tcp_handler_task_handle);
        //tcp_listen_task_handle = NULL;
        //vTaskDelete(NULL);
    } Catch(err) {
        ESP_LOGE(TAG, "error while handling connection: %d", err);
    }
}

static void tcp_c_listen_task(void* args) {
    volatile err_c_t err = ERR_C_OK;
    char receive_checksum[TCP_C_RECEIVE_BUFLEN];

    struct sockaddr_storage dest_addr;
    Try {

        tcp_c_prepare_sock_addr(&dest_addr);
        ESP_LOGI(TAG, "Address prepared.");
        listen_socket = tcp_c_prepare_listen_socket();
        ESP_LOGI(TAG, "Socket created");
        ERR_C_CHECK_AND_THROW_ERR(tcp_c_bind_socket(listen_socket, &dest_addr));
        ESP_LOGI(TAG, "Socket bound, port: %d", TCP_C_PORT);
        ERR_C_CHECK_AND_THROW_ERR(tcp_c_listen_on_socket(listen_socket));
        ESP_LOGI(TAG, "Socket listening.");
        //Accept new connection
        connected_socket = tcp_c_accept_socket(listen_socket);
        //Inform that new connection was accepted.
        ESP_LOGI(TAG, "Socket accepted.");
        while (1) {
            tcp_c_receive(receive_checksum);
            ESP_LOGI(TAG, "Received: %s", receive_checksum);
            tcp_c_send(receive_checksum, TCP_C_RECEIVE_BUFLEN);
            ESP_LOGI(TAG, "Sended: %s", receive_checksum);
            //tcp_c_start_handler_task(3);
            //vTaskDelay(pdMS_TO_TICKS(4000));
        }
    } Catch(err) {
        ESP_LOGE(TAG, "error while listening: %d", err);
    }
}

void tcp_c_server_loop(void* args)
{
    volatile err_c_t err = ERR_C_OK;
    Try {
        while (1) {
            ESP_LOGD(TAG, "Socket listening.");
            //Accept new connection
            connected_socket = tcp_c_accept_socket(listen_socket);
            //Inform that new connection was accepted.
            ESP_LOGD(TAG, "Socket accepted.");
            xEventGroupSetBits(tcp_event_group, TCP_C_ACCEPTED_SOCKET_BIT);
            //Wait till transmission is finished.
            xEventGroupWaitBits(tcp_event_group, TCP_C_FINISHED_TRANSMISSION | TCP_C_SENDED_DATA_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(3500));
            //Close the connection.
            tcp_c_close_socket();
            //Inform that connection was closed.
            xEventGroupSetBits(tcp_event_group, TCP_C_SOCKET_CLOSED_BIT);
            ESP_LOGD(TAG, "Socket closed.");
        }
    } Catch(err) {
        ESP_LOGE(TAG, "error: %d", err);
    }
}

int tcp_c_start_tcp_server(EventGroupHandle_t event_group, uint16_t task_priority) {
    volatile err_c_t err = ERR_C_OK;
    tcp_event_group = event_group;
    tcp_receive_mutex = xSemaphoreCreateMutex();
    tcp_send_mutex = xSemaphoreCreateMutex();
    tcp_conn_socket_mutex = xSemaphoreCreateMutex();
    Try {
        //xTaskCreate(tcp_c_server_loop, "loop", 4096, NULL, 3, NULL);
        //xTaskCreate(tcp_c_listen_task, "tcp_listen_task", 4096, NULL, task_priority, &tcp_listen_task_handle);
        tcp_c_start_listen_task(3);
        //xTaskCreate(tcp_c_handler_task, "tcp_handler_task", 4096, NULL, task_priority, &tcp_handler_task_handle);
    } Catch(err) {
        ESP_LOGE(TAG, "Error during starting tcp server: %d", err);
        TCP_C_LOG_ERRNO(err);
    }

    return err;
}