#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <nfc/nfc.h>
#include <stdio.h>

// Forward declaration of dict brute-force function
void run_dict_bruteforce(NfcTag* tag);

int main(void) {
    furi_hal_init();
    NfcTag tag;
    nfc_init();

    printf("Hold an NFC tag near the Flipper Zero...\n");

    while(1) {
        if(nfc_poll(&tag)) {
            printf("NFC Tag detected!\n");

            switch(tag.type) {
                case NFC_TYPE_MIFARE_CLASSIC:
                    printf("Type: MIFARE Classic\n");
                    printf("UID: ");
                    for(int i = 0; i < tag.uid_len; i++) printf("%02X ", tag.uid[i]);
                    printf("\n");
                    printf("Vulnerabilities:\n");
                    printf("- Uses CRYPTO1 (broken)\n");
                    printf("- Default keys often used\n");
                    printf("- Can be cloned / replayed\n");

                    printf("\nPress OK to run dict brute-force, or BACK to skip.\n");

                    // Wait for user input
                    InputEvent event;
                    while(1) {
                        if(furi_hal_input_event_get(&event, 100)) {
                            if(event.type == InputTypePress) {
                                if(event.key == InputKeyOk) {
                                    run_dict_bruteforce(&tag);
                                    break;
                                } else if(event.key == InputKeyBack) {
                                    printf("Skipping brute-force.\n");
                                    break;
                                }
                            }
                        }
                    }
                    break;

                case NFC_TYPE_NTAG:
                    printf("Type: NTAG\n");
                    printf("UID: ");
                    for(int i = 0; i < tag.uid_len; i++) printf("%02X ", tag.uid[i]);
                    printf("\n");
                    printf("Vulnerabilities:\n");
                    printf("- Weak password protection on some models\n");
                    printf("- Susceptible to replay attacks\n");
                    printf("- Can be cloned if no dynamic auth\n");
                    break;

                case NFC_TYPE_FELICA:
                    printf("Type: FeliCa\n");
                    printf("IDm: ");
                    for(int i = 0; i < tag.uid_len; i++) printf("%02X ", tag.uid[i]);
                    printf("\n");
                    printf("Vulnerabilities:\n");
                    printf("- Stronger than MIFARE Classic, but weak key configs possible\n");
                    break;

                default:
                    printf("Unknown NFC type\n");
                    break;
            }

            printf("-------------------------\n");
            furi_delay_ms(2000); // Wait a bit before next read
        }

        furi_delay_ms(100);
    }

    return 0;
}