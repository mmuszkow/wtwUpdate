/// BSON parser/decoder
/// Author: Maciek Muszkowski
/// 
/// Primitive types:
/// byte    1 byte (8-bits)
/// int32   4 bytes(32 - bit signed integer, two's complement)
/// int64   8 bytes(64 - bit signed integer, two's complement)
/// double  8 bytes(64 - bit IEEE 754 floating point)
/// string  UTF-8 ending with 0x00
/// boolean byte, 0 = false, 1 = true
/// 
/// Supported grammar:
/// document ::= int32 e_list "\x00"
/// list     ::= element e_list | "\x00"
/// element  ::= "\x01" e_name double
///            | "\x02" e_name string
///            | "\x03" e_name document
///            | "\x04" e_name document
///            | "\x08" e_name boolean
///            | "\x09" e_name int64
///            | "\x0A" e_name
///            | "\x10" e_name int32
///            | "\x11" e_name int64
///            | "\x12" e_name int64
/// e_name 	 ::= cstring
/// cstring  ::= (byte*) "\x00"
/// string 	 ::= int32(byte*) "\x00"
/// For more details: http://bsonspec.org/spec.html
///

#ifndef __BSON_DECODER_H__
#define __BSON_DECODER_H__

#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h> // needs C99

#define bool	_Bool
#define false	0
#define true	1

#include <string.h>

// naming convention
// bd = BSON decoder
// bdf = function
// bdt = type
// bds = struct
// bde = enum
// bdu = union



#define bdf_malloc(s)   malloc(s)
#define bdf_strdup(str) strdup(str)
#define bdf_free(p)     free(p)


typedef unsigned char bdt_byte;
typedef int           bdt_int32;
typedef long long     bdt_int64;
typedef double        bdt_double;

struct bds_node;
/// Used for reading/keeping data.
typedef union bdu_64 {
	bdt_byte   b[8];
	bdt_int32  i32[2];
	bdt_int64  i64;
	bdt_double d;
	char*	   str;
	struct bds_node* node;
} bdu_64;

/// cstring is used for element names. This is it's maximal length.
#define BD_MAX_CSTR_LEN 1024

/// Element types.
typedef enum bde_type {
	E_ERR,
	E_NUL,
	E_BOOL, // long name to avoid conflict with Windows.h
	E_STR,
	E_INT32,
	E_INT64,
	E_DOUBLE,
	E_DOC,
	E_ARRAY
} bde_type;

/// Basic element. Keeps values either by value or by pointer.
typedef struct bds_elem {
	char*    name;
	bde_type type;
	bdu_64   data;
} bds_elem;

/// Complex element - document or array.
typedef struct bds_node {
	bds_elem         elem;
	struct bds_node* next;
} bds_node;

/// Hasmap element.
typedef struct bds_strings_map_elem {
	char*                        str;
	struct bds_strings_map_elem* next;
} bds_strings_map_elem;

/// Hashmap containing all strings. For saving the memory.
typedef struct bds_strings_map {
	bds_strings_map_elem** map;
	int                    size;
} bds_strings_map;

/// Read from file buffer size
#define BD_READ_BUFF_LEN  4096

/// Contains stream/buffer.
typedef struct bds_decoder {
	bdt_byte* buff;
	FILE*     fp;
	long      pos;
	size_t	  buffPos;
	size_t	  buffSize;
	bool      fp_created_by_us;
	bds_strings_map* map;
} bds_decoder;

#ifdef __cplusplus
extern "C" {
#endif
	// Parser creating
	bool bdf_stream_read(bds_decoder* decoder, bdt_byte* b, int inc);
	bds_decoder* bdf_decoder_create_from(const char* fileName);
	bds_decoder* bdf_decoder_create_from_fp(FILE* f);
	bds_decoder* bdf_decoder_create_from_buff(bdt_byte* buff, size_t buff_len);
	bds_decoder* bdf_decoder_create_with_cache_from(const char* fileName, bds_strings_map* map);
	bds_decoder* bdf_decoder_create_with_cache_from_fp(FILE* f, bds_strings_map* map);
	bds_decoder* bdf_decoder_create_with_cache_from_buff(bdt_byte* buff, size_t buff_len, bds_strings_map* map);
	void bdf_decoder_destroy(bds_decoder* decoder);

	// Functions for reading objects from stream
	bool bdf_read_byte(bds_decoder* decoder, bdt_byte* b);
	bool bdf_read_next(bds_decoder* decoder, bdt_byte* b);
	bool bdf_read_zero(bds_decoder* decoder);
	bool bdf_read_32(bds_decoder* decoder, bdu_64* i);
	bool bdf_read_64(bds_decoder* decoder, bdu_64* i);
	bool bdf_read_str(bds_decoder* decoder, char** p);
	bool bdf_read_cstr(bds_decoder* decoder, char** p);
	bool bdf_read_list(bds_decoder* decoder, bds_node** n);
	bool bdf_read_doc(bds_decoder* decoder, bds_node** n);
	bool bdf_read_elem(bds_decoder* decoder, bds_elem* e);

	// Functions for getting node element content
	bde_type bdf_elem_type(bds_node* n);
	bds_node* bdf_elem_child(bds_node* n, const char* name);
	bool bdf_elem_name(bds_node* n, char** str);
	bool bdf_elem_int32(bds_node* n, bdt_int32* i);
	bool bdf_elem_int64(bds_node* n, bdt_int64* i);
	bool bdf_elem_str(bds_node* n, char** str);
	bool bdf_elem_double(bds_node* n, double* d);
	bool bdf_elem_bool(bds_node* n, bool* b);
	bool bdf_elem_node(bds_node* n, bds_node** n2);

	// Functions for creating/dumping objects
	bool bdf_node_dump(FILE* fp, bds_node* n, int indent, bool skipName);
	bool bdf_elem_dump(FILE* fp, bds_elem* elem, int indent, bool skipName);
	bool bdf_json_dump(FILE* fp, bds_node* n);
	bds_node* bdf_node_create();
	void bdf_node_destroy(bds_node* n, bool free_strings);

	// Hash map with strings
	bds_strings_map* bdf_strings_map_create(int size);
	void bdf_strings_map_destroy(bds_strings_map* m);
	unsigned int bdf_string_hash(const char* str);
	char* bdf_strings_map_add(bds_strings_map* m, char* str);

	// Helpers
	char* bdf_json_escape(const char* str);
	bool bdf_write_indent(FILE* stream, int indent);

#ifdef __cplusplus
}
#endif

#endif
