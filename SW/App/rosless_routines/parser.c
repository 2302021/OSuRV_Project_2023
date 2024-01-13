#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

typedef struct Variable
{
	char name[256];
	Vector vector;
} Variable;

char current_entity = HAND;

Variable variables[256];
int num_variables = 0;

CommandNode* commands = NULL;

char add_command(Command command)
{
	CommandNode* new_command_node = malloc(sizeof(CommandNode));

	if (new_command_node == NULL)
	{
		return 0;
	}

	new_command_node->command = command;
	new_command_node->next = NULL;

	if (commands == NULL)
	{
		// If the list is empty, set the new node as the head
		commands = new_command_node;
	}
	else
	{
		// Otherwise, find the last node and set its next pointer to the new node
		CommandNode* last = commands;
		while (last->next != NULL)
		{
			last = last->next;
		}
		last->next = new_command_node;
	}

	return 1;
}

CommandNode* parse_routine(const char *file_path, char *success)
{
	FILE *file = fopen(file_path, "r");
	if (file == NULL)
	{
		printf("Cannot open file %s\n", file_path);
		*success = 0;
		return NULL;
	}

	char line[256];
	while (fgets(line, sizeof(line), file))
	{
		if (strncmp(line, "%include", 8) == 0)
		{
			char included_file[256] = ROUTINE_DIR;
			char success;
			sscanf(line, "%*s %s", included_file + strlen(included_file));
			parse_routine(included_file, &success);
			if (!success) break;
		}
		else if (strncmp(line, "go", 2) == 0)
		{
			char variable_name[256];
			if (sscanf(line, "%*s %s", variable_name) == 1)
			{
				for (int i = 0; i < num_variables; i++)
				{
					if (strcmp(variables[i].name, variable_name) == 0)
					{
						Command command;
						command.type = GO;
						command.entity = current_entity;
						command.vector = variables[i].vector;

						if (!add_command(command)) {
							*success = 0;
							printf("[ERROR] Failed to add command\n");
							return NULL;
						}

						break;
					}
				}
			}
		}
		else if (strncmp(line, "wait", 4) == 0)
		{
			double seconds;
			if (sscanf(line, "%*s %lf", &seconds) == 1)
			{
				Command command;
				command.type = WAIT;
				command.vector.length = 1;
				command.vector.values[0] = seconds;
				
				if (!add_command(command)) {
					*success = 0;
					printf("[ERROR] Failed to add command\n");
					return NULL;
				}
			}
		}
		else if (strncmp(line, "use", 3) == 0)
		{
			char entity[256];
			if (sscanf(line, "%*s %s", entity) == 1)
			{
				if (strcmp(entity, "hand") == 0)
				{
					current_entity = HAND;
				}
				else if (strcmp(entity, "arm") == 0)
				{
					current_entity = ARM;
				}
			}
		}
		else if (strchr(line, '=') != NULL)
		{
			Variable new_variable;
			int existing_variable_index = -1;
			if (sscanf(line, "%s = [%lf %lf %lf]", new_variable.name, &(new_variable.vector.values[0]), &(new_variable.vector.values[1]), &(new_variable.vector.values[2])) == 4)
			{
				new_variable.vector.length = 3;
			}
			else if (sscanf(line, "%s = [%lf %lf]", new_variable.name, &(new_variable.vector.values[0]), &(new_variable.vector.values[1])) == 3)
			{
				new_variable.vector.length = 2;
			}

			for (int i = 0; i < num_variables; i++)
			{
				if (strcmp(variables[i].name, new_variable.name) == 0)
				{
					existing_variable_index = i;
					break;
				}
			}

			if (existing_variable_index != -1)
			{
				variables[existing_variable_index] = new_variable;
			}
			else
			{
				variables[num_variables++] = new_variable;
			}
		}
		else
		{
			if (line[0] != '#' && line[0] != '\n') {
				printf("[ERROR] Unknown command: %s\n", line);
				*success = 0;
				return NULL;
			}
		}
	}

	fclose(file);

	*success = 1;

	return commands;
}
