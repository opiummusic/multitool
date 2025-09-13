# NFC Attacks Multitool for Flipper Zero

**This project is a multitool for NFC attacks on Flipper Zero.**  
It combines dictionary, nested, and hardnested attacks for MIFARE Classic and other NFC cards.  
Use this to brute-force card keys, escalate access, and dump card data—all from your Flipper.

---

## Features

- **Dictionary Attack:**  
  Brute-force MIFARE Classic keys using a dictionary file.
- **Nested Attack:**  
  Uses a known key to escalate and recover additional sector keys (Crypto-1 cipher).
- **Hardnested Attack:**  
  Attempts to recover keys using advanced brute-force (if all else fails).
- **Card Dump & Emulation:**  
  Dumps cracked card data and (optionally) emulates the card.

---

## TODO List for Full Attack Support


### Dictionary Attack
- [ ] Read keys from `dict.nfc` file.
- [ ] For each key, try authenticating against the detected MIFARE Classic tag (sector 0, key A).
- [ ] Return the first successful key to the JS layer, or `null` if none found.

### Nested Attack
- [ ] Use a found key to run nested authentication.
- [ ] Extract additional sector keys.
- [ ] Return success/failure (and keys) to JS.

### Hardnested Attack
- [ ] Implement hardnested brute-force logic.
- [ ] Attempt key recovery if previous attacks fail.
- [ ] Return success/failure (and keys) to JS.

### Card Dump & Emulation
- [ ] Dump the card data after successful attack (`tag.raw` property).
- [ ] Save dump to `/ext/apps_data/`.
- [ ] Optionally, emulate the cracked card.

### JS Integration
- [ ] Ensure results from C functions are passed to JS (strings, booleans, etc.).
- [ ] Show user progress and results via GUI toasts.
- [ ] Handle errors gracefully.

