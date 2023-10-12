#include "cli_manager.h"
#include "err_controller.h"
#include "wifi_controller.h"
#include "tcp_controller.h"

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "freertos/task.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"

#define SSID_SOL "OstNet952235plus"
#define PASSWORD_SOL "Solonka106"

static cli_cmd_t cli_manager_read_cmd(const char recbuf[]);

static const char* TAG = "cli_manager";                         ///< Cli manager log tag.
static cli_cmd_config_t cmd_configs[CLI_CMD_MAX_COMMANDS];      ///< Struct array containing command configs.
static uint16_t last_cmd = 0;                                   ///< Number storing last stored command.

static EventGroupHandle_t tcp_event_group;                      ///< Freertos event group used to communicate with tcp controller.
static char receive_buffer[TCP_C_RECEIVE_BUFLEN];               ///< Receive buffer.
static char send_buffer[TCP_C_SEND_BUFLEN];                     ///< Send buffer.

/**
 * @brief Command to do when no other command is found to do.
 * 
 */
static void cli_manager_no_command(void) {
    ESP_LOGI(TAG, "No command found");
    cli_manager_log("No command found");
    cli_manager_end_transmission();
}

/**
 * @brief Function used to read commands from receive buffer.
 * 
 * @param recbuf Buffer from which read commands.
 * @return cli_cmd_t Parsed structure containing both commands and args.
 */
static cli_cmd_t cli_manager_read_cmd(const char recbuf[]) {
    assert(recbuf);
    cli_cmd_t cmd;
    memcpy(&cmd.cmd_num, &recbuf[0], CLI_CMD_INTS_NUMBER);
    memcpy(&cmd.args, &recbuf[CLI_CMD_INTS_NUMBER], CLI_CMD_ARGS_NUMBER);
    ESP_LOGD(TAG, "Received commands: %d %d", cmd.cmd_num[0], cmd.cmd_num[1]);
    ESP_LOGD(TAG, "Received arguments: %s", cmd.args);
    return cmd;
}

/**
 * @brief Inform tcp server that transmission can end.
 * @return int error code
 */
int cli_manager_end_transmission(void) {
    //Inform tcp controller that data is ready to send
    ESP_LOGD(TAG, "Data ready to send.");
    xEventGroupSetBits(tcp_event_group, TCP_C_DATA_READY_TO_SEND);
    return 0;
}

/**
 * @brief Log statement into the send buffer.
 * 
 * @note This function clears the buffer before writting to it.
 * 
 * @param log Log statement.
 * @return int error code.
 */
int cli_manager_log(char* log) {
    cli_manager_write_to_send_buf(log, (uint16_t) strlen(log));
    return 0;
}

/**
 * @brief Concatenate log statement into the send buffer.
 * 
 * @warning This function doesn't clear buffer before writting.
 * 
 * @note This function inserts newspace after each log statement.
 * 
 * @param log Log statement to concatenate.
 * @return int error function.
 */
int cli_manager_add_log(char* log) {
    cli_manager_add_to_send_buf("\n");
    cli_manager_add_to_send_buf(log);
    return 0;
}

/**
 * @brief Register new command for cli manager.
 * 
 * @param cmd1 Character specifying first part of command.
 * @param cmd2 Character specifying second part of command.
 * @param cmd_fun Callback function to do when both commands are met.
 * @return int error code
 */
int cli_manager_register_command(char cmd1, char cmd2, void (*cmd_fun)(void* args)) {
    err_c_t err = ERR_C_OK;

    assert(cmd_fun);
    if(last_cmd < CLI_CMD_MAX_COMMANDS) {
        cli_cmd_config_t new_cmd;
        new_cmd.cmd_fun = cmd_fun;
        new_cmd.cmd_num[0] = cmd1;
        new_cmd.cmd_num[1] = cmd2;
        cmd_configs[last_cmd] = new_cmd;
        last_cmd++;
    } else {
        err = CLI_ERR_TOO_MANY_CMD;
    }
    return err;
} 

/**
 * @brief Find and do callback function for specified commands.
 * 
 * @param cmd Commands to search for.
 */
static void cli_manager_do_cmd(cli_cmd_t* cmd) {
    bool found_one = false;
    for (uint16_t i = 0; i < last_cmd; i++)
    {
        if(cmd->cmd_num[0] == cmd_configs[i].cmd_num[0] && cmd->cmd_num[1] == cmd_configs[i].cmd_num[1]) {
            found_one = true;
            cmd_configs[i].cmd_fun((void*)(&cmd->args));
        }
    }
    if (found_one == false)
    {
        cli_manager_no_command();
    }
}

/**
 * @brief Cli manager task operate commands and tcp server.
 * 
 * @param args Additional arguments to be passed to this task.
 */
void cli_manager_task(void* args) {
    cli_cmd_t cmd;
    while(1) {
        xEventGroupWaitBits(tcp_event_group, TCP_C_ACCEPTED_SOCKET_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        tcp_c_receive(receive_buffer);
        ESP_LOGD(TAG, "%s", receive_buffer);

        cmd = cli_manager_read_cmd(receive_buffer);
        cli_manager_do_cmd(&cmd);
        ESP_LOGD(TAG, "Data to send: %s", send_buffer);

        xEventGroupWaitBits(tcp_event_group, TCP_C_DATA_READY_TO_SEND, pdTRUE, pdFALSE, pdMS_TO_TICKS(3500));

        tcp_c_send(send_buffer, strlen(send_buffer));

        xEventGroupSetBits(tcp_event_group, TCP_C_FINISHED_TRANSMISSION);
    }
}

/**
 * @brief Concatenate string into the send buffer.
 * 
 * @note This function uses strlen to get string lenght, so string should be null terminated.
 * 
 * @param to_write String to write.
 * @return int error code.
 */
int cli_manager_add_to_send_buf(const char* to_write) {
    assert(to_write);
    int16_t len = (int16_t) strlen(to_write);
    int16_t send_len = (int16_t) strlen(send_buffer);

    memcpy(&send_buffer[send_len], to_write, len);

    return 0;
}

/**
 * @brief Write data into the buffer.
 * 
 * @param to_write Data to write.
 * @param to_write_len Data lenght.
 * @return int error code.
 */
int cli_manager_write_to_send_buf(const char to_write[], uint16_t to_write_len) {
    err_c_t err = ERR_C_OK;
    if(to_write_len >= TCP_C_SEND_BUFLEN) {
        return 1;
    }

    assert(to_write);
    memset(send_buffer, 0, sizeof(send_buffer));
    memcpy(send_buffer, to_write, to_write_len);

    return err;
}

/**
 * @brief Read data from receive buffer.
 * 
 * @param space_to_read Buffer where data will be copied.
 * @param space_to_read_len Buffer size.
 * @return int error code.
 */
int cli_manager_read_from_rec_buf(char space_to_read[], uint16_t space_to_read_len) {
    err_c_t err = ERR_C_OK;
    int len = (int) strlen(receive_buffer);
    if(space_to_read_len < len) {
        return 1;
    }

    assert(space_to_read);

    memcpy(space_to_read, receive_buffer, len);

    return err;
}

/**
 * @brief Starting point of Cli manager.
 * 
 * @return int error co de.
 */
int cli_manager_init(void) {
    tcp_event_group = xEventGroupCreate();

    wifi_c_init_wifi(WIFI_C_MODE_APSTA);
    wifi_c_start_sta(SSID_SOL, PASSWORD_SOL);
    wifi_c_start_ap("ESP32", "");
    vTaskDelay(pdMS_TO_TICKS(4000));

    tcp_c_start_tcp_server(tcp_event_group, 3);

    //xTaskCreate(cli_manager_task, "cli_task", 4096, NULL, 2, NULL);
    return 0;
}