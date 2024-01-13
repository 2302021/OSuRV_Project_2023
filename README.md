# OSuRV Project 2023
This application is designed to read and parse custom files with the extension `.moveitcmd` located in the `ROS/arm_and_chassis_ws/src/common_teleop/routines/s3a` directory. Subsequently, it orchestrates the execution of a sophisticated command pipeline, transmitting signals to a driver that adeptly conveys PWD signals to GPIO pins on a Raspberry Pi 2.

## Structure
The application is divided into several parts:
- `ROS/arm_and_chassis_ws/src/common_teleop/routines/s3a`: This directory contains the `.moveitcmd` files which the application reads, parses and executes.
- `rosless_routine.c`: The main entry point of the application. Located in the `SW/App/rosless_routines` directory.
- `parser.c`: This file contains the parser for the `.moveitcmd` files.

## Building
To build the application, first navigate to the `ROS/arm_and_chasis_ws` directory and initate the building process by running `./tmuxer.py build_drv`. This command builds both driver and application files. Upon successful compilation, the terminal signals completion. Subsequently,  exit the tmux environment, either by typing tmux_exit or manually exiting tmux.

## Running
After successful compilation, initiate the application with `./tmuxer.py run_drv`. Tmux automatically orchestrates the setup, presenting a pre-typed command in the upper right corner of your terminal. A simple press of Enter seamlessly launches the application.

## Contributing
Please read through our contributing guidelines. Included are directions for opening issues, coding standards, and notes on development.

## License
This project is licensed under the MIT License, so it's open for collaboration and use.