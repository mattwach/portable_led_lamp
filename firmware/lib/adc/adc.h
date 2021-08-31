#ifndef ADC_H
#define ADC_H
// Libraries intended to simplify ADC usage (for synchronbous reads)

#include <inttypes.h>
#include <avr/io.h>

#ifdef __AVR_MEGA__
// ATMega chips
#define ADC_REF_AREF 0x00       // Use AREF pin
#define ADC_REF_AVCC 0x40       // Use AVCC (or VCC)
#define ADC_REF_VCC 0x40        // Use AVCC (or VCC)
#define ADC_REF_INTERNAL11 0xC0 // Use internal 1.1V source
#else
// ATTiny chips
#define ADC_REF_VCC 0x00        // Use VCC
#define ADC_REF_AREF 0x40       // Use AREF at PB0
#define ADC_REF_INTERNAL11 0x80 // Use internal 1.1V source
#define ADC_REF_INTERNAL256 0x90 // Use internal 2.56V source
#define ADC_REF_INTERNAL256_BYPASS 0xD0 // Use internal 2.56V source with 
                                        // cap bypass at PB0
#endif

#define ADC_PRESCALER_2 0x00
#define ADC_PRESCALER_4 0x02
#define ADC_PRESCALER_8 0x03
#define ADC_PRESCALER_16 0x04
#define ADC_PRESCALER_32 0x05
#define ADC_PRESCALER_64 0x06
#define ADC_PRESCALER_128 0x07

// Reads ADC as an 8-bit value where 255->vref and 0->GND
//   adc_channel: Which adc channel to read from
//   reference: One of ADC_REF_AREF, ADC_REF_AVCC, ADC_REF_INTERNAL.  ADC_REF_AVCC
//     is the usual choice amd reads between 0 and VCC
//   prescaler: Tradeoff between accuracy and speed.  Use 0-7 (where 0 is the 
//   fastest) or, alternately, use ADC_PRESCALER_2 through ADC_PRESCALER_128
//   where ADC_PRESCALER_2 is the fastest
static inline uint8_t adc_read8(
    uint8_t adc_channel,
    uint8_t reference,
    uint8_t prescaler) {
  // Select channel
  ADMUX = reference | (1 << ADLAR) | adc_channel;
  // Start the conversion
  ADCSRA = (1 << ADEN) | (1 << ADSC) | prescaler;
  // Wait for completion
  while (ADCSRA & (1<<ADSC));
  return ADCH;
}

// Reads ADC as a 10-bit value where 1024->vref and 0->gnd
//
//   adc_channel: Which adc channel to read from
//   reference: One of ADC_REF_AREF, ADC_REF_AVCC, ADC_REF_INTERNAL.  ADC_REF_AVCC
//     is the usual choice amd reads between 0 and VCC
//   prescaler: Tradeoff between accuracy and speed.  Use 0-7 (where 0 is the 
//   fastest) or, alternately, use ADC_PRESCALER_2 through ADC_PRESCALER_128
//   where ADC_PRESCALER_2 is the fastest
static inline uint16_t adc_read16(
    uint8_t adc_channel,
    uint8_t reference,
    uint8_t prescaler) {
  // Select channel
  ADMUX = reference | adc_channel;
  // Start the conversion
  ADCSRA = (1 << ADEN) | (1 << ADSC) | prescaler;
  // Wait for completion
  while (ADCSRA & (1<<ADSC));
  return ADC;
}

// Use ADC to generate a source of entropy.
//
// Produce 8, 16, or 32 bit values by reading an ADC channel several rounds for
// each bit and bit-shifting the LSB results.  The intent is to provide a source
// of entropy for a random number or random seed.
//
// Note that reading the ADC a few times produces poor enrtropy.  If you setup
// the project in examples/bitmap/adc_plot, you can see the distinct patterns
// for yourself. There are three basic ways to improve the situation and of
// course these can be combined.
//
// 1) Read the ADC many times for each bit over an extended period of time.  As
//    shown in the demo above, increase the number of rounds enough and the
//    predictable patterns dissapate leading to good overall coverage and lack
//    of visible patterns (looks like white noise).  I'm not sure how this would
//    hold up against hardened tests, however so don't recommend using this
//    method for crypto.  Anyway, due to the slow generation, the practical
//    approach is to generate just a few of these and use them to seed a
//    pseudorandom algorithm.
// 2) Attach something to the ADC pin that adds noise.  A series of antennas
//    (open wires) of different length seems to help a little.  Better would be
//    to construct a reverse PN junction circuit.  Using this method should
//    allow you to use considerably less rounds and get better entropy.
// 3) Combine with other sources of entropy.  A microcontroller doesn't often
//    have many of these, but every connected circuit could be sampled XORing
//    them all together. If some of the soures are deterministic, they will not
//    degrade the result (e.g. if a source is always 1, XORing it will not
//    degrade the result).
//
//   adc_channel: Which adc channel to read from.  The ADC does not have to
//     be connected to anything, just make sure you are not picking
//     an output pin.
//   bits: The number of bits you need in the result which corresponds to the
//     nmber of ADC reads and thus the amount of time the process will take.
//   rounds: The number of times to read each ADC bit.  Resits are combined in
//     an XOR chain and there is an increased delay between each read.  Expect
//     59 rounds to take about 2ms per bit.  Yes, that is slow.
#define DEFAULT_ROUNDS 59
uint8_t adc_entropy8(uint8_t adc_channel, uint8_t bits, uint8_t rounds);
uint16_t adc_entropy16(uint8_t adc_channel, uint8_t bits, uint8_t rounds);
uint32_t adc_entropy32(uint8_t adc_channel, uint8_t bits, uint8_t rounds);

#endif
