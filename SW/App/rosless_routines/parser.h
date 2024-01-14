#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

#define ROUTINE_DIR "../../../ROS/arm_and_chassis_ws/src/common_teleop/routines/s3a/"

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
	uint8_t length;
} Vector;

typedef struct Command
{
	uint8_t type;
	uint8_t entity;
	Vector vector;
} Command;

typedef struct CommandNode
{
	Command command;
	struct CommandNode* prev;
	struct CommandNode* next;
} CommandNode;

CommandNode* parse_routine(const char *file_path, uint8_t *success);

#endif // PARSER_H