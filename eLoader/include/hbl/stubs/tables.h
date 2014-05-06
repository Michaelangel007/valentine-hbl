#ifndef ELOADER_TABLES
#define ELOADER_TABLES

#include <common/sdk.h>
#include <hbl/eloader.h>

// Struct holding all NIDs imported by the game and their respective jump/syscalls
typedef struct
{
	u32 nid;	            // NID
	u32 call;	            // Syscall/jump associated to the NID
	unsigned int lib_index; // Index to the library descriptor tSceLibrary
} tNIDResolver;

typedef struct
{
	unsigned int num;					// Number of nids on table
	tNIDResolver table[NID_TABLE_SIZE];	// NID resolver
} HBLNIDTable;

typedef enum
{
	SYSCALL_MODE = 0,
	JUMP_MODE = 1
} tCallingMode;

// Struct holding info to help syscall estimation
// This struct is for each library imported by the game
typedef struct
{
	char name[MAX_LIBRARY_NAME_LENGTH];			// Library name
	tCallingMode calling_mode;					// Defines how library exports are called
	unsigned int num_library_exports;			// Number of exported functions in library
	unsigned int num_known_exports;				// Number of known exported functions (exports we know the syscall of)
	u32 lowest_syscall;							// Lowest syscall number found
	u32 lowest_nid;								// NID associated to lowest syscall
	unsigned int lowest_index;					// Lowest NID index nids_table
    u32 highest_syscall;                        // Highest syscall number found
	u32 gap;                                    // Offset between the syscall for the highest and the lowest nid index in 6.20
} tSceLibrary;

typedef struct
{
	unsigned int num;
	tSceLibrary table[MAX_LIBRARIES];
} HBLLibTable;


// Arrays for perfect syscall estimation are ordered by this enum
typedef enum
{
	LIBRARY_INTERRUPTMANAGER = 0,
	LIBRARY_THREADMANFORUSER,
	LIBRARY_STDIOFORUSER,
	LIBRARY_IOFILEMGRFORUSER,
	LIBRARY_MODULEMGRFORUSER,
	LIBRARY_SYSMEMUSERFORUSER,
	LIBRARY_SCESUSPENDFORUSER,
	LIBRARY_UTILSFORUSER,
	LIBRARY_LOADEXECFORUSER,
	LIBRARY_SCEGE_USER,
	LIBRARY_SCERTC,
	LIBRARY_SCEAUDIO,
	LIBRARY_SCEDISPLAY,
	LIBRARY_SCECTRL,
	LIBRARY_SCEHPRM,
	LIBRARY_SCEPOWER,
	LIBRARY_SCEOPENPSID,
	LIBRARY_SCEWLANDRV,
	LIBRARY_SCEUMDUSER,
	LIBRARY_SCEUTILITY,
	LIBRARY_SCESASCORE,
	LIBRARY_SCEIMPOSE,
	LIBRARY_SCEREG,
	LIBRARY_SCECHNNLSV
} HBLLibraryIndex;


// Describes the information found at the memory address
// for the lowest syscall in the GO kernel memory dump
typedef struct
{
	u32 lowest_syscall;			// Lowest syscall assigned to the library
	char padding[16];			// Unknown/unimportant data
	u16 offset_to_zero_index;	// Random padding to the first NIDs syscall
	u16 gap;					// Random Padding (1 to 4) between the last and the first NIDs syscall
} HBLKernelLibraryStruct;


// Auxiliary structures to help with syscall estimation
extern HBLLibTable* library_table;
extern HBLNIDTable* nid_table;
/*
extern HBLLibTable library_table;
extern HBLNIDTable nid_table;
*/

// Returns nid_table index where the call is found, -1 if not found
int get_call_index(u32 call);

// Gets i-th nid and its associated library
// Returns library name length
int get_lib_nid(u32 index, char* lib_name, u32* pnid);

// Return index in NID table for the call that corresponds to the NID pointed by "nid"
// Puts call in call_buffer
u32 get_call_nidtable(u32 nid, u32* call_buffer);

// Return real instruction that makes the system call (jump or syscall)
u32 get_good_call(u32* call_pointer);

// Fills remaining information on a library
tSceLibrary* complete_library(tSceLibrary* plibrary, int reference_library_index, int is_cfw);

// Returns index of NID in table
int get_nid_index(u32 nid);

// Checks if a library is in the global library description table
// Returns index if it's there
int get_library_index(const char* library_name);

// Fills NID Table and a part of library_table
// Returns NIDs copied
int build_nid_table();

/*
 * Retrieves highest known syscall of the previous library,
 * and lowest known syscall of the next library, to get some
 * rough boundaries of where current library's syscalls should be
 * returns 1 on success, 0 on failure
*/
int get_syscall_boundaries(int lib_index, u32* low, u32* high);

// Adds NID entry to nid_table
int add_nid_to_table(u32 nid, u32 call, unsigned int lib_index);

// Initialize nid_table
void* init_nid_table();

// Initialize library_table
void* init_library_table();

// Return index in nid_table for closest higher known NID
int get_higher_known_nid(unsigned int lib_index, u32 nid);

// Return index in nid_table for closest lower known NID
int get_lower_known_nid(unsigned int lib_index, u32 nid);

// Adds a new library
int add_library_to_table(const tSceLibrary lib);

#endif