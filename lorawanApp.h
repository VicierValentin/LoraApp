#include "mbed.h"
#include "events/EventQueue.h"

// Application helpers
#include "trace_helper.h"

#include "lorawan/system/lorawan_data_structures.h"
#include "lorawan/LoRaWANInterface.h"

using namespace events;

class LorawanApp {
public:
    LorawanApp();
    int lorawanInit();
    void lora_event_handler(lorawan_event_t event);
    void send_message();
    void receive_message();

private:

    LoRaWANInterface* lorawan;
    lorawan_app_callbacks_t callbacks;
    lorawan_event_t event;
    EventQueue ev_queue;
    uint8_t tx_buffer[30];
    uint8_t rx_buffer[30];
    int CONFIRMED_MSG_RETRY_COUNTER = 3;
    int MAX_NUMBER_OF_EVENTS = 10;
    int TX_TIMER = 10000;
};