#define F_CPU 16000000UL // 16 MHz clock speed for the delay macro

#include <avr/io.h>
#include <util/delay.h>

// --- THE MOCK EXCHANGE PACKET ---
// Memory-aligned structure to hold our 5-byte ITCH-like binary packet
typedef struct {
    uint8_t start_byte;
    uint8_t ticker_id;
    uint8_t price_high;
    uint8_t price_low;
    uint8_t checksum;
} trade_packet_t;

void uart_init(void) {
    // 1. Set the baud rate for 500,000 bps (Normal Asynchronous Mode)
    // At a 16MHz system clock, the calculated UBRR0 value is 1.
    uint16_t ubrr_value = 1;

    // Shift the 16-bit value to isolate the high and low bytes for the 8-bit data bus
    UBRR0H = (unsigned char)(ubrr_value >> 8);
    UBRR0L = (unsigned char)(ubrr_value);

    // 2. Enable the transmitter
    // Bitwise OR ensures we only flip the TXEN0 bit (bit 3) without altering other configs
    UCSR0B |= (1 << TXEN0);

    // 3. Set frame format: 8-N-1 (8 data bits, No parity, 1 stop bit)
    // Writing 0x06 sets UCSZ01 and UCSZ00 to 1, configuring an 8-bit character size
    UCSR0C = 0x06;
}

void uart_transmit(uint8_t data) {
    // 1. Wait for empty transmit buffer
    // This is a blocking loop. It traps the CPU until the UDRE0 bit goes HIGH,
    // indicating the hardware has finished shifting out the previous byte onto the wire.
    while (!(UCSR0A & (1 << UDRE0))) {
        // Spin and wait...
    }

    // 2. Load data into the transmission register
    // The UART hardware immediately grabs this byte and begins transmission over the TX pin.
    UDR0 = data;
}

int main(void) {
    // Initialize the bare-metal UART hardware
    uart_init();

    // Instantiate our trade packet struct in memory
    trade_packet_t my_packet;

    while (1) {
        // --- THE TRADE ENGINE TICK ---
        
        // 1. Generate a fake market price ($65,000)
        uint16_t mock_price = 65000;

        // 2. Populate the known packet fields
        // 0xAA (10101010) acts as a highly visible synchronis ation flag on an oscilloscope
        my_packet.start_byte = 0xAA;
        my_packet.ticker_id = 0x01; // e.g., 0x01 represents AAPL

        // 3. Split the 16-bit price into two 8-bit registers
        // Right-shift by 8 bits to isolate the upper half of the price
        my_packet.price_high = (uint8_t)(mock_price >> 8);
        // Casting to uint8_t automatically truncates the integer, keeping only the lower 8 bits
        my_packet.price_low = (uint8_t)(mock_price);

        // 4. Calculate the XOR checksum for data integrity
        // If a bit flips due to wire noise, the FPGA's calculated checksum will fail.
        my_packet.checksum = my_packet.start_byte ^ my_packet.ticker_id ^ my_packet.price_high ^ my_packet.price_low;

        // 5. Transmit the packet sequentially
        uart_transmit(my_packet.start_byte);
        uart_transmit(my_packet.ticker_id);
        uart_transmit(my_packet.price_high);
        uart_transmit(my_packet.price_low);
        uart_transmit(my_packet.checksum);

        // Throttle the exchange so our logic analyser can easily catch discrete packets later
        _delay_ms(100); 
    }
    
    return 0;
}