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
history_link head = NULL;//history list의 헤드
history_link tail = NULL;//history list의 테일

typedef struct opcode_entry* opcode_link;
typedef struct opcode_entry {
	char mnemonic[10];
	char format[10];
	int opcode;
	opcode_link link;
}Opcode_Entry;
Opcode_Entry* opcode_table[20];//opcode hash table

typedef struct symbol* symbol_link;
typedef struct symbol {
	char label[31];//30자+\0;
	int location_counter;//location counter;
	symbol_link link;
}Symbol;
symbol_link symbol_head = NULL;//symbol table list
symbol_link symbol_tail = NULL;
symbol_link real_symbol_head = NULL;//가장 나중에 성공한 symbol list(출력시 사용)
symbol_link real_symbol_tail = NULL;


typedef struct object_code* object_code_link;
typedef struct object_code {
	char object_code[11];
	int location_counter;//location counter;
	int is_last;//record의 마지막인지를 나타내는 flag//1이면 마지막이라서 이후부터는 다음줄에 출력되어야함.
	object_code_link link;
}Object_Code;
object_code_link object_code_head = NULL; //object code들을 list로써 저장(object code 저장시 사용)
object_code_link object_code_tail = NULL;

typedef struct modi_code* modi_link;
typedef struct modi_code {
	int address;//4형식의 address
	modi_link link;
}Modi_Code;
modi_link modi_head = NULL;//object Code의 relocation을 위한 modify 에 대한 정보 저장하는 list
modi_link modi_tail = NULL;

char command[200];//전체 shell command 저장
char order[200];//전체 shell command중 명령어 부분만 저장
char last_part[200];//전체 shell command중 명령어 제외한 부분 저장.
char argument[4][50];//전체 shell command중 start 부분 저장, argument[3]에는 쓰레기값
char mnemonic[10];
int start, end;//dump의 인자 start, end값 저장
int address, value;//edit의 인자 address와 value 저장
int base_address = 0;//마지막 address + 1번지를 담고 있는 주소 즉 출력을 시작할 주소.
char filename[50];//filename을 저장
unsigned char memory_space[1048576];//가상의 메모리 공간
char program_name[50];//program 이름 저장.
int start_address, program_length;//프로그램 시작주소, 길이


void get_normalize_command();
void get_order_of_command();
void get_normalize_last_part();


int check_is_right_hexargument(char* argument);
int check_argument_has_noblank(char* argument);
int string_to_int(char* str);


void store_history(int count);

void init_opcode_table();
int hash_function(char* mnemonic);


void free_list();
void print_error(int line_num, char* description);
void parse_instruction(char* instruction, char* label, char* opcode, char* operand);
int find_format(char* mnemonic);
int find_opcode(char *mnemonic);
int add_symbol_to_symbol_list(char* label, int line_num, int location_counter);
void print_record(FILE* obj_fp);
int get_register_value(char* reg);
void add_object_code(char* object_code, int location_counter);
void add_modi_code(int address);
int pass1();
int pass2();


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
void type();
void assemble();
void symbol();



////////////////////////////////////////////////
//함수명: init_opcode_table                   //
//기능: opcode.txt로부터 opcode정보를 읽어와서//
//opcode table(hash_table)에 저장하는 역할.   //
//리턴: 없음                                  // 
////////////////////////////////////////////////
void init_opcode_table()
{
	FILE* fp = fopen("opcode.txt", "r");//file을 오픈함
	int res, opcode;
	char mnemonic[10];
	char format[10];
	int hash_key;
	while (1)
	{
		res = fscanf(fp, "%x %s %s", &opcode, mnemonic, format);//파일로부터 opcode, mnemonic, format 읽어옴
		if (res == EOF)//파일의 끝이면 읽어오는것 종료
		{
			break;
		}
		hash_key = hash_function(mnemonic);//hash_key값 만들어 냄.

		Opcode_Entry* opcode_entry = (Opcode_Entry*)malloc(sizeof(Opcode_Entry));
		opcode_entry->opcode = opcode;
		strcpy(opcode_entry->mnemonic, mnemonic);
		strcpy(opcode_entry->format, format);
		opcode_entry->link = NULL;
		//읽어온 값들을 새로운 opcode entry에 저장.


		if (opcode_table[hash_key] == NULL)//아직 아무것도 연결된 것이 없다면 바로 연결
		{
			opcode_table[hash_key] = opcode_entry;
		}
		else//연결된 것이 이미 있다면 해당 리스트의 끝에 연결
		{
			Opcode_Entry* ptr = opcode_table[hash_key];
			for (; ptr->link != NULL; ptr = ptr->link);
			ptr->link = opcode_entry;
		}
	}
	fclose(fp);
}

/////////////////////////////////////////
//함수명: hash_function                //
//기능: mnemonic을 받아 이를 기준으로  //
//적절한 해시키값(0~19)을 생성하는 역할//
//리턴: 해시키값                       //
/////////////////////////////////////////
int hash_function(char* mnemonic)
{
	int hash_key;
	int i;
	int temp_value = 0;
	for (i = 0; i < strlen(mnemonic); i++)
	{
		temp_value += mnemonic[i] / 2;
	}

	double a = 0.618934;
	a = temp_value * a - (int)(temp_value * a);
	a *= 512;
	hash_key = (int)a % 20;//임의의 hash key값을 만들어내는 과정
	return hash_key;
}

////////////////////////////////////////////////// 
//함수명: string_to_int                         //    
//기능: 16진수 string을 받아 이를               //
//10진수로 바꾸어 주는 역할.                    //
//리턴: 10진수 정수(바꿀수 없다면  -1)          //
//////////////////////////////////////////////////
int string_to_int(char* str)
{
	int decimal = 0;
	int i;
	int exp = 1;
	if (strlen(str) == 0)
		return -1;//길이가 0이면 hexdecimal 형태가 아님.
	for (i = strlen(str) - 1; i >= 0; i--) {

		if ('a' <= str[i] && str[i] <= 'f') {//소문자인경우
			decimal += exp * (str[i] + 10 - 'a');
			exp *= 16;
		}
		else if ('A' <= str[i] && str[i] <= 'F') {//대문자인 경우
			decimal += exp * (str[i] + 10 - 'A');
			exp *= 16;
		}

		else if ('0' <= str[i] && str[i] <= '9') {
			decimal += exp * (str[i] - '0');
			exp *= 16;
		}
		else {	//hex decimal 형태가 아니므로 -1 리턴
			return -1;
		}
	}

	return decimal;
}

//////////////////////////////////////////////
//함수명: check_is_right_hexargument        //
//기능: argument를 받아 해당 argument가     //
//적절한 16진수 argument인지 판단하는 역할  //
//리턴: 1(적절한 16진수 인자),              //
//      0(적절하지 않은 16진수 인자)        //
//////////////////////////////////////////////
int check_is_right_hexargument(char* argument)
{
	int flag = check_argument_has_noblank(argument);

	if (string_to_int(argument) == -1)//hex로 바꿀수 없는 argument라면 잘못된 argument
		flag = 0;
	return flag;
}

//////////////////////////////////////////////
//함수명: check_argument_has_noblank        //
//기능: argument를 받아 해당 argument가.    //
//빈칸이 없는 argument인지 판단하는 역할    //
//리턴: 1(빈칸이 없음, 적절한 인자),        //
//      0(빈칸이 있음, 적절하지 않은 인자   //
//////////////////////////////////////////////
int check_argument_has_noblank(char* argument)
{
	int flag = 1;//공백이 없는 argument이면 1, 아니면 0;
	char temp[200];//argument를 임시로 저장할 공간


	strcpy(temp, argument);

	int start = 0;	//argument에서 빈칸이 아닌 최초 index
	int end = strlen(argument) - 1;//argument에서 빈칸이 아닌 마지막 index
	while (temp[start] == ' ' || temp[start] == '\t')
	{
		start++;
	}
	while (temp[end] == ' ' || temp[end] == '\t')
	{
		end--;
	}
	if (start > end)//start가 end보다 크면 공백으로만 이루어진 argument이므로 잘못된 argument
		return 0;
	strncpy(argument, temp + start, end - start + 1);//argumnet의 앞뒤 공백 제거
	argument[end - start + 1] = '\0';

	for (int i = 0; i < strlen(argument); i++)
	{
		if (argument[i] == ' ' || argument[i] == '\t')//앞뒤 공백을 제거했는데 안에 공백이 있다면 잘못된 argument임
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

//////////////////////////////////////////////
//함수명: get_normalize_command             //
//기능: command의 앞뒤 공백을 제거하는 역할 //
//리턴: 없음.                               //
//////////////////////////////////////////////
void get_normalize_command()
{
	printf("sicsim> ");
	fgets(command, 200, stdin);//command에 전체 명령어 받아옴
	command[strlen(command) - 1] = '\0';//fgets로 받으면 \n도 들어가므로 \n을 \0으로 바꾸어줌

	char temp[200];//command를 임시로 저장할 공간
	strcpy(temp, command);

	int start = 0;	//command에서 빈칸이 아닌 최초 index
	int end = strlen(command) - 1;//command에서 빈칸이 아닌 마지막 index
	if (start > end)
	{
		command[0] = '\0';
		return;
	}
	while (temp[start] == ' ' || temp[start] == '\t')
	{
		start++;
	}
	while (temp[end] == ' ' || temp[end] == '\t')
	{
		end--;
	}

	if (start > end)
	{
		command[0] = '\0';
		return;
	}


	strncpy(command, temp + start, end - start + 1);//앞뒤 공백 제거
	command[end - start + 1] = '\0';
}

//////////////////////////////////////////////
//함수명: get_order_of_command              //
//기능: command로부터 실제 명령어만을 가져  //
//      와 order에 저장하는 역할            //
//리턴: 없음.                               //
//////////////////////////////////////////////
void get_order_of_command()
{
	int i;
	for (i = 0; command[i] != ' ' && command[i] != '\t' && command[i] != '\0'; i++);
	strncpy(order, command, i);
	order[i] = '\0';
}

//////////////////////////////////////////////
//함수명: get_normalize_last_part           //
//기능: command에서 실제 명령어를 제외한 나 //
//머지 파트를 앞뒤 공백을 제거하고 last_part//
//에 저장하는 역할                          //
//리턴: 없음.                               //
//////////////////////////////////////////////
void get_normalize_last_part()
{
	int i;
	for (i = 0; command[i] != ' ' && command[i] != '\t' && command[i] != '\0'; i++); //command에서 order부분 제외
	int start = i;	//나머지 part에서 빈칸이 아닌 최초 index
	int end = strlen(command) - 1;//나머지 part에서 빈칸이 아닌 마지막 index
	if (start > end)
	{
		last_part[0] = '\0';
		return;
	}
	while (command[start] == ' ' || command[start] == '\t')
	{
		start++;
	}//나머지 부분의 앞부분의 공백 제외
	while (command[end] == ' ' || command[end] == '\t')
	{
		end--;
	}//나머지 부분의 뒷부분의 공백 제외
	if (start > end)
	{
		last_part[0] = '\0';
		return;
	}
	strncpy(last_part, command + start, end - start + 1);//앞뒤 공백 제거
	last_part[end - start + 1] = '\0';
}

//////////////////////////////////////////////
//함수명: store_history                     //
//기능: 수행된 명령어를 list에 저장하는 역할//
//리턴: 없음.                               //
//////////////////////////////////////////////
void store_history(int count) {
	History* data = (History*)malloc(sizeof(History));
	int space_index;//빈칸을 저장할 인덱스
	int i;

	strcpy(data->command, order);
	space_index = strlen(data->command);
	if (count != 0)//인자 있음
	{
		data->command[space_index] = ' ';
		for (i = 0; i < count; i++)
		{
			strcpy((data->command) + space_index + 1, argument[i]);
			space_index = strlen(data->command) + 1;
			if (i != count - 1)//마지막이 아닌경우에는 ', ' 넣어줘야함.
			{
				data->command[space_index - 1] = ',';
				data->command[space_index] = ' ';
			}

		}

	}
	data->link = NULL;
	if (head == NULL)//head가 null이면 head에 연결해줌.
	{
		data->count = 1;
		head = data;
		tail = data;
	}
	else //head가 null이 아니면 tail에 연결해줌.
	{
		data->count = tail->count + 1;
		tail->link = data;
		tail = data;
	}
}

//////////////////////////////////////////////
//함수명: help                              //
//기능: shell에서 실행 가능한 명령어들 출력 //
//리턴: 없음.                               //
//////////////////////////////////////////////
void help()
{
	printf("h[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
	//명령어들 출력
	store_history(0);
}

//////////////////////////////////////////////
//함수명: dir                               //
//기능: 현재 디렉토리에 있는 파일들 출력.   //
//리턴: 없음.                               //
//////////////////////////////////////////////
void dir()
{
	DIR* dir = opendir(".");//현재 디렉토리
	struct dirent* file;
	struct stat file_stat;
	int count = 0;
	while ((file = readdir(dir)))//파일들 순회
	{
		count++;
		stat(file->d_name, &file_stat);//파일 정보
		if (S_ISDIR(file_stat.st_mode))//directory라면뒤에 / 붙여줌
		{
			printf("\t%s/", file->d_name);
		}
		else if (S_IXUSR & file_stat.st_mode)//excutable file이라면 뒤에 * 붙여줌
		{
			printf("\t%s*", file->d_name);
		}
		else//etc
		{
			printf("\t%s", file->d_name);
		}
		if (count % 4 == 0)
			printf("\n");
	}
	printf("\n");
	closedir(dir);

	store_history(0);
}

//////////////////////////////////////////////
//함수명: quit                              //
//기능: 동적할당된 메모리들을 deallocate    //
//리턴: 없음.                               //
//////////////////////////////////////////////
void quit()
{
	//free opcode table
	int i;
	for (i = 0; i < 20; i++)//opcode table 순회
	{
		Opcode_Entry* ptr = opcode_table[i];
		opcode_table[i] = NULL;
		while (ptr != NULL)//동적할당된 노드들 deallocate
		{
			Opcode_Entry* temp = ptr->link;
			free(ptr);
			ptr = temp;
		}
	}
	//free history list
	History* ptr = head;
	head = NULL;
	tail = NULL;
	while (ptr != NULL)//history 순회하면서 동적할당된 노드들 deallocate
	{
		History* temp = ptr->link;
		free(ptr);
		ptr = temp;
	}

	store_history(0);

}

////////////////////////////////////////////////
//함수명: history                             //
//기능: 현재까지 사용한 명령어들 순서대로 출력//
//리턴: 없음.                                 //
////////////////////////////////////////////////
void history()
{
	store_history(0);
	History* ptr;
	//history list 순회하면서 사용된 명령어들 출력
	for (ptr = head; ptr != NULL; ptr = ptr->link)
	{
		printf("%d\t %s\n", ptr->count, ptr->command);
	}
}
//////////////////////////////////////////////
//함수명: dump                              //
//기능: 할당되어 있는 메모리를 10라인 출력. //
//리턴: 없음.                               //
//////////////////////////////////////////////
void dump()
{
	start = base_address;
	end = start + 159;//160개

	if (end > 0xfffff)//메모리 범위를 end가 넘어간다면
	{
		end = 0xfffff;
	}
	dump_start_end(0);
}
//////////////////////////////////////////////
//함수명: dump_start                        //
//기능: 할당되어 있는 메모리를 start 주소부 //
//      터 10라인 출력.                     //
//리턴: 없음.                               //
//////////////////////////////////////////////
void dump_start()
{
	end = (start + 159);//10개라인결정

	if (end > 0xfffff)//메모리 범위를 end가 넘어간다면
	{
		end = 0xfffff;
	}
	dump_start_end(1);
}
//////////////////////////////////////////////
//함수명: dump_start_end                    //
//기능: 할당되어 있는 메모리를 start 주소부 //
//      터 end주소까지 출력                 //
//리턴: 없음.                               //
//////////////////////////////////////////////
void dump_start_end(int count)//count는 인자의 개수
{
	if (start < 0 || end < 0 || start > end || start > 0xfffff || end > 0xfffff) {//잘못된 메모리 접근의 경우
		printf("wrong memory access!\n");
		return;
	}
	base_address = end + 1;//base address에 마지막 메모리 주소+1 넣어줌
	if (base_address > 0xfffff)
		base_address = 0;

	int line_addr;
	int addr;
	for (line_addr = start / 16 * 16; line_addr <= end / 16 * 16; line_addr = line_addr + 16)
	{
		printf("%05X", line_addr);//5자리 memory line address 출력
		for (addr = line_addr; addr < line_addr + 16; addr++)//각 line의 memory 안의 값들 출력(16개)
		{
			if (addr<start || addr>end)//start와 end 사이에 없는 경우
			{
				printf("   ");
			}
			else//start와 end 사이에 있는경우
			{
				printf(" %02X", memory_space[addr]);
			}
		}
		printf(" ; ");
		for (addr = line_addr; addr < line_addr + 16; addr++)//각 line의 memory 안의 값들 ascii code출력(16개).
		{
			if (addr<start || addr>end)//start와 end 사이에 없는 경우
			{
				printf(".");
			}
			else if (memory_space[addr] < 0x20 || memory_space[addr]>0x7e)//ascii code 컬럼에서 나타나지 않는 부분
			{
				printf(".");
			}
			else//ascii code 컬럼에서 나타나는 부분
			{
				printf("%c", memory_space[addr]);
			}
		}
		printf("\n");
	}
	store_history(count);
}

//////////////////////////////////////////////
//함수명: edit_address_value                //
//기능: 메모리의 address번지의 값을 value에 //
//      지정된 값으로 변경                  //
//리턴: 없음.                               //
//////////////////////////////////////////////
void edit_address_value()
{
	if (address < 0x00000 || address>0xfffff)//잘못된 address access
	{
		printf("Wrong memory space address!\n");
		return;
	}
	if (value < 0x00 || value>0xff)//잘못된 value
	{
		printf("Wrong argument!\n");
		return;
	}
	memory_space[address] = value;//메모리 공간에 값 대입
	store_history(2);
}
//////////////////////////////////////////////
//함수명: fill_start_end_value              //
//기능: 메모리의 start번지부터 end번지까지  //
//      값을 value에 지정된 값으로 변경     //
//리턴: 없음.                               //
//////////////////////////////////////////////
void fill_start_end_value()
{
	if (start < 0 || end < 0 || start > end || start > 0xfffff || end > 0xfffff) {//잘못된 메모리 접근의 경우
		printf("wrong memory access!\n");
		return;
	}
	if (value < 0x00 || value>0xff)//잘못된 value
	{
		printf("Wrong argument!\n");
		return;
	}

	int addr;
	for (addr = start; addr <= end; addr++)//start부터 end까지의 memory공간을 모두 value로 set
	{
		memory_space[addr] = value;
	}
	store_history(3);
}
//////////////////////////////////////////////
//함수명: reset                             //
//기능: 메모리 전체를 전부 0으로 변경.      //
//                                          //
//리턴: 없음.                               //
//////////////////////////////////////////////
void reset()
{
	int addr;
	for (addr = 0; addr < 1048576; addr++)//메모리 전체를 전부 0으로 변경
		memory_space[addr] = 0;
	store_history(0);
}

//////////////////////////////////////////////
//함수명: opcode_mnemonic                   //
//기능: 명령어에 해당하는 opcode를 출력.    //
//                                          //
//리턴: 없음.                               //
//////////////////////////////////////////////
void opcode_mnemonic()
{
	int hash_key = hash_function(mnemonic);//mnemonic을 통해 hash_key찾아냄
	int flag = 0;//찾았으면 1, 없으면 0;
	int opcode;

	Opcode_Entry* ptr = opcode_table[hash_key];
	for (; ptr != NULL; ptr = ptr->link)//opcode_table[hash_key]의 list 순회하면서 해당 mnemonic있는지 찾음
	{
		if (!strcmp(ptr->mnemonic, mnemonic))//mnemonic이 있다면 flag를 1로 바꾸어주고 break;
		{
			opcode = ptr->opcode;
			flag = 1;
			break;
		}
	}
	if (flag == 1)//있는 mnemonic이라면
	{
		printf("opcode is %02X\n", opcode);
		store_history(1);
	}
	else//없는 mnemonic이라면
	{
		printf("Wrong Mnemonic!\n");
		return;
	}

}

//////////////////////////////////////////////
//함수명: opcodelist                        //
//기능: opcode hash table의 내용을 출력.    //
//                                          //
//리턴: 없음.                               //
//////////////////////////////////////////////
void opcodelist()
{
	int i;
	for (i = 0; i < 20; i++)//opcode table 순회
	{
		Opcode_Entry* ptr = opcode_table[i];
		printf("%3d: ", i);
		for (; ptr != NULL; ptr = ptr->link)//opcode_table[i]의 list 순회하면서 출력
		{
			printf("[%s, %02X]", ptr->mnemonic, ptr->opcode);
			if (ptr->link != NULL)//다음에 가리키는 것이 있다면
				printf(" -> ");
		}
		printf("\n");
	}
	store_history(0);
}

//////////////////////////////////////////////
//함수명: type                              //
//기능: file name에 해당하는 파일을 읽어서  //
//      화면에 출력                         //
//리턴: 없음.                               //
//////////////////////////////////////////////
void type()
{
	FILE* fp = fopen(filename, "r");//open file
	if (fp == NULL)//if file not exists in directory.
	{
		printf("%s not exists.\n", filename);
		return;
	}
	int res;
	char c;
	while (1)
	{
		res = fscanf(fp, "%c", &c);//read char.
		if (res == EOF)//if end of file.
			break;
		printf("%c", c);//print char
	}
	store_history(1);
}

//////////////////////////////////////////////
//함수명: assemble                          //
//기능: file name에 해당하는 파일을 읽어서  //
//      object file과 리스팅 파일을 생성    //
//리턴: 없음.                               //
//////////////////////////////////////////////
void assemble()
{
	char obj_filename[50];
	char lst_filename[50];
	int pass1_flag = 0;
	int pass2_flag = 0;


	//파일 이름이 포맷에 맞지 않는다면 오류 출력
	if (strlen(filename) < 5 || strcmp(filename + strlen(filename) - 4, ".asm"))
	{
		printf("file name should form like *.asm.\n");
		return;
	}


	//obj_filename과 lst_filename 초기화
	strcpy(obj_filename, filename);
	strcpy(lst_filename, filename);
	strcpy(obj_filename + strlen(obj_filename) - 3, "obj");
	strcpy(lst_filename + strlen(obj_filename) - 3, "lst");
	

	//pass1을 실행
	pass1_flag = pass1();
	
	//만약 pass1이 실패한다면 symbol table과 modify list와, object_code list, intermediate file을 삭제
	if (pass1_flag == 0)
	{
		free_list();//free symbol table, modi list, object_code list, intermediate file
		return;
	}

	//pass2를 실행
	pass2_flag = pass2();

	//만약 pass2가 실패한다면 symbol table과 modify list와, object_code list, intermediate file을 삭제
	//object file과 lst file도 삭제
	if (pass2_flag == 0)
	{
		free_list();//free symbol table, modi list, object_code list, intermediate file
		remove(obj_filename);
		remove(lst_filename);
		return;
	}

	//성공시

	Symbol* symbol_ptr = real_symbol_head;//원래 real symbol값들 없애줌.
	real_symbol_head = NULL;
	real_symbol_tail = NULL;
	while (symbol_ptr != NULL)//symbol list 순회하면서 동적할당된 노드들 deallocate
	{
		Symbol* temp = symbol_ptr->link;
		free(symbol_ptr);
		symbol_ptr = temp;
	}

	real_symbol_head = symbol_head;//출력되는 symbol table 을 나타내는 head 업데이트
	real_symbol_tail = symbol_tail;
	symbol_head = NULL;//또 free_list에 의해 삭제되지 않도록 기존의 head는 null로 바꾸어줌.
	symbol_tail = NULL;

	free_list();//free symbol table, modi list, object_code list
	printf("[%s], [%s]\n", lst_filename, obj_filename);
	store_history(1);
}

//////////////////////////////////////////////
//함수명: symbol                            //
//기능: 가장 최근에 assemble한 symbol table //
//      을 화면에 출력                      //
//리턴: 없음.                               //
//////////////////////////////////////////////
void symbol()
{
	Symbol* ptr;
	//real symbol list 순회하면서 사용된 명령어들 출력

	if (real_symbol_head == NULL)//symbol table이 아직 생성되지 않음.
	{
		printf("Symbol Table not exists.\n");
		return;
	
	}

	for (ptr = real_symbol_head; ptr != NULL; ptr = ptr->link)
	{
		printf("\t%s\t %04X\n", ptr->label, ptr->location_counter);//심볼테이블 순회하면서 출력.
	}
	store_history(0);
}

//////////////////////////////////////////////
//함수명: free_list                         //
//기능: symbol table, intermediate file,    //
//      obcode list, modi list를 free해줌   //
//리턴: 없음.                               //
//////////////////////////////////////////////
void free_list()
{
	//free symbol list
	Symbol* symbol_ptr = symbol_head;
	symbol_head = NULL;
	symbol_tail = NULL;
	while (symbol_ptr != NULL)//symbol list 순회하면서 동적할당된 노드들 deallocate
	{
		Symbol* temp = symbol_ptr->link;
		free(symbol_ptr);
		symbol_ptr = temp;
	}

	//free object code list
	Object_Code* object_ptr = object_code_head;
	object_code_head = NULL;
	object_code_tail = NULL;
	while (object_ptr != NULL)//object code list 순회하면서 동적할당된 노드들 deallocate
	{
		Object_Code* temp = object_ptr->link;
		free(object_ptr);
		object_ptr = temp;
	}

	//free modi code list
	Modi_Code* modi_ptr = modi_head;
	modi_head = NULL;
	modi_tail = NULL;
	while (modi_ptr != NULL)//modify list 순회하면서 동적할당된 노드들 deallocate
	{
		Modi_Code* temp = modi_ptr->link;
		free(modi_ptr);
		modi_ptr = temp;
	}

	remove("intermediate");
}
//////////////////////////////////////////////
//함수명: print_error                       //
//기능: error line 번호와 description을 받아//
//      error를 화면에 출력                 //
//리턴: 없음.                               //
//////////////////////////////////////////////
void print_error(int line_num, char* description)
{
	printf("[LINE:%d]%s.\n", line_num, description);
}

//////////////////////////////////////////////
//함수명: parse_instruction                 //
//기능: assembly instruction을 받아서 label,//
//      opcode(mnemonic), operand로 나눠줌  //
//리턴: 없음.                               //
//////////////////////////////////////////////
void parse_instruction(char* instruction, char* label, char* opcode, char* operand)
{
	int label_it = 0, opcode_it = 0, operand_it = 0;//label, opcode, operand iterator;
	int comma_flag = 0;//instruction에 comma가 있으면 1, 없으면 0

	int i;

	if (!strcmp(instruction, "\n"))//빈문장이면 return;
		return;

	//comma가 있는지 없는지 체크
	for (i = 0; i < strlen(instruction); i++)
	{
		if (instruction[i] == ',')
		{
			comma_flag = 1;
			break;
		}
	}

	//label parsing
	for (i = 0; instruction[i] != ' ' && instruction[i] != '\t'; i++)
	{
		label[label_it++] = instruction[i];
	}
	label[label_it] = '\0';

	//빈칸은 지나감
	while (instruction[i] == ' ' || instruction[i] == '\t')
	{
		i++;
	}


	//opcode parsing
	for (; instruction[i] != ' ' && instruction[i] != '\t' && instruction[i] != '\n'; i++)
	{
		opcode[opcode_it++] = instruction[i];
	}
	opcode[opcode_it] = '\0';

	//빈칸은 지나감
	while (instruction[i] == ' ' || instruction[i] == '\t')
	{
		i++;
	}

	//operand parsing
	for (; instruction[i] != ' ' && instruction[i] != '\t' && instruction[i] != '\n'; i++)
	{
		operand[operand_it++] = instruction[i];
	}
	//comma가 있으면 ,뒤의 operand를 더해줌
	if (comma_flag == 1)
	{
		operand[operand_it++] = instruction[i++];
		for (; instruction[i] != ' ' && instruction[i] != '\t' && instruction[i] != '\n'; i++)
		{
			operand[operand_it++] = instruction[i];
		}
	}

	operand[operand_it] = '\0';

}


/////////////////////////////////////////////////////////
//함수명: find_format                                  //
//기능: mnemonic을 받아 해당 opcode의 type 반환        //
//리턴: 1(1형식), 2(2형식), 3(3/4형식), 0(없는 opcode) //
/////////////////////////////////////////////////////////
int find_format(char* mnemonic)
{
	int hash_key = hash_function(mnemonic);//mnemonic을 통해 hash_key찾아냄
	int format = 0;//1(1형식), 2(2형식), 3(3/4형식), 0(없는 mnemonic)

	Opcode_Entry* ptr = opcode_table[hash_key];
	//opcode_table[hash_key]의 list 순회하면서 해당 mnemonic있는지 찾음
	for (; ptr != NULL; ptr = ptr->link)
	{
		if (!strcmp(ptr->mnemonic, mnemonic))//mnemonic이 있다면 format값 저장
		{
			if (!strcmp(ptr->format, "1"))
				format = 1;
			else if (!strcmp(ptr->format, "2"))
				format = 2;
			else
				format = 3;
			break;
		}
	}
	return format;

}
/////////////////////////////////////////////////////////
//함수명: add_symbol_to_symbol_list                    //
//기능: symbol을 symbol table에 저장                   //
//리턴: 성공시 1, 실패시 0                             //
/////////////////////////////////////////////////////////
int add_symbol_to_symbol_list(char* label, int line_num, int location_counter)
{
	Symbol* ptr;
	//symbol list 순회하면서 중복되는 label이 있는지 체크
	for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
	{
		if (!strcmp(ptr->label, label))//중복되는 label 있으면 실패임.
		{
			return 0;//실패시
		}
	}

	//중복되는 label이 없는경우
	Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
	strcpy(new_symbol->label, label);
	new_symbol->location_counter = location_counter;
	new_symbol->link = NULL;

	if (symbol_head == NULL)//symbol list에 아무것도 없으면
	{
		symbol_head = new_symbol;
		symbol_tail = new_symbol;
	}
	else//symbol list가 비어있지 않은 경우
	{

		//새로운 심볼이 가장 앞에 들어가는 경우(오름차순)
		if (strcmp(new_symbol->label, symbol_head->label) < 0)
		{
			new_symbol->link = symbol_head;
			symbol_head = new_symbol;
		}
		//새로운 심볼이 가장 뒤에 들어가는 경우(오름차순)
		else if (strcmp(new_symbol->label, symbol_tail->label) > 0)
		{
			symbol_tail->link = new_symbol;
			symbol_tail = new_symbol;
		}
		//새로운 심볼이 중간에 들어가는 경우(오름차순)
		else
		{
			Symbol* prev = symbol_head;
			Symbol* ptr;
			for (ptr = symbol_head->link; ptr != NULL; ptr = ptr->link)
			{
				//new_symbol이 ptr이전거보다 크고 ptr보다 작으면 그 사이에 new_symbol이 들어가야함.
				if (strcmp(new_symbol->label, prev->label) > 0 && strcmp(new_symbol->label, ptr->label) < 0)
				{
					
					prev->link = new_symbol;
					new_symbol->link = ptr;
					break;
				}
				prev = ptr;
			}
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////
//함수명: pass1                                        //
//기능: assemble과정에서 pass1을 수행                  //
//리턴: 성공시 1, 실패시 0                             //
/////////////////////////////////////////////////////////
int pass1()
{
	FILE* asm_fp = fopen(filename, "r");
	FILE* inter_fp = fopen("intermediate", "w");
	char asm_inst[100];//asm instruction one line 저장
	char label[31];
	char opcode[50];
	char operand[50];
	int line_num = 0;
	int location_counter = 0;
	int start_flag = 0;//start가 들어오면 1로 바뀜
	int end_flag = 0;//end가 들어오면 1로 바뀜

	//file이 현재 디렉토리에 존재하지 않는다면
	if (asm_fp == NULL)
	{
		printf("%s not exists.\n", filename);
		return 0;
	}
	//intermediate file을 만들 수 없다면
	if (inter_fp == NULL)
	{
		printf("Cannot make intermediate file.\n");
		return 0;
	}
	strcpy(program_name, "");
	do {
		//각 변수들 초기화
		line_num += 5;
		memset(asm_inst, '\0', sizeof(asm_inst));
		memset(label, '\0', sizeof(label));
		memset(opcode, '\0', sizeof(opcode));
		memset(operand, '\0', sizeof(operand));

		//만약 end 신호가 들어오지 않았는데 파일이 끝나버린경우 오류
		if (fgets(asm_inst, 100, asm_fp) == NULL)
		{
			print_error(line_num, "asm should finish with end signature.");
			fclose(inter_fp);
			fclose(asm_fp);
			return 0;
		}
		if (!strcmp(asm_inst, "\n"))//빈문장이면 continue;
			continue;
		//comment인 경우
		if (asm_inst[0] == '.')
		{
			fprintf(inter_fp, "%d\t____\t%s", line_num, asm_inst);
			continue;
		}
		//assembly instruction을 parsing해줌
		parse_instruction(asm_inst, label, opcode, operand);

		//만약 label이 숫자로 시작한다면 잘못된 것임
		if (label[0] >= '0' && label[0] <= '9')
		{
			print_error(line_num, "number cannot be the first part of the label.");
			fclose(inter_fp);
			fclose(asm_fp);
			return 0;
		}
		//opcode가 start인 경우에는 locounter 초기화 해줌.
		if (start_flag == 0 && !strcmp(opcode, "START"))
		{
			start_flag = 1;
			strcpy(program_name, label);//program 이름 저장.
			start_address = string_to_int(operand);//start address 지정.
			location_counter = start_address;
			if (start_address == -1)//hex로 바꿀수 없다면
			{
				print_error(line_num, "invalid start address");
				fclose(inter_fp);
				fclose(asm_fp);
				return 0;
			}

			fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);

		}
		//opcode가 END인 경우
		else if (end_flag == 0 && !strcmp(opcode, "END"))
		{
			//intermediate file에 써줌.
			end_flag = 1;
			fprintf(inter_fp, "%d\t____\t%s", line_num, asm_inst);
			break;//while문 빠져나옴.
		}
		else
		{
			//start로 시작하지 않은 경우.
			if (start_flag == 0 && strcmp(opcode, "START"))
			{
				start_flag=1;
				start_address=0;
				location_counter=start_address;
			}

			if (label[0] != '\0')//label이 있는 경우
			{
				//LABEL이 있는 경우 SYMBOL TABLE에 넣어줌.
				if (!add_symbol_to_symbol_list(label, line_num, location_counter))
				{
					//symbol 중복인 경우
					print_error(line_num, "symbol duplicated");
					fclose(inter_fp);
					fclose(asm_fp);
					return 0;
				}

			}

			//BASE인 경우 location counter 표시안함
			if (!strcmp(opcode, "BASE"))
			{
				fprintf(inter_fp, "%d\t____\t%s", line_num, asm_inst);
			}
			else if (!strcmp(opcode, "RSUB"))//RSUB인 경우
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += 3;
			}
			else if (!strcmp(opcode, "BYTE"))//BYTE인 경우
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				if (operand[0] == 'C')
				{
					location_counter += (strlen(operand) - 3);//C''의 3개의 char 빼줌
				}
				else if (operand[0] == 'X')
				{
					location_counter += (strlen(operand) - 3) / 2;//X''의 3개의 문자 빼준 이후 각 2개의 문자가 하나의 byte이므로 2로 나누어줌.
				}
			}
			else if (!strcmp(opcode, "RESB"))//RESB인 경우
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += atoi(operand);
			}
			else if (!strcmp(opcode, "WORD"))//WORD인 경우
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += 3;
			}
			else if (!strcmp(opcode, "RESW"))//RESW인 경우
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += 3 * atoi(operand);
			}
			else if (opcode[0] == '+')//4형식인 경우
			{
				int format = find_format(opcode + 1);
				if (format == 0)//0인 경우에는 해당 opcode가 잘못된 것임
				{
					print_error(line_num, "non-exist opcode.");
					fclose(inter_fp);
					fclose(asm_fp);
					return 0;
				}
				if (format != 3)//3,4형이 안되는 포맷임
				{
					print_error(line_num, "cannot be a format4.");
					fclose(inter_fp);
					fclose(asm_fp);
					return 0;
				}
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += 4;
			}
			else//1, 2, 3형식인 경우
			{
				int format = find_format(opcode);//형식을 찾아옴
				if (format == 0)//0인 경우에는 해당 opcode가 잘못된 것임
				{
					print_error(line_num, "non-exist opcode.");
					fclose(inter_fp);
					fclose(asm_fp);
					return 0;
				}
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += format;
			}
		}


	} while (1);
	program_length = location_counter - start_address;//프로그램 길이 저장.
	fclose(inter_fp);
	fclose(asm_fp);
	return 1;
}


/////////////////////////////////////////////////////////
//함수명: find_opcode                                  //
//기능: opcode table에서 mnemonic에 해당하는 opcode반환//
//리턴: 성공시 opcode, 실패시 -1                       //
/////////////////////////////////////////////////////////
int find_opcode(char *mnemonic)
{
	int hash_key = hash_function(mnemonic);//mnemonic을 통해 hash_key찾아냄
	int flag = 0;//찾았으면 1, 없으면 0;
	int opcode;

	Opcode_Entry* ptr = opcode_table[hash_key];
	for (; ptr != NULL; ptr = ptr->link)//opcode_table[hash_key]의 list 순회하면서 해당 mnemonic있는지 찾음
	{
		if (!strcmp(ptr->mnemonic, mnemonic))//mnemonic이 있다면 flag를 1로 바꾸어주고 break;
		{
			opcode = ptr->opcode;
			flag = 1;
			break;
		}
	}
	if (flag == 1)//있는 mnemonic이라면
	{
		return opcode;
	}
	else//없는 mnemonic이라면
	{
		return -1;
	}

}
/////////////////////////////////////////////////////////
//함수명: get_register_value                           //
//기능: register에 해당하는 값 반환                    //
//리턴: 성공시 register value, 실패시 -1               //
/////////////////////////////////////////////////////////
int get_register_value(char* reg)
{
	if (!strcmp(reg, "A"))
		return 0;
	if (!strcmp(reg, "X"))
		return 1;
	if (!strcmp(reg, "L"))
		return 2;
	if (!strcmp(reg, "PC"))
		return 8;
	if (!strcmp(reg, "SW"))
		return 9;
	if (!strcmp(reg, "B"))
		return 3;
	if (!strcmp(reg, "S"))
		return 4;
	if (!strcmp(reg, "T"))
		return 5;
	if (!strcmp(reg, "F"))
		return 6;
	return -1;
}

/////////////////////////////////////////////////////////
//함수명: add_object_code                              //
//기능: object_code를 object code list에 추가          //
//리턴: 없음                                           //
/////////////////////////////////////////////////////////
void add_object_code(char* object_code, int location_counter)
{
	//새로운 object_code node를 만들어주고 정보입력
	Object_Code* new_object_code = (Object_Code*)malloc(sizeof(Object_Code));
	strcpy(new_object_code->object_code, object_code);
	new_object_code->location_counter = location_counter;
	new_object_code->link = NULL;

	if (object_code_head == NULL)//list가 비어있는 경우
	{
		object_code_head = new_object_code;
		object_code_tail = new_object_code;
	}
	else//list가 비어있지 않은경우
	{
		object_code_tail->link = new_object_code;
		object_code_tail = new_object_code;
	}
}

/////////////////////////////////////////////////////////
//함수명: add_modi_code                                //
//기능: modi가 들어가야하는 address를 list에 저장      //
//리턴: 없음                                           //
/////////////////////////////////////////////////////////
void add_modi_code(int address)
{
	//새로운 modi_code node를 만들어주고 정보입력
	Modi_Code* new_modi = (Modi_Code*)malloc(sizeof(Modi_Code));
	new_modi->address = address;
	new_modi->link = NULL;

	if (modi_head == NULL)//list가 비어있는 경우
	{
		modi_head = new_modi;
		modi_tail = new_modi;
	}
	else//list가 비어있지 않은경우
	{
		modi_tail->link = new_modi;
		modi_tail = new_modi;
	}
}

/////////////////////////////////////////////////////////
//함수명: print_record                                 //
//기능: 생성한 object file에 record정보를 출력         //
//리턴: 없음                                           //
/////////////////////////////////////////////////////////
void print_record(FILE* obj_fp)
{
	if (strlen(program_name) > 6)//6자를 출력해야하므로
		program_name[6] = '\0';
	fprintf(obj_fp, "H%-6s%06X%06X\n", program_name, start_address, program_length);//head record 출력
	Object_Code* ptr;
	Modi_Code* modi_ptr;
	int i;
	char record[61] = { '\0' };//record를 저장하는 배열 (object 코드 저장)
	int record_index = 0;
	int record_address=0;
	memset(record, '\0', sizeof(record));

	for (ptr = object_code_head; ptr != NULL; ptr = ptr->link)//text record 출력
	{
		if (record_index == 0)//record의 시작점을 record_address에 저장.
			record_address = ptr->location_counter;


		for (i = 0; i < strlen(ptr->object_code); i++) //record에 object 코드들 넣어줌.
		{
			record[record_index++] = (ptr->object_code)[i];
		}

		//ptr의 object코드가 record의 마지막 object 코드이거나
		//ptr의 다음링크가 널이거나
		//다음 object코드를 더했을 때 30byte를 넘어가면
		if (ptr->is_last == 1 || ptr->link == NULL || record_index + strlen(ptr->link->object_code) > 60)
		{
			record[record_index] = '\0';

			//record 한줄을 써주고
			fprintf(obj_fp, "T%06X%02X%s\n", record_address,record_index/2, record);
			memset(record, '\0', sizeof(record));//record배열 초기화
			record_index = 0;//record index를 초기화 해줌.
		}
	}

	//modify code에 대한 정보들 출력
	for (modi_ptr = modi_head; modi_ptr != NULL; modi_ptr = modi_ptr->link)
	{
		fprintf(obj_fp, "M%06X05\n", modi_ptr->address);
	}


	fprintf(obj_fp, "E%06X\n", start_address);//end record 출력
}
/////////////////////////////////////////////////////////
//함수명: pass2                                        //
//기능: assemble과정에서 pass2을 수행                  //
//리턴: 성공시 1, 실패시 0                             //
/////////////////////////////////////////////////////////
int pass2()
{
	FILE* obj_fp, * lst_fp, * inter_fp;
	char obj_filename[50];
	char lst_filename[50];
	char inter_line[200];//intermediate file one line
	int line_num;
	char loc_count[40], label[31], mnemonic[50], operand[50];
	char real_operand[50], real_mnemonic[50];//앞의 특수문자를 제거한 operand, mnemonic;
	char after_comma[50];//comma 다음에 나오는 문자 저장됨
	int N, I, X, B, P, E, PC;//레지스터
	int format = 0;
	int BASE = 0;
	char object_code[40];
	int i;

	//obj_filename, lst filename 지정
	strcpy(obj_filename, filename);
	strcpy(lst_filename, filename);
	strcpy(obj_filename + strlen(obj_filename) - 3, "obj");
	strcpy(lst_filename + strlen(obj_filename) - 3, "lst");

	obj_fp = fopen(obj_filename, "w");
	lst_fp = fopen(lst_filename, "w");
	inter_fp = fopen("intermediate", "r");

	//파일을 열수 없다면
	if (obj_fp == NULL || lst_fp == NULL || inter_fp == NULL)
	{
		printf("cannot open file\n");
		return 0;
	}

	PC = start_address;// pc초기화

	do
	{
		//변수들 초기화
		memset(inter_line, '\0', sizeof(inter_line));
		line_num = 0;
		memset(loc_count, '\0', sizeof(loc_count));
		memset(label, '\0', sizeof(label));
		memset(mnemonic, '\0', sizeof(mnemonic));
		memset(operand, '\0', sizeof(operand));
		memset(after_comma, '\0', sizeof(after_comma));
		memset(object_code, '\0', sizeof(object_code));

		//end를 만나기전에 프로그램이 끝난다면
		if (fgets(inter_line, 200, inter_fp) == NULL)
		{
			return 0;
		}
		//intermediate file의 한개의 라인을 parsing
		sscanf(inter_line, "%d %s %s %s %s %s", &line_num, loc_count, label, mnemonic, operand, after_comma);

		//뒤의 \n문자를 \0로 바꾸어줌.
		if (strlen(inter_line) > 0)
			inter_line[strlen(inter_line) - 1] = '\0';

		int comma_flag = 0;//comma가 있는지 없는지 check
		for (i = 0; i < strlen(inter_line); i++)
		{
			if (inter_line[i] == ',')
			{
				comma_flag = 1;
				break;
			}

		}

		//interline에서 원래 label이 들어가야하는 index를 계산
		int tab_count = 0;//interline의 탭 개수를 셈
		int label_index = 0;//label이 들어가야 할 인덱스 저장.
		for (label_index = 0;label_index < strlen(inter_line); label_index++)
		{
			if (inter_line[label_index]=='\t')
			{
				tab_count++;
			}
			if (tab_count == 2)//tab 2번 이후에는 label이 나와야함.
				break;
		}


		//label이 있는지 없는지 체크
		//label이 없는 경우 label부터 하나씩 미뤄서 저장해줘야 제대로 저장됨.
		if (inter_line[label_index + 1] == ' ' || inter_line[label_index + 1] == '\t')
		{
			strcpy(after_comma, operand);
			strcpy(operand, mnemonic);
			strcpy(mnemonic, label);
			strcpy(label, "");
		}


		//real mnemonic에 특수문자 제외하고 저장.
		if (mnemonic[0] == '+')
			strcpy(real_mnemonic, mnemonic + 1);
		else
			strcpy(real_mnemonic, mnemonic);

		//real operand에 특수문자 제외하고 저장.
		if (operand[0] == '#' || operand[0] == '@')
			strcpy(real_operand, operand + 1);
		else
			strcpy(real_operand, operand);


		if (!strcmp(loc_count, "____"))//location counter가 없는 경우 해당 부분 ' '로 바꾸어줌.
		{
			int loc_index;//location counter가 없는 경우 underbar 되있기 때문에 해당부분 인덱스 받아옴
			for (loc_index = 0; inter_line[loc_index] != '\t'; loc_index++);
			for (i = loc_index + 1; inter_line[i] == '_'; i++)
				inter_line[i] = ' ';
		}		//location counter의 underbar를 ' '로 바꾸어줌.

		/// object code 없는 경우
		if (label[0] == '.')//comment인 경우
		{
			//comment는 object code를 만들지 않음
			//PC 건들지 않음
			fprintf(lst_fp, "%-60s\n", inter_line);//lst code는 만들어냄.
			continue;
		}
		else if (!strcmp(mnemonic, "START"))//start인 경우
		{
			//object code를 만들지 않음
			//PC 건들지 않음
			fprintf(lst_fp, "%-60s\n", inter_line);//lst code 만들어냄.
		}
		else if (!strcmp(mnemonic, "END"))//end 인 경우
		{
			//object code를 만들지 않음
			//PC 건들지 않음
			fprintf(lst_fp, "%-60s\n", inter_line);//lst code 만들어냄.
			break;
		}
		else if (!strcmp(mnemonic, "BASE"))
		{
			//OBJECT 코드 만들지 않음
			//PC 건들지 않음
			int symbol_exists = 0;//operand가 symbol list에 있으면 1, 없으면 0;
			Symbol* ptr;
			for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
			{
				if (!strcmp(ptr->label, operand))
				{
					//symbol list에 operand가 있음
					symbol_exists = 1;
					break;
				}
			}
			if (symbol_exists == 0)
			{
				print_error(line_num, "symbol not in symbol table.");
				fclose(lst_fp);
				fclose(obj_fp);
				fclose(inter_fp);
				return 0;
			}
			BASE = ptr->location_counter;
			fprintf(lst_fp, "%-60s\n", inter_line);//lst code 만들어냄.
		}
		else if (!strcmp(mnemonic, "RESB"))
		{
			//OBJECT 코드 만들지 않음
			PC += atoi(real_operand);
			fprintf(lst_fp, "%-60s\n", inter_line);

			//resb나 resw의 경우 text record를 다음 줄로 내려줘야함. 
			//따라서 현재 taild의 object code가 가장 마지막 record라는 걸 플래그로 저장해줌.
			object_code_tail->is_last = 1;
		}
		else if (!strcmp(mnemonic, "RESW"))
		{
			//OBJECT 코드 만들지 않음
			PC += 3 * atoi(real_operand);
			fprintf(lst_fp, "%-60s\n", inter_line);

			//resb나 resw의 경우 text record를 다음 줄로 내려줘야함.
			//따라서 현재 taild의 object code가 가장 마지막 record라는 걸 플래그로 저장해줌.
			object_code_tail->is_last = 1;
		}
		else//object code 가 있는 경우. 레지스터 값부터 지정해줌.
		{
			if (operand[0] == '#')//immediate operand
			{
				//set register
				N = 0; I = 1; B = 0;
				if ('0' <= operand[1] && operand[1] <= '9')//DIRECT ADDRESSING
					P = 0;
				else
					P = 1;
				X = 0; E = 0;
			}
			else if (operand[0] == '@')//indirect addressing
			{
				N = 1; I = 0; B = 0;
				P = 1; X = 0; E = 0;
			}
			else//Simple addressing
			{
				N = 1; I = 1; B = 0;
				P = 1; X = 0; E = 0;
			}

			if (!strcmp(mnemonic, "BYTE"))
			{
				format = 5;
				if (real_operand[0] == 'C')
				{
					PC += (strlen(real_operand) - 3);//C''의 3개의 char 빼줌
				}
				else if (real_operand[0] == 'X')
				{
					PC += (strlen(real_operand) - 3) / 2;//X''의 3개의 문자 빼준 이후 각 2개의 문자가 하나의 byte이므로 2로 나누어줌.
				}

			}
			else if (!strcmp(mnemonic, "WORD"))
			{
				format = 6;
				PC += 3;
			}
			else if (mnemonic[0] == '+')//4형식
			{
				B = 0; P = 0; E = 1;
				format = 4;//위에서 intermediate 파일 만들때 4형식이 될수 있는지 미리 체크 다 했기 때문에
				//여기서는 할필요 없음.
				PC += format;
			}
			else
			{
				format = find_format(real_mnemonic);
				//format저장해줌. 없는 mnemonic은 있을 수 없음 위에서 intermediate 파일 만들때 체크해줌.
				PC += format;
			}

			//object code 계산
			if (format == 1)
			{
				//format1인데 operand가 있으면 안됨.
				if (real_operand[0]!='\0')
				{
					print_error(line_num, "format1 should not have a operand!");
					fclose(lst_fp);
					fclose(obj_fp);
					fclose(inter_fp);
					return 0;
				}
				int opcode=find_opcode(real_mnemonic);
				sprintf(object_code, "%02X", opcode);//16진수로 바꾸어줌
				add_object_code(object_code, string_to_int(loc_count));
				fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.
			}
			if (format == 2)
			{
				int opcode = find_opcode(real_mnemonic);
				if (comma_flag == 1)//comma가 있는경우
				{
					real_operand[strlen(real_operand) - 1] = '\0';
					int first_reg_val = get_register_value(real_operand);
					int second_reg_val = get_register_value(after_comma);
					if (first_reg_val == -1 || second_reg_val == -1)
					{
						print_error(line_num, "Invalid register.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					sprintf(object_code, "%04X", opcode*16*16+first_reg_val*16+second_reg_val);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.			

				}
				else//comma가 없는 경우
				{
					int reg_val = get_register_value(real_operand);
					if (reg_val == -1)
					{
						print_error(line_num, "Invalid register.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					sprintf(object_code, "%04X", opcode * 16 * 16 + reg_val * 16);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.		
				}

			}
			if (format == 3)
			{
				char object1[10] = { '\0' };//first part object 코드로 변경한거 저장
				char object2[10] = { '\0' };//second part object 코드로 변경한거 저장
				char object3[10] = { '\0' };//third part object 코드로 변경한거 저장
				int first8;//opcode+ni
				int second4;//xbpe
				int third12 = 0;//address

				int opcode = find_opcode(real_mnemonic);
				if (real_operand[0] == '\0')//operand가 없는 경우
				{
					P = 0;//pc relative가 아님.

					first8 = opcode + 2 * N + I;//opcode+ni
					second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
					third12=0;//address

					//각각을 16진수로 바꾸어줌
					sprintf(object1, "%02X", first8);
					sprintf(object2, "%01X", second4);
					sprintf(object3, "%03X", third12);

					//16진수로 바꾼 것들을 붙여 넣어줌
					strcat(object_code, object1);
					strcat(object_code, object2);
					strcat(object_code, object3);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.
				}
				else if(N==1 && P==1)//SIMPLE ADDRESSSING인 경우
				{
					first8 = opcode + 2 * N + I;//opcode+ni

					third12 = 0;//disp

					if (comma_flag == 1 && !strcmp(after_comma, "X"))//comma가 있고 comma뒤에 x가 있는 경우
					{
						real_operand[strlen(real_operand) - 1] = '\0';//comma 제거해 줌.
						X = 1;//X비트 셋
					}
					else if (comma_flag == 1 && strcmp(after_comma, "X"))//comma가 있는데 뒤에가 X가 아닌경우
					{
						print_error(line_num, "Unvalid Register.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					Symbol* ptr;
					//symbol list 순회하면서 operand가 label인지 체크
					for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
					{
						if (!strcmp(ptr->label, real_operand))
						{
							break;
						}
					}

					if (ptr != NULL)//operand가 label이라면
					{
						third12 = ptr->location_counter - PC;
					}
					else
					{
						print_error(line_num, "cannot find operand in symtab!");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					if (third12 < -2048 || third12>2047)//pc relative가 될 수 없다면
					{
						P = 0; B = 1;//PC RELATIVE에서 BASE RELATIVE로 바꿈.
						third12 = ptr->location_counter - BASE;
						if (third12 <0 ||third12>4095)//base relative도 될 수 없다면
						{
							print_error(line_num, "cannot be a format3");
							fclose(lst_fp);
							fclose(obj_fp);
							fclose(inter_fp);
							return 0;
						}
					}
					second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
					if (third12 < 0)//HEX 3개만 표기되게 하기 위하여
						third12 &= 0xfff;
					//각각을 16진수로 바꾸어줌
					sprintf(object1, "%02X", first8);
					sprintf(object2, "%01X", second4);
					sprintf(object3, "%03X", third12);

					//16진수로 바꾼 것들을 붙여 넣어줌
					strcat(object_code, object1);
					strcat(object_code, object2);
					strcat(object_code, object3);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.

				}
				else//Indirect, Immediate
				{
					first8 = opcode + 2 * N + I;//opcode+ni
					second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
					third12 = 0;//disp
					if (P == 1)//PC RELATIVE(Indirect or Immediate)
					{
						Symbol* ptr;
						//symbol list 순회하면서 operand가 label인지 체크
						for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
						{
							if (!strcmp(ptr->label, real_operand))
							{
								break;
							}
						}

						if (ptr != NULL)//operand가 label이라면
						{
							third12 = ptr->location_counter - PC;
						}
						else
						{
							print_error(line_num, "cannot find operand in symtab!");
							fclose(lst_fp);
							fclose(obj_fp);
							fclose(inter_fp);
							return 0;
						}
					}
					else//DIRECT(immediate)
					{

						int num_flag = 1;//operand가 숫자인지 check하는 flag;//1이면 숫자
						for (i = 0; i < strlen(real_operand); i++)
						{
							if (real_operand[i] < '0' || real_operand[i]>'9')
							{
								num_flag = 0;
								break;
							}

						}
						if (num_flag == 0)
						{
							print_error(line_num, "invalid operand");
							fclose(lst_fp);
							fclose(obj_fp);
							fclose(inter_fp);
							return 0;
						}

						third12 = atoi(real_operand);

						if (third12 < 0 || third12>4095)//base relative도 될 수 없다면
						{
							print_error(line_num, "cannot be a format3");
							fclose(lst_fp);
							fclose(obj_fp);
							fclose(inter_fp);
							return 0;
						}
					}

					third12 &= 0xfff;//HEX 3개만 표기되게 하기 위하여

					//각각을 16진수로 바꾸어줌
					sprintf(object1, "%02X", first8);
					sprintf(object2, "%01X", second4);
					sprintf(object3, "%03X", third12);

					//16진수로 바꾼 것들을 붙여 넣어줌
					strcat(object_code, object1);
					strcat(object_code, object2);
					strcat(object_code, object3);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.
				}

			}
			if (format == 4)
			{

				int opcode = find_opcode(real_mnemonic);
				char object1[10] = { '\0' };//first part object 코드로 변경한거 저장
				char object2[10] = { '\0' };//second part object 코드로 변경한거 저장
				char object3[10] = { '\0' };//third part object 코드로 변경한거 저장

				int first8 = opcode + 2 * N + I;//opcode+ni
				int second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
				int third20;//address
				if (comma_flag == 1)//comma가 있다면
				{
					real_operand[strlen(real_operand) - 1] = '\0';//comma 제거해 줌.
				}
				Symbol* ptr;
				//symbol list 순회하면서 operand가 label인지 체크
				for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
				{
					if (!strcmp(ptr->label, real_operand))
					{
						break;
					}
				}

				if (ptr != NULL)//operand가 label이라면
				{
					if (comma_flag == 1 && !strcmp(after_comma, "X"))//comma가 있고 comma뒤에 x가 있는 경우
					{
						X = 1;//X비트 셋
						second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
					}
					else if (comma_flag == 1 && strcmp(after_comma, "X"))//comma가 있는데 뒤에가 X가 아닌경우
					{
						print_error(line_num, "Unvalid Register.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}
					third20 = ptr->location_counter;
				}
				else//operand가 label이 아니라면 operand는 숫자임.
				{
					int num_flag=1;//operand가 숫자인지 check하는 flag;//1이면 숫자
					for (i = 0; i < strlen(real_operand); i++)
					{
						if (operand[0]!='#' || real_operand[i] < '0' || real_operand[i]>'9')
						{
							num_flag = 0;
							break;
						}

					}
					if (num_flag == 0)
					{
						print_error(line_num, "invalid operand");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					third20 = atoi(real_operand);
				}

				//각각을 16진수로 바꾸어줌
				sprintf(object1, "%02X", first8);
				sprintf(object2, "%01X", second4);
				sprintf(object3, "%05X", third20);

				//16진수로 바꾼 것들을 붙여 넣어줌
				strcat(object_code, object1);
				strcat(object_code, object2);
				strcat(object_code, object3);
				add_object_code(object_code, string_to_int(loc_count));//object code text record 출력을 위해서.
				if (operand[0] != '#')//만약 immediate가 아닌 4형식이라면
				{
					add_modi_code(string_to_int(loc_count) + 1);//modi에도 추가해줌.
					// 나중에 object 코드 M출력을 위해서.
				}
				fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.
			}
			if (format == 5)//byte
			{
				int integer_object = 0; // object code의 integer 형태
				int hex = 1;//16의 x제곱형태
				if (real_operand[0] == 'C')
				{
					int char_length = (strlen(real_operand) - 3);//char의 길이
					for (i = char_length+1; i >=2; i--)//진짜 char 돌면서
					{
						integer_object += real_operand[i] * hex;
						hex *= 256;
					}
					sprintf(object_code, "%X", integer_object);
				}
				else if(real_operand[0] == 'X')
				{
					int char_length = (strlen(real_operand) - 3);//문자의 길이
					if (char_length % 2 == 1)//홀수인 경우 앞에 0을 넣어줌
					{
						object_code[0] = '0';
						int j = 1;
						for (i = 2; real_operand[i] != '\''; i++)//그리고 그대로 복사해줌.
						{
							object_code[j++] = real_operand[i];
						}
						object_code[j] = '\0';
					}
					else//짝수인 경우에는 그냥 그대로 복사해주면 오브젝트코드임.
					{
						int j = 0;
						for (i = 2; real_operand[i] != '\''; i++)
						{
							object_code[j++] = real_operand[i];
						}
						object_code[j] = '\0';
					}
					if (string_to_int(object_code) == -1)//16진수 형태가 아니라면
					{
						print_error(line_num, "operand is not the type of hex.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}
				}
				add_object_code(object_code, string_to_int(loc_count));
				fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.
			}
			if (format==6)//word
			{
				int number=atoi(real_operand);
				number &= 0xffffff;//3byte
				sprintf(object_code, "%06X", number);
				add_object_code(object_code, string_to_int(loc_count));
				fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code 만들어냄.
			}

		}

	} while (1);

	print_record(obj_fp);//object file 만들어줌.

	fclose(lst_fp);
	fclose(obj_fp);
	fclose(inter_fp);
	return 1;
}
