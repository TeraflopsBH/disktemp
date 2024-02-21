#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_DISKS 16
#define PROGRAM_VERSION "1.0"
#define PROGRAM_NAME "disktemp - HDD Temperature Checker"
#define PROGRAM_AUTHOR "Nedim Karahmetovic"
#define PROGRAM_BRANCH "dev"

// Colors /////////////////////////////////////////////////////////////////////////////

// Function to print disk name with color
void printColoredDiskName(char *name) {
    printf("\033[0;95m%s\033[0m", name); // Light magenta
}

// Function to print temperature with color, based on value
void printColoredTemperature(int temperature) {
    if (temperature < 0) {
        printf("\033[01;37m%d\033[0m", temperature); // Light gray
    } else if (temperature <= 18) {
        printf("\033[01;34m%d\033[0m", temperature); // Light blue
    } else if (temperature <= 35) {
        printf("\033[01;32m%d\033[0m", temperature); // Light green
    } else if (temperature <= 50) {
        printf("\033[01;33m%d\033[0m", temperature); // Light yellow
    } else if (temperature <= 60) {
        printf("\033[0;33m%d\033[0m", temperature); // Orange
    } else {
        printf("\033[01;31m%d\033[0m", temperature); // Red
    }
}

// Smartmontools //////////////////////////////////////////////////////////////////////

// Function to check if smartmontools is installed
bool isSmartmontoolsInstalled() {
    FILE *fp = popen("which smartctl", "r");
    if (fp == NULL) {
        return false;
    }
    char buffer[128];
    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);
    return strlen(buffer) > 0;
}

// Function to install smartmontools using apt
void installSmartmontools() {
    system("sudo apt update");
    system("sudo apt install -y smartmontools");
}

// Function to perform the smartmontools check and return status
bool checkSmartmontools() {
    // Check if smartmontools is installed
    if (!isSmartmontoolsInstalled()) {
        // Prompt the user to install smartmontools
        printf("\n\033[01;34mINFO\033[0m - \033[01;32msmartmontools\033[0m is required for this program to run.\n");
        printf("\n       Do you want to install \033[01;32msmartmontools\033[0m now? (\033[01;32my\033[0m/\033[01;31mn\033[0m): ");
        char answer[2];
        fgets(answer, sizeof(answer), stdin);
        if (tolower(answer[0]) == 'y') {
            // Install smartmontools
            installSmartmontools();
            // Check again if smartmontools is now installed
            if (!isSmartmontoolsInstalled()) {
                printf("\n\033[01;31mERROR\033[0m - failed to install \033[01;32msmartmontools\033[0m. \033[01;31mProgram terminated!\033[0m\n");
                return false; // Exit with status indicating failure
            }
        } else {
            printf("\n\033[01;31mProgram terminated!\033[0m\n\n");
            return false; // Exit with status indicating failure
        }
    }

    // Smartmontools is installed or installation is successful
    return true; // Exit with status indicating success
}

// Function to check for specified disk
bool chkspecdsk(char *specified_disk) {

    // Check if smartmontools is installed
    if (!checkSmartmontools()) {
        // Exit if smartmontools check fails
        printf("\n\033[01;31mERROR\033[0m - failed to check disk '%s'.\n\n", specified_disk);
        printf("Smartmontools not installed.\n");
        return false;
    }

    bool disk_found = false;
    char disks[128];

    // Run smartctl command to get disk information
    FILE *fp;
    char command[512];
    char result[1024];

    sprintf(command, "smartctl --scan");
    fp = popen(command, "r");
    if (fp != NULL) {
        while (fgets(result, sizeof(result), fp) != NULL) {
            sscanf(result, "%s", disks);
            if (strcmp(specified_disk, disks) == 0) {
                disk_found = true;
                break;
            }
        }
        pclose(fp);
    }

    if (!disk_found) {
        printf("\n\033[01;31mERROR\033[0m - no such disk '\033[0;94m%s:\033[0m' present.\n\n", specified_disk);
        return false;
    }

    // Retrieve disk name and temperature of the specified disk
    char disk_name[128] = "";
    char temperature[16] = "";

    sprintf(command, "smartctl -i %s | grep 'Device Model' | cut -d: -f2- | sed 's/^ *//' | tr -d '\n'", specified_disk);
    fp = popen(command, "r");
    if (fp != NULL) {
        fgets(disk_name, sizeof(disk_name), fp);
        pclose(fp);

        // Remove trailing newline character and spaces
        char *end = disk_name + strlen(disk_name) - 1;
        while (end > disk_name && (*end == ' ' || *end == '\n')) {
                    *end-- = '\0';
        }

        sprintf(command, "smartctl -a %s | grep 'Temperature_Celsius' | awk '{ print $10 }'", specified_disk);
        fp = popen(command, "r");
        if (fp != NULL) {
            fgets(temperature, sizeof(temperature), fp);
            pclose(fp);

            // Remove trailing newline character and spaces
            end = temperature + strlen(temperature) - 1;
            while (end > temperature && (*end == ' ' || *end == '\n')) {
                        *end-- = '\0';
            }

            // Print disk information with colors
            printf("\033[0;94m%s:\033[0m ", specified_disk); // Disk name in light blue
            printColoredDiskName(disk_name); // Disk name in light magenta
            printf(" ");
            printColoredTemperature(atoi(temperature)); // Temperature value with color
            printf("°C\n");

            return true;
        } else {
            printf("\n\033[01;31mERROR\033[0m - failed to retrieve temperature information for disk '%s'.\n\n", specified_disk);
            return false;
        }
    } else {
        printf("\n\033[01;31mERROR\033[0m - no such disk '\033[0;94m%s:\033[0m' present.\n\n", specified_disk);
        return false;
    }
}

// Program switch control ////////////////////////////////////////////////////////////

// Function to print program usage
void printUsage() {
    printf("\n\033[01;32m%s\033[0m\n", PROGRAM_NAME);
    printf("\n\033[01;33mUsage:\033[0m \033[01;34m./disktemp\033[0m [\033[01;31m-V\033[0m] [\033[01;31m-h\033[0m | \033[01;31m--help\033[0m] [\033[01;31m-d <disk>\033[0m]\n");
    printf("\n\033[01;33mOptions:\033[0m\n");
    printf("  \033[01;31m-V\033[0m                Print program version\n");
    printf("  \033[01;31m-h\033[0m, \033[01;31m--help\033[0m        Print this help message\n");
    printf("  \033[01;31m-d <disk>\033[0m         Check temperature of specified disk\n");
    printf("\n");
}

// Function to print credits
void printCredits() {
    printf("\n\033[0;95m* * * * * * * * * * * * * * * * * * * *\033[0m\n");
    printf("\033[0;95m*\033[0m \033[01;32m%s\033[0m\n", PROGRAM_NAME);
    printf("\033[0;95m*\033[0m version: \033[01;34m%s\033[0m-\033[01;31m%s\033[0m\n", PROGRAM_VERSION, PROGRAM_BRANCH);
    printf("\033[0;95m*\033[0m by: \033[01;33m%s\033[0m\n", PROGRAM_AUTHOR);
    printf("\033[0;95m* * * * * * * * * * * * * * * * * * * *\033[0m\n\n");
}

// Main function ////////////////////////////////////////////////////////////////////

// Function to parse command line arguments
int main(int argc, char *argv[]) {
    
    // Check for command line switches
    if (argc == 1) {
        // No switches provided, check for smartmontools, get disk information, and print it
        if (!checkSmartmontools()) {
            return 1;
        }
        
        FILE *fp;
        char command[512];
        char result[1024];
        char disks[MAX_DISKS][64];

        sprintf(command, "smartctl --scan");
        fp = popen(command, "r");
        if (fp == NULL) {
            printf("\n\033[01;31mERROR\033[0m - failed to run command\n\n");
            return 1;
        }

        int disk_count = 0;
        while (fgets(result, sizeof(result), fp) != NULL && disk_count < MAX_DISKS) {
            sscanf(result, "%s", disks[disk_count]);
            disk_count++;
        }
        pclose(fp);

        char disk_names[MAX_DISKS][128];
        char temperatures[MAX_DISKS][16];

        // Retrieve disk names and temperatures
        for (int i = 0; i < disk_count; i++) {
            // Get disk name
            sprintf(command, "smartctl -i %s | grep 'Device Model' | cut -d: -f2- | sed 's/^ *//' | tr -d '\n'", disks[i]);
            fp = popen(command, "r");
            if (fp == NULL) {
                printf("\n\033[01;31mERROR\033[0m - failed to run command\n\n");
                return 1;
            }
            fgets(disk_names[i], sizeof(disk_names[i]), fp);
            pclose(fp);

            // Remove trailing newline character and spaces
            char *end = disk_names[i] + strlen(disk_names[i]) - 1;
            while (end > disk_names[i] && (*end == ' ' || *end == '\n')) {
                *end-- = '\0';
            }

            // Get temperature of the disk
            sprintf(command, "smartctl -a %s | grep 'Temperature_Celsius' | awk '{ print $10 }'", disks[i]);
            fp = popen(command, "r");
            if (fp == NULL) {
                printf("\n\033[01;31mERROR\033[0m - failed to run command\n\n");
                return 1;
            }
            fgets(temperatures[i], sizeof(temperatures[i]), fp);
            pclose(fp);

            // Remove trailing newline character and spaces
            end = temperatures[i] + strlen(temperatures[i]) - 1;
            while (end > temperatures[i] && (*end == ' ' || *end == '\n')) {
                *end-- = '\0';
            }
        }

        // Print disk information with colors
        for (int i = 0; i < disk_count; i++) {
            printf("\033[0;94m%s:\033[0m ", disks[i]); // Light blue disk
            printColoredDiskName(disk_names[i]); // Disk name in light magenta
            printf(" ");
            printColoredTemperature(atoi(temperatures[i])); // Temperature value with color
            printf("°C\n");
        }
    } else if (argc == 2) {
        // Check for -V or -h/--help
        if (strcmp(argv[1], "-V") == 0) {
            printCredits();
        } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printUsage();
        } else {
            printf("\n\033[01;31mERROR\033[0m - invalid command line switch\n");
            printf("        use \033[01;33m-h\033[0m or \033[01;33m--help\033[0m for a list of the switches available\n\n");
            return 1;
        }
    } else if (argc == 3) {
        // Check for -d
        if (strcmp(argv[1], "-d") == 0) {
            bool success = chkspecdsk(argv[2]);
            if (!success) {
                return 1;
            }
        } else {
            printf("\n\033[01;31mERROR\033[0m - invalid command line switch\n");
            printf("        use \033[01;33m-h\033[0m or \033[01;33m--help\033[0m for a list of the switches available\n\n");
            return 1;
        }
    } else {
        printf("\n\033[01;31mERROR\033[0m - invalid number of arguments.\n\n");
        return 1;
    }

    return 0;
}
