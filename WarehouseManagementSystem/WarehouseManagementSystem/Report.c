#include "Report.h"
#include "utilities.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define REPORT_PATH_CAPACITY 260U
#define REPORT_TIMESTAMP_CAPACITY 32U

/*
 * Author: Alex English and Ivan Immanuel Shaji
 * Input: Caller-supplied file path and writable destination buffer.
 * Output: Stores a validated path and returns 0, otherwise -1.
 * Purpose: Validate every Reports file-path input through Utilities.
 */
static int validate_file_path(const char* file_path, char* validated_path)
{
    size_t length;

    if (util_check_null(file_path) != 0 ||
        util_check_null(validated_path) != 0)
    {
        return -1;
    }

    length = strlen(file_path);

    if (length == 0U || length >= REPORT_PATH_CAPACITY)
    {
        return -1;
    }

    memcpy(validated_path, file_path, length + 1U);
    return util_validate_string(validated_path, REPORT_PATH_CAPACITY - 1U);
}

/*
 * Author: Alex English and Ivan Immanuel Shaji
 * Input: An open report stream.
 * Output: Writes the current local timestamp and returns 0, otherwise -1.
 * Purpose: Place the required generation timestamp inside each text report.
 */
static int write_timestamp_header(FILE* report)
{
    time_t current_time;
    struct tm local_time;
    char timestamp[REPORT_TIMESTAMP_CAPACITY];

    if (util_check_null(report) != 0)
    {
        return -1;
    }

    current_time = time(NULL);

    if (current_time == (time_t)-1 ||
        localtime_s(&local_time, &current_time) != 0 ||
        strftime(
            timestamp,
            sizeof(timestamp),
            "%Y-%m-%d %H:%M:%S",
            &local_time
        ) == 0U)
    {
        return -1;
    }

    return fprintf(report, "Generated: %s\n", timestamp) < 0 ? -1 : 0;
}

/*
 * Author: Alex English and Ivan Immanuel Shaji
 * Input: An open report stream.
 * Output: Closes the stream and returns 0 only when writing and closing succeed.
 * Purpose: Ensure Reports never leaves a file handle open between calls.
 */
static int close_report(FILE* report)
{
    int write_failed;
    int close_failed;

    if (util_check_null(report) != 0)
    {
        return -1;
    }

    write_failed = ferror(report);
    close_failed = fclose(report);
    return (write_failed != 0 || close_failed != 0) ? -1 : 0;
}

/*
 * Author: Alex English and Ivan Immanuel Shaji
 * Input: An open CSV stream and a validated field value.
 * Output: Writes one correctly escaped CSV field and returns 0, otherwise -1.
 * Purpose: Preserve commas, quotes, and line breaks in exported product data.
 */
static int write_csv_field(FILE* report, const char* value)
{
    const char* current;
    int quoted;

    if (util_check_null(report) != 0 || util_check_null(value) != 0)
    {
        return -1;
    }

    quoted = strpbrk(value, ",\"\r\n") != NULL;

    if (quoted != 0 && fputc('"', report) == EOF)
    {
        return -1;
    }

    for (current = value; *current != '\0'; ++current)
    {
        if (*current == '"' && fputc('"', report) == EOF)
        {
            return -1;
        }

        if (fputc(*current, report) == EOF)
        {
            return -1;
        }
    }

    if (quoted != 0 && fputc('"', report) == EOF)
    {
        return -1;
    }

    return 0;
}

/*
 * Author: Alex English and Ivan Immanuel Shaji
 * Input: Read-only inventory list and caller-supplied text-file path.
 * Output: Writes a timestamped inventory report and returns 0, otherwise -1.
 * Purpose: Generate the complete human-readable inventory report.
 */
int report_generate_text(const Product* head, const char* file_path)
{
    char validated_path[REPORT_PATH_CAPACITY];
    const Product* current;
    FILE* report = NULL;

    if (validate_file_path(file_path, validated_path) != 0 ||
        fopen_s(&report, validated_path, "w") != 0 ||
        report == NULL)
    {
        return -1;
    }

    if (fputs("Warehouse Inventory Report\n", report) == EOF ||
        write_timestamp_header(report) != 0 ||
        fputs("ID | Name | Quantity | Location\n", report) == EOF)
    {
        (void)fclose(report);
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        if (fprintf(
                report,
                "%u | %s | %d | %s\n",
                current->id,
                current->name,
                current->quantity,
                current->location
            ) < 0)
        {
            (void)fclose(report);
            return -1;
        }
    }

    if (close_report(report) != 0)
    {
        return -1;
    }

    printf("Text report generated: %s\n", validated_path);
    return 0;
}

/*
 * Author: Alex English and Ivan Immanuel Shaji
 * Input: Read-only inventory list and caller-supplied CSV-file path.
 * Output: Writes a CSV inventory report and returns 0, otherwise -1.
 * Purpose: Export the complete inventory for use by external tools.
 */
int report_generate_csv(const Product* head, const char* file_path)
{
    char validated_path[REPORT_PATH_CAPACITY];
    const Product* current;
    FILE* report = NULL;

    if (validate_file_path(file_path, validated_path) != 0 ||
        fopen_s(&report, validated_path, "w") != 0 ||
        report == NULL)
    {
        return -1;
    }

    if (fputs("Product ID,Name,Quantity,Location\n", report) == EOF)
    {
        (void)fclose(report);
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        if (fprintf(report, "%u,", current->id) < 0 ||
            write_csv_field(report, current->name) != 0 ||
            fprintf(report, ",%d,", current->quantity) < 0 ||
            write_csv_field(report, current->location) != 0 ||
            fputc('\n', report) == EOF)
        {
            (void)fclose(report);
            return -1;
        }
    }

    if (close_report(report) != 0)
    {
        return -1;
    }

    printf("CSV report generated: %s\n", validated_path);
    return 0;
}
