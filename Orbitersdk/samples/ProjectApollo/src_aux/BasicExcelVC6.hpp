/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008 Yap Chun Wei 

  BasicExcel from http://www.codeproject.com/KB/office/BasicExcel.aspx

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************************************************************/

// Created by Yap Chun Wei
// Version 1.0 (20 April 2006)
// Version 1.1 (22 April 2006)
	// - Fixed bugs with compound files not being able to write files more than 65535 bytes.
	// - Fixed bugs with reading and writing to Excel files containing many strings.
// Version 1.2 (30 April 2006)
	// - Added operator<< to pass BasicExcelCell to an output stream.
	// - Added Print() to BasicExcelWorksheet to print the worksheet to an output stream.
	// - Change BasicExcelCell Get functions to const functions.
	// - Rename BasicExcelWorksheet functions RenameWorkSheet() to Rename().
// Version 1.3 (10 May 2006)
	// - Fixed bugs with reading from Excel files containing Asian characters.
// Version 1.4 (13 May 2006)
	// - Fixed bugs with reading and writing to Excel files containing many strings.
// Version 1.5 (15 May 2006)
	// - Remove code for ExtSST because it was causing problems with reading and writing to Excel files containing many strings.
// Version 1.6 (16 May 2006)
	// - Optimized code for reading and writing.
// Version 1.7 (22 May 2006)
	// - Fixed code to remove some warnings.
	// - Fixed bug with BasicExcelWorksheet::Cell.
	// - Fixed bug with BasicExcel::UpdateWorksheets().
// Version 1.8 (23 May 2006)
	// - Fixed bug with reading Excel files containing many unicode strings.
	// - Fixed code to remove some warnings.
	// - Fixed variable code_ duplication in BoolErr.
	// - Minor changes to BasicExcelCell:Set functions.
// Version 1.9 (24 May 2006)
	// - Changed name_ in Style from SmallString to LargeString.
	// - Fixed bug in BasicExcelCell::GetString and BasicExcelCell::GetWString.
	// - Minor changes to functions in BasicExcel and BasicExcelWorksheet which checks for unicode.
	// - Minor change to SmallString::Read.
// Version 1.10 (30 May 2006)
	// - Fixed bug with reading Excel files containing many strings.
	// - Remove memory leaks.
// Version 1.11 (2 June 2006)
	// - Fixed bug with reading and writing Excel files containing many unicode and ANSI strings.
// Version 1.12 (6 June 2006)
	// - Fixed bug with reading and writing Excel files containing many unicode and ANSI strings.
// Version 1.13 (1 August 2006)
	// - Changed BasicExcelCell::Get() so that it will get a stored double as an integer or vice versa if necessary.
	// - Changed BasicExcelCell::Get() so that it will not cause any errors if a string is empty.
	// - Changed BasicExcelCell::SetString() and BasicExcelCell::SetWString() so that it will not save an empty string.
// Version 1.14 (6 August 2006)
	// - Fixed bug with reading Excel files that contain a null string.

#ifndef BASICEXCEL_HPP
#define BASICEXCEL_HPP

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

#define UTF16
#ifdef UTF16
	#define SIZEOFWCHAR_T 2
#else
	#define SIZEOFWCHAR_T sizeof(wchar_t)
#endif

namespace YCompoundFiles
{
class Block
// PURPOSE: In charge of handling blocks of data from a file
{
public:
	Block();

// File handling functions
	bool Create(const wchar_t* filename);
	bool Open(const wchar_t* filename, ios_base::openmode mode=ios_base::in | ios_base::out);
	bool Close();
	bool IsOpen();

// Block handling functions
	bool Read(size_t index, char* block);
	bool Write(size_t index, const char* block);
	bool Swap(size_t index1, size_t index2);
	bool Move(size_t from, size_t to);
	bool Insert(size_t index, const char* block);
	bool Erase(size_t index);
	bool Erase(vector<size_t>& indices);

// Misc functions
	size_t GetBlockSize() const {return blockSize_;}
	void SetBlockSize(size_t size) 
	{
		blockSize_ = size;
		indexEnd_ = fileSize_/blockSize_ + (fileSize_ % blockSize_ ? 1 : 0);
	}
	
protected:
	vector<char> filename_;
	ios_base::openmode mode_;
	fstream file_;
	size_t blockSize_;
	size_t indexEnd_;
	size_t fileSize_;
};

struct LittleEndian
{
	#define READWRITE(Type) \
	static void Read(const char* buffer, Type& retVal, int pos=0, int bytes=0)	\
	{	\
		retVal = Type(0);	\
		if (bytes == 0) bytes = sizeof(Type);	\
		for (size_t i=0; i<bytes; ++i)	\
		{	\
			retVal |= ((Type)((unsigned char)buffer[pos+i])) << 8*i;	\
		}	\
	}	\
	static void ReadString(const char* buffer, Type* str, int pos=0, int bytes=0)	\
	{	\
		for (size_t i=0; i<bytes; ++i) Read(buffer, str[i], pos+i*sizeof(Type));	\
	}	\
	static void Write(char* buffer, Type val, int pos=0, int bytes=0)	\
	{	\
		if (bytes == 0) bytes = sizeof(Type);	\
		for (size_t i=0; i<bytes; ++i)	\
		{	\
			buffer[pos+i] = (unsigned char)val;	\
			val >>= 8;	\
		}	\
	}	\
	static void WriteString(char* buffer, Type* str, int pos=0, int bytes=0)	\
	{	\
		for (size_t i=0; i<bytes; ++i) Write(buffer, str[i], pos+i*sizeof(Type));	\
	}	\
	static void Read(const vector<char>& buffer, Type& retVal, int pos=0, int bytes=0)	\
	{	\
		retVal = Type(0);	\
		if (bytes == 0) bytes = sizeof(Type);	\
		for (size_t i=0; i<bytes; ++i)	\
		{	\
			retVal |= ((Type)((unsigned char)buffer[pos+i])) << 8*i;	\
		}	\
	}	\
	static void ReadString(const vector<char>& buffer, Type* str, int pos=0, int bytes=0)	\
	{	\
		for (size_t i=0; i<bytes; ++i) Read(buffer, str[i], pos+i*sizeof(Type));	\
	}	\
	static void Write(vector<char>& buffer, Type val, int pos=0, int bytes=0)	\
	{	\
		if (bytes == 0) bytes = sizeof(Type);	\
		for (size_t i=0; i<bytes; ++i)	\
		{	\
			buffer[pos+i] = (unsigned char)val;	\
			val >>= 8;	\
		}	\
	}	\
	static void WriteString(vector<char>& buffer, Type* str, int pos=0, int bytes=0)	\
	{	\
		for (size_t i=0; i<bytes; ++i) Write(buffer, str[i], pos+i*sizeof(Type));	\
	}	\

	READWRITE(char)
	READWRITE(unsigned char)
	READWRITE(short)
	READWRITE(int)
	READWRITE(unsigned int)
	READWRITE(long)
	READWRITE(unsigned long)
	READWRITE(__int64)
	READWRITE(unsigned __int64)

	#undef READWRITE


	static void Read(const char* buffer, wchar_t& retVal, int pos=0, int bytes=0)
	{
		retVal = wchar_t(0);
		if (bytes == 0) bytes = SIZEOFWCHAR_T;
		for (int i=0; i<bytes; ++i)
		{
			retVal |= ((wchar_t)((unsigned char)buffer[pos+i])) << 8*i;
		}
	}

	static void ReadString(const char* buffer, wchar_t* str, int pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i) Read(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Write(char* buffer, wchar_t val, int pos=0, int bytes=0)
	{
		if (bytes == 0) bytes = SIZEOFWCHAR_T;
		for (int i=0; i<bytes; ++i)
		{
			buffer[pos+i] = (unsigned char)val;
			val >>= 8;
		}
	}

	static void WriteString(char* buffer, wchar_t* str, int pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i) Write(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Read(const vector<char>& buffer, wchar_t& retVal, int pos=0, int bytes=0)
	{
		retVal = wchar_t(0);
		if (bytes == 0) bytes = SIZEOFWCHAR_T;
		for (int i=0; i<bytes; ++i)
		{
			retVal |= ((wchar_t)((unsigned char)buffer[pos+i])) << 8*i;
		}
	}

	static void ReadString(const vector<char>& buffer, wchar_t* str, int pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i) Read(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Write(vector<char>& buffer, wchar_t val, int pos=0, int bytes=0)
	{
		if (bytes == 0) bytes = SIZEOFWCHAR_T;
		for (int i=0; i<bytes; ++i)
		{
			buffer[pos+i] = (unsigned char)val;
			val >>= 8;
		}
	}

	static void WriteString(vector<char>& buffer, wchar_t* str, int pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i) Write(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}
};

class CompoundFile
{
public:
	enum {DUPLICATE_PROPERTY=-6,
		  NAME_TOO_LONG=-5, FILE_NOT_FOUND=-4, 
		  DIRECTORY_NOT_EMPTY=-3, DIRECTORY_NOT_FOUND=-2, 
		  INVALID_PATH=-1, 
		  SUCCESS=1};

	CompoundFile();
	~CompoundFile();

// User accessible functions
public:
	// Compound File functions
	bool Create(const wchar_t* filename);
	bool Open(const wchar_t* filename, ios_base::openmode mode=ios_base::in | ios_base::out);
	bool Close();
	bool IsOpen();

	// Directory functions
	int ChangeDirectory(const wchar_t* path);
	int MakeDirectory(const wchar_t* path);
	int PresentWorkingDirectory(wchar_t* path);
	int PresentWorkingDirectory(vector<wchar_t>& path);
	int RemoveDirectory(const wchar_t* path);
	int DelTree(const wchar_t* path);
	int DirectoryList(vector<vector<wchar_t> >& list, const wchar_t* path=0);

	// File functions
	int MakeFile(const wchar_t* path);
	int RemoveFile(const wchar_t* path);
	int FileSize(const wchar_t* path, size_t& size);
	int ReadFile(const wchar_t* path, char* data);
	int ReadFile(const wchar_t* path, vector<char>&data);
	int WriteFile(const wchar_t* path, const char* data, size_t size);
	int WriteFile(const wchar_t* path, const vector<char>&data, size_t size);


	// ANSI char functions
	bool Create(const char* filename);
	bool Open(const char* filename, ios_base::openmode mode=ios_base::in | ios_base::out);
	int ChangeDirectory(const char* path);
	int MakeDirectory(const char* path);
	int PresentWorkingDirectory(char* path);
	int PresentWorkingDirectory(vector<char>& path);
	int RemoveDirectory(const char* path);
	int DelTree(const char* path);
	int MakeFile(const char* path);
	int RemoveFile(const char* path);
	int FileSize(const char* path, size_t& size);
	int ReadFile(const char* path, char* data);
	int ReadFile(const char* path, vector<char>& data);
	int WriteFile(const char* path, char* data, size_t size);
	int WriteFile(const char* path, vector<char>& data, size_t size);

// Protected functions and data members
protected:
	// General functions and data members
	void IncreaseLocationReferences(vector<size_t> indices);
	void DecreaseLocationReferences(vector<size_t> indices);
	void SplitPath(const wchar_t* path, wchar_t*& parentpath, wchar_t*& propertyname);
	vector<char> block_;
	Block file_;

	// Header related functions and data members
	bool LoadHeader();
	void SaveHeader();
	class Header
	{
	public:
		Header();
		void Write(char* block);
		void Read(char* block);

		__int64 fileType_;		// Magic number identifying this as a compound file system (0x0000)
		int uk1_;					// Unknown constant (0x0008)
		int uk2_;					// Unknown constant (0x000C)
		int uk3_;					// Unknown constant (0x0010)
		int uk4_;					// Unknown constant (0x0014)
		short uk5_;					// Unknown constant (revision?) (0x0018)
		short uk6_;					// Unknown constant	(version?) (0x001A)
		short uk7_;					// Unknown constant (0x001C)
		short log2BigBlockSize_;	// Log, base 2, of the big block size (0x001E)
		int log2SmallBlockSize_;	// Log, base 2, of the small block size (0x0020)
		int uk8_;					// Unknown constant (0x0024)
		int uk9_;					// Unknown constant (0x0028)
		int BATCount_;				// Number of elements in the BAT array (0x002C)
		int propertiesStart_;		// Block index of the first block of the property table (0x0030)
		int uk10_;					// Unknown constant (0x0034)
		int uk11_;					// Unknown constant (0x0038)
		int SBATStart_;				// Block index of first big block containing the small block allocation table (SBAT) (0x003C)
		int SBATCount_;				// Number of big blocks holding the SBAT (0x0040)
		int XBATStart_;				// Block index of the first block in the Extended Block Allocation Table (XBAT) (0x0044)
		int XBATCount_;				// Number of elements in the Extended Block Allocation Table (to be added to the BAT) (0x0048)
		int BATArray_[109];			// Array of block indices constituting the Block Allocation Table (BAT) (0x004C, 0x0050, 0x0054 ... 0x01FC)

		size_t bigBlockSize_;
		size_t smallBlockSize_;

	private:
		void Initialize();
	};	
	Header header_;

	// BAT related functions and data members
	void LoadBAT();
	void SaveBAT();
	size_t DataSize(size_t startIndex, bool isBig);
	size_t ReadData(size_t startIndex, char* data, bool isBig);
	size_t WriteData(const char* data, size_t size, int startIndex, bool isBig);
	void GetBlockIndices(size_t startIndex, vector<size_t>& indices, bool isBig);
	size_t GetFreeBlockIndex(bool isBig);
	void ExpandBATArray(bool isBig);
	void LinkBlocks(size_t from, size_t to, bool isBig);
	void FreeBlocks(vector<size_t>& indices, bool isBig);
	vector<int> blocksIndices_;
	vector<int> sblocksIndices_;	

	// Properties related functions and data members
	class Property
	{
	public:
		Property();		
		void Write(char* block);
		void Read(char* block);
		friend bool operator==(const Property& lhs, const Property& rhs) 
		{
			return (!wcscmp(lhs.name_, rhs.name_));
		}
		friend bool operator< (const Property& lhs, const Property& rhs)
		{
			size_t maxLen1 = wcslen(lhs.name_);
			size_t maxLen2 = wcslen(rhs.name_);
			if (maxLen1 < maxLen2) return true;
			else if (maxLen1 > maxLen2) return false;
			else
			{
				int result = wcscmp(lhs.name_, rhs.name_);
				if (result <= 0) return true;
				else return false;
			}
		}
		friend bool operator!=(const Property& lhs, const Property& rhs) {return !(lhs == rhs);}
		friend bool operator> (const Property& lhs, const Property& rhs) {return (rhs < lhs);}
		friend bool operator<=(const Property& lhs, const Property& rhs) {return !(rhs < lhs);}
		friend bool operator>=(const Property& lhs, const Property& rhs) {return !(lhs < rhs);}

		wchar_t name_[32];				// A unicode null-terminated uncompressed 16bit string (lblocke the high bytes) containing the name of the property. (0x00, 0x02, 0x04, ... 0x3E)
		size_t nameSize_;				// Number of characters in the NAME field (0x40)
		unsigned char propertyType_;	// Property type (directory, file, or root) Byte 1 (directory), 2 (file), or 5 (root entry) (0x42)
		unsigned char nodeColor_;		// Node color (0x43)
		int previousProp_;				// Previous property index (0x44)
		int nextProp_;					// Next property index (0x48)
		int childProp_;					// First child property index (0x4c)
		int uk1_;
		int uk2_;
		int uk3_;
		int uk4_;
		int uk5_;
		int seconds1_;					// Seconds component of the created timestamp? (0x64)
		int days1_;						// Days component of the created timestamp? (0x68)
		int seconds2_;					// Seconds component of the modified timestamp? (0x6C)
		int days2_;						// Days component of the modified timestamp? (0x70)
		int startBlock_;				// Starting block of the file, used as the first block in the file and the pointer to the next block from the BAT (0x74)
		int size_;						// Actual size of the file this property points to. (used to truncate the blocks to the real size). (0x78)
	};
	class PropertyTree
	{
	public:
		PropertyTree();
		~PropertyTree();
		PropertyTree* parent_;
		Property* self_;
		size_t index_;
		vector<PropertyTree*> children_;
	};
	void LoadProperties();
	void SaveProperties();
	int MakeProperty(const wchar_t* path, Property* property);
	PropertyTree* FindProperty(size_t index);
	PropertyTree* FindProperty(const wchar_t* path);
	PropertyTree* FindProperty(PropertyTree* parentTree, wchar_t* name);
	void InsertPropertyTree(PropertyTree* parentTree, Property* property, size_t index);
	void DeletePropertyTree(PropertyTree* tree);
	void UpdateChildrenIndices(PropertyTree* parentTree);
	void IncreasePropertyReferences(PropertyTree* parentTree, size_t index);
	void DecreasePropertyReferences(PropertyTree* parentTree, size_t index);
	PropertyTree* propertyTrees_;
	PropertyTree* currentDirectory_;
	vector<Property*> properties_;
	vector<PropertyTree*> previousDirectories_;
};
} // YCompoundFiles namespace end

namespace YExcel
{
using namespace YCompoundFiles;

struct CODE
{
	enum {	FORMULA=0x0006,		//Token array and the result of a formula cell.
			YEOF=0x000A,			//End of a record block with leading BOF record.
			CALCCOUNT=0x000C,	//Maximum number of times the forumlas should be iteratively calculated
			CALCMODE=0x000D,	//Calculate formulas manually, automatically, or automatically except for multiple table operations
			PRECISION=0x000E,	//Whether formulas use the real cell values for calculation or the values displayed on the screen.
			REFMODE=0x000F,		//Method used to show cell addresses in formulas.
			DELTA=0x0010,		//Maximum change of the result to exit an iteration.
			ITERATION=0x0011,	//Whether iterations are allowed while calculating recursive formulas.
			PROTECT=0x0012,		//Whether worksheet or a workbook is protected against modification.
			PASSWORD=0x0013,	//16-bit hash value, calculated from the worksheet or workbook protection password.
			HEADER=0x0014,		//Page header string for the current worksheet.
			FOOTER=0x0015,		//Page footer string for the current worksheet.
			EXTERNSHEET=0x0017,	//List with indexes to SUPBOOK records
			NAME=0x0018,		//Name and token array of an internal defined name.
			WINDOWPROTECT=0x0019,	//Whether the window configuration of the document is protected.
			SELECTION=0x001D,	//Addresses of all selected cell ranges and position of the active cell for a pane in the current sheet.
			DATEMODE=0x0022,	//Base date for displaying date values.
			EXTERNNAME=0x0023,	//Name of an external defined name, name of an add-in function, a DDE item or an OLE object storage identifier.
			LEFTMARGIN=0x0026,	//Left page margin of the current worksheet.
			RIGHTMARGIN=0x0027,	//Right page margin of the current worksheet.
			TOPMARGIN=0x0028,	//Top page margin of the current worksheet.
			BOTTOMMARGIN=0x0029,	//Bottom page margin of current worksheet
			PRINTHEADERS=0x002A,	//Whether row and column headers (the areas with row numbers and column letters) will be printed.
			PRINTGRIDLINES=0x002B,	//Whether sheet grid lines will be printed.
			FILEPASS=0x002F,	//Information about the read/write password of the file.
			FONT=0x0031,		//Information about a used font, including character formatting.
			TABLE=0x0036,		//Information about a multiple operation table in the sheet.
			CONTINUE=0x003C,	//Continue from previous record
			WINDOW1=0x003D,		//General settings for the workbook global settings.
			BACKUP=0x0040,		//Make backup of file while saving?
			PANE=0x0041,		//Position of window panes.
			CODEPAGE=0x0042,	//Text encoding used to encode byte strings
			DCONREF=0x0051,
			DEFCOLWIDTH=0x0055,	//Default column width for columns that do not have a specific width set
			XCT=0x0059,			//Number of immediately following CRN records.
			CRN=0x005A,			//Contents of an external cell or cell range.
			FILESHARING=0x005B,	//Information about write protection, for instance the write protection password.
			WRITEACCESS=0x005C,	//Name of the user that has saved the file.
			UNCALCED=0x005E,	//Formulas have not been recalculated before the document was saved.
			SAVERECALC=0x005F,	//"Recalculate before save" option
			OBJECTPROTECT=0x0063,	//Whether objects of the current sheet are protected.
			COLINFO=0x007D,		//Width for a given range of columns
			GUTS=0x0080,		//Layout of outline symbols.
			WSBOOL=0x0081,		//16-bit value with boolean options for the current sheet.
			GRIDSET=0x0082,		//Whether option to print sheet grid lines has ever been changed.
			HCENTER=0x0083,		//Sheet is centred horizontally when printed.
			VCENTER=0x0084,		//Whether sheet is centred vertically when printed.
			BOUNDSHEET=0x0085,	//Sheet inside of the workbook
			WRITEPROT=0x0086,	//Whether file is write protected.
			COUNTRY=0x008C,		//User interface language of the Excel version that has saved the file, system regional settings at the time the file was saved.
			HIDEOBJ=0x008D,		//Whether and how to show objects in the workbook.
			SORT=0x0090,		//Last settings from the "Sort" dialogue for each sheet.
			PALETTE=0x0092,		//Definition of all user-defined colours available for cell and object formatting.
			SETUP=0x00A1,		//Page format settings of the current sheet.
			SHRFMLA=0x00BC,		//Token array of a shared formula.
			MULRK=0x00BD,		//Cell range containing RK value cells. All cells are located in the same row.
			MULBLANK=0x00BE,	//Cell range of empty cells. All cells are located in the same row.
			DBCELL=0x00D7,		//Relative offsets to calculate stream position of the first cell record for each row.
			BOOKBOOL=0x00DA,	//Save values linked from external workbooks records and XCT records?
			SCENPROTECT=0x00DD,	//Whether scenarios of the current sheet are protected.
			XF=0x00E0,			//Formatting information for cells, rows, columns or styles.
			MERGEDCELLS=0x00E5,	//All merged cell ranges of the current sheet.
			SST=0x00FC,			//List of all strings used anywhere in the workbook.
			LABELSST=0x00FD,	//Cell that contains a string.
			EXTSST=0x00FF,		//Create a hash table with stream offsets to the SST record to optimise string search operations.
			LABELRANGES=0x015F,	//Addresses of all row and column label ranges in the current sheet.
			USESELFS=0x0160,	//Whether formulas in the workbook can use "natural language formulas".
			DSF=0x0161,			//Whether file contains an addition BIFF5/BIFF7 workbook stream.
			SUPBOOK=0x01AE,		//URL of an external document and a list of sheet names inside this document.
			CONDFMT=0x01B0,		//List of cell range addresses for all cells with equal conditional formatting.
			CF=0x01B1,			//Condition and the formatting attributes applied to the cells specified in the CONDFMT record, if the condition is met
			DVAL=0x01B2,		//List header of the data validity table in the current sheet.
			HLINK=0x01B8,		//One cell address or a cell range where all cells contain the same hyperlink.
			DV=0x01BE,			//Data validity settings and a list of cell ranges which contain these settings.
			DIMENSIONS=0x0200,	//Range address of the used area in the current sheet.
			BLANK=0x0201,		//Empty cell, contains cell address and formatting information
			NUMBER=0x0203,		//Cell that contains a floating-point value.
			BOOLERR=0x0205,		//Error value cell
			STRING=0x0207,		//Result of a string formula.
			ROW=0x0208,			//Properties of a single row in the sheet.
			INDEX=0x020B,		//Range of used rows and stream positions of several records of the current sheet.
			ARRAY=0x0221,		//Token array of an array formula
			WINDOW2=0x023E,		//Additional settings for the window of a specific worksheet.
			RK=0x027E,			//Cell that contains an RK value (encoded integer or floating point value).
			STYLE=0x0293,		//Name of a user-defined cell style or specific options for a built-in cell style.
			FORMAT=0x041E,		//Number format.
			SHRFMLA1=0x04BC,	//Token array of a shared formula (added).
			QUICKTIP=0x0800,	//Cell range and text for a tool tip.
			BOF=0x0809,			//Beginning of file
			SHEETLAYOUT=0x0862,	//Colour of the tab below the sheet containing the sheet name.
			SHEETPROTECTION=0x0867,	//Additional options for sheet protection.
			RANGEPROTECTION=0x0868	//Information about special protected ranges in a protected sheet.
		};
};

class Record
{
public:
	Record();
	virtual ~Record();
	virtual size_t Read(const char* data);
	virtual size_t Write(char* data);	
	virtual size_t DataSize();
	virtual size_t RecordSize();
	short code_;
	vector<char> data_;
	size_t dataSize_;
	size_t recordSize_;
	vector<size_t> continueIndices_;
};

struct BOF : public Record
{
	BOF();
	virtual size_t Read(const char* data);
	virtual size_t Write(char* data);	
	short version_;
	short type_;
	short buildIdentifier_;
	short buildYear_;
	int fileHistoryFlags_;
	int lowestExcelVersion_;
};

struct YEOF : public Record
{
	YEOF();
};

struct SmallString
{
	SmallString();
	~SmallString();
	SmallString(const SmallString& s);
	SmallString& operator=(const SmallString& s);
	const SmallString& operator=(const char* str);
	const SmallString& operator=(const wchar_t* str);
	void Reset();
	size_t Read(const char* data);
	size_t Write(char* data);	
	size_t DataSize();
	size_t RecordSize();
	size_t StringSize();
	wchar_t* wname_;
	char* name_;
	char unicode_;
};

struct LargeString
{
	LargeString();
	~LargeString();
	LargeString(const LargeString& s);
	LargeString& operator=(const LargeString& s);
	const LargeString& operator=(const char* str);
	const LargeString& operator=(const wchar_t* str);
	void Reset();
	size_t Read(const char* data);
	size_t ContinueRead(const char* data, size_t size);
	size_t Write(char* data);	
	size_t DataSize();
	size_t RecordSize();
	size_t StringSize();
	vector<wchar_t> wname_;
	vector<char> name_;
	char unicode_;
	short richtext_;
	int phonetic_;
};

class Workbook
{
public:
	Workbook();

public:
	struct FileProtection;
	struct CodePage;
	struct DSF;
	struct TabID;
	struct FnGroupCount;
	struct WorkbookProtection;
	struct Window1 : public Record
	{
		Window1();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		short horizontalPos_;
		short verticalPos_;
		short width_;
		short height_;
		short options_;
		short activeWorksheetIndex_;
		short firstVisibleTabIndex_;
		short selectedWorksheetNo_;
		short worksheetTabBarWidth_;
	};
	struct Backup;
	struct HideObj;
	struct DateMode;
	struct Precision;
	struct RefreshAll;
	struct BookBool;
	struct Font : public Record
	{
		Font();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		virtual size_t DataSize();
		virtual size_t RecordSize();
		short height_;
		short options_;
		short colourIndex_;
		short weight_;
		short escapementType_;
		char underlineType_;
		char family_;
		char characterSet_;
		char unused_;
		SmallString name_;
	};
	struct Format;
	struct XF : public Record
	{
		XF();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		short fontRecordIndex_;
		short formatRecordIndex_;
		short protectionType_;
		char alignment_;
		char rotation_;
		char textProperties_;
		char usedAttributes_;
		int borderLines_;
		int colour1_;
		short colour2_;
	};
	struct Style : public Record
	{
		Style();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		virtual size_t DataSize();
		virtual size_t RecordSize();
		short XFRecordIndex_;
		char identifier_;
		char level_;
		LargeString name_;		
	};
	struct Palette;
	struct UseSelfs;
	struct BoundSheet : public Record
	{
		BoundSheet();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		virtual size_t DataSize();
		virtual size_t RecordSize();
		int BOFpos_;
		char visibility_;
		char type_;
		SmallString name_;		
	};
	struct Country;
	struct LinkTable;
	struct SharedStringTable : public Record
	{
		SharedStringTable();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		virtual size_t DataSize();
		virtual size_t RecordSize();
		int stringsTotal_;
		int uniqueStringsTotal_;
		vector<LargeString> strings_;	
	};
	struct ExtSST : public Record
	{
		ExtSST();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);
		virtual size_t DataSize();
		virtual size_t RecordSize();
		size_t stringsTotal_;
		vector<int> streamPos_;
		vector<size_t> firstStringPos_;
		vector<size_t> unused_;
	};
	size_t Read(const char* data);
	size_t Write(char* data);
	size_t DataSize();
	size_t RecordSize();

	BOF bof_;
	Window1 window1_;
	vector<Font> fonts_;
	vector<XF> XFs_;
	vector<Style> styles_;
	vector<BoundSheet> boundSheets_;
	SharedStringTable sst_;
	ExtSST extSST_;
	YEOF eof_;
};

class Worksheet
{
public:
	Worksheet();

public:
	struct Uncalced;
	struct Index : public Record
	{
		Index();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		virtual size_t DataSize();
		virtual size_t RecordSize();
		int unused1_;
		size_t firstUsedRowIndex_;
		size_t firstUnusedRowIndex_;
		int unused2_;
		vector<size_t> DBCellPos_;
		
	};
	struct CalculationSettings
	{
		struct CalcCount;
		struct CalcMode;
		struct RefMode;
		struct Delta;
		struct Iteration;
		struct SafeRecalc;		
	};
	struct PrintHeaders;
	struct PrintGridlines;
	struct Gridset;
	struct Guts;
	struct DefaultRowHeight;
	struct WSBool;
	struct PageSettings
	{
		struct Header;
		struct Footer;
		struct HCenter;
		struct VCenter;
		struct LeftMargin;
		struct RightMargin;
		struct TopMargin;
		struct BottomMargin;
		struct PLS;
		struct Setup;
	};
	struct WorksheetProtection;
	struct DefColWidth;
	struct ColInfo;
	struct Sort;
	struct Dimensions : public Record
	{
		Dimensions();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		size_t firstUsedRowIndex_;
		size_t lastUsedRowIndexPlusOne_;
		size_t firstUsedColIndex_;
		size_t lastUsedColIndexPlusOne_;
		short unused_;		
	};
	struct CellTable
	{
		struct RowBlock
		{
			struct Row : public Record
			{
				Row();
				virtual size_t Read(const char* data);
				virtual size_t Write(char* data);	
				size_t rowIndex_;
				size_t firstCellColIndex_;
				size_t lastCellColIndexPlusOne_;
				size_t height_;
				short unused1_;
				short unused2_;
				int options_;
			};
			struct CellBlock
			{
				struct Blank : public Record
				{
					Blank();
					virtual size_t Read(const char* data);
					virtual size_t Write(char* data);	
					short rowIndex_;
					short colIndex_;
					short XFRecordIndex_;					
				};
				struct BoolErr : public Record
				{
					BoolErr();
					virtual size_t Read(const char* data);
					virtual size_t Write(char* data);	
					short rowIndex_;
					short colIndex_;
					short XFRecordIndex_;
					char value_;
					char error_;	
				};
				struct LabelSST : public Record
				{
					LabelSST();
					virtual size_t Read(const char* data);
					virtual size_t Write(char* data);	
					size_t rowIndex_;
					size_t colIndex_;
					size_t XFRecordIndex_;
					size_t SSTRecordIndex_;	
				};
				struct MulBlank : public Record
				{
					MulBlank();
					virtual size_t Read(const char* data);
					virtual size_t Write(char* data);	
					virtual size_t DataSize();
					virtual size_t RecordSize();
					short rowIndex_;
					short firstColIndex_;
					vector<short> XFRecordIndices_;
					short lastColIndex_;
				};
				struct MulRK : public Record
				{
					MulRK();
					virtual size_t Read(const char* data);
					virtual size_t Write(char* data);
					virtual size_t DataSize();
					virtual size_t RecordSize();
					struct XFRK
					{
						XFRK();
						void Read(const char* data);
						void Write(char* data);
						short XFRecordIndex_;
						int RKValue_;
					};
					size_t rowIndex_;
					size_t firstColIndex_;
					vector<XFRK> XFRK_;
					size_t lastColIndex_;
				};
				struct Number : public Record
				{
					Number();
					virtual size_t Read(const char* data);
					virtual size_t Write(char* data);	
					size_t rowIndex_;
					size_t colIndex_;
					size_t XFRecordIndex_;
					double value_;

				private:
					union 
					{
						__int64 intvalue_;
						double doublevalue_;
					} intdouble_;
				};
				struct RK : public Record
				{
					RK();
					virtual size_t Read(const char* data);
					virtual size_t Write(char* data);	
					size_t rowIndex_;
					size_t colIndex_;
					size_t XFRecordIndex_;
					int value_;	
				};

				struct Formula : public Record
				{
					struct Array : public Record
					{
						Array();
						virtual size_t Read(const char* data);
						virtual size_t Write(char* data);	
						virtual size_t DataSize();
						virtual size_t RecordSize();
						short firstRowIndex_;
						short lastRowIndex_;
						char firstColIndex_;
						char lastColIndex_;
						short options_;
						int unused_;
						vector<char> formula_;
					};
					struct ShrFmla : public Record
					{
						ShrFmla();
						virtual size_t Read(const char* data);
						virtual size_t Write(char* data);	
						virtual size_t DataSize();
						virtual size_t RecordSize();
						short firstRowIndex_;
						short lastRowIndex_;
						char firstColIndex_;
						char lastColIndex_;
						short unused_;
						vector<char> formula_;
					};
					struct ShrFmla1 : public Record
					{
						ShrFmla1();
						virtual size_t Read(const char* data);
						virtual size_t Write(char* data);	
						virtual size_t DataSize();
						virtual size_t RecordSize();
						short firstRowIndex_;
						short lastRowIndex_;
						char firstColIndex_;
						char lastColIndex_;
						short unused_;
						vector<char> formula_;
					};
					struct Table : public Record
					{
						Table();
						virtual size_t Read(const char* data);
						virtual size_t Write(char* data);	
						short firstRowIndex_;
						short lastRowIndex_;
						char firstColIndex_;
						char lastColIndex_;
						short options_;
						short inputCellRowIndex_;
						short inputCellColIndex_;
						short inputCellColumnInputRowIndex_;
						short inputCellColumnInputColIndex_;
					};
					struct String : public Record
					{
						String();
						virtual size_t Read(const char* data);
						virtual size_t Write(char* data);	
						virtual size_t DataSize();
						virtual size_t RecordSize();
						vector<wchar_t> string_;
					};

					Formula();
					virtual size_t Read(const char* data);
					virtual size_t Write(char* data);	
					virtual size_t DataSize();
					virtual size_t RecordSize();
					short rowIndex_;
					short colIndex_;
					short XFRecordIndex_;
					char result_[8];
					short options_;
					int unused_;
					vector<char> RPNtoken_;
					short type_;

					Array array_;
					ShrFmla shrfmla_;
					ShrFmla1 shrfmla1_;
					Table table_;
					String string_;
				};

				CellBlock();
				~CellBlock();
				void Reset();
				size_t Read(const char* data);
				size_t Write(char* data);
				size_t DataSize();
				size_t RecordSize();
				size_t RowIndex();
				size_t ColIndex();
				size_t LastColIndex();
				short type_;
				bool normalType_;
				
				Blank blank_;
				BoolErr boolerr_;
				LabelSST labelsst_;
				MulBlank mulblank_;
				MulRK mulrk_;
				Number number_;
				RK rk_;
				Formula formula_;
			};
			struct DBCell : public Record
			{
				DBCell();
				virtual size_t Read(const char* data);
				virtual size_t Write(char* data);	
				virtual size_t DataSize();
				virtual size_t RecordSize();
				int firstRowOffset_;
				vector<size_t> offsets_;
			};			
			
			size_t Read(const char* data);
			size_t Write(char* data);
			size_t DataSize();
			size_t RecordSize();
		
			vector<Row> rows_;
			vector<CellBlock> cellBlocks_;
			DBCell dbcell_;
		};	
		size_t Read(const char* data);
		size_t Write(char* data);
		size_t DataSize();
		size_t RecordSize();
	
		vector<RowBlock> rowBlocks_;		
	};
	struct Window2 : public Record
	{
		Window2();
		virtual size_t Read(const char* data);
		virtual size_t Write(char* data);	
		short options_;
		short firstVisibleRowIndex_;
		short firstVisibleColIndex_;
		short gridLineColourIndex_;
		short unused1_;
		short magnificationFactorPageBreakPreview_;
		short magnificationFactorNormalView_;
		int unused2_;		
	};
	struct SCL;
	struct Pane;
	struct Selection;
	struct MergedCells;
	struct LabelRanges;
	struct ConditionalFormattingTable;
	struct HyperlinkTable;
	struct SheetLayout;
	struct SheetProtection;
	struct RangeProtection;

	size_t Read(const char* data);
	size_t Write(char* data);
	size_t DataSize();
	size_t RecordSize();

	BOF bof_;
	Index index_;
	Dimensions dimensions_;
	CellTable cellTable_;
	Window2 window2_;
	YEOF eof_;
};

bool IsRKValueAnInteger(int rkValue);		///< Returns true if the supplied rk value contains an integer. 
bool IsRKValueADouble(int rkValue);			///< Returns true if the supplied rk value contains a double.
double GetDoubleFromRKValue(int rkValue);	///< Convert a rk value to a double.
int GetIntegerFromRKValue(int rkValue);		///< Convert a rk value to an integer.
int GetRKValueFromDouble(double value);		///< Convert a double to a rk value.
int GetRKValueFromInteger(int value);		///< Convert an integer to a rk value.
bool CanStoreAsRKValue(double value);		///< Returns true if the supplied double can be stored as a rk value.

// Forward declarations
class BasicExcel;
class BasicExcelWorksheet;
class BasicExcelCell;

/*******************************************************************************************************/
/*                         Actual classes to read and write to Excel files                             */
/*******************************************************************************************************/
class BasicExcel
{
public:
	BasicExcel();
	BasicExcel(const char* filename);
	~BasicExcel();

public: // File functions.
	void New(int sheets=3);	///< Create a new Excel workbook with a given number of spreadsheets (Minimum 1).
	bool Load(const char* filename);	///< Load an Excel workbook from a file.
	bool Save();	///< Save current Excel workbook to opened file.
	bool SaveAs(const char* filename);	///< Save current Excel workbook to a file.

public: // Worksheet functions.
	size_t GetTotalWorkSheets();	///< Total number of Excel worksheets in current Excel workbook.

	BasicExcelWorksheet* GetWorksheet(size_t sheetIndex);	///< Get a pointer to an Excel worksheet at the given index. Index starts from 0. Returns 0 if index is invalid.
	BasicExcelWorksheet* GetWorksheet(const char* name);	///< Get a pointer to an Excel worksheet that has given ANSI name. Returns 0 if there is no Excel worksheet with the given name.
	BasicExcelWorksheet* GetWorksheet(const wchar_t* name);	///< Get a pointer to an Excel worksheet that has given Unicode name. Returns 0 if there is no Excel worksheet with the given name.
	
	BasicExcelWorksheet* AddWorksheet(int sheetIndex=-1);	///< Add a new Excel worksheet to the given index. Name given to worksheet is SheetX, where X is a number which starts from 1. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.
	BasicExcelWorksheet* AddWorksheet(const char* name, int sheetIndex=-1);	///< Add a new Excel worksheet with given ANSI name to the given index. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.
	BasicExcelWorksheet* AddWorksheet(const wchar_t* name, int sheetIndex=-1);	///< Add a new Excel worksheet with given Unicode name to the given index. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.

	bool DeleteWorksheet(size_t sheetIndex);	///< Delete an Excel worksheet at the given index. Index starts from 0. Returns true if successful, false if otherwise.
	bool DeleteWorksheet(const char* name);		///< Delete an Excel worksheet that has given ANSI name. Returns true if successful, false if otherwise.
	bool DeleteWorksheet(const wchar_t* name);	///< Delete an Excel worksheet that has given Unicode name. Returns true if successful, false if otherwise.

	char* GetAnsiSheetName(size_t sheetIndex);	///< Get the worksheet name at the given index. Index starts from 0. Returns 0 if name is in Unicode format.
	wchar_t* GetUnicodeSheetName(size_t sheetIndex);///< Get the worksheet name at the given index. Index starts from 0. Returns 0 if name is in Ansi format.
	bool GetSheetName(size_t sheetIndex, char* name);		///< Get the worksheet name at the given index. Index starts from 0. Returns false if name is in Unicode format.
	bool GetSheetName(size_t sheetIndex, wchar_t* name);	///< Get the worksheet name at the given index. Index starts from 0. Returns false if name is in Ansi format.

	bool RenameWorksheet(size_t sheetIndex, const char* to);		///< Rename an Excel worksheet at the given index to the given ANSI name. Index starts from 0. Returns true if successful, false if otherwise.
	bool RenameWorksheet(size_t sheetIndex, const wchar_t* to);		///< Rename an Excel worksheet at the given index to the given Unicode name. Index starts from 0. Returns true if successful, false if otherwise.
	bool RenameWorksheet(const char* from, const char* to);			///< Rename an Excel worksheet that has given ANSI name to another ANSI name. Returns true if successful, false if otherwise.
	bool RenameWorksheet(const wchar_t* from, const wchar_t* to);	///< Rename an Excel worksheet that has given Unicode name to another Unicode name. Returns true if successful, false if otherwise.

private: // Functions to read and write raw Excel format.
	size_t Read(const char* data, size_t dataSize);
	size_t Write(char* data);
	void AdjustStreamPositions();
	void AdjustBoundSheetBOFPositions();
	void AdjustDBCellPositions();
	void AdjustExtSSTPositions();

	enum {WORKBOOK_GLOBALS=0x0005, VISUAL_BASIC_MODULE=0x0006,
		  WORKSHEET=0x0010, CHART=0x0020};
	
private: // Internal functions
	void UpdateYExcelWorksheet();	///< Update yesheets_ using information from worksheets_.
	void UpdateWorksheets();		///< Update worksheets_ using information from yesheets_.

public:
	CompoundFile file_;						///< Compound file handler.
	Workbook workbook_;						///< Raw Workbook.
	vector<Worksheet> worksheets_;			///< Raw Worksheets.
	vector<BasicExcelWorksheet> yesheets_;	///< Parsed Worksheets.
};

class BasicExcelWorksheet
{
	friend class BasicExcel;

public:
	BasicExcelWorksheet(BasicExcel* excel, size_t sheetIndex);

public: // Worksheet functions
	char* GetAnsiSheetName();	///< Get the current worksheet name. Returns 0 if name is in Unicode format.
	wchar_t* GetUnicodeSheetName();///< Get the current worksheet name. Returns 0 if name is in Ansi format.
	bool GetSheetName(char* name);		///< Get the current worksheet name. Returns false if name is in Unicode format.
	bool GetSheetName(wchar_t* name);	///< Get the current worksheet name. Returns false if name is in Ansi format.
	bool Rename(const char* to);	///< Rename current Excel worksheet to another ANSI name. Returns true if successful, false if otherwise.
	bool Rename(const wchar_t* to);///< Rename current Excel worksheet to another Unicode name. Returns true if successful, false if otherwise.
	void Print(ostream& os, char delimiter=',', char textQualifier='\0'); ///< Print entire worksheet to an output stream, separating each column with the defined delimiter and enclosing text using the defined textQualifier. Leave out the textQualifier argument if do not wish to have any text qualifiers.

public: // Cell functions
	size_t GetTotalRows();	///< Total number of rows in current Excel worksheet.
	size_t GetTotalCols();	///< Total number of columns in current Excel worksheet.

	BasicExcelCell* Cell(size_t row, size_t col); ///< Return a pointer to an Excel cell. row and col starts from 0. Returns 0 if row exceeds 65535 or col exceeds 255.
	bool EraseCell(size_t row, size_t col); ///< Erase content of a cell. row and col starts from 0. Returns true if successful, false if row or col exceeds range.

private: // Internal functions
	void UpdateCells();	///< Update cells using information from BasicExcel.worksheets_.

private:
	BasicExcel* excel_;					///< Pointer to instance of BasicExcel.
	size_t sheetIndex_;					///< Index of worksheet in workbook.
	size_t maxRows_;					///< Total number of rows in worksheet.
	size_t maxCols_;					///< Total number of columns in worksheet.
	vector<vector<BasicExcelCell> > cells_;	///< Cells matrix.
};

class BasicExcelCell
{
public:
	BasicExcelCell();

public:
	enum {UNDEFINED, INT, DOUBLE, STRING, WSTRING};
	int Type() const;		///< Get type of value stored in current Excel cell. Returns one of the above enums. 

	bool Get(int& val) const;		///< Get an integer value. Returns false if cell does not contain an integer or a double.
	bool Get(double& val) const;	///< Get a double value. Returns false if cell does not contain a double or an integer.
	bool Get(char* str) const;	///< Get an ANSI string. Returns false if cell does not contain an ANSI string.
	bool Get(wchar_t* str) const;	///< Get an Unicode string. Returns false if cell does not contain an Unicode string.
	size_t GetStringLength() const;	///< Return length of ANSI or Unicode string (excluding null character).

	int GetInteger() const;		///< Get an integer value. Returns 0 if cell does not contain an integer.
	double GetDouble() const;		///< Get a double value. Returns 0.0 if cell does not contain a double.
	const char* GetString() const;		///< Get an ANSI string. Returns 0 if cell does not contain an ANSI string.
	const wchar_t* GetWString() const;	///< Get an Unicode string. Returns 0 if cell does not contain an Unicode string.

	friend ostream& operator<<(ostream& os, const BasicExcelCell& cell);	///< Print cell to output stream. Print a null character if cell is undefined.

	void Set(int val);				///< Set content of current Excel cell to an integer.
	void Set(double val);			///< Set content of current Excel cell to a double.
	void Set(const char* str);		///< Set content of current Excel cell to an ANSI string.
	void Set(const wchar_t* str);	///< Set content of current Excel cell to an Unicode string.
	
	void SetInteger(int val);			///< Set content of current Excel cell to an integer.
	void SetDouble(double val);			///< Set content of current Excel cell to a double.
	void SetString(const char* str);	///< Set content of current Excel cell to an ANSI string.
	void SetWString(const wchar_t* str);///< Set content of current Excel cell to an Unicode string.

	void EraseContents();	///< Erase the content of current Excel cell. Set type to UNDEFINED.

private:
	int type_;				///< Type of value stored in current Excel cell. Contains one of the above enums.
	int ival_;				///< Integer value stored in current Excel cell.
	double dval_;			///< Double value stored in current Excel cell.
	vector<char> str_;		///< ANSI string stored in current Excel cell. Include null character.
	vector<wchar_t> wstr_;	///< Unicode string stored in current Excel cell. Include null character.
};

} // Namespace end
#endif
