#include <stdint.h> // uint16_t and family
#include <stdio.h> // printf and family
#include <unistd.h> // file ops
#include <fcntl.h> // open() flags
#include <string.h> // strerror()
#include <errno.h> // errno

#include "motor_ctrl.h"
#include "parser.h"

#define HALF_PI 1.57079632679489661923

// The speed of the motor in [rad/s]. π rad in 2 seconds.
#define SPEED HALF_PI // [rad/s]
#define FREQUENCY 50 // [Hz]
#define T (1.0/FREQUENCY) // Period of the PWM signal [s]
#define SPEED_PER_T ((SPEED)*T) // Amount of rotation that occurs in each period [rad]

uint16_t angle_to_duty(double radians) {
	if (radians < -HALF_PI) radians = -HALF_PI;
	else if (radians > HALF_PI) radians = HALF_PI;
	return (uint16_t)(500 + 400 * radians / HALF_PI);
}

double duty_to_angle(uint16_t duty) {
	return (double)(duty - 500) * HALF_PI / 400;
}

uint8_t increment(double *s0, double *s1, double *s2, double *s3, double s0_target, double s1_target, double s2_target, double s3_target) {
	uint8_t changed = 0;
	if (*s0 < s0_target) {
		*s0 += SPEED_PER_T;
		if (*s0 > s0_target) *s0 = s0_target;
		changed = 1;
	}
	else if (*s0 > s0_target) {
		*s0 -= SPEED_PER_T;
		if (*s0 < s0_target) *s0 = s0_target;
		changed = 1;
	}

	if (*s1 < s1_target) {
		*s1 += SPEED_PER_T;
		if (*s1 > s1_target) *s1 = s1_target;
		changed = 1;
	}
	else if (*s1 > s1_target) {
		*s1 -= SPEED_PER_T;
		if (*s1 < s1_target) *s1 = s1_target;
		changed = 1;
	}

	if (*s2 < s2_target) {
		*s2 += SPEED_PER_T;
		if (*s2 > s2_target) *s2 = s2_target;
		changed = 1;
	}
	else if (*s2 > s2_target) {
		*s2 -= SPEED_PER_T;
		if (*s2 < s2_target) *s2 = s2_target;
		changed = 1;
	}

	if (*s3 < s3_target) {
		*s3 += SPEED_PER_T;
		if (*s3 > s3_target) *s3 = s3_target;
		changed = 1;
	}
	else if (*s3 > s3_target) {
		*s3 -= SPEED_PER_T;
		if (*s3 < s3_target) *s3 = s3_target;
		changed = 1;
	}

	return changed;
}

void printCommand(Command command) {
    printf("Command: ");
    if (command.type == GO) {
        printf("GO ");
        if (command.entity == ARM) printf("ARM ");
        else if (command.entity == HAND) printf("HAND ");

        printf("[");
        for (int i = 0; i < command.vector.length; i++) {
            printf("%f", command.vector.values[i]);
            if (i < command.vector.length - 1) printf(", ");
        }
        printf("]\n");
    }
    else if (command.type == WAIT) {
        printf("WAIT ");
        printf("%f\n", command.vector.values[0]);
    }
}

//#define OPEN open(DEV_FN, O_RDWR)
//#define WRITE (write(fd, (char*)&duties, sizeof(duties)))
//#define WAIT(x) usleep(1000000 * x)
//#define PRINTDUTIES for(size_t i = 0; i < MOTOR_CLTR__N_SERVO; ++i) {printf("duties[%d] = %d, ", i, duties[i]);} printf("\n")

int main(int argc, char** argv){
	if (argc < 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

	ssize_t r;
	int fd;

	uint16_t duties[MOTOR_CLTR__N_SERVO] = {500, 500, 500, 500}; // we only need to write to the first 4 servos
	
	fd = open(DEV_FN, O_WRONLY);
	if(fd < 0){
		fprintf(stderr, "[ERROR] \"%s\" not opened!\n", DEV_FN);
		fprintf(stderr, "fd = %d %s\n", fd, strerror(-fd));
		return 4;
	}

	char success;
    CommandNode *commands = parse_routine(argv[1], &success);
    if (!success) {
        printf("[ERROR] Failed to parse routine\n");
        return 1;
    }

    CommandNode *current = commands;
    while (current != NULL) {
        printCommand(current->command);
        
		if (current->command.type == WAIT) {
			printf("Waiting %f seconds\n", current->command.vector.values[0]);
			usleep(current->command.vector.values[0] * 1000 * 1000);
			current = current->next;
			continue;
		}
		
		double s0, s1, s2, s3, s0_target, s1_target, s2_target, s3_target;

		s0 = duty_to_angle(duties[0]);
		s1 = duty_to_angle(duties[1]);
		s2 = duty_to_angle(duties[2]);
		s3 = duty_to_angle(duties[3]);

		if (current->command.entity == ARM) {
			s0_target = current->command.vector.values[0];
			s1_target = current->command.vector.values[1];
			s2_target = current->command.vector.values[2];
			s3_target = s3;
		}
		else if (current->command.entity == HAND) {
			s0_target = s0;
			s1_target = s1;
			s2_target = s2;
			s3_target = current->command.vector.values[0];
		}

		while (increment(&s0, &s1, &s2, &s3, s0_target, s1_target, s2_target, s3_target)) {
			duties[0] = angle_to_duty(s0);
			duties[1] = angle_to_duty(s1);
			duties[2] = angle_to_duty(s2);
			duties[3] = angle_to_duty(s3);

			printf("duties[0] = %d, duties[1] = %d, duties[2] = %d, duties[3] = %d\n", duties[0], duties[1], duties[2], duties[3]);
			
			r = write(fd, (char*)&duties, sizeof(duties));
			if(r != sizeof(duties)){
				fprintf(stderr, "[ERROR] write went wrong!\n");
				return 4;
			}

			close(fd);
			
			usleep(T * 1000 * 1000);
			fd = open(DEV_FN, O_WRONLY);
		}

		current = current->next;
    }

	close(fd);

	CommandNode *next;
	current = commands;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}

	printf("Routine culminated successfully.\n");

	return 0;
}
