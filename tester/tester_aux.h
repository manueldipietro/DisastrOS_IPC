#pragma once

#define TESTER_ANSI_RESET      "\033[0m"
#define TESTER_ANSI_BOLD       "\033[1m"
#define TESTER_ANSI_BRIGHT_RED "\033[91m"
#define TESTER_INIT_TAG        "\033[1m\033[93mINIT\033[0m| "
#define TESTER_READER_TAG      "\033[1m\033[94mREADER\033[0m| "
#define TESTER_WRITER_TAG      "\033[1m\033[95mWRITER\033[0m| "
#define TESTER_READER_TAG_WID  "\033[1m\033[94mREADER %d\033[0m| "
#define TESTER_WRITER_TAG_WID  "\033[1m\033[95mWRITER %d\033[0m| "

void tester_aux_sleeper();