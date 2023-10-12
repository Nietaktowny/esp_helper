#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define TCP_C_PORT                        27015          ///< TCP port on which listen to.
#define TCP_C_KEEPALIVE_IDLE              300            ///< TCP keep-alive idle time(s).
#define TCP_C_KEEPALIVE_INTERVAL          60              ///< TCP keep-alive interval time(s).
#define TCP_C_KEEPALIVE_COUNT             3              ///< TCP keep-alive packet retry send counts.
#define TCP_C_KEEPALIVE                   1
#define TCP_C_RECEIVE_BUFLEN              512            ///< Size of buffer to receive data.
#define TCP_C_SEND_BUFLEN                 512            ///< Size of buffer from which send data.
#define TCP_C_ADDR_FAMILY                 AF_INET        ///< Use Internet Protocol v4 addresses.
#define TCP_C_IP_PROTOCOL                 IPPROTO_IP     ///< Use automatic choice depending on socket type and family.
#define TCP_C_BACKLOG_NUM                 4              ///< Specifies the maximum number of queued connections.
#define TCP_C_HANDLER_WAIT_TIME           4000
#define TCP_C_CHECKSUM_SIZE               5


#define TCP_C_ERR_BASE                  0x000F                      ///< Error base used to indicate where tcp_controller errors numbers start.
#define TCP_C_ERR_NULL_MUTEX            TCP_C_ERR_BASE + 0x01      ///< 
#define TCP_ERR_WRONG_CHECKSUM          TCP_C_ERR_BASE + 0x02
#define TCP_C_ERR_UNABLE_TO_GET_MUTEX   TCP_C_ERR_BASE + 0x03



/**
 * @brief Macro used to log errno description to log.
 * 
 */
#define TCP_C_LOG_ERRNO(err) ESP_LOGE(TAG, "file:%s line:%d\n error description:\n%s", __FILE__, __LINE__, strerror(err));

/**
 * @brief Prepare tcp controller to handle transmissions.
 * 
 * @param event_group EventGroupHandle_t Event group with which communicate with tcp controller.
 * @param task_priority Prority of tcp controller task.
 * @return int Error code.
 */
int tcp_c_start_tcp_server(EventGroupHandle_t event_group, uint16_t task_priority);

/**
 * @brief Server loop in which all connections are handled.
 * 
 */
void tcp_c_server_loop(void* args);

/**
 * @brief Receive data into passed buffer
 * 
 * @param rx_buffer Buffer in which save data.
 * @return int Error code.
 */
int tcp_c_receive(char rx_buffer[]);

/**
 * @brief Send data from passed buffer.
 * 
 * @param tx_buffer Buffer from which send data.
 * @param buflen Buffer lenght.
 * @return int Error code.
 */
int tcp_c_send(const char tx_buffer[], int buflen);

/**
 * @brief Function used to deinit tcp controller.
 * 
 * @return int error code
 */
int tcp_c_deinit(void);

int tcp_c_close_socket(void);

int tcp_c_start_listen_task(uint16_t task_priority);

int tcp_c_start_handler_task(uint16_t task_priority);