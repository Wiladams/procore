#pragma once

#include "pconfig.hpp"
#include "bspan.h"

#include <cstdio>

namespace pcore {
    static void writeChunkToFile(const ByteSpan& chunk, const char* filename);
    static void writeChunk(const ByteSpan& chunk);
    static void writeChunkBordered(const ByteSpan& chunk);
    static void printChunk(const ByteSpan& chunk);
}


namespace pcore {
	static void writeChunkToFile(const ByteSpan& chunk, const char* filename)
	{
		FILE* f{};
		errno_t err = fopen_s(&f, filename, "wb");
		if ((err != 0) || (f == nullptr))
			return;
		
		fwrite(chunk.data(), 1, chunk.size(), f);
		fclose(f);
	}
	
	static void writeChunk(const ByteSpan& chunk)
	{
		ByteSpan s = chunk;

		while (s && *s) {
			printf("%c", *s);
			s++;
		}
	}

	static void writeChunkBordered(const ByteSpan& chunk)
	{
		ByteSpan s = chunk;

		printf("||");
		while (s && *s) {
			printf("%c", *s);
			s++;
		}
		printf("||");
	}

	static void printChunk(const ByteSpan& chunk)
	{
		if (chunk)
		{
			writeChunk(chunk);
			printf("\n");
		}
		else
			printf("BLANK==CHUNK\n");

	}
}
