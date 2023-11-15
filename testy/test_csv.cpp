

#include "csv.h"
#include "mappedfile.h"
#include "generator.h"

using namespace pcore;





void testQuoted()
{
	ByteSpan s = ByteSpan("\"a,b\",c,d,e\n");
	auto line = readCsvLine(s);
	std::vector<ByteSpan> values;
	gatherColumnValues(line, values);
	for (auto& v : values) {
		printf("%.*s\n", (int)v.size(), v.data());
	}
	printf("-- LAST --\n");
}


void testColumnPositions(const ByteSpan &chunk)
{
	ByteSpan s = chunk;
	ByteSpan fBOM = readBOM(s);
	auto columnLine = readCsvLine(s);

	// 2. Parse the column headings into a vector of CSVColumn
	std::vector<CSVColumn> columns;
	gatherColumnHeadings(columnLine, columns);
	printf("==== gatherColumnHeadings ====\n");
	int ctr = 0;
	for (auto& c : columns) {
		printf("[%2d]: %.*s\n", ctr, (int)c.fNameSpan.size(), c.fNameSpan.data());
		ctr++;
	}
}


pcore::Generator<int> firstn(std::size_t n)
{
	for (int i = 0; i < n; ++i) {
		co_yield i;
	}
}

pcore::Generator<int>
numRange(int start, int end, int step = 1)
{
	for (int i = start; i <= end; i += step) {
		co_yield i;
	}
}


void testGenerator()
{
	printf("==== testGenerator ====\n");
	printf("firstn(10)\n");
	for (const auto &i : firstn(10)) {
		printf("%d\n", i);
	}
	
	printf("numRange(0,12,2)\n");
	for (const auto& i : numRange(0, 12, 2)) {
		printf("%d\n", i);
	}
}

void testColumnNames(const ByteSpan &chunk)
{
	ByteSpan s = chunk;
	ByteSpan fBOM = readBOM(s);

	auto columnLine = readCsvLine(s);
	
	auto gen = generateColumnValues(columnLine, ",");
		
	for (const auto &c : gen) {
		printf("%.*s\n", (int)c.size(), c.data());
	}
}

void testSelectAll(const ByteSpan& src)
{
	ByteSpan s = src;
	ByteSpan fBOM = readBOM(s);
	
	// Get the first line
	auto columnLine = readCsvLine(s);
	
	// Turn it into named columns
	std::map<ByteSpan, CSVColumn> headings{};
	gatherColumnHeadings(columnLine, headings);


	// Iterate the remaining rows of the data, 
	// printing the column values along the way

	while (s) {
		// Get the next line
		auto line = readCsvLine(s);

		if (!line)
			break;

		// Print the values
		for (auto& v : generateColumnValues(line, ",")) {
			printf("%.*s, ", (int)v.size(), v.data());
		}
		printf("\n");
	}
}

// retrieve only certain columns of the set
void testProjection(const ByteSpan &src, const ByteSpan && columnNames)
{
	ByteSpan s = src;

	// Read the Byte Order Mark (BOM) if there is one
	auto bom = readBOM(s);
	
	// Get the first line
	auto columnLine = readCsvLine(s);

	// Turn the first line into named columns
	std::map<ByteSpan, CSVColumn> headings{};
	gatherColumnHeadings(columnLine, headings);
	
	std::vector<ByteSpan> projNamesVec{};
	gatherColumnValues(columnNames, projNamesVec);

	
	// Iterate the remaining rows of the data, gathering
	// the column values along the way
	std::vector<ByteSpan> values;
	
	while (s) {
		// Get the next line
		auto line = readCsvLine(s);
		
		if (!line)
			break;
		
		//printf("CSV ==> %.*s ", (int)line.size(), line.data());
		
		// Get the projected, column values
		gatherProjectedColumnValues(line, projNamesVec, headings, values);

		// Print the values
		for (auto& v : values) {
			printf("%.*s, ", (int)v.size(), v.data());
		}
		printf("\n");
		values.clear();
	}
}

void testCSVTableValues(const ByteSpan& src, const char *colNames=nullptr)
{
	CSVTable tbl(src);
	
	// Iterate the remaining rows of the data, gathering
	// the column values along the way
	std::vector<ByteSpan> values;
	auto rowGen = tbl.valuesGenerator();
	

	while (rowGen(values, colNames)) 
	{
		// Print the values
		for (auto& v : values) {
			printf("%.*s, ", (int)v.size(), v.data());
		}
		printf("\n");
		values.clear();
	}
}

void testCSVTableRows(const ByteSpan& src)
{

	CSVTable tbl(src);

	// Iterate the remaining rows of the data, gathering
	// the column values along the way
	auto rowGen = tbl.rowGenerator();

	while(true)
	{
		auto row = rowGen();
		if (!row)
			break;
		
		// Print the values
		auto value = row[ByteSpan("First Name")];
		
		printf("%.*s \n", (int)value.size(), value.data());
	} 
}



int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: csvtest <filename>.csv\n");
		return 0;
	}

	std::string filename = argv[1];
	auto file = MappedFile::create_shared(filename);

	if (!file) {
		printf("Failed to open file: %s\n", filename.c_str());
		return 0;
	}

	ByteSpan fileChunk(file->data(), file->size());
	
	//testGenerator();
	//testColumnPositions(fileChunk);
	//testColumnNames(fileChunk);
	//testSelectAll(fileChunk);
	//testProjection(fileChunk, "First Name, Last Name, E-mail Address, E-mail 3 Address, Notes");	// contacts.csv
	//testProjection(fileChunk, "E-mail Address, First, Last, Notes");	// contacts.csv// simple.csv
	testProjection(fileChunk, "MTFCC, FULLNAME");	// edges.csv

	//testCSVTableValues(fileChunk, "First Name, Last Name");
	//testCSVTableRows(fileChunk);

	//testQuoted();
	


	return 1;

}
