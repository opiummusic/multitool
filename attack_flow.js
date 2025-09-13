let gui = require("gui");
let nfc_attacks = require("nfc_attacks"); // from your C binding

async function main() {
    gui.showToast("Starting dictionary attack...");
    let key = nfc_attacks.dict_attack("/ext/nfc/assets/mf_classic_dict_user.nfc");
    if (key) {
        gui.showToast("Found key: " + key);
        // Continue with nested attack etc
        let nested = nfc_attacks.nested_attack(key);
        if (nested) {
            gui.showToast("Nested success!");
        } else {
            gui.showToast("Nested failed, trying hardnested...");
            let hard = nfc_attacks.hardnested_attack();
            if (hard) gui.showToast("Hardnested success!");
            else gui.showToast("Hardnested failed");
        }
    } else {
        gui.showToast("No keys found in dictionary.");
    }
}

main();
