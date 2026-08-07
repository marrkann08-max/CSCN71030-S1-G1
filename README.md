# Warehouse Management System

This is a terminal-based warehouse management system. It supports authentication, configuration, inventory, orders, searching, reports, low-stock alerts, and transaction logging.

## Team modules

- Ivan Immanuel Shaji - Main and Config
- Keshav Kumar Markan - Inventory and Utilities
- Inderpreet Kaur Hundal - Orders and Search
- Alex English - Logger and Reports
- Navkirat Kaur - Authentication and Alerts

## Build and run

Open `WarehouseManagementSystem/WarehouseManagementSystem.slnx` in Visual Studio and build the solution. The application supports x64 and x86 Debug and Release configurations.

Optional command-line settings can be supplied when the program starts:

```text
--threshold 5 --seed 2026
```

The program also reads `config.txt`. Command-line values take priority over values from the file.

Demo accounts:

| Username | Password |
| --- | --- |
| admin | Admin123 |
| manager | Manager123 |
| warehouse | Warehouse123 |

## Tests

Open Test Explorer in Visual Studio and select **Run All**. The solution contains ten test projects and 60 automated tests.

## Doxygen

Run the following command from the repository root:

```text
doxygen Doxyfile
```

The generated HTML documentation will be placed in `docs/doxygen/html`. Zip that HTML folder for the final Doxygen submission.
