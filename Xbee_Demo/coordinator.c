#include <msp430.h>

// defines
#define TXLED	BIT0	// turn on when transmitting UART signal
#define RXLED	BIT6	// turn on when receiving UART signal
#define TXD 	BIT2	// UART Transmit pin (Port 1 pin 2)
#define RXD		BIT1	// UART Receive pin (Port 1 pin 1)
#define RESET	BIT0	// Xbee Reset pin, active low (Port 2 pin 0)

// function prototypes
void init_ports(void);
void init_timers(void);
void init_uart(void);

// global variables
unsigned char buffer;

// functions, alphabetical after main
int main(void) {

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	// initialize everything
	init_timers();
	init_ports();
	init_uart();
	__bis_SR_register(GIE); // enable interrupts

	while (1) {

	}

	return 0;
}

void init_ports(void) {
	P1SEL |= RXD + TXD; // P1.1 = RXD, P1.2=TXD
	P1SEL2 |= RXD + TXD; // P1.1 = RXD, P1.2=TXD
	P1DIR |= RXLED + TXLED; // leds are outputs
	P1OUT &= 0x00;

	P2OUT |= RESET;	// sets reset pin high (active low)
	P2DIR |= RESET; // sets reset pin as output

	P2OUT &= ~RESET; //toggle reset low (activate it)
	P2OUT |= RESET; //bring it back high (deactivate reset)
}

void init_timers(void) {
	DCOCTL = 0; 			// Select lowest DCOx and MODx settings<
	BCSCTL1 = CALBC1_1MHZ;	// Set DCO
	DCOCTL = CALDCO_1MHZ;	// calibrate to 1MHz
}

void init_uart(void) {
	UCA0CTL1 |= UCSSEL_2; // SMCLK
	UCA0BR0 = 0x68; // 1MHz 9600
	UCA0BR1 = 0x0; // 1MHz 9600
	UCA0MCTL = 0x4; // Modulation for baud rate of 9600
	UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
	UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
}

// interrupts for UART, timers and buttons
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
	/*
	 * UART receive buffer interrupt
	 */
	P1OUT |= RXLED; //turn on receive led

	buffer = UCA0RXBUF;
	UCA0TXBUF = 'a';

	P1OUT &= ~RXLED; //turn off receive kled
}
