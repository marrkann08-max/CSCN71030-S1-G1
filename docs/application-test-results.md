# Application-Level Test Results

- Tested by: Ivan Immanuel Shaji
- Date: August 7, 2026
- Branch: `main`
- Commit: `c05e5da`
- Configuration: Debug x64
- Tracking issue: #23
- Overall result: PASS

## APP-I-001 - Startup, Authentication and Shutdown

The application accepted `--threshold 5 --seed 2026`, authenticated the admin account, and displayed threshold 5 and seed 2026. Menu choice 9 was rejected without crashing, the menu was displayed again, and choice 0 exited cleanly with exit code 0.

Result: PASS

## APP-I-002 - Shared Inventory Workflow

Product 1001 was added as Safety Gloves with quantity 3 and was reported as low stock. Receiving 4 units changed the quantity to 7, dispatching 2 units changed it to 5, and a case-insensitive search for `gloves` returned the same product and appeared in search history. Alerts correctly reported no low-stock product when the quantity equalled the threshold.

Result: PASS

## APP-I-003 - Reports and Logger Workflow

Receiving 3 units changed Hammer from quantity 5 to 8. Dispatching 50 units was rejected and did not change Inventory. The text and CSV reports contained Hammer with quantity 8 and Nails with quantity 10. `warehouse.log` contained the authenticated session, approved Receive transaction, rejected Dispatch transaction, and clean shutdown entry.

Result: PASS
