#include <furi.h>
#include <furi_hal.h>
#include <nfc/nfc_device.h>
#include <mjs.h>
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

    // Simulate: Open dictionary file, try keys
    // Real code: For each key, try authentication, return first valid key

    // For demo, pretend we found a key
    mjs_return(mjs, mjs_mk_string(mjs, "FF FF FF FF FF FF", ~0, 1));
}

// ---- Attack: Nested ----
static void js_nested_attack(struct mjs* mjs) {
    const char* key = mjs_get_string(mjs, mjs_arg(mjs, 0), NULL);

    // Simulate nested attack: Use known key to escalate
    // Real code: Use key to run nested authentication and extract more keys

    // For demo, return success (true)
    mjs_return(mjs, mjs_mk_boolean(mjs, true));
}

// ---- Attack: Hardnested ----
static void js_hardnested_attack(struct mjs* mjs) {
    // Real code: Perform hardnested attack

    // For demo, return failure (false)
    mjs_return(mjs, mjs_mk_boolean(mjs, false));
}

// ---- NFC Tag Dump ----
static void js_read_tag(struct mjs* mjs) {
    // Simulated tag data
    const char* uid = "04AABBCCDD";
    const char* type = "MIFARE Classic";
    uint8_t raw[16*16] = {0}; // Example: 16 sectors x 16 bytes (classic 1K)
    memset(raw, 0xAB, sizeof(raw)); // Dummy fill

    mjs_val_t tag_obj = make_tag_object(mjs, uid, type, raw, sizeof(raw));
    mjs_return(mjs, tag_obj);
}

// ---- Dump Writer ----
static void js_save_dump(struct mjs* mjs) {
    const char* path = mjs_get_string(mjs, mjs_arg(mjs, 0), NULL);
    const char* raw = mjs_get_string(mjs, mjs_arg(mjs, 1), NULL);

    // Real code: Write raw to file at path
    // Simulate: Always succeed
    mjs_return(mjs, mjs_mk_boolean(mjs, true));
}

// ---- Emulation Stub ----
static void js_emulate_card(struct mjs* mjs) {
    // Real code: Would trigger NFC emulation
    // Simulate: Always succeed
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
