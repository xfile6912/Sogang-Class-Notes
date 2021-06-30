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
					if (check_is_right_hexargument(argument[0]) && comma_count == 0)//argument가 맞는 양식이라면
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

					if (check_is_right_hexargument(argument[0]) && check_is_right_hexargument(argument[1]) && comma_count == 1)//argument가 맞는 양식이라면
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

					if (check_is_right_hexargument(argument[0]) && check_is_right_hexargument(argument[1]) && comma_count == 1)//argument가 맞는 양식이라면
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
						&& comma_count == 2)//argument가 맞는 양식이라면
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

					if (check_argument_has_noblank(argument[0]) && comma_count == 0)//argument가 맞는 양식이라면
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
		else if (!strcmp(order, "type"))
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
				else//인자가 1개인 경우 type filename
				{

					if (check_argument_has_noblank(argument[0]) && comma_count == 0)//argument가 맞는 양식이라면
					{
						strcpy(filename, argument[0]);
						type();
					}
					else {//잘못된 argument가 있다면
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "type"))
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
				else//인자가 1개인 경우 type filename
				{

					if (check_argument_has_noblank(argument[0]) && comma_count == 0)//argument가 맞는 양식이라면
					{
						strcpy(filename, argument[0]);
						type();
					}
					else {//잘못된 argument가 있다면
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "assemble"))
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
				else//인자가 1개인 경우 assemble filename
				{

					if (check_argument_has_noblank(argument[0]) && comma_count == 0)//argument가 맞는 양식이라면
					{
						strcpy(filename, argument[0]);
						assemble();
					}
					else {//잘못된 argument가 있다면
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "symbol"))
		{
			if (!strcmp(last_part, ""))//last_part가 비어있으면 올바른 command
			{
				symbol();
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

