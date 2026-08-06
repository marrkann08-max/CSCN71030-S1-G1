//--------------------------------------------------------------------------
// main.c - Entry point and orchestrator for the Warehouse Management System
//          Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#include "main.h"

#include "Logger.h"
#include "Report.h"
#include "alerts.h"
#include "authentication.h"
#include "orders.h"
#include "search.h"
#include "utilities.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Author: Ivan Immanuel Shaji
 * Input: Characters remaining in standard input.
 * Output: Removes characters through the next newline or end of file.
 * Purpose: Recover safely after an overlength interactive entry.
 */
static void discard_input_remainder(void)
{
    int character;

    do
    {
        character = getchar();
    } while (character != '\n' && character != EOF);
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Prompt, writable buffer, capacity, and permitted stored length.
 * Output: Stores one trimmed field and returns 0, otherwise -1.
 * Purpose: Read bounded text consistently for every interactive module.
 */
static int read_validated_line(
    const char* prompt,
    char* buffer,
    size_t capacity,
    size_t maximum_length
)
{
    size_t length;

    if (prompt == NULL || buffer == NULL || capacity < 2U ||
        maximum_length == 0U || maximum_length >= capacity)
    {
        return -1;
    }

    fputs(prompt, stdout);
    fflush(stdout);

    if (fgets(buffer, (int)capacity, stdin) == NULL)
    {
        return -1;
    }

    length = strlen(buffer);

    if (length > 0U && buffer[length - 1U] == '\n')
    {
        buffer[--length] = '\0';
    }
    else if (!feof(stdin))
    {
        discard_input_remainder();
        return -1;
    }

    if (length > 0U && buffer[length - 1U] == '\r')
    {
        buffer[length - 1U] = '\0';
    }

    return util_validate_string(buffer, maximum_length);
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Prompt, inclusive signed range, and output pointer.
 * Output: Stores a validated integer and returns 0, otherwise -1.
 * Purpose: Convert interactive signed-number input through Utilities.
 */
static int prompt_int(
    const char* prompt,
    int minimum,
    int maximum,
    int* out_value
)
{
    char input[MENU_INPUT_CAPACITY];

    if (out_value == NULL || minimum > maximum ||
        read_validated_line(
            prompt,
            input,
            sizeof(input),
            sizeof(input) - 1U
        ) != 0)
    {
        return -1;
    }

    return util_string_to_int(input, minimum, maximum, out_value);
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Prompt, inclusive unsigned range, and output pointer.
 * Output: Stores a validated unsigned integer and returns 0, otherwise -1.
 * Purpose: Convert interactive unsigned-number input through Utilities.
 */
static int prompt_uint(
    const char* prompt,
    unsigned int minimum,
    unsigned int maximum,
    unsigned int* out_value
)
{
    char input[MENU_INPUT_CAPACITY];

    if (out_value == NULL || minimum > maximum ||
        read_validated_line(
            prompt,
            input,
            sizeof(input),
            sizeof(input) - 1U
        ) != 0)
    {
        return -1;
    }

    return util_string_to_uint(input, minimum, maximum, out_value);
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Read-only Product pointer.
 * Output: Writes the Product fields to standard output.
 * Purpose: Give Main and Search one consistent Product presentation.
 */
static void display_product(const Product* product)
{
    if (product == NULL)
    {
        printf("Item not found.\n");
        return;
    }

    printf(
        "ID: %u | Name: %s | Quantity: %d | Location: %s\n",
        product->id,
        product->name,
        product->quantity,
        product->location
    );

    if (product->quantity == 0)
    {
        printf("Status: Unavailable\n");
    }
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Read-only Inventory head.
 * Output: Writes every Product or an empty-inventory message.
 * Purpose: Display the shared Inventory without modifying it.
 */
static void display_inventory(const Product* head)
{
    const Product* current = head;

    if (current == NULL)
    {
        printf("Inventory is empty.\n");
        return;
    }

    while (current != NULL)
    {
        display_product(current);
        current = current->next;
    }
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Read-only Search result.
 * Output: Writes each matching Product and the result count.
 * Purpose: Present non-owning Search results consistently.
 */
static void display_search_results(const SearchResult* result)
{
    int index;

    if (result == NULL)
    {
        return;
    }

    printf("Matches: %d\n", result->count);

    if (result->count == 0)
    {
        printf("Item not found.\n");
        return;
    }

    for (index = 0; index < result->count; ++index)
    {
        display_product(result->products[index]);
    }
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Address of the shared Inventory head.
 * Output: Performs validated Inventory actions and returns 0, otherwise -1.
 * Purpose: Connect Main's Inventory submenu to the Inventory module.
 */
static int handle_inventory(Product** head)
{
    int choice;
    unsigned int id;
    int quantity;
    char name[PRODUCT_NAME_LENGTH];
    char location[PRODUCT_LOCATION_LENGTH];

    if (head == NULL)
    {
        return -1;
    }

    for (;;)
    {
        printf(
            "\nInventory\n"
            "1. Add product\n"
            "2. Update quantity\n"
            "3. Delete product\n"
            "4. Display inventory\n"
            "0. Back\n"
        );

        if (prompt_int("Enter your selection: ", 0, 4, &choice) != 0)
        {
            fprintf(stderr, "Invalid Inventory selection.\n");
            continue;
        }

        if (choice == 0)
        {
            return 0;
        }

        if (choice == 4)
        {
            display_inventory(*head);
            continue;
        }

        if (prompt_uint("Product ID: ", 1U, UINT_MAX, &id) != 0)
        {
            fprintf(stderr, "Invalid Product ID.\n");
            continue;
        }

        if (choice == 1)
        {
            if (read_validated_line(
                    "Product name: ",
                    name,
                    sizeof(name),
                    PRODUCT_NAME_LENGTH - 1U
                ) != 0 ||
                prompt_int("Quantity: ", 0, INT_MAX, &quantity) != 0 ||
                read_validated_line(
                    "Location: ",
                    location,
                    sizeof(location),
                    PRODUCT_LOCATION_LENGTH - 1U
                ) != 0)
            {
                fprintf(stderr, "Invalid product details.\n");
                continue;
            }

            if (inventory_add_product(
                    head,
                    id,
                    name,
                    quantity,
                    location
                ) != 0)
            {
                fprintf(stderr, "Unable to add product. Check for a duplicate ID.\n");
                continue;
            }

            printf("Product added successfully.\n");
        }
        else if (choice == 2)
        {
            if (prompt_int("New quantity: ", 0, INT_MAX, &quantity) != 0 ||
                inventory_update_quantity(*head, id, quantity) != 0)
            {
                fprintf(stderr, "Unable to update product quantity.\n");
                continue;
            }

            printf("Product quantity updated successfully.\n");
        }
        else if (inventory_delete_product(head, id) != 0)
        {
            fprintf(stderr, "Unable to delete product.\n");
        }
        else
        {
            printf("Product deleted successfully.\n");
        }
    }
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Shared Inventory and read-only Config structure.
 * Output: Processes or simulates Orders and returns 0, otherwise -1.
 * Purpose: Pass user order data and Config.seed into the Orders module.
 */
static int handle_orders(Product* head, const Config* config)
{
    int choice;
    unsigned int id;
    int quantity;
    char order_type;
    Order* order;
    int result;

    if (config == NULL)
    {
        return -1;
    }

    if (head == NULL)
    {
        printf("Inventory is empty. Add a product before processing Orders.\n");
        return 0;
    }

    printf(
        "\nOrders\n"
        "1. Receive stock\n"
        "2. Dispatch stock\n"
        "3. Simulate deterministic order\n"
        "0. Back\n"
    );

    if (prompt_int("Enter your selection: ", 0, 3, &choice) != 0)
    {
        fprintf(stderr, "Invalid Orders selection.\n");
        return 0;
    }

    if (choice == 0)
    {
        return 0;
    }

    if (choice == 3)
    {
        result = order_simulate(head, config->seed);
        printf(
            result == ORDER_SUCCESS
                ? "Simulated order completed using seed %u.\n"
                : "Simulated order was rejected using seed %u.\n",
            config->seed
        );
        return 0;
    }

    if (prompt_uint("Product ID: ", 1U, UINT_MAX, &id) != 0 ||
        prompt_int("Order quantity: ", 1, INT_MAX, &quantity) != 0)
    {
        fprintf(stderr, "Invalid order details.\n");
        return 0;
    }

    order_type = choice == 1 ? ORDER_RECEIVE : ORDER_DISPATCH;
    order = order_create(id, quantity, order_type);

    if (order == NULL)
    {
        fprintf(stderr, "Unable to create order.\n");
        return 0;
    }

    result = order_process(head, order);
    order_free(order);

    if (result != ORDER_SUCCESS)
    {
        fprintf(stderr, "Order rejected. Verify the Product ID and stock level.\n");
    }
    else
    {
        printf("Order completed successfully.\n");
    }

    return 0;
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Shared Inventory and read-only Config structure.
 * Output: Performs one Search action and returns 0, otherwise -1.
 * Purpose: Connect Main to all Search operations and recent history.
 */
static int handle_search(Product* head, const Config* config)
{
    int choice;
    unsigned int id;
    int minimum;
    int maximum;
    int result_count;
    int history_count;
    int index;
    char keyword[SEARCH_TEXT_LENGTH];
    char criteria[SEARCH_TEXT_LENGTH];
    Product* product;
    SearchResult result = { NULL, 0 };
    SearchHistoryEntry entries[SEARCH_HISTORY_CAPACITY];

    if (config == NULL)
    {
        return -1;
    }

    printf(
        "\nSearch\n"
        "1. Search by Product ID\n"
        "2. Search by name\n"
        "3. Search below configured threshold\n"
        "4. Search by quantity range\n"
        "5. Show all products\n"
        "6. Show recent searches\n"
        "7. Clear recent searches\n"
        "0. Back\n"
    );

    if (prompt_int("Enter your selection: ", 0, 7, &choice) != 0)
    {
        fprintf(stderr, "Invalid Search selection.\n");
        return 0;
    }

    if (choice == 0)
    {
        return 0;
    }

    if (choice == 6)
    {
        history_count = search_get_history(entries);

        if (history_count < 0)
        {
            return -1;
        }

        if (history_count == 0)
        {
            printf("Search history is empty.\n");
        }

        for (index = 0; index < history_count; ++index)
        {
            printf(
                "%d. %s (%d result%s)\n",
                index + 1,
                entries[index].criteria,
                entries[index].resultCount,
                entries[index].resultCount == 1 ? "" : "s"
            );
        }

        return 0;
    }

    if (choice == 7)
    {
        search_clear_history();
        printf("Search history cleared.\n");
        return 0;
    }

    if (choice == 1)
    {
        if (prompt_uint("Product ID: ", 1U, UINT_MAX, &id) != 0)
        {
            fprintf(stderr, "Invalid Product ID.\n");
            return 0;
        }

        product = search_by_id(head, id);
        result_count = product == NULL ? 0 : 1;
        (void)snprintf(criteria, sizeof(criteria), "Product ID %u", id);
        printf("Matches: %d\n", result_count);
        display_product(product);
    }
    else if (choice == 2)
    {
        if (read_validated_line(
                "Name keyword: ",
                keyword,
                sizeof(keyword),
                SEARCH_TEXT_LENGTH - 1U
            ) != 0 ||
            search_by_name(head, keyword, &result) != 0)
        {
            fprintf(stderr, "Unable to search by name.\n");
            return 0;
        }

        result_count = result.count;
        (void)snprintf(criteria, sizeof(criteria), "Name: %.56s", keyword);
        display_search_results(&result);
        search_free_results(&result);
    }
    else if (choice == 3)
    {
        if (search_below_threshold(head, config->threshold, &result) != 0)
        {
            return -1;
        }

        result_count = result.count;
        (void)snprintf(
            criteria,
            sizeof(criteria),
            "Quantity below %d",
            config->threshold
        );
        display_search_results(&result);
        search_free_results(&result);
    }
    else if (choice == 4)
    {
        if (prompt_int("Minimum quantity: ", 0, INT_MAX, &minimum) != 0 ||
            prompt_int("Maximum quantity: ", minimum, INT_MAX, &maximum) != 0 ||
            search_by_quantity_range(head, minimum, maximum, &result) != 0)
        {
            fprintf(stderr, "Invalid quantity range.\n");
            return 0;
        }

        result_count = result.count;
        (void)snprintf(
            criteria,
            sizeof(criteria),
            "Quantity %d to %d",
            minimum,
            maximum
        );
        display_search_results(&result);
        search_free_results(&result);
    }
    else
    {
        if (search_all_products(head, &result) != 0)
        {
            return -1;
        }

        result_count = result.count;
        (void)snprintf(criteria, sizeof(criteria), "All products");
        display_search_results(&result);
        search_free_results(&result);
    }

    if (search_record_history(criteria, result_count) != 0)
    {
        fprintf(stderr, "Warning: Unable to record Search history.\n");
    }

    return 0;
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Read-only Inventory head.
 * Output: Generates selected report files and returns 0, otherwise -1.
 * Purpose: Pass the shared Inventory and exact paths to Reports.
 */
static int handle_reports(const Product* head)
{
    int choice;
    int failed = 0;

    printf(
        "\nReports\n"
        "1. Generate text report\n"
        "2. Generate CSV report\n"
        "3. Generate both reports\n"
        "0. Back\n"
    );

    if (prompt_int("Enter your selection: ", 0, 3, &choice) != 0)
    {
        fprintf(stderr, "Invalid Reports selection.\n");
        return 0;
    }

    if (choice == 0)
    {
        return 0;
    }

    if ((choice == 1 || choice == 3) &&
        report_generate_text(head, MAIN_TEXT_REPORT_PATH) != 0)
    {
        fprintf(stderr, "Unable to generate the text report.\n");
        failed = 1;
    }

    if ((choice == 2 || choice == 3) &&
        report_generate_csv(head, MAIN_CSV_REPORT_PATH) != 0)
    {
        fprintf(stderr, "Unable to generate the CSV report.\n");
        failed = 1;
    }

    return failed == 0 ? 0 : -1;
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Read-only Inventory head and configured threshold.
 * Output: Writes low-stock alerts and returns 0, otherwise -1.
 * Purpose: Pass Config.threshold and the alert path to Alerts.
 */
static int handle_alerts(const Product* head, int threshold)
{
    int alert_count;

    if (head == NULL)
    {
        printf("Inventory is empty. No low-stock alerts were generated.\n");
        return 0;
    }

    alert_count = check_low_stock(head, threshold, MAIN_ALERT_FILE_PATH);

    if (alert_count < 0)
    {
        fprintf(stderr, "Unable to generate low-stock alerts.\n");
        return -1;
    }

    printf(
        "%d low-stock alert%s written to %s.\n",
        alert_count,
        alert_count == 1 ? "" : "s",
        MAIN_ALERT_FILE_PATH
    );
    return 0;
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Startup argument count and startup argument array.
 * Output: Returns EXIT_SUCCESS after clean shutdown, otherwise EXIT_FAILURE.
 * Purpose: Authenticate, initialize modules, run Main, and release resources.
 */
int main(int argc, char* argv[])
{
    Config config;
    Product* head = NULL;
    char username[AUTH_FIELD_SIZE];
    int menu_result;

    if (load_config(argc, argv, &config) != 0)
    {
        fprintf(stderr, "Error: Unable to initialize the configuration.\n");
        return EXIT_FAILURE;
    }

    if (authentication_login(username, sizeof(username)) != 0)
    {
        fprintf(stderr, "Error: Authentication failed.\n");
        return EXIT_FAILURE;
    }

    if (logger_initialize(MAIN_LOG_FILE_PATH, username) != 0)
    {
        fprintf(stderr, "Error: Unable to initialize Logger.\n");
        return EXIT_FAILURE;
    }

    order_set_logger(logger_write_transaction);

    if (logger_write_string("Authentication successful.") != 0)
    {
        fprintf(stderr, "Error: Unable to write the session log.\n");
        logger_shutdown();
        return EXIT_FAILURE;
    }

    printf("Authentication successful. Welcome, %s.\n", username);
    print_config(&config);
    menu_result = run_menu(&head, &config);

    search_clear_history();
    inventory_free_all(&head);
    logger_shutdown();

    return menu_result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: None.
 * Output: Writes all available menu choices to standard output.
 * Purpose: Display the Warehouse Management System menu.
 */
void display_menu(void)
{
    printf("\nWarehouse Management System\n");
    printf("1. Inventory\n");
    printf("2. Orders\n");
    printf("3. Search\n");
    printf("4. Reports\n");
    printf("5. Alerts\n");
    printf("0. Exit\n");
    printf("Enter your selection: ");
    fflush(stdout);
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Editable menu text and a MenuChoice output pointer.
 * Output: Stores a validated choice and returns 0, otherwise returns -1.
 * Purpose: Convert validated character input into a menu selection.
 */
int parse_menu_choice(char* input, MenuChoice* out_choice)
{
    int converted_choice;

    if (util_check_null(input) != 0 || util_check_null(out_choice) != 0)
    {
        return -1;
    }

    if (util_validate_string(input, MENU_INPUT_CAPACITY - 1U) != 0)
    {
        return -1;
    }

    if (util_string_to_int(input, (int)MENU_EXIT, (int)MENU_ALERTS,
            &converted_choice) != 0)
    {
        return -1;
    }

    *out_choice = (MenuChoice)converted_choice;
    return 0;
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: A MenuChoice output pointer.
 * Output: Reads and stores one validated selection, returning 0 or -1.
 * Purpose: Read one menu selection safely from standard input.
 */
int read_menu_choice(MenuChoice* out_choice)
{
    char input[MENU_INPUT_CAPACITY];
    size_t length;

    if (util_check_null(out_choice) != 0)
    {
        return -1;
    }

    if (fgets(input, (int)sizeof(input), stdin) == NULL)
    {
        return -1;
    }

    length = strlen(input);

    if (length > 0U && input[length - 1U] == '\n')
    {
        input[--length] = '\0';
    }
    else if (!feof(stdin))
    {
        discard_input_remainder();
        return -1;
    }

    if (length > 0U && input[length - 1U] == '\r')
    {
        input[length - 1U] = '\0';
    }

    return parse_menu_choice(input, out_choice);
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Selected menu value, shared Inventory pointer, and Config pointer.
 * Output: Calls the selected module and returns its success or failure status.
 * Purpose: Centralize switch-based dispatch and module status checking.
 */
int dispatch_menu_option(
    int choice,
    Product** head,
    const Config* config
)
{
    if (head == NULL || config == NULL)
    {
        return -1;
    }

    switch ((MenuChoice)choice)
    {
    case MENU_EXIT:
        return 0;

    case MENU_INVENTORY:
        return handle_inventory(head);

    case MENU_ORDERS:
        return handle_orders(*head, config);

    case MENU_SEARCH:
        return handle_search(*head, config);

    case MENU_REPORTS:
        return handle_reports(*head);

    case MENU_ALERTS:
        return handle_alerts(*head, config->threshold);

    default:
        return -1;
    }
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Shared Inventory pointer and read-only Config pointer.
 * Output: Runs until Exit/EOF and returns 0, or -1 for a system failure.
 * Purpose: Control the validated interactive application menu loop.
 */
int run_menu(Product** head, const Config* config)
{
    MenuChoice choice;

    if (head == NULL || config == NULL)
    {
        return -1;
    }

    for (;;)
    {
        display_menu();

        if (read_menu_choice(&choice) != 0)
        {
            if (ferror(stdin))
            {
                fprintf(stderr, "Error: Unable to read menu input.\n");
                return -1;
            }

            if (feof(stdin))
            {
                printf("\nInput ended. Exiting the system.\n");
                return 0;
            }

            fprintf(stderr, "Invalid selection. Enter a number from 0 to 5.\n");
            continue;
        }

        if (choice == MENU_EXIT)
        {
            printf("Exiting the Warehouse Management System.\n");
            return 0;
        }

        if (dispatch_menu_option((int)choice, head, config) != 0)
        {
            fprintf(stderr, "The selected operation could not be completed.\n");
        }
    }
}
