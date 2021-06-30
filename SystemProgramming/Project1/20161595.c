#include "20161595.h"

int main(void) {
	init_opcode_table();//opcode_hash_table 생성
	while (1)
	{
		get_normalize_command();//command받아서 앞뒤 공백 제거
		get_order_of_command();//command에서 order 가져옴
		get_normalize_last_part();//order를 제외한 나머지 part 앞뒤 공백 제거후 last_part에 저장

		int comma_count = 0;
		int i;
		for (i = 0; i < strlen(last_part); i++)
		{
			if (last_part[i] == ',')
				comma_count++;
		}

		//printf("%s", last_part);
		if (!strcmp(order, "h") || !strcmp(order, "help"))
		{
			if (!strcmp(last_part, ""))//last_part가 비어있으면 올바른 command
			{
				help();
			}
			else//last_part가 비어있지 않으면 올바르지 않은 command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "d") || !strcmp(order, "dir"))
		{
			if (!strcmp(last_part, ""))//last_part가 비어있으면 올바른 command
			{
				dir();
			}
			else//last_part가 비어있지 않으면 올바르지 않은 command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "q") || !strcmp(order, "quit"))
		{
			if (!strcmp(last_part, ""))//last_part가 비어있으면 올바른 command
			{
				quit();//free 해주는 작업
				break;
			}
			else//last_part가 비어있지 않으면 올바르지 않은 command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "hi") || !strcmp(order, "history"))
		{

			if (!strcmp(last_part, ""))//last_part가 비어있으면 올바른 command
			{
				history();
			}
			else//last_part가 비어있지 않으면 올바르지 않은 command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "du") || !strcmp(order, "dump"))
		{
			if (last_part[strlen(last_part) - 1] == ',')//인자의 끝이 comma로 끝나면 잘못된 command
			{
				printf("Wrong argument!\n");
				continue;
			}
			if (!strcmp(last_part, ""))//아무런 인자도 들어오지 않은 경우 [dump]
			{
				dump();
			}
			else {//인자가 있는 경우

				int count = 0;
				char* ptr = strtok(last_part, ",");//,를 기준으로 parsing
				while (ptr != NULL)
				{
					strcpy(argument[count], ptr);
					ptr = strtok(NULL, ",");
					count++;
				}

				if (count >= 3)//인자가 3개이상인 경우 잘못된 argument
				{
					printf("Wrong argument!\n");
					continue;
				}
				else if (count == 1)//인자가 한개인 경우 [dump start]
				{
					if (check_is_right_hexargument(argument[0]) && comma_count==0)//argument가 맞는 양식이라면
					{
						start = string_to_int(argument[0]);
						dump_start();
					}
					else {//argument가 잘못되었다면
						printf("Wrong argument!\n");
						continue;
					}
				}
				else if (count == 2)//인자가 두개인 경우 [dump start, end]
				{

					if (check_is_right_hexargument(argument[0]) && check_is_right_hexargument(argument[1]) && comma_count==1)//argument가 맞는 양식이라면
					{
						start = string_to_int(argument[0]);
						end = string_to_int(argument[1]);
						dump_start_end(2);
					}
					else {//argument가 잘못되었다면
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "e") || !strcmp(order, "edit"))
		{
			if (last_part[strlen(last_part) - 1] == ',')//인자의 끝이 comma로 끝나면 잘못된 argument
			{
				printf("Wrong argument!\n");
				continue;
			}
			if (!strcmp(last_part, ""))//아무런 인자도 들어오지 않은 경우 잘못된 argument
			{
				printf("Wrong argument!\n");
				continue;
			}
			else {//인자가 있는 경우
				int count = 0;
				char* ptr = strtok(last_part, ",");//,를 기준으로 parsing
				while (ptr != NULL)
				{
					strcpy(argument[count], ptr);
					ptr = strtok(NULL, ",");
					count++;
				}

				if (count != 2)//인자가 2개가 아닌 경우 잘못된 argument
				{
					printf("Wrong argument!\n");
					continue;
				}
				else//인자가 두개인 경우 edit address, value
				{

					if (check_is_right_hexargument(argument[0]) && check_is_right_hexargument(argument[1]) && comma_count==1)//argument가 맞는 양식이라면
					{
						address = string_to_int(argument[0]);
						value = string_to_int(argument[1]);
						edit_address_value();
					}
					else {//잘못된 argument가 있다면
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "f") || !strcmp(order, "fill"))
		{
			if (last_part[strlen(last_part) - 1] == ',')//인자의 끝이 comma로 끝나면 잘못된 argument
			{

				printf("Wrong argument!\n");
				continue;
			}
			if (!strcmp(last_part, ""))//아무런 인자도 들어오지 않은 경우 잘못된 argument
			{

				printf("Wrong argument!\n");
				continue;
			}
			else {//인자가 있는 경우
				int count = 0;
				char* ptr = strtok(last_part, ",");//,를 기준으로 parsing
				while (ptr != NULL)
				{

					strcpy(argument[count], ptr);
					ptr = strtok(NULL, ",");
					count++;
				}

				if (count != 3)//인자가 3개가 아닌 경우 잘못된 argument
				{
					printf("Wrong argument!\n");
					continue;
				}
				else//인자가 3개인 경우 fill start, end, value
				{


					if (check_is_right_hexargument(argument[0])
						&& check_is_right_hexargument(argument[1])
						&& check_is_right_hexargument(argument[2])
						&& comma_count==2)//argument가 맞는 양식이라면
					{
						start = string_to_int(argument[0]);
						end = string_to_int(argument[1]);
						value = string_to_int(argument[2]);
						fill_start_end_value();
					}
					else {//잘못된 argument가 있다면
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "reset"))
		{
			if (!strcmp(last_part, ""))//last_part가 비어있으면 올바른 command
			{
				reset();
			}
			else//last_part가 비어있지 않으면 올바르지 않은 command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "opcode"))
		{
			if (last_part[strlen(last_part) - 1] == ',')//인자의 끝이 comma로 끝나면 잘못된 argument
			{
				printf("Wrong argument!\n");
				continue;
			}
			if (!strcmp(last_part, ""))//아무런 인자도 들어오지 않은 경우 잘못된 argument
			{
				printf("Wrong argument!\n");
				continue;
			}
			else {//인자가 있는 경우
				int count = 0;
				char* ptr = strtok(last_part, ",");//,를 기준으로 parsing
				while (ptr != NULL)
				{
					strcpy(argument[count], ptr);
					ptr = strtok(NULL, ",");
					count++;
					if (count > 3)
						break;
				}

				if (count != 1)//인자가 1개가 아닌 경우 잘못된 argument
				{
					printf("Wrong argument!\n");
					continue;
				}
				else//인자가 1개인 경우 opcode mnemonic
				{

					if (check_argument_has_noblank(argument[0]) && comma_count==0)//argument가 맞는 양식이라면
					{
						strcpy(mnemonic, argument[0]);
						opcode_mnemonic();
					}
					else {//잘못된 argument가 있다면
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "opcodelist"))
		{
			if (!strcmp(last_part, ""))//last_part가 비어있으면 올바른 command
			{
				opcodelist();
			}
			else//last_part가 비어있지 않으면 올바르지 않은 command
			{
				printf("Wrong argument!\n");
			}
		}
		else//그외 모든 커맨드들은 잘못된 커맨드
		{
			printf("Wrong Command!\n");
		}
	}
}


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
	char type[10];
	int hash_key;
	while (1)
	{
		res = fscanf(fp, "%x %s %s", &opcode, mnemonic, type);//파일로부터 opcode, mnemonic, type 읽어옴
		if (res == EOF)//파일의 끝이면 읽어오는것 종료
		{
			break;
		}
		hash_key = hash_function(mnemonic);//hash_key값 만들어 냄.

		Opcode_Entry* opcode_entry = (Opcode_Entry*)malloc(sizeof(Opcode_Entry));
		opcode_entry->opcode = opcode;
		strcpy(opcode_entry->mnemonic, mnemonic);
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
