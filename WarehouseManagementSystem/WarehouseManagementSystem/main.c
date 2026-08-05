#include <stdio.h>
#include <stdlib.h>
#include "inventory.h"
#include "Report.h"

int main(void)
{
	Product* head = NULL;
	//Product* listHead = malloc(sizeof(Product));
	inventory_add_product(&head, 1001U, "Hammer", 5, "A-01");
	inventory_add_product(&head, 2002U, "Nail", 10, "B-02");
	inventory_add_product(&head, 3003U, "Screw", 15, "C-03");
	inventory_add_product(&head, 4004U, "STUFF", 20, "D-04");
	report_generate_text(&head, "Reports/");
	report_generate_csv(&head, "Reports/");

}