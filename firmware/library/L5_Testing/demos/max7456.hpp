#pragma once
#include <stdint.h>

// ==============================
// MAX7456 Driver
// ==============================
// The MAX7456 is a analog video text overlay device
class MAX7456
{
public:
	enum TRANSFER_MODES
	{
		READ = 0,
		WRITE = 1
	};

	static const uint8_t VIDEO_MODE 				= 0x00;
	static const uint8_t VIDEO_MODE_ENABLE_OSD 		= (1 << 3);

	static const uint8_t OSD_BLACK_LEVEL 			= 0x6C;
	static const uint8_t OSD_BL_ENABLE_CONTROL 		= ~(1 << 4);

	static const uint8_t DISPLAY_MEMORY_MODE 		= 0x04;
	static const uint8_t OPERATION_MODE_8BIT 		= (1 << 6);
	static const uint8_t DISPLAY_MEMORY_MODE_CLR 	= (1 << 2);

	static const uint8_t DISPLAY_MEMORY_ADDR_H 		= 0x05;
	static const uint8_t DISPLAY_MEMORY_ADDR_L 		= 0x06;

	static const uint8_t DISPLAY_MEM_DATA_REG       = 0x07;

	static const uint8_t VERTICAL_OFFSET_REG 		= 0x03;
	static const uint8_t VERTICAL_OFFSET 			= (1 << 4) | 8;

	static const uint8_t HORIZONTAL_OFFSET_REG 		= 0x02;
	static const uint8_t HORIZONTAL_OFFSET 			= (1 << 5) | 4;

	static const uint8_t CENTER_OF_SCREEN           = 225;

	static const uint8_t NVM_A 						= 0x0B;
	static const uint8_t NVM_a 						= 0x25;
	static const uint8_t NVM__ 						= 0x00;
	static const uint8_t NVM_0 						= 0x0A;
	static const uint8_t NVM_1 						= 0x01;
	static const uint8_t NVM_COLON 					= 0x44;
	//// Replaced a chinese character with an empty character
	static const uint8_t EMPTY_MASK					= 0xEB;
	static const uint8_t NVM_EMPTY					= 0x00;

	static const uint8_t DISPLAY_LEFT				= 1;
	static const uint8_t DISPLAY_WIDTH              = 30;
	static const uint16_t DISPLAY_HEIGHT			= 450;

	static const uint8_t DISPLAY_TOP	 			= DISPLAY_WIDTH;
	static const uint8_t DISPLAY_MIDDLE 			= ((DISPLAY_HEIGHT/DISPLAY_WIDTH)/2)-1;
	static const uint8_t DISPLAY_CENTER				= (DISPLAY_WIDTH/2);

	MAX7456(void (*nChipSelect)(bool Select), uint8_t (*nSPI)(uint8_t Data))
	{
		ChipSelect = nChipSelect;
		SPITransfer = nSPI;
	}
	bool Initialize()
	{
		//// Shift Vertical Offset defined Pixels
		VideoTransfer(
			HORIZONTAL_OFFSET_REG,
			HORIZONTAL_OFFSET,
			WRITE
		);
		//// Shift Vertical Offset defined Pixels
		VideoTransfer(
			VERTICAL_OFFSET_REG,
			VERTICAL_OFFSET,
			WRITE
		);
		//// Clear Display Memory
		VideoTransfer(
			DISPLAY_MEMORY_MODE,
			DISPLAY_MEMORY_MODE_CLR,
			WRITE
		);
		//// Enable OSD
		VideoTransfer(
			VIDEO_MODE,
			VIDEO_MODE_ENABLE_OSD,
			WRITE
		);
		//// Read OSD Black Level Register
		uint8_t OSDBLReg = VideoTransfer(
			OSD_BLACK_LEVEL,
			0x00,
			READ
		);
		//// Enable Automatic Black Level Control
		////    By clearing OSDBL[4] = 0
		VideoTransfer(
			OSD_BLACK_LEVEL,
			OSDBLReg & OSD_BL_ENABLE_CONTROL,
			WRITE
		);
		//// Set Display Memory to 8Bit mode
		VideoTransfer(
			DISPLAY_MEMORY_MODE,
			OPERATION_MODE_8BIT,
			WRITE
		);
		return true;
	}
	void ClearScreen()
	{
		VideoTransfer(
			DISPLAY_MEMORY_MODE,
			DISPLAY_MEMORY_MODE_CLR,
			WRITE
		);
	}
	void WriteCharacterToScreen(uint16_t Position, uint8_t Character)
	{
		//// Loaded 8th bit MSB to Display memory address High byte
		//// DISPLAY_MEMORY_ADDR_H[1] is set to 0 to keep it on
		//// Character memory mode
		VideoTransfer(
			DISPLAY_MEMORY_ADDR_H,
			((Position >> 8) & 0x01),
			WRITE
		);
		//// Load [7:0] bits of position
		VideoTransfer(
			DISPLAY_MEMORY_ADDR_L,
			(Position & 0xFF),
			WRITE
		);
		//// Load NVM Character address into display memory data
		//// register to be written to display memory matrix
		VideoTransfer(
			DISPLAY_MEM_DATA_REG,
			Character,
			WRITE
		);
	}
	void WriteString(uint16_t Position, char * Message)
	{
		for(uint16_t i = 0; Message[i] != 0; i++)
		{
			switch(Message[i])
			{
				case 'A' ... 'Z':
					WriteCharacterToScreen(Position+i, NVM_A+(Message[i]-'A'));
					break;
				case 'a' ... 'z':
					WriteCharacterToScreen(Position+i, NVM_a+(Message[i]-'a'));
					break;
				case '1' ... '9':
					WriteCharacterToScreen(Position+i, NVM_1+(Message[i]-'1'));
					break;
				case '0':
					WriteCharacterToScreen(Position+i, NVM_0);
					break;
				case ' ':
					WriteCharacterToScreen(Position+i, NVM__);
					break;
				case ':':
					WriteCharacterToScreen(Position+i, NVM_COLON);
					break;
				case EMPTY_MASK:
					WriteCharacterToScreen(Position+i, NVM_EMPTY);
					break;
				default:
					WriteCharacterToScreen(Position+i, Message[i]);
					break;
			}
		}
	}

private:
	void (*ChipSelect)(bool Select);
	uint8_t (*SPITransfer)(uint8_t Data);

	inline uint8_t READ_OPERATION(uint8_t ADDR) const
	{
		return (ADDR |  (1 << 7));
	}
	inline uint8_t WRITE_OPERATION(uint8_t ADDR) const
	{
		return (ADDR & ~(1 << 7));
	}
	uint16_t CoordsToPosition(uint8_t X, uint8_t Y)
	{
		return (Y*DISPLAY_WIDTH)+(X);
	}
	uint8_t VideoTransfer(uint8_t Addr, uint8_t Data, bool Write)
	{
		ChipSelect(true);
		if(Write)
		{
			SPITransfer(WRITE_OPERATION(Addr));
		}
		else
		{
			SPITransfer(READ_OPERATION(Addr));
		}
		uint8_t result = SPITransfer(Data);
		ChipSelect(false);

		return result;
	}
};
