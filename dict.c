#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <nfc/nfc.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static void js_dict_attack(struct mjs* mjs) {
    const char* dict_path = mjs_get_string(mjs, mjs_arg(mjs, 0), NULL);

    // Open storage
    Storage* storage = furi_record_open("storage");
    if(!storage) {
        printf("Error: Could not open storage.\n");
        mjs_return(mjs, mjs_mk_null());
        return;
    }

    File* file = storage_file_alloc(storage);
    if(!file) {
        printf("Error: Could not allocate file.\n");
        furi_record_close("storage");
        mjs_return(mjs, mjs_mk_null());
        return;
    }

    // Initialize NFC
    NfcDevice* nfc = furi_record_open("nfc");
    if(!nfc) {
        printf("Error: Could not open NFC device.\n");
        storage_file_free(file);
        furi_record_close("storage");
        mjs_return(mjs, mjs_mk_null());
        return;
    }

    NfcTagInfo tag_info;
    printf("Scanning NFC tag...\n");

    if(!nfc_device_scan_tag(nfc, &tag_info, 5000)) {
        printf("No tag detected. Hold an NFC tag near the Flipper Zero.\n");
        goto cleanup;
    }

    if(tag_info.type != NFC_TYPE_MIFARE_CLASSIC) {
        printf("This tag is not MIFARE Classic. Type: %d\n", tag_info.type);
        goto cleanup;
    }

    printf("MIFARE Classic tag detected!\nUID: ");
    for(int i=0; i<tag_info.uid_len; i++) printf("%02X ", tag_info.uid[i]);
    printf("\nStarting dictionary attack...\n");

    if(!storage_file_open(file, dict_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        printf("Error: Could not open dictionary file: %s\n", dict_path);
        goto cleanup;
    }

    char line[32];
    int line_count = 0;
    bool success = false;

    while(storage_file_gets(file, line, sizeof(line))) {
        // Remove newline
        line[strcspn(line, "\r\n")] = 0;
        line_count++;

        // Allow user to cancel
        InputEvent event;
        if(furi_hal_input_event_get(&event, 1)) {
            if(event.type == InputTypePress && event.key == InputKeyBack) {
                printf("Attack cancelled by user.\n");
                break;
            }
        }

        // Only process lines of 12 hex chars
        if(strlen(line) != 12) continue;

        uint8_t key[6];
        for(int i=0; i<6; i++) {
            sscanf(line + i*2, "%2hhx", &key[i]);
        }

        // Attempt authentication on sector 0
        printf("Trying key %s...\n", line);
        if(nfc_device_mifare_classic_authenticate(nfc, &tag_info, 0, key, NfcKeyTypeA)) {
            printf("===================================\n");
            printf("SUCCESS! Key found: %s\n", line);
            printf("===================================\n");
            success = true;
            break;
        }

        if(line_count % 10 == 0) {
            printf("%d keys tried so far...\n", line_count);
        }
    }

    if(!success) {
        printf("No valid key found in dictionary.\n");
    }

    storage_file_close(file);

cleanup:
    storage_file_free(file);
    furi_record_close("storage");
    furi_record_close("nfc");
    mjs_return(mjs, success ? mjs_mk_string(mjs, line, ~0, 1) : mjs_mk_null());
}
