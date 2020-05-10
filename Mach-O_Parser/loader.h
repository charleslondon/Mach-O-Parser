#pragma once 
#include <stdint.h>

typedef int integer_t;

typedef integer_t	cpu_type_t;
typedef integer_t	cpu_subtype_t;
typedef integer_t	cpu_threadtype_t;

typedef int	vm_prot_t;

/*
 * A variable length string in a load command is represented by an lc_str
 * union.  The strings are stored just after the load command structure and
 * the offset is from the start of the load command structure.  The size
 * of the string is reflected in the cmdsize field of the load command.
 * Once again any padded bytes to bring the cmdsize field to a multiple
 * of 4 bytes must be zero.
 */
union lc_str 
{
    uint32_t	offset;	/* offset to the string */
#ifndef __LP64__
    char* ptr;	/* pointer to the string */
#endif 
};


/*
 * Fixed virtual memory shared libraries are identified by two things.  The
 * target pathname (the name of the library as found for execution), and the
 * minor version number.  The address of where the headers are loaded is in
 * header_addr. (THIS IS OBSOLETE and no longer supported).
 */
struct fvmlib 
{
    union lc_str	name;		/* library's target pathname */
    uint32_t	minor_version;	/* library's minor version number */
    uint32_t	header_addr;	/* library's header address */
};

/*
 * Dynamically linked shared libraries are identified by two things.  The
 * pathname (the name of the library as found for execution), and the
 * compatibility version number.  The pathname must match and the compatibility
 * number in the user of the library must be greater than or equal to the
 * library being used.  The time stamp is used to record the time a library was
 * built and copied into user so it can be use to determined if the library used
 * at runtime is exactly the same as used to built the program.
 */
struct dylib 
{
    union lc_str  name;			/* library's path name */
    uint32_t timestamp;			/* library's build time stamp */
    uint32_t current_version;		/* library's current version number */
    uint32_t compatibility_version;	/* library's compatibility vers number*/
};

/*
 * The symseg_command contains the offset and size of the GNU style
 * symbol table information as described in the header file <symseg.h>.
 * The symbol roots of the symbol segments must also be aligned properly
 * in the file.  So the requirement of keeping the offsets aligned to a
 * multiple of a 4 bytes translates to the length field of the symbol
 * roots also being a multiple of a long.  Also the padding must again be
 * zeroed. (THIS IS OBSOLETE and no longer supported).
 */
struct symseg_command 
{
    uint32_t	cmd;		/* LC_SYMSEG */
    uint32_t	cmdsize;	/* sizeof(struct symseg_command) */
    uint32_t	offset;		/* symbol segment offset */
    uint32_t	size;		/* symbol segment size in bytes */
};

struct ident_command 
{
	uint32_t cmd;		/* LC_IDENT */
	uint32_t cmdsize;	/* strings that follow this command */
};

/*
 * The fvmfile_command contains a reference to a file to be loaded at the
 * specified virtual address.  (Presently, this command is reserved for
 * internal use.  The kernel ignores this command when loading a program into
 * memory).
 */
struct fvmfile_command 
{
    uint32_t cmd;			/* LC_FVMFILE */
    uint32_t cmdsize;		/* includes pathname string */
    union lc_str	name;		/* files pathname */
    uint32_t	header_addr;	/* files virtual address */
};

/*
 * Thread commands contain machine-specific data structures suitable for
 * use in the thread state primitives.  The machine specific data structures
 * follow the struct thread_command as follows.
 * Each flavor of machine specific data structure is preceded by an unsigned
 * long constant for the flavor of that data structure, an uint32_t
 * that is the count of longs of the size of the state data structure and then
 * the state data structure follows.  This triple may be repeated for many
 * flavors.  The constants for the flavors, counts and state data structure
 * definitions are expected to be in the header file <machine/thread_status.h>.
 * These machine specific data structures sizes must be multiples of
 * 4 bytes  The cmdsize reflects the total size of the thread_command
 * and all of the sizes of the constants for the flavors, counts and state
 * data structures.
 *
 * For executable objects that are unix processes there will be one
 * thread_command (cmd == LC_UNIXTHREAD) created for it by the link-editor.
 * This is the same as a LC_THREAD, except that a stack is automatically
 * created (based on the shell's limit for the stack size).  Command arguments
 * and environment variables are copied onto that stack.
 */
struct thread_command 
{
    uint32_t	cmd;		/* LC_THREAD or  LC_UNIXTHREAD */
    uint32_t	cmdsize;	/* total size of this command */
    /* uint32_t flavor		   flavor of thread state */
    /* uint32_t count		   count of longs in thread state */
    /* struct XXX_thread_state state   thread state for this flavor */
    /* ... */
};

/*
 * A fixed virtual shared library (filetype == MH_FVMLIB in the mach header)
 * contains a fvmlib_command (cmd == LC_IDFVMLIB) to identify the library.
 * An object that uses a fixed virtual shared library also contains a
 * fvmlib_command (cmd == LC_LOADFVMLIB) for each library it uses.
 * (THIS IS OBSOLETE and no longer supported).
 */
struct fvmlib_command 
{
    uint32_t	cmd;		/* LC_IDFVMLIB or LC_LOADFVMLIB */
    uint32_t	cmdsize;	/* includes pathname string */
    struct fvmlib	fvmlib;		/* the library identification */
};

struct mach_header 
{
	uint32_t		magic;			/* mach magic number identifier */
	cpu_type_t		cputype;		/* cpu specifier */
	cpu_subtype_t	cpusubtype;		/* machine specifier */
	uint32_t		filetype;		/* type of file */
	uint32_t		ncmds;			/* number of load commands */
	uint32_t		sizeofcmds;		/* the size of all the load commands */
	uint32_t		flags;			/* flags */
};

struct mach_header_64 
{
	uint32_t		magic;		/* mach magic number identifier */
	cpu_type_t		cputype;	/* cpu specifier */
	cpu_subtype_t	cpusubtype;	/* machine specifier */
	uint32_t		filetype;	/* type of file */
	uint32_t		ncmds;		/* number of load commands */
	uint32_t		sizeofcmds;	/* the size of all the load commands */
	uint32_t		flags;		/* flags */
	uint32_t		reserved;	/* reserved */
};

struct load_command 
{
	uint32_t    cmd;		/* type of load command */
	uint32_t    cmdsize;	/* total size of command in bytes */
};

struct segment_command 
{
	uint32_t	cmd;			/* LC_SEGMENT */
	uint32_t	cmdsize;		/* includes sizeof section structs */
	char		segname[16];	/* segment name */
	uint32_t	vmaddr;			/* memory address of this segment */
	uint32_t	vmsize;			/* memory size of this segment */
	uint32_t	fileoff;		/* file offset of this segment */
	uint32_t	filesize;		/* amount to map from the file */
	vm_prot_t	maxprot;		/* maximum VM protection */
	vm_prot_t	initprot;		/* initial VM protection */
	uint32_t	nsects;			/* number of sections in segment */
	uint32_t	flags;			/* flags */
};

/*
 * The 64-bit segment load command indicates that a part of this file is to be
 * mapped into a 64-bit task's address space.  If the 64-bit segment has
 * sections then section_64 structures directly follow the 64-bit segment
 * command and their size is reflected in cmdsize.
 */
struct segment_command_64       /*  for 64-bit architectures */
{
    uint32_t	cmd;		    /*  LC_SEGMENT_64 */
    uint32_t	cmdsize;	    /*  includes sizeof section_64 structs*/
    char		segname[16];    /*	segment name */
    uint64_t	vmaddr;		    /*  memory address of this segment */
    uint64_t	vmsize;		    /*  memory size of this segment */
    uint64_t	fileoff;	    /*  file offset of this segment */
    uint64_t	filesize;	    /*  amount to map from the file */
    vm_prot_t	maxprot;	    /*  maximum VM protection */
    vm_prot_t	initprot;	    /*  initial VM protection */
    uint32_t	nsects;		    /*  number of sections in segment */
    uint32_t	flags;		    /*  flags */
};

struct symtab_command
{
	uint32_t	cmd;		/*  LC_SYMTAB */
	uint32_t	cmdsize;	/*  sizeof(struct symtab_command) */
	uint32_t	symoff;		/*  symbol table offset */
	uint32_t	nsyms;		/*  number of symbol table entries */
	uint32_t	stroff;		/*  string table offset */
	uint32_t	strsize;	/*  string table size in bytes */
};

struct routines_command { /* for 32-bit architectures */
	uint32_t	cmd;		    /*  LC_ROUTINES */
	uint32_t	cmdsize;	    /*  total size of this command */
	uint32_t	init_address;	/*  address of initialization routine */
	uint32_t	init_module;	/*  index into the module table that
						            the init routine is defined in */
	uint32_t	reserved1;
	uint32_t	reserved2;
	uint32_t	reserved3;
	uint32_t	reserved4;
	uint32_t	reserved5;
	uint32_t	reserved6;
};

/*
 * The 64-bit routines command.  Same use as above.
 */
struct routines_command_64 
{ /* for 64-bit architectures */
	uint32_t	cmd;		    /* LC_ROUTINES_64 */
	uint32_t	cmdsize;	    /* total size of this command */
	uint64_t	init_address;	/* address of initialization routine */
	uint64_t	init_module;	/* index into the module table that
					               the init routine is defined in */
	uint64_t	reserved1;
	uint64_t	reserved2;
	uint64_t	reserved3;
	uint64_t	reserved4;
	uint64_t	reserved5;
	uint64_t	reserved6;
};

struct dysymtab_command 
{
    uint32_t cmd;	    /*  LC_DYSYMTAB */
    uint32_t cmdsize;	/*  sizeof(struct dysymtab_command) */

    /*
     * The symbols indicated by symoff and nsyms of the LC_SYMTAB load command
     * are grouped into the following three groups:
     *    local symbols (further grouped by the module they are from)
     *    defined external symbols (further grouped by the module they are from)
     *    undefined symbols
     *
     * The local symbols are used only for debugging.  The dynamic binding
     * process may have to use them to indicate to the debugger the local
     * symbols for a module that is being bound.
     *
     * The last two groups are used by the dynamic binding process to do the
     * binding (indirectly through the module table and the reference symbol
     * table when this is a dynamically linked shared library file).
     */
    uint32_t ilocalsym;	/* index to local symbols */
    uint32_t nlocalsym;	/* number of local symbols */

    uint32_t iextdefsym;/* index to externally defined symbols */
    uint32_t nextdefsym;/* number of externally defined symbols */

    uint32_t iundefsym;	/* index to undefined symbols */
    uint32_t nundefsym;	/* number of undefined symbols */

    /*
     * For the for the dynamic binding process to find which module a symbol
     * is defined in the table of contents is used (analogous to the ranlib
     * structure in an archive) which maps defined external symbols to modules
     * they are defined in.  This exists only in a dynamically linked shared
     * library file.  For executable and object modules the defined external
     * symbols are sorted by name and is use as the table of contents.
     */
    uint32_t tocoff;	/* file offset to table of contents */
    uint32_t ntoc;	/* number of entries in table of contents */

    /*
     * To support dynamic binding of "modules" (whole object files) the symbol
     * table must reflect the modules that the file was created from.  This is
     * done by having a module table that has indexes and counts into the merged
     * tables for each module.  The module structure that these two entries
     * refer to is described below.  This exists only in a dynamically linked
     * shared library file.  For executable and object modules the file only
     * contains one module so everything in the file belongs to the module.
     */
    uint32_t modtaboff;	/* file offset to module table */
    uint32_t nmodtab;	/* number of module table entries */

    /*
     * To support dynamic module binding the module structure for each module
     * indicates the external references (defined and undefined) each module
     * makes.  For each module there is an offset and a count into the
     * reference symbol table for the symbols that the module references.
     * This exists only in a dynamically linked shared library file.  For
     * executable and object modules the defined external symbols and the
     * undefined external symbols indicates the external references.
     */
    uint32_t extrefsymoff;	/* offset to referenced symbol table */
    uint32_t nextrefsyms;	/* number of referenced symbol table entries */

    /*
     * The sections that contain "symbol pointers" and "routine stubs" have
     * indexes and (implied counts based on the size of the section and fixed
     * size of the entry) into the "indirect symbol" table for each pointer
     * and stub.  For every section of these two types the index into the
     * indirect symbol table is stored in the section header in the field
     * reserved1.  An indirect symbol table entry is simply a 32bit index into
     * the symbol table to the symbol that the pointer or stub is referring to.
     * The indirect symbol table is ordered to match the entries in the section.
     */
    uint32_t indirectsymoff; /* file offset to the indirect symbol table */
    uint32_t nindirectsyms;  /* number of indirect symbol table entries */

    /*
     * To support relocating an individual module in a library file quickly the
     * external relocation entries for each module in the library need to be
     * accessed efficiently.  Since the relocation entries can't be accessed
     * through the section headers for a library file they are separated into
     * groups of local and external entries further grouped by module.  In this
     * case the presents of this load command who's extreloff, nextrel,
     * locreloff and nlocrel fields are non-zero indicates that the relocation
     * entries of non-merged sections are not referenced through the section
     * structures (and the reloff and nreloc fields in the section headers are
     * set to zero).
     *
     * Since the relocation entries are not accessed through the section headers
     * this requires the r_address field to be something other than a section
     * offset to identify the item to be relocated.  In this case r_address is
     * set to the offset from the vmaddr of the first LC_SEGMENT command.
     * For MH_SPLIT_SEGS images r_address is set to the the offset from the
     * vmaddr of the first read-write LC_SEGMENT command.
     *
     * The relocation entries are grouped by module and the module table
     * entries have indexes and counts into them for the group of external
     * relocation entries for that the module.
     *
     * For sections that are merged across modules there must not be any
     * remaining external relocation entries for them (for merged sections
     * remaining relocation entries must be local).
     */
    uint32_t extreloff;	/* offset to external relocation entries */
    uint32_t nextrel;	/* number of external relocation entries */

    /*
     * All the local relocation entries are grouped together (they are not
     * grouped by their module since they are only used if the object is moved
     * from it statically link edited address).
     */
    uint32_t locreloff;	/* offset to local relocation entries */
    uint32_t nlocrel;	/* number of local relocation entries */
};

/*
 * The uuid load command contains a single 128-bit unique random number that
 * identifies an object produced by the static link editor.
 */
struct uuid_command 
{
    uint32_t	cmd;		/*  LC_UUID */
    uint32_t	cmdsize;	/*  sizeof(struct uuid_command) */
    uint8_t	    uuid[16];	/*  the 128-bit uuid */
};

/*
 * A program that uses a dynamic linker contains a dylinker_command to identify
 * the name of the dynamic linker (LC_LOAD_DYLINKER).  And a dynamic linker
 * contains a dylinker_command to identify the dynamic linker (LC_ID_DYLINKER).
 * A file can have at most one of these.
 * This struct is also used for the LC_DYLD_ENVIRONMENT load command and
 * contains string for dyld to treat like environment variable.
 */
struct dylinker_command 
{
    uint32_t	    cmd;		/*  LC_ID_DYLINKER, LC_LOAD_DYLINKER or
                                    LC_DYLD_ENVIRONMENT */
    uint32_t	    cmdsize;	/*  includes pathname string */
    union lc_str    name;	    /*  dynamic linker's path name */
};

/*
 * The source_version_command is an optional load command containing
 * the version of the sources used to build the binary.
 */
struct source_version_command 
{
	uint32_t  cmd;	    /*  LC_SOURCE_VERSION */
	uint32_t  cmdsize;	/*  16 */
	uint64_t  version;	/*  A.B.C.D.E packed as a24.b10.c10.d10.e10 */
};

/*
 * The version_min_command contains the min OS version on which this
 * binary was built to run.
 */
struct version_min_command 
{
    uint32_t	cmd;		/*  LC_VERSION_MIN_MACOSX or
                                LC_VERSION_MIN_IPHONEOS  */
    uint32_t	cmdsize;	/*  sizeof(struct min_version_command) */
    uint32_t    version;    /*  X.Y.Z is encoded in nibbles xxxx.yy.zz */
    uint32_t	sdk;		/*  X.Y.Z is encoded in nibbles xxxx.yy.zz */
};

/*
 * The linkedit_data_command contains the offsets and sizes of a blob
 * of data in the __LINKEDIT segment.
 */
struct linkedit_data_command 
{
    uint32_t	cmd;		/*  LC_CODE_SIGNATURE, LC_SEGMENT_SPLIT_INFO,
                                LC_FUNCTION_STARTS, LC_DATA_IN_CODE,
                                or LC_DYLIB_CODE_SIGN_DRS */
    uint32_t	cmdsize;	/*  sizeof(struct linkedit_data_command) */
    uint32_t	dataoff;	/*  file offset of data in __LINKEDIT segment */
    uint32_t	datasize;	/*  file size of data in __LINKEDIT segment  */
};

/*
 * A dynamically linked shared library (filetype == MH_DYLIB in the mach header)
 * contains a dylib_command (cmd == LC_ID_DYLIB) to identify the library.
 * An object that uses a dynamically linked shared library also contains a
 * dylib_command (cmd == LC_LOAD_DYLIB, LC_LOAD_WEAK_DYLIB, or
 * LC_REEXPORT_DYLIB) for each library it uses.
 */
struct dylib_command 
{
    uint32_t	    cmd;        /*  LC_ID_DYLIB, LC_LOAD_{,WEAK_}DYLIB,
                                    LC_REEXPORT_DYLIB */
    uint32_t	    cmdsize;    /*  includes pathname string */
    struct dylib	dylib;      /*  the library identification */
};

struct section 
{
	char		sectname[16];	/* name of this section */
	char		segname[16];	/* segment this section goes in */
	uint32_t	addr;			/* memory address of this section */
	uint32_t	size;			/* size in bytes of this section */
	uint32_t	offset;			/* file offset of this section */
	uint32_t	align;			/* section alignment (power of 2) */
	uint32_t	reloff;			/* file offset of relocation entries */
	uint32_t	nreloc;			/* number of relocation entries */
	uint32_t	flags;			/* flags (section type and attributes)*/
	uint32_t	reserved1;		/* reserved */
	uint32_t	reserved2;		/* reserved */
};

/*
 * The dyld_info_command contains the file offsets and sizes of
 * the new compressed form of the information dyld needs to
 * load the image.  This information is used by dyld on Mac OS X
 * 10.6 and later.  All information pointed to by this command
 * is encoded using byte streams, so no endian swapping is needed
 * to interpret it.
 */
struct dyld_info_command {
    uint32_t   cmd;		/* LC_DYLD_INFO or LC_DYLD_INFO_ONLY */
    uint32_t   cmdsize;		/* sizeof(struct dyld_info_command) */

     /*
      * Dyld rebases an image whenever dyld loads it at an address different
      * from its preferred address.  The rebase information is a stream
      * of byte sized opcodes whose symbolic names start with REBASE_OPCODE_.
      * Conceptually the rebase information is a table of tuples:
      *    <seg-index, seg-offset, type>
      * The opcodes are a compressed way to encode the table by only
      * encoding when a column changes.  In addition simple patterns
      * like "every n'th offset for m times" can be encoded in a few
      * bytes.
      */
    uint32_t   rebase_off;	/* file offset to rebase info  */
    uint32_t   rebase_size;	/* size of rebase info   */

    /*
     * Dyld binds an image during the loading process, if the image
     * requires any pointers to be initialized to symbols in other images.
     * The bind information is a stream of byte sized
     * opcodes whose symbolic names start with BIND_OPCODE_.
     * Conceptually the bind information is a table of tuples:
     *    <seg-index, seg-offset, type, symbol-library-ordinal, symbol-name, addend>
     * The opcodes are a compressed way to encode the table by only
     * encoding when a column changes.  In addition simple patterns
     * like for runs of pointers initialzed to the same value can be
     * encoded in a few bytes.
     */
    uint32_t   bind_off;	/* file offset to binding info   */
    uint32_t   bind_size;	/* size of binding info  */

    /*
     * Some C++ programs require dyld to unique symbols so that all
     * images in the process use the same copy of some code/data.
     * This step is done after binding. The content of the weak_bind
     * info is an opcode stream like the bind_info.  But it is sorted
     * alphabetically by symbol name.  This enable dyld to walk
     * all images with weak binding information in order and look
     * for collisions.  If there are no collisions, dyld does
     * no updating.  That means that some fixups are also encoded
     * in the bind_info.  For instance, all calls to "operator new"
     * are first bound to libstdc++.dylib using the information
     * in bind_info.  Then if some image overrides operator new
     * that is detected when the weak_bind information is processed
     * and the call to operator new is then rebound.
     */
    uint32_t   weak_bind_off;	/* file offset to weak binding info   */
    uint32_t   weak_bind_size;  /* size of weak binding info  */

    /*
     * Some uses of external symbols do not need to be bound immediately.
     * Instead they can be lazily bound on first use.  The lazy_bind
     * are contains a stream of BIND opcodes to bind all lazy symbols.
     * Normal use is that dyld ignores the lazy_bind section when
     * loading an image.  Instead the static linker arranged for the
     * lazy pointer to initially point to a helper function which
     * pushes the offset into the lazy_bind area for the symbol
     * needing to be bound, then jumps to dyld which simply adds
     * the offset to lazy_bind_off to get the information on what
     * to bind.
     */
    uint32_t   lazy_bind_off;	/* file offset to lazy binding info */
    uint32_t   lazy_bind_size;  /* size of lazy binding infs */

    /*
     * The symbols exported by a dylib are encoded in a trie.  This
     * is a compact representation that factors out common prefixes.
     * It also reduces LINKEDIT pages in RAM because it encodes all
     * information (name, address, flags) in one small, contiguous range.
     * The export area is a stream of nodes.  The first node sequentially
     * is the start node for the trie.
     *
     * Nodes for a symbol start with a uleb128 that is the length of
     * the exported symbol information for the string so far.
     * If there is no exported symbol, the node starts with a zero byte.
     * If there is exported info, it follows the length.
     *
     * First is a uleb128 containing flags. Normally, it is followed by
     * a uleb128 encoded offset which is location of the content named
     * by the symbol from the mach_header for the image.  If the flags
     * is EXPORT_SYMBOL_FLAGS_REEXPORT, then following the flags is
     * a uleb128 encoded library ordinal, then a zero terminated
     * UTF8 string.  If the string is zero length, then the symbol
     * is re-export from the specified dylib with the same name.
     * If the flags is EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER, then following
     * the flags is two uleb128s: the stub offset and the resolver offset.
     * The stub is used by non-lazy pointers.  The resolver is used
     * by lazy pointers and must be called to get the actual address to use.
     *
     * After the optional exported symbol information is a byte of
     * how many edges (0-255) that this node has leaving it,
     * followed by each edge.
     * Each edge is a zero terminated UTF8 of the addition chars
     * in the symbol, followed by a uleb128 offset for the node that
     * edge points to.
     *
     */
    uint32_t   export_off;	/* file offset to lazy binding info */
    uint32_t   export_size;	/* size of lazy binding infs */
};

/*
 * The entry_point_command is a replacement for thread_command.
 * It is used for main executables to specify the location (file offset)
 * of main().  If -stack_size was used at link time, the stacksize
 * field will contain the stack size need for the main thread.
 */
struct entry_point_command 
{
    uint32_t  cmd;	        /* LC_MAIN only used in MH_EXECUTE filetypes */
    uint32_t  cmdsize;	    /* 24 */
    uint64_t  entryoff;	    /* file (__TEXT) offset of main() */
    uint64_t  stacksize;    /* if not zero, initial stack size */
};

/* Constants for the filetype field of the mach_header */
#define	MH_OBJECT	0x1		/* relocatable object file */
#define	MH_EXECUTE	0x2		/* demand paged executable file */
#define	MH_FVMLIB	0x3		/* fixed VM shared library file */
#define	MH_CORE		0x4		/* core file */
#define	MH_PRELOAD	0x5		/* preloaded executable file */
#define	MH_DYLIB	0x6		/* dynamically bound shared library file*/
#define	MH_DYLINKER	0x7		/* dynamic link editor */
#define	MH_BUNDLE	0x8		/* dynamically bound bundle file */

/* Constants for the flags field of the mach_header */
#define	MH_NOUNDEFS		0x1		/*	the object file has no undefined
									references, can be executed */
#define	MH_INCRLINK		0x2		/*	the object file is the output of an
									incremental link against a base file
									and can't be link edited again */
#define MH_DYLDLINK		0x4		/*	the object file is input for the
									dynamic linker and can't be statically
									link edited again */
#define MH_BINDATLOAD	0x8		/*	the object file's undefined
									references are bound by the dynamic
									linker when loaded. */
#define MH_PREBOUND		0x10	/*	the file has it's dynamic undefined
									references prebound. */

#define	MH_MAGIC	0xfeedface	/*  the mach magic number */
#define MH_CIGAM	0xcefaedfe
#define MH_MAGIC_64 0xfeedfacf	/*  the 64-bit mach magic number */
#define MH_CIGAM_64 0xcffaedfe 

#define	LC_SEGMENT			0x1		/*	segment of this file to be mapped */
#define	LC_SYMTAB			0x2		/*	link-edit stab symbol table info */
#define	LC_SYMSEG			0x3		/*	link-edit gdb symbol table info (obsolete) */
#define	LC_THREAD			0x4		/*	thread */
#define	LC_UNIXTHREAD		0x5		/*	unix thread (includes a stack) */
#define	LC_LOADFVMLIB		0x6		/*	load a specified fixed VM shared library */
#define	LC_IDFVMLIB			0x7		/*	fixed VM shared library identification */
#define	LC_IDENT			0x8		/*	object identification info (obsolete) */
#define LC_FVMFILE			0x9		/*	fixed VM file inclusion (internal use) */
#define LC_PREPAGE			0xa		/*	pre-page command (internal use) */
#define	LC_DYSYMTAB			0xb		/*	dynamic link-edit symbol table info */
#define	LC_LOAD_DYLIB		0xc		/*	load a dynamically linked shared library */
#define	LC_ID_DYLIB			0xd		/*	dynamically linked shared lib ident */
#define LC_LOAD_DYLINKER	0xe		/*	load a dynamic linker */
#define LC_ID_DYLINKER		0xf		/*	dynamic linker identification */
#define	LC_PREBOUND_DYLIB	0x10	/*	modules pre-bound for a dynamically
										linked shared library */
#define	LC_ROUTINES			0x11	/*	image routines */
#define	LC_SUB_FRAMEWORK	0x12	/*	sub framework */
#define	LC_SUB_UMBRELLA		0x13	/*	sub umbrella */
#define	LC_SUB_CLIENT		0x14	/*	sub client */
#define	LC_SUB_LIBRARY		0x15	/*	sub library */
#define	LC_TWOLEVEL_HINTS	0x16	/*	two-level namespace lookup hints */
#define	LC_PREBIND_CKSUM	0x17	/*	prebind checksum */


#define LC_REQ_DYLD 0x80000000

/*
 * load a dynamically linked shared library that is allowed to be missing
 * (all symbols are weak imported).
 */
#define	LC_LOAD_WEAK_DYLIB      (0x18 | LC_REQ_DYLD)
#define	LC_SEGMENT_64			0x19					/* 64-bit segment of this file to be mapped */
#define	LC_ROUTINES_64			0x1a					/* 64-bit image routines */
#define LC_UUID					0x1b					/* the uuid */
#define LC_RPATH				(0x1c | LC_REQ_DYLD)	/* runpath additions */
#define LC_CODE_SIGNATURE		0x1d					/* local of code signature */
#define LC_SEGMENT_SPLIT_INFO	0x1e					/* local of info to split segments */
#define LC_REEXPORT_DYLIB		(0x1f | LC_REQ_DYLD)	/* load and re-export dylib */
#define	LC_LAZY_LOAD_DYLIB		0x20					/* delay load of dylib until first use */
#define	LC_ENCRYPTION_INFO		0x21					/* encrypted segment information */
#define	LC_DYLD_INFO 			0x22					/* compressed dyld information */
#define	LC_DYLD_INFO_ONLY		(0x22 | LC_REQ_DYLD)	/* compressed dyld information only */
#define	LC_LOAD_UPWARD_DYLIB	(0x23 | LC_REQ_DYLD)	/* load upward dylib */
#define LC_VERSION_MIN_MACOSX	0x24					/* build for MacOSX min OS version */
#define LC_VERSION_MIN_IPHONEOS	0x25					/* build for iPhoneOS min OS version */
#define LC_FUNCTION_STARTS		0x26					/* compressed table of function start addresses */
#define LC_DYLD_ENVIRONMENT		0x27					/* string for dyld to treat
														like environment variable */
#define LC_MAIN					(0x28 | LC_REQ_DYLD)	/* replacement for LC_UNIXTHREAD */
#define LC_DATA_IN_CODE			0x29					/* table of non-instructions in __text */
#define LC_SOURCE_VERSION		0x2A					/* source version used to build binary */
#define LC_DYLIB_CODE_SIGN_DRS	0x2B					/* Code signing DRs copied from linked dylibs */