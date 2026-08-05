#pragma once
#include "inventory.h"

// Generate the name of the file into the string given
int genReportName(char* buffer);
// Generate the report
int report_generate_text(Product** head, const char* filePath);

int report_generate_csv(Product** head, const char* filePath);