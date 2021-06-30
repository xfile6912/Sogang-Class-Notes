#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


typedef struct history* history_link;
typedef struct history {
	char command[200];
	int count;
	history_link link;
}History;
history_link head=NULL;//history list의 헤드
history_link tail=NULL;//history list의 테일

typedef struct opcode_entry* opcode_link;
typedef struct opcode_entry {
	char mnemonic[10];
	int opcode;
	opcode_link link;
}Opcode_Entry;
Opcode_Entry* opcode_table[20];//opcode hash table


char command[200];//전체 shell command 저장
char order[200];//전체 shell command중 명령어 부분만 저장
char last_part[200];//전체 shell command중 명령어 제외한 부분 저장.
char argument[4][50];//전체 shell command중 start 부분 저장, argument[3]에는 쓰레기값
char mnemonic[10];
int start, end;//dump의 인자 start, end값 저장
int address, value;//edit의 인자 address와 value 저장
int base_address = 0;//마지막 address + 1번지를 담고 있는 주소 즉 출력을 시작할 주소.
unsigned char memory_space[1048576];//가상의 메모리 공간


void get_normalize_command();
void get_order_of_command();
void get_normalize_last_part();

int check_is_right_hexargument(char* argument);
int check_argument_has_noblank(char* argument);
int string_to_int(char* str);

void store_history(int count);

void init_opcode_table();
int hash_function(char *mnemonic);

void help();
void dir();
void quit();
void history();
void dump();
void dump_start();
void dump_start_end(int count);
void edit_address_value();
void fill_start_end_value();
void reset();
void opcode_mnemonic();
void opcodelist();

