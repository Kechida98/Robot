/*
 * Copyright (C) 2024 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BLUEKITCHEN
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

/**
 * @title Link Loss Service Client
 * 
 */

#ifndef LINK_LOSS_SERVICE_CLIENT_H
#define LINK_LOSS_SERVICE_CLIENT_H

#include <stdint.h>
#include "btstack_defines.h"
#include "bluetooth.h"
#include "btstack_linked_list.h"
#include "ble/gatt_client.h"
#include "ble/gatt_service_client.h"
#include "ble/gatt-service/link_loss_service_util.h"

#if defined __cplusplus
extern "C" {
#endif


/** 
 * @text The Link Loss Service Client 
 */
typedef enum {
    LINK_LOSS_SERVICE_CLIENT_STATE_IDLE = 0,
    LINK_LOSS_SERVICE_CLIENT_STATE_W4_CONNECTION,
    LINK_LOSS_SERVICE_CLIENT_STATE_READY,
    LINK_LOSS_SERVICE_CLIENT_STATE_W2_READ_CHARACTERISTIC_VALUE,
    LINK_LOSS_SERVICE_CLIENT_STATE_W4_READ_CHARACTERISTIC_VALUE_RESULT,
    LINK_LOSS_SERVICE_CLIENT_STATE_W2_WRITE_CHARACTERISTIC_VALUE,
    LINK_LOSS_SERVICE_CLIENT_STATE_W4_WRITE_CHARACTERISTIC_VALUE_RESULT
} link_loss_service_client_state_t;

typedef struct {
    btstack_linked_item_t item;

    gatt_service_client_connection_t basic_connection;
    btstack_packet_handler_t packet_handler;

    link_loss_service_client_state_t state;

    // Used for read characteristic queries
    uint8_t characteristic_index;

    // Used to store parameters for write characteristic
    uint8_t write_buffer[1];
} lls_client_connection_t;

/* API_START */

    
/**
 * @brief Initialize Link Loss Service. 
 */
void link_loss_service_client_init(void);

 /**
 * @brief Connect to a Link Loss Service instance of remote device. The client will automatically register for notifications.
 * The mute state is received via GATTSERVICE_SUBEVENT_LLS_CLIENT_MUTE event.
 * The mute state can be 0 - off, 1 - on, 2 - disabeled and it is valid if the ATT status is equal to ATT_ERROR_SUCCESS,
 * see ATT errors (see bluetooth.h) for other values.
 *   
 * Event GATTSERVICE_SUBEVENT_LLS_CLIENT_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
 * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no audio input control service is found, or ATT errors (see bluetooth.h). 
 *
 * @param con_handle
 * @param packet_handler
 * @param llc_connection
 * @param llc_characteristics_storage           storage for characteristics
 * @param llc_characteristics_num               == LINK_LOSS_SERVICE_CLIENT_NUM_CHARACTERISTICS
 * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED
 */
uint8_t link_loss_service_client_connect(
         hci_con_handle_t con_handle,
         btstack_packet_handler_t packet_handler,
         lls_client_connection_t * llc_connection,
         gatt_service_client_characteristic_t * llc_characteristics_storage, uint8_t llc_characteristics_num,
         uint16_t * lls_cid
);

uint8_t link_loss_service_client_read_alert_level(uint16_t lls_cid);
uint8_t link_loss_service_client_write_alert_level(uint16_t lls_cid, lls_alert_level_t alert_level);


/**
 * @brief Disconnect.
 * @param lls_cid
 * @return status
 */
uint8_t link_loss_service_client_disconnect(uint16_t lls_cid);

/**
 * @brief De-initialize Link Loss Service. 
 */
void link_loss_service_client_deinit(void);

/* API_END */

#if defined __cplusplus
}
#endif

#endif
