/**
 * @file ansi_colors.h
 * @author Wojciech Mytych
 * @brief ANSI color codes for logger library.
 * @version 2.0.3
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define RESET                           "\e[0m"


#define RED                             "\e[0;31m"
#define GREEN                           "\e[0;32m"
#define BLACK                           "\e033[0;30m"
#define YELLOW                          "\e[0;33m"
#define BLUE                            "\e[0;34m"
#define PURPLE                          "\e[0;35m"
#define CYAN                            "\e[0;36m"
#define WHITE                           "\e[0;37m"

#define BLACK_BOLD                      "\e[1;30m"
#define RED_BOLD                        "\e[1;31m"
#define GREEN_BOLD                      "\e[1;32m"
#define YELLOW_BOLD                     "\e[1;33m"
#define BLUE_BOLD                       "\e[1;34m"
#define PURPLE_BOLD                     "\e[1;35m"
#define CYAN_BOLD                       "\e[1;36m"
#define WHITE_BOLD                      "\e[1;37m"

#define BLACK_UNDERLINE                 "\e[4;30m"
#define RED_UNDERLINE                   "\e[4;31m"
#define GREEN_UNDERLINE                 "\e[4;32m"
#define YELLOW_UNDERLINE                "\e[4;33m"
#define BLUE_UNDERLINE                  "\e[4;34m"
#define PURPLE_UNDERLINE                "\e[4;35m"
#define CYAN_UNDERLINE                  "\e[4;36m"
#define WHITE_UNDERLINE                 "\e[4;37m"

#define BLACK_BACKGROUND                "\e[40m"
#define RED_BACKGROUND                  "\e[41m"
#define GREEN_BACKGROUND                "\e[42m"
#define YELLOW_BACKGROUND               "\e[43m"
#define BLUE_BACKGROUND                 "\e[44m"
#define PURPLE_BACKGROUND               "\e[45m"
#define CYAN_BACKGROUND                 "\e[46m"
#define WHITE_BACKGROUND                "\e[47m"

#define BLACK_HIGH_INTENSITY            "\e[0;90m"
#define RED_HIGH_INTENSITY              "\e[0;91m"
#define GREEN_HIGH_INTENSITY            "\e[0;92m"
#define YELLOW_HIGH_INTENSITY           "\e[0;93m"
#define BLUE_HIGH_INTENSITY             "\e[0;94m"
#define PURPLE_HIGH_INTENSITY           "\e[0;95m"
#define CYAN_HIGH_INTENSITY             "\e[0;96m"
#define WHITE_HIGH_INTENSITY            "\e[0;97m"

#define BLACK_BOLD_HIGH_INTENSITY       "\e[1;90m"
#define RED_BOLD_HIGH_INTENSITY         "\e[1;91m"
#define GREEN_BOLD_HIGH_INTENSITY       "\e[1;92m"
#define YELLOW_BOLD_HIGH_INTENSITY      "\e[1;93m"
#define BLUE_BOLD_HIGH_INTENSITY        "\e[1;94m"
#define PURPLE_BOLD_HIGH_INTENSITY      "\e[1;95m"
#define CYAN_BOLD_HIGH_INTENSITY        "\e[1;96m"
#define WHITE_BOLD_HIGH_INTENSITY       "\e[1;97m"

#define BLACK_BOLD_UNDERLINE            "\e[30;1;4m"
#define RED_BOLD_UNDERLINE              "\e[31;1;4m"
#define GREEN_BOLD_UNDERLINE            "\e[32;1;4m"
#define YELLOW_BOLD_UNDERLINE           "\e[33;1;4m"
#define BLUE_BOLD_UNDERLINE             "\e[34;1;4m"
#define PURPLE_BOLD_UNDERLINE           "\e[35;1;4m"
#define CYAN_BOLD_UNDERLINE             "\e[36;1;4m"
#define WHITE_BOLD_UNDERLINE            "\e[37;1;4m"

#define BLACK_ITALIC                    "\e[30;1;3m"
#define RED_ITALIC                      "\e[31;1;3m"
#define GREEN_ITALIC                    "\e[32;1;3m"
#define YELLOW_ITALIC                   "\e[33;1;3m"
#define BLUE_ITALIC                     "\e[34;1;3m"
#define PURPLE_ITALIC                   "\e[35;1;3m"
#define CYAN_ITALIC                     "\e[36;1;3m"
#define WHITE_ITALIC                    "\e[37;1;3m"