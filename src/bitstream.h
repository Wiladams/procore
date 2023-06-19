#pragma once

#include "bspan.h"

namespace pcore {
	// Representation of a stream of bits
	// you can read up to 64 bits at a time using readBits(num);
	// Takes a ByteSpan as the source of bits
	struct BitStream
	{
		uint8_t* fMem{ nullptr };		// Pointer to the source memory chunk
		size_t fMemSize{ 0 };			// Size of the source memory chunk

		size_t fByteOffset{ 0 };		// offset where next byte will be read
		int fCurrentByte{ 0 };			// current byte being read
		
		int fNumBitsRemaining{ 0 };		// number of bits available to be read from current byte


		//
		// Construct a bitstream assuming we want to treat
		// the entire span as a stream of bits
		BitStream(void* mem, size_t memSize)
			:fMem((uint8_t *)mem)
			,fMemSize(memSize)
		{
		}

		BitStream(const ByteSpan& span) noexcept
		{
			fMem = (uint8_t *)span.data();
			fMemSize = span.size();
		}
		
		// returns the current bit position, between 0 and 7
		int getBitPosition() const noexcept
		{
			return 8 - fNumBitsRemaining;
		}
		
		// Discard bits to align to byte boundary
		void alignToNextByte()
		{
			// BUGBUG
			// should be able to simply say
			fNumBitsRemaining = 0;
			//while (getBitPosition() != 0)
			//	readUint(1);
		}
		
		// Read a single byte, 
		// ignoring any of the bits that have already
		// been read and advance the byte offset
		int read_byte() noexcept
		{
			if (fByteOffset >= fMemSize)
				return -1;
			
			fCurrentByte = fMem[fByteOffset];
			fNumBitsRemaining = 8;				// discard unread bits
			fByteOffset++;
			
			return fCurrentByte;
		}
		
		// Try to read a bit.  
		// If there is a bit to be read, return 0 or 1
		// return -1 if there are no more bits to be read
		// end of stream
		int tryReadBit() noexcept
		{
			if (fCurrentByte == -1)
				return -1;

			if (fNumBitsRemaining == 0)
			{
				fCurrentByte = read_byte();

				if (fCurrentByte == -1)
					return -1;
				
				// unnecessary check
				if (fCurrentByte < 0 || fCurrentByte > 255)
					return -1;
				
				fNumBitsRemaining = 8;
			}

			// another pointless check
			if (fNumBitsRemaining <= 0 || fNumBitsRemaining > 8)
				return -1;
			
			fNumBitsRemaining--;
			
			return (fCurrentByte >> (7 - fNumBitsRemaining)) & 1;
		}
		
		// Read a Uint little endian
		int readUint(int numBits)
		{
			int result = 0;
			for (int i = 0; i < numBits; i++)
			{
				int bit = tryReadBit();
				if (bit == -1)
					return -1;

				result |= bit << i;
			}

			return result;
		}

/*


		// Read multiple bits from the stream, in 'little-endian' order
		// return false on failure
		uint64_t read_bits_le(size_t nbits) noexcept
		{
			uint64_t value = 0;
			for (size_t i = 0; i < nbits; i++)
			{
				value |= (uint64_t)read_bit() << i;
			}
			return value;
		}

		// Read multiple bits from the stream, in 'big-endian' order
		// return false if there is a failure
		uint64_t read_bits_be(size_t nbits) noexcept
		{
			uint64_t value{ 0 };


			for (size_t i = 0; i < nbits; i++) {
				value <<= 1;
				value |= read_bit();
			}
			return value;
		}

		// Read an unsigned integer (up to 8 bytes) little-endian format
		uint64_t read_uint_le(size_t numBytes) noexcept
		{
			uint64_t value{ 0 };

			for (size_t i = 0; i < numBytes; i++)
			{
				uint8_t abyte = read_byte();;
				value |= (uint64_t)abyte << (i * 8);
			}
			return value;
		}

		

		//
		// Read with protection against going out of bounds
		//
		bool readByte(uint8_t& abyte) noexcept
		{
			if (fByteOffset >= fMemSize)
				return false;
			
			abyte = read_byte();
			
			return true;
		}

		
		// Read an unsigned integer (up to 8 bytes) little-endian format
		bool readUintLe(size_t numBytes, uint64_t &value) noexcept
		{
			if (fByteOffset + numBytes > fMemSize)
				return false;
			
			value = read_uint_le(numBytes);
			return true;
		}
		
		// The safe checking version
		bool readBit(uint64_t &abit) noexcept
		{
			if (fNumBitsRemaining <= 0)
			{
				if (!readByte(fCurrentByte))
					return false;
				fNumBitsRemaining = 8;
			}
			
			fNumBitsRemaining -= 1;
			
			// shift one bit out of the fCurrentByte
			abit = fCurrentByte & 0x01U;
			fCurrentByte >>= 1;

			return true;
		}

		bool readBits(size_t nbits, uint64_t &value) noexcept
		{
			uint64_t tmpBit = 0;
			value = 0;
			for (size_t i = 0; i < nbits; i++) {
				readBit(tmpBit);
				value |= tmpBit<<i;
			}
			return true;
		}
		
		// Read multiple bits from the stream, in 'big-endian' order
		// return false if there is a failure
		bool readBitsReversed(size_t nbits, uint64_t &value) noexcept
		{
			uint64_t tmpBit = 0;
			value = 0;
			for (size_t i = 0; i < nbits; i++) {
				value <<= 1;
				readBit(tmpBit);
				value |= tmpBit;
			}
			return true;
		}
		*/
		
	};

}