# OSuRV Project 2023
This application is designed to read and parse custom files with the extension `.moveitcmd` located in the `/routines` directory. It then executes the command pipeline and sends the signal to GPIO pins on a Raspberry Pi.

## Structure
The application is divided into several parts:
- `main.c`: The main entry point of the application. Located in the `src` directory.
- `routines`: This directory contains the `.moveitcmd` files that the application will read and parse.
- `parser`: This directory contains the parser for the `.moveitcmd` files. The main parser file is `parser.c` and the header file is `parser.h`.
- `Makefile`: This file contains the build instructions for the application.

## Building
To build the application, run `make` in the root directory of the project. You can build for debug or release using `make debug` or `make release` respectively.

temporary `gcc -Iinclude -o main src/main.c src/parser/parser.c include/parser.h`

## Running
After building the application, you can run it with `./bin/debug/ros-less_routines` for the debug version or `./bin/release/ros-less_routines` for the release version.

## Contributing
Please read through our contributing guidelines. Included are directions for opening issues, coding standards, and notes on development.

## License
This project is licensed under the MIT License.