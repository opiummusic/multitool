#include <furi.h>
#include <furi_hal.h>
#include <nfc/nfc_device.h>
#include <mjs.h>

// Example: dictionary attack stub
static void js_dict_attack(struct mjs* mjs) {
    const char* dict_path = mjs_get_string(mjs, mjs_arg(mjs, 0), NULL);

    // Here you would run the C code:
    // mifare_classic_dict_attack(dict_path);

    // For now return success/found key
    mjs_return(mjs, mjs_mk_string(mjs, "FF FF FF FF FF FF", ~0, 1));
}

// Binding table
static const mjs_cfunction_entry mjs_nfc_attacks_funcs[] = {
    {"dict_attack", js_dict_attack},
    // add nested_attack, hardnested_attack, etc
};

// Init function
void mjs_init_nfc_attacks(struct mjs* mjs) {
    mjs_val_t obj = mjs_mk_object(mjs);
    for(size_t i=0; i<COUNT_OF(mjs_nfc_attacks_funcs); i++) {
        mjs_set(mjs, obj,
            mjs_nfc_attacks_funcs[i].name, ~0,
            mjs_mk_foreign_func(mjs, mjs_nfc_attacks_funcs[i].func));
    }
    mjs_set(mjs, mjs_get_global(mjs), "nfc_attacks", ~0, obj);
}
