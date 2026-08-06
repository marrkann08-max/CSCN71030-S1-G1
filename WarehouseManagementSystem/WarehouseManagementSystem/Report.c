#include "Report.h"
#include "utilities.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define REPORT_PATH_CAPACITY 260
#define REPORT_COLLISION_LIMIT 1000

static int validate_directory(const char* directory, char* validated)
{
    size_t length;

    if (directory == NULL || validated == NULL) {
        return -1;
    }

    length = strlen(directory);
    if (length == 0 || length >= REPORT_PATH_CAPACITY) {
        return -1;
    }

    memcpy(validated, directory, length + 1);
    return util_validate_string(validated, REPORT_PATH_CAPACITY - 1);
}

static int build_report_path(
    const char* directory,
    const char* extension,
    char* path,
    size_t path_capacity)
{
    char timestamp[32];
    struct tm local_time;
    time_t now;
    size_t directory_length;
    const char* separator;
    int suffix;

    now = time(NULL);
    if (now == (time_t)-1 || localtime_s(&local_time, &now) != 0 ||
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H-%M-%S", &local_time) == 0) {
        return -1;
    }

    directory_length = strlen(directory);
    separator = (directory[directory_length - 1] == '\\' ||
        directory[directory_length - 1] == '/') ? "" : "\\";

    for (suffix = 0; suffix < REPORT_COLLISION_LIMIT; ++suffix) {
        FILE* existing = NULL;
        int written;

        if (suffix == 0) {
            written = snprintf(path, path_capacity, "%s%sReport %s.%s",
                directory, separator, timestamp, extension);
        }
        else {
            written = snprintf(path, path_capacity, "%s%sReport %s_%d.%s",
                directory, separator, timestamp, suffix, extension);
        }

        if (written < 0 || (size_t)written >= path_capacity) {
            return -1;
        }

        if (fopen_s(&existing, path, "r") != 0) {
            return 0;
        }
        fclose(existing);
    }

    return -1;
}

static int close_report(FILE* report)
{
    int write_failed = ferror(report);
    int close_failed = fclose(report);
    return (write_failed || close_failed != 0) ? -1 : 0;
}

static int write_csv_field(FILE* report, const char* value)
{
    const char* current;
    int quoted;

    quoted = strpbrk(value, ",\"\r\n") != NULL;
    if (quoted && fputc('"', report) == EOF) {
        return -1;
    }

    for (current = value; *current != '\0'; ++current) {
        if (*current == '"' && fputc('"', report) == EOF) {
            return -1;
        }
        if (fputc(*current, report) == EOF) {
            return -1;
        }
    }

    if (quoted && fputc('"', report) == EOF) {
        return -1;
    }
    return 0;
}

int report_generate_text(Product** head, const char* directory)
{
    char validated_directory[REPORT_PATH_CAPACITY];
    char path[REPORT_PATH_CAPACITY];
    Product* current;
    FILE* report = NULL;

    if (head == NULL || validate_directory(directory, validated_directory) != 0 ||
        build_report_path(validated_directory, "txt", path, sizeof(path)) != 0 ||
        fopen_s(&report, path, "w") != 0) {
        return -1;
    }

    if (fputs("Warehouse Inventory Report\n"
        "ID | Name | Quantity | Location\n", report) == EOF) {
        fclose(report);
        return -1;
    }

    for (current = *head; current != NULL; current = current->next) {
        if (fprintf(report, "%u | %s | %d | %s\n", current->id,
            current->name, current->quantity, current->location) < 0) {
            fclose(report);
            return -1;
        }
    }

    return close_report(report);
}

int report_generate_csv(Product** head, const char* directory)
{
    char validated_directory[REPORT_PATH_CAPACITY];
    char path[REPORT_PATH_CAPACITY];
    Product* current;
    FILE* report = NULL;

    if (head == NULL || validate_directory(directory, validated_directory) != 0 ||
        build_report_path(validated_directory, "csv", path, sizeof(path)) != 0 ||
        fopen_s(&report, path, "w") != 0) {
        return -1;
    }

    if (fputs("Product ID,Name,Quantity,Location\n", report) == EOF) {
        fclose(report);
        return -1;
    }

    for (current = *head; current != NULL; current = current->next) {
        if (fprintf(report, "%u,", current->id) < 0 ||
            write_csv_field(report, current->name) != 0 ||
            fprintf(report, ",%d,", current->quantity) < 0 ||
            write_csv_field(report, current->location) != 0 ||
            fputc('\n', report) == EOF) {
            fclose(report);
            return -1;
        }
    }

    return close_report(report);
}
