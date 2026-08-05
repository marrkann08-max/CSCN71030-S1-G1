#ifndef ALERTS_H
#define ALERTS_H

typedef struct Product Product;

#ifdef __cplusplus
extern "C" {
#endif

int check_low_stock(
    const Product* head,
    int threshold,
    const char* file_path
);

#ifdef __cplusplus
}
#endif

#endif
