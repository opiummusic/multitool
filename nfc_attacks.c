#include <furi.h>
#include <furi_hal.h>
#include <nfc/nfc_device.h>
#include <mjs.h>
#include <storage/storage.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// ---- Helper: Simulated NFC tag dump ----
static mjs_val_t make_tag_object(struct mjs* mjs, const char* uid, const char* type, const uint8_t* raw, size_t raw_len) {
    mjs_val_t obj = mjs_mk_object(mjs);
    mjs_set(mjs, obj, "uid", ~0, mjs_mk_string(mjs, uid, ~0, 1));
    mjs_set(mjs, obj, "type", ~0, mjs_mk_string(mjs, type, ~0, 1));
    mjs_set(mjs, obj, "raw", ~0, mjs_mk_string(mjs, (const char*)raw, raw_len, 1));
    return obj;
}

// ---- Attack: Dictionary brute force ----
static void js_dict_attack(struct mjs* mjs) {
    const char* dict_path = mjs_get_string(mjs, mjs_arg(mjs, 0), NULL);

    // Open dictionary file
    Storage* storage = furi_record_open("storage");
    File* file = storage_file_alloc(storage);

    // Open NFC device
    NfcDevice* nfc = furi_record_open("nfc");
    NfcTagInfo tag_info;
    bool tag_present = false;
    // Wait for a tag
    if(nfc_device_scan_tag(nfc, &tag_info, 2000)) { // 2s timeout
        tag_present = true;
    }

    if(!tag_present) {
        storage_file_free(file);
        furi_record_close("storage");
        furi_record_close("nfc");
        mjs_return(mjs, mjs_mk_null());
        return;
    }

    // Try keys
    if(storage_file_open(file, dict_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        char line[32];
        while(storage_file_gets(file, line, sizeof(line))) {
            // Remove trailing newline
            line[strcspn(line, "\r\n")] = 0;

            // Convert hex string to 6-byte key
            uint8_t key[6] = {0};
            if(strlen(line) == 12) {
                for(int i=0; i<6; i++) {
                    sscanf(line + i*2, "%2hhx", &key[i]);
                }

                // Try authenticating sector 0 with key A
                bool success = false;
                // --- Flipper API: You may need to adapt this line ---
                if(nfc_device_mifare_classic_authenticate(nfc, &tag_info, 0, key, NfcKeyTypeA)) {
                    success = true;
                }
                // ---------------------------------------------------

                if(success) {
                    storage_file_close(file);
                    storage_file_free(file);
                    furi_record_close("storage");
                    furi_record_close("nfc");
                    mjs_return(mjs, mjs_mk_string(mjs, line, ~0, 1));
                    return;
                }
            }
        }
        storage_file_close(file);
    }

    storage_file_free(file);
    furi_record_close("storage");
    furi_record_close("nfc");

    // No key found
    mjs_return(mjs, mjs_mk_null());
}

// ---- Attack: Nested ----
static void js_nested_attack(struct mjs* mjs) {
    // ... unchanged, you may implement real logic if desired ...
    const char* key = mjs_get_string(mjs, mjs_arg(mjs, 0), NULL);
    bool success = false;
    if(strcmp(key, "FFFFFFFFFFFF") == 0) success = true;
    mjs_return(mjs, mjs_mk_boolean(mjs, success));
}

// ---- Attack: Hardnested ----
static void js_hardnested_attack(struct mjs* mjs) {
    // ... unchanged ...
    mjs_return(mjs, mjs_mk_boolean(mjs, false));
}

// ---- NFC Tag Dump ----
static void js_read_tag(struct mjs* mjs) {
    // Simulated tag data, you can implement real reading if desired
    const char* uid = "04AABBCCDD";
    const char* type = "MIFARE Classic";
    uint8_t raw[16*16] = {0}; // Example: 16 sectors x 16 bytes (classic 1K)
    memset(raw, 0xAB, sizeof(raw)); // Dummy fill

    mjs_val_t tag_obj = make_tag_object(mjs, uid, type, raw, sizeof(raw));
    mjs_return(mjs, tag_obj);
}

// ---- Dump Writer ----
static void js_save_dump(struct mjs* mjs) {
    // ... unchanged ...
    const char* path = mjs_get_string(mjs, mjs_arg(mjs, 0), NULL);
    const char* raw = mjs_get_string(mjs, mjs_arg(mjs, 1), NULL);
    mjs_return(mjs, mjs_mk_boolean(mjs, true));
}

// ---- Emulation Stub ----
static void js_emulate_card(struct mjs* mjs) {
    // ... unchanged ...
    mjs_return(mjs, mjs_mk_boolean(mjs, true));
}

// ---- Binding Table ----
static const mjs_cfunction_entry mjs_nfc_attacks_funcs[] = {
    {"dict_attack", js_dict_attack},
    {"nested_attack", js_nested_attack},
    {"hardnested_attack", js_hardnested_attack},
    {"read_tag", js_read_tag},
    {"save_dump", js_save_dump},
    {"emulate_card", js_emulate_card},
};

// ---- Init ----
void mjs_init_nfc_attacks(struct mjs* mjs) {
    mjs_val_t obj = mjs_mk_object(mjs);
    for(size_t i=0; i<COUNT_OF(mjs_nfc_attacks_funcs); i++) {
        mjs_set(mjs, obj,
            mjs_nfc_attacks_funcs[i].name, ~0,
            mjs_mk_foreign_func(mjs, mjs_nfc_attacks_funcs[i].func));
    }
    mjs_set(mjs, mjs_get_global(mjs), "nfc_attacks", ~0, obj);
}