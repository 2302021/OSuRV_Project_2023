#ifndef PARSER_H
#define PARSER_H

enum Entity
{
	HAND,
	ARM,
};

enum CommandTypes
{
	GO,
	WAIT,
};

typedef struct Vector
{
	double values[3];
	int length;
} Vector;

typedef struct Command
{
	char type;
	char entity;
	Vector vector;
} Command;

typedef struct CommandNode
{
	Command command;
	struct CommandNode* prev;
	struct CommandNode* next;
} CommandNode;

CommandNode* parse_routine(const char *file_path, char *success);

#endif // PARSER_H