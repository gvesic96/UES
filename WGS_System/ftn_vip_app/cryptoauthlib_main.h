/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#ifndef CRYPTOAUTHLIB_MAIN_H
#define CRYPTOAUTHLIB_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cryptoauthlib.h>

ATCAIfaceCfg CRYPTO_AUTHENTICATION_INTERFACE_desc;

/**
 * Init CRYPTO_AUTHENTICATION_INTERFACE
 */
void CRYPTO_AUTHENTICATION_INTERFACE_init(void);

/**
 * Example code for how to read serial number from CRYPTO_AUTHENTICATION_INTERFACE
 */
void CRYPTO_AUTHENTICATION_INTERFACE_example(void);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTOAUTHLIB_MAIN_H */
