#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

typedef struct Variable
{
	char name[256];
	Vector vector;
} Variable;

uint8_t current_entity = HAND;

Variable variables[256];
uint8_t num_variables = 0;

CommandNode* commands = NULL;

uint8_t add_command(Command command)
{
	// Create a new node for the command
	CommandNode* new_command_node = malloc(sizeof(CommandNode));

	// If the node cannot be created, return 0
	if (new_command_node == NULL)
	{
		return 0;
	}

	// Set the command and next pointer of the new node
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

// Function to parse a routine from a file
CommandNode* parse_routine(const char *file_path, uint8_t *success)
{
	// Open the file
	FILE *file = fopen(file_path, "r");
	// If the file cannot be opened, print an error message and return NULL
	if (file == NULL)
	{
		printf("Cannot open file %s\n", file_path);
		*success = 0;
		return NULL;
	}

	char line[256];
	// Read the file line by line
	while (fgets(line, sizeof(line), file))
	{
		// If the line starts with "%include", it's an include directive
		if (strncmp(line, "%include", 8) == 0)
		{
			char included_file[256] = ROUTINE_DIR;
			uint8_t success_recursive;
			// Parse the included file
			sscanf(line, "%*s %s", included_file + strlen(included_file));
			parse_routine(included_file, &success_recursive);
			// If parsing the included file fails, break the loop
			if (!success_recursive) break;
		}
		// If the line starts with "go", it's a go command
		else if (strncmp(line, "go", 2) == 0)
		{
			char variable_name[256];
			// Parse the variable name
			if (sscanf(line, "%*s %s", variable_name) == 1)
			{
				// Find the variable in the list of variables
				for (uint8_t i = 0; i < num_variables; i++)
				{
					if (strcmp(variables[i].name, variable_name) == 0)
					{
						Command command;
						command.type = GO;
						command.entity = current_entity;
						command.vector = variables[i].vector;

						// Add the command to the list of commands
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
		// If the line starts with "wait", it's a wait command
		else if (strncmp(line, "wait", 4) == 0)
		{
			double seconds;
			// Parse the number of seconds to wait
			if (sscanf(line, "%*s %lf", &seconds) == 1)
			{
				Command command;
				command.type = WAIT;
				command.vector.length = 1;
				command.vector.values[0] = seconds;
				
				// Add the command to the list of commands
				if (!add_command(command)) {
					*success = 0;
					printf("[ERROR] Failed to add command\n");
					return NULL;
				}
			}
		}
		// If the line starts with "use", it's a use command
		else if (strncmp(line, "use", 3) == 0)
		{
			char entity[256];
			// Parse the entity to use
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
		// If the line contains "=", it's a variable assignment
		else if (strchr(line, '=') != NULL)
		{
			Variable new_variable;
			int8_t existing_variable_index = -1;
			// Parse the variable assignment
			if (sscanf(line, "%s = [%lf %lf %lf]", new_variable.name, &(new_variable.vector.values[0]), &(new_variable.vector.values[1]), &(new_variable.vector.values[2])) == 4)
			{
				new_variable.vector.length = 3;
			}
			else if (sscanf(line, "%s = [%lf %lf]", new_variable.name, &(new_variable.vector.values[0]), &(new_variable.vector.values[1])) == 3)
			{
				new_variable.vector.length = 2;
			}
			
			// Find the variable in the list of variables
			for (uint8_t i = 0; i < num_variables; i++)
			{
				if (strcmp(variables[i].name, new_variable.name) == 0)
				{
					existing_variable_index = i;
					break;
				}
			}

			// If the variable already exists, update it
			if (existing_variable_index != -1)
			{
				variables[existing_variable_index] = new_variable;
			}
			// If the variable doesn't exist, add it to the list of variables
			else
			{
				variables[num_variables++] = new_variable;
			}
		}
		// If the line doesn't match any of the above, it's an unknown command
		else
		{
			if (line[0] != '#' && line[0] != '\n') {
				printf("[ERROR] Unknown command: %s\n", line);
				*success = 0;
				return NULL;
			}
		}
	}

	// Close the file
	fclose(file);

	// Signify that the routine is parsed successfully
	*success = 1;

	// Return the list of commands
	return commands;
}
