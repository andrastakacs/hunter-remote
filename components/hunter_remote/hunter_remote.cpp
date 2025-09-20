#include "esphome/core/log.h"
#include "hunter_remote.h"
#include <vector>
#define START_INTERVAL 900
#define SHORT_INTERVAL 208
#define LONG_INTERVAL 1875

namespace esphome {
namespace hunter_remote {

static const char *TAG = "hunter_remote.component";

void HunterRemote::loop() {

}

void HunterRemote::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty component");
}
/**
 * Write low bit on the bus.
 */
void HunterRemote::sendLow() {
    pin_->digital_write(true); 
	delayMicroseconds(SHORT_INTERVAL);
	pin_->digital_write(false);
	delayMicroseconds(LONG_INTERVAL);
  }

/**
 * Write high bit on the bus
 * 
 * Arguments: none
 */
void HunterRemote::sendHigh() {
	pin_->digital_write(true); 
	delayMicroseconds(LONG_INTERVAL);
	pin_->digital_write(false);
	delayMicroseconds(SHORT_INTERVAL);
  }

/**
 * Write the bit sequence out of the bus
 * 
 * @param buffer blob containing the bits to transmit
 * @param extrabit if true, then write an extra 1 bit
 */
void HunterRemote::writeBus(std::vector<std::uint8_t> buffer, bool extrabit) {
	// Resetimpulse
	pin_->digital_write(true); 
	delay(325); //milliseconds
	pin_->digital_write(false);
	delay(65); //milliseconds

	// Startimpulse
	pin_->digital_write(true);
	delayMicroseconds(START_INTERVAL);
	pin_->digital_write(false);
	delayMicroseconds(SHORT_INTERVAL);

	// Write the bits out
	for (auto &sendByte : buffer) {
		for (int inner = 0; inner < 8; inner++) {
			// Send high order bits first
			((int)sendByte & 0x80) ? sendHigh() : sendLow();
			sendByte <<= 1;
		}
	}

	// Include an extra 1 bit
	if (extrabit) {
		sendHigh();
	}

	// Write the stop pulse
	sendLow();
  }

/**	
 * Set a value with an arbitrary bit width to a bit position within a blob.
 * 
 * @param bits blob to write the value to
 * @param pos position within the blob
 * @param val to write
 * @param len in bits of the value
 */
void HunterRemote::hunterBitfield(std::vector <std::uint8_t> &bits, std::uint8_t pos, std::uint8_t val, uint8_t len) {
	while (len > 0) {
		if ((int)val & 0x1) {
			bits[pos / 8] = bits[pos / 8] | 0x80 >> (pos % 8);
		} else {
			bits[pos / 8] = bits[pos / 8] & ~(0x80 >> (pos % 8));
		}
		len--;
		val = val >> 1;
		pos++;
	}
  }

/**
 * Start a zone
 * 
 * @param zone zone number (1-48)
 * @param time time in minutes (0-240)
 */
  void HunterRemote::startZone(int zone, int time) {
	// Start out with a base frame
	std::vector<std::uint8_t> buffer = {0xff,0x00,0x00,0x00,0x10,0x00,0x00,0x04,0x00,0x00,0x01,0x00,0x01,0xb8,0x3f};

	if (zone < 1 || zone > 48) {
	    ESP_LOGE(TAG, "Startzone wrong zone input");
	}

	if (time < 0 || time > 240) {
	    ESP_LOGE(TAG, "Startzone wrong time input");
	}
	// The bus protocol is a little bizzare, not sure why
	// Bits 9:10 are 0x1 for zones > 12 and 0x2 otherwise
	if (zone > 12) {
		hunterBitfield(buffer, 9, 0x1, 2);
	} else {
		hunterBitfield(buffer, 9, 0x2, 2);
	}

	// Zone + 0x17 is at bits 23:29 and 36:42
	hunterBitfield(buffer, 23, (std::uint8_t)(zone + 0x17), 7);
	hunterBitfield(buffer, 36, (std::uint8_t)(zone + 0x17), 7);

	// Zone + 0x23 is at bits 49:55 and 62:68
	hunterBitfield(buffer, 49, (std::uint8_t)(zone + 0x23), 7);
	hunterBitfield(buffer, 62, (std::uint8_t)(zone + 0x23), 7);

	// Zone + 0x2f is at bits 75:81 and 88:94
	hunterBitfield(buffer, 75, (std::uint8_t)(zone + 0x2f), 7);
	hunterBitfield(buffer, 88, (std::uint8_t)(zone + 0x2f), 7);

	// Time is encoded in three places and broken up by nibble
	// Low nibble: bits 31:34, 57:60, and 83:86
	// High nibble: bits 44:47, 70:73, and 96:99
	hunterBitfield(buffer, 31, (std::uint8_t)time, 4);
	hunterBitfield(buffer, 44, (std::uint8_t)time >> 4, 4);
	hunterBitfield(buffer, 57, (std::uint8_t)time, 4);
	hunterBitfield(buffer, 70, (std::uint8_t)time >> 4, 4);
	hunterBitfield(buffer, 83, (std::uint8_t)time, 4);
	hunterBitfield(buffer, 96, (std::uint8_t)time >> 4, 4);

	// Bottom nibble of zone - 1 is at bits 109:112
	hunterBitfield(buffer, 109, (std::uint8_t)(zone - 1), 4);

	// Write the bits out of the bus
	writeBus(buffer, true);
	char logtext[100];
	if (time > 0) {
        ESP_LOGI(TAG, "Startzone executed Zone: %d, duration: %d min",zone,time);
	}
	if (time == 0) {
        ESP_LOGI(TAG, "Stopzone executed Zone: %d",zone);
	}
//	return;
  }

/**
 * Stop a zone
 * 
 * @param zone - zone number (1-48)
 */
void HunterRemote::stopZone(int zone) {
	startZone(zone, 0);
  }

/**
 * Run a program
 * 
 * @param num - program number (1-4)
 */
void HunterRemote::startProgram(int num) {
	// Start with a basic program frame
	std::vector<std::uint8_t> buffer = {0xff, 0x40, 0x03, 0x96, 0x09 ,0xbd ,0x7f};

	if (num < 1 || num > 4) {
		ESP_LOGE(TAG, "Startprogram wrong program number input");
	}

	// Program number - 1 is at bits 31:32
	hunterBitfield(buffer, 31, (std::uint8_t)(num - 1), 2);
	writeBus(buffer, false);
    ESP_LOGI(TAG, "Startprogram executed program_nr: %d",num);
  }


}  // namespace myhunter_component
}  // namespace esphome