#ifndef INVENTORY_H
#define INVENTORY_H

#define PRODUCT_NAME_LENGTH 64
#define PRODUCT_LOCATION_LENGTH 32

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct Product
    {
        unsigned int id;
        char name[PRODUCT_NAME_LENGTH];
        int quantity;
        char location[PRODUCT_LOCATION_LENGTH];
        struct Product* next;
    } Product;

    /**
     * @brief Adds a new product to the inventory.
     *
     * Author: Keshav Kumar Markan
     * Input: Inventory head, product ID, name, quantity, and location.
     * Output: Allocates and links a new Product node.
     * Purpose: Store a validated product in the inventory linked list.
     *
     * @return 0 on success, or -1 for invalid input, duplicate ID,
     * or allocation failure.
     */
    int inventory_add_product(
        Product** head,
        unsigned int id,
        const char* name,
        int quantity,
        const char* location
    );

    /**
     * @brief Updates the quantity of an existing product.
     *
     * Author: Keshav Kumar Markan
     * Input: Inventory head, product ID, and new quantity.
     * Output: Updates the matching Product quantity.
     * Purpose: Modify the stock quantity of an existing product.
     *
     * @return 0 on success, or -1 if the input is invalid or the
     * product is not found.
     */
    int inventory_update_quantity(
        Product* head,
        unsigned int id,
        int new_quantity
    );

    /**
     * @brief Deletes a product from the inventory.
     *
     * Author: Keshav Kumar Markan
     * Input: Inventory head and product ID.
     * Output: Unlinks and frees the matching Product node.
     * Purpose: Remove a product and release its allocated memory.
     *
     * @return 0 on success, or -1 if the input is invalid or the
     * product is not found.
     */
    int inventory_delete_product(
        Product** head,
        unsigned int id
    );

    /**
     * @brief Retrieves a product by its unique ID.
     *
     * Author: Keshav Kumar Markan
     * Input: Inventory head and product ID.
     * Output: Does not modify the inventory.
     * Purpose: Find and return an existing Product.
     *
     * @return Pointer to the matching Product, or NULL if not found.
     */
    Product* inventory_get_product(
        Product* head,
        unsigned int id
    );

    /**
     * @brief Frees every product in the inventory.
     *
     * Author: Keshav Kumar Markan
     * Input: Address of the Inventory head pointer.
     * Output: Frees all Product nodes and sets the head to NULL.
     * Purpose: Release Inventory memory during cleanup.
     */
    void inventory_free_all(Product** head);

#ifdef __cplusplus
}
#endif

#endif