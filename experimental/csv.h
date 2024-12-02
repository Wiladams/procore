#pragma once

#include "charset.h"
#include "bspanutil.h"
#include "generator.h"


#include <vector>
#include <unordered_map>
#include <map>
#include <functional>


namespace pcore {
	static charset csvln("\r\n\t");
	static charset csvwsp("\r\n ");

	
	// A description of a CSV column
// we want the name, the position, and the possible data type
	struct CSVColumn {
		ByteSpan fNameSpan;
		int fPosition;
	};

	
	// There may be a Byte Order Mark (BOM) at the beginning of the 
	// data.  ReadBOM() will read it, if it exists, otherwise
	// it will leave it alone.
	// The return value is the BOM, if it was read
	// The input span is altered to reflect reading the BOM
	// Note:  It might be better to pass the BOM in as a parameter
	// and make the input stream a const, but return an altered stream
	// as the return value.
	static ByteSpan readBOM(ByteSpan& s)
	{
		ByteSpan bom = chunk_take(s, 3);
		
		if (bom.size() == 3) {
			if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
				s = chunk_skip(s, 3);	// s = s.drop(3);
			}
		}
		return bom;
	}
	
	
	// Read a single line from the input span
	// update the input span to the beginning of
	// the next line
	// line termination is either
	// \r\n or \n
	// but there can be quoted strings inside, that contain the line terminators
	// 
	static ByteSpan readCsvLine(ByteSpan& s)
	{
		ByteSpan line = s;
		line.fEnd = line.fStart;
		bool inQuote = false;
		
		do {
			// we've reached end of input
			if (line.fEnd == s.fEnd) {
				break;
			}
			
			if (*s == '"') {
				inQuote = !inQuote;
			}

			if (!inQuote) {
				if (csvln.contains(*s)) {
					s++;
					break;
				}
			}

			line.fEnd++;
			s++;
		} while (s );

		if (*s == '\r') {
			s++;
		}
		if (*s == '\n')
			s++;

		return line;

	}
	

	
	// generateColumnValues
// Given an input that is a single line of CSV data
// yield a single column's value each time around
// This generator will work with a range-based for loop
// Example:
// for (auto col : generateColumnValues(line)) {
//	printf("%.*s\n", (int)col.size(), col.data());
// }
	static Generator<ByteSpan> generateColumnValues(const ByteSpan& chunk, charset&& csvdlm)
	{
		// first trim leading whitespace characters
		// BUGBUG - maybe not?
		//ByteSpan s = chunk_ltrim(chunk, csvwsp);
		ByteSpan s = chunk;

		// look for a '"' character
		// if that's found, then read until the end of that
		// then look for delim
		bool inQuote = false;


		// Assume we're at the beginning of a column value
		ByteSpan col = s;
		col.fEnd = col.fStart;

		// Expand the end of the value until we see a delimeter
		while (s) {
			if (*s == '"') {
				inQuote = !inQuote;
				s++;
			}

			if (inQuote) {
				col.fEnd++;
				s++;
				continue;
			}

			if (csvdlm.contains(*s)) {
				s++;
				co_yield col;

				// Reset for next column
				col.fStart = s.fStart;
				col.fEnd = s.fStart;
			}
			else {
				col.fEnd++;
				s++;
			}
		}

		if (col.size() > 0)
			co_yield col;

	}
	
	// gatherColumnValues()
	// Fill in a vector with ByteSpans that represent the column values
	// This uses an implied ',' separator, and can NOT handle quoted
	// delimeters
	//
	static bool gatherColumnValues(const ByteSpan& inChunk, std::vector<ByteSpan>& values)
	{
		ByteSpan s = inChunk;

		for (const auto& value : generateColumnValues(s, ","))
		{
			values.push_back(chunk_ltrim(value, csvwsp));
		}

		return true;
	}

	// we want to only retrieve the values of the columns specified by name
	// And a 
	static bool gatherProjectedColumnValues(const ByteSpan& inChunk, const std::vector<ByteSpan> & projNamesVec, const std::map<ByteSpan, CSVColumn>& nameDict, std::vector<ByteSpan>& values)
	{
		// The inChunk represents a single line of CSV data
		// 1. Gather the values from that row
		std::vector<ByteSpan> colValues{};
		gatherColumnValues(inChunk, colValues);
		
		// 2. For each name in the projNames, see if it's in the nameDict
		// if it is, then use the column position to retrieve the value from 
		// the colValues
		// 3. Add the value to the values vector
		
		for (auto &name : projNamesVec) {
			auto it = nameDict.find(name);
			if (it != nameDict.end()) {
				const auto & col = it->second;
				values.push_back(colValues[col.fPosition]);
			}
		}

		return true;
	}
	
	static bool gatherColumnHeadings(const ByteSpan& chunk, std::vector<CSVColumn>& columns)
	{
		ByteSpan s = chunk;

		auto gen = generateColumnValues(s, ",");

		for (const auto& c : gen) {
			ByteSpan value = chunk_trim(c, csvwsp);
			columns.push_back({ value, (int)columns.size() });
		}

		return true;
	}
	
	

	
	
	static bool gatherColumnHeadings(const ByteSpan& chunk, std::map<ByteSpan, CSVColumn>& columns)
	{
		ByteSpan s = chunk;

		auto gen = generateColumnValues(s, ",");

		size_t colPos{ 0 };
		
		for (const auto& c : gen) {
			//printf("%.*s\n", (int)c.size(), c.data());
			ByteSpan value = chunk_trim(c, csvwsp);
			columns[value] = { value, (int)colPos };
			colPos++;
		}

		return true;
	}

	//
	// A CSVTable provides a number of convenience mechanisms for dealing with 
	// Comma Separated Value tables. 
	//
	struct CSVRow
	{
		std::vector<ByteSpan> fValues;
		std::map<ByteSpan, CSVColumn> &fColumnHeadings;

		CSVRow(std::map<ByteSpan, CSVColumn>& columnHeadings, const ByteSpan &rowSpan) : fColumnHeadings(columnHeadings)
		{
			gatherColumnValues(rowSpan, fValues);
		}

		operator bool() const
		{
			return fValues.size() > 0;
		}
		
		ByteSpan operator[](const ByteSpan& columnName) const
		{
			auto it = fColumnHeadings.find(columnName);
			if (it != fColumnHeadings.end()) {
				auto col = it->second;
				return fValues[col.fPosition];
			}
			return ByteSpan();
		}
	};

	struct CSVTable
	{
		ByteSpan fSourceSpan{};
		ByteSpan fBOM{};
		ByteSpan fDataSpan{};
		std::map<ByteSpan, CSVColumn> fColumnHeadings{};

		
		CSVTable(const ByteSpan &src) 
		{
			reset(src);
		}

		// Assumes we need to parse the column headings
		void reset(const ByteSpan& src, bool namesInFirstLine = true)
		{
			fSourceSpan = src;

			
			// Read the Byte Order Mark (BOM) if there is one
			fBOM = readBOM(fSourceSpan);
			fDataSpan = fSourceSpan;
			
			if (namesInFirstLine) {
				// Read the column headings
				ByteSpan line = readCsvLine(fDataSpan);
				gatherColumnHeadings(line, fColumnHeadings);
			}

		}

		// valuesGenerator()
		// create an iterator over the rows
		//
		std::function<ByteSpan(std::vector<ByteSpan>&, const char *cNames)> valuesGenerator()
		{
			// Return a function which will retrieve the next row
			// allow the user to specify which columns they want to return
			// if columnNames == nullptr, or "*", return all
			return [this](std::vector<ByteSpan>& values, const char* columnNames = nullptr) {
				ByteSpan line = readCsvLine(fDataSpan);
				
				// if line isn't blank, then process it
				if (line) {
					// BUGBUG - this should happen only once and be passed in probably
					// as part of the params
					if (nullptr == columnNames) {
						gatherColumnValues(line, values);
					}
					else
					{
						// create a vector of the column names
						std::vector<ByteSpan> projNamesVec{};
						gatherColumnValues(columnNames, projNamesVec);
						gatherProjectedColumnValues(line, projNamesVec, fColumnHeadings, values);
					}
				}
				
				return line;
			};
		}
		
		// rowGenerator()
		// Create an iterator over the rows
		// Usage: 
		//		auto rgen = tbl.rowGenerator()
		//      
		std::function<CSVRow()> rowGenerator()
		{
			ByteSpan rowSpans = fDataSpan;
			
			return [this, &rowSpans]() {
				ByteSpan line = readCsvLine(rowSpans);
				return CSVRow(fColumnHeadings, line);
			};
		}
	};
}