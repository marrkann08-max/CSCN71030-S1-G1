# Warehouse Management System

The Warehouse Management System is a terminal-based C application for managing common warehouse activities. It provides user authentication, startup configuration, inventory management, receiving and dispatching orders, product searches, low-stock alerts, inventory reports, and transaction logging.

## Features

- Authenticate warehouse users before allowing access
- Load threshold and random-seed settings from `config.txt` or command-line arguments
- Add, update, delete, display, and release inventory products
- Receive and dispatch stock while preventing invalid quantity changes
- Search products by ID, name, quantity, and threshold
- Generate low-stock alerts without modifying inventory
- Generate text and CSV inventory reports
- Record authenticated sessions and approved or rejected transactions
- Exit cleanly and release allocated memory and open files

## Team modules

| Team member | Modules |
| --- | --- |
| Ivan Immanuel Shaji | Main and Config |
| Keshav Kumar Markan | Inventory and Utilities |
| Inderpreet Kaur Hundal | Orders and Search |
| Alex English | Logger and Reports |
| Navkirat Kaur | Authentication and Alerts |

## Requirements

- Windows
- Visual Studio with the **Desktop development with C++** workload
- Doxygen, only when regenerating the HTML documentation

## Build and run

1. Clone the repository:

   ```powershell
   git clone https://github.com/marrkann08-max/CSCN71030-S1-G1.git
   ```

2. Open `WarehouseManagementSystem/WarehouseManagementSystem.slnx` in Visual Studio.
3. Select an x64 or x86 Debug/Release configuration.
4. Build the solution and set `WarehouseManagementSystem` as the startup project.
5. Run the application.

The application reads `config.txt` when it starts. The supported settings are:

```text
threshold=5
seed=2026
```

The same values can be supplied through command-line arguments:

```text
--threshold 5 --seed 2026
```

Command-line values take priority over values from `config.txt`.

## Demo accounts

| Username | Password |
| --- | --- |
| `admin` | `Admin123` |
| `manager` | `Manager123` |
| `warehouse` | `Warehouse123` |

## Generated files

Depending on the operations selected, the application creates:

- `warehouse.log` - session and transaction log
- `low_stock.txt` - current low-stock alert report
- `inventory_report.txt` - text inventory report
- `inventory_report.csv` - CSV inventory report

## Testing

The Visual Studio solution contains ten test projects and 60 automated tests covering every module. Open **Test Explorer** and select **Run All** to execute the full suite.

The three completed application-level workflows are recorded in [docs/application-test-results.md](docs/application-test-results.md).

Final verification result: **60 passed, 0 failed**.

## Doxygen documentation

From the repository root, run:

```powershell
doxygen Doxyfile
```

The generated documentation will be available at:

```text
docs/doxygen/html/index.html
```
