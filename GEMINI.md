# Gemini Project: C Terminal Productivity App

## Project Overview

This project is a terminal-based productivity application written in C. The main application appears to be a to-do list manager with features for adding, deleting, and managing tasks and groups within different pages. It uses the `ncurses` library to create a terminal user interface (TUI) and the `cJSON` library for data serialization to save the application state.

**Key Technologies:**
*   Language: C
*   UI: `ncurses`, `panel`
*   Data: `cJSON`

**Architecture:**
*   `main.c`: Contains the core application logic, UI rendering, and event handling.
*   `cJSON.c`/`cJSON.h`: A third-party library for JSON manipulation.
*   `makefile`: Defines build and clean procedures.
*   `update.sh`: A script to build and "install" the application by copying it to a local binary directory.
*   `backup.sh`: A placeholder script for backing up files. The `Backups/` directory suggests manual backups are being made.

## Building and Running

### Build
To compile the application, run the `make` command. This will use `gcc` to compile the source files and create an executable named `app`.

```sh
make
```

### Run
To run the application after building, execute the `app` file.

```sh
./app
```

### Clean
To remove the compiled executable, run:

```sh
make clean
```

### Install
The `update.sh` script provides a way to build the application and copy it to a user-local directory for easier access.

```sh
./update.sh
```
This script copies the `app` executable to `/home/user23565/.local/bin/Balls_on_Fire/app`.

## Development Conventions

*   **Data Storage:** The application reads and writes data to a JSON file located at `/home/user23565/.config/ballsonfire/data.txt`.
*   **Backups:** There is a `Backups/` directory containing timestamped copies of `main.c`. This indicates a manual backup strategy. The `backup.sh` script is currently empty and does not perform any actions.
*   **Dependencies:** The project depends on the `ncurses` and `panel` libraries, which must be installed on the system.
