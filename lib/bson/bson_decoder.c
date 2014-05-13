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

#include "bson_decoder.h"

/// Escapes string so it can be put into json, buffer is automatically allocated.
char* bdf_json_escape(const char* str) {
	char* escaped;
	size_t i, j, specials = 0, preLen, postLen;

	// determine needed length
	preLen = postLen = strlen(str);
	for (i = 0; i < preLen; i++)
		if ((str[i] > 0x00 && str[i] < 0x20) || str[i] == '\\' || str[i] == '"')
			postLen += 6;

	// escape
	escaped = (char*)bdf_malloc(postLen + 1);
	for (i = 0, j = 0; i < preLen, j < postLen; i++, j++) {
		if ((str[i] > 0x00 && str[i] < 0x20) || str[i] == '\\' || str[i] == '"') {
			if (j + 5 >= postLen) // overflow, impossible
				break;
			sprintf(&escaped[j], "\\u00%.2u", str[i]);
			j += 5;
		} else
			escaped[j] = str[i];
	}
	escaped[postLen] = 0;
	return escaped;
}

/// Writes spaces indent to file.
bool bdf_write_indent(FILE* fp, int indent) {
	static char buff[256] = 
		"                                                                " \
	    "                                                                " \
	    "                                                                " \
	    "                                                               ";

	while(indent >= 256) {
		if(fwrite(buff, 1, 256, fp) != 256)
			return false;

		indent -= 256;
	}

	if(fwrite(buff, 1, indent, fp) != indent)
		return false;

	return true;
}

/// Allocates and opens the reading stream.
bds_decoder* bdf_decoder_create_from(const char* fileName) {
	return bdf_decoder_create_with_cache_from(fileName, NULL);
}

/// Allocates the stream for opened FILE (should be opened to read in binary mode).
bds_decoder* bdf_decoder_create_from_fp(FILE* f) {
	return bdf_decoder_create_with_cache_from_fp(f, NULL);
}

/// Allocates the stream for in-memory buffer.
bds_decoder* bdf_decoder_create_from_buff(bdt_byte* buff, size_t buff_len) {
	return bdf_decoder_create_with_cache_from_buff(buff, buff_len, NULL);
}

/// Allocates and opens the reading stream. Map will be used to store strings.
bds_decoder* bdf_decoder_create_with_cache_from(const char* fileName, bds_strings_map* map) {
	bds_decoder* decoder = bdf_decoder_create_with_cache_from_fp(fopen(fileName, "rb"), map);
	if (!decoder)
		return NULL;

	decoder->fp_created_by_us = true;
	return decoder;
}

/// Allocates the stream for opened FILE (should be opened to read in binary mode). 
/// Map will be used to store strings.
bds_decoder* bdf_decoder_create_with_cache_from_fp(FILE* f, bds_strings_map* map) {
	bds_decoder* decoder;

	if (!f)
		return NULL;

	decoder = (bds_decoder*)bdf_malloc(sizeof(bds_decoder));
	decoder->buff = (bdt_byte*)bdf_malloc(BD_READ_BUFF_LEN);

	// read first chunk
	if (fread(decoder->buff, 1, BD_READ_BUFF_LEN, f) <= 0) {
		bdf_free(decoder->buff);
		bdf_free(decoder);
		return NULL;
	}

	decoder->fp = f;
	decoder->pos = 0;
	decoder->buffPos = 0;
	decoder->buffSize = BD_READ_BUFF_LEN;
	decoder->map = map;
	decoder->fp_created_by_us = false;

	return decoder;
}

/// Allocates the stream for buffer in memory.
bds_decoder* bdf_decoder_create_with_cache_from_buff(bdt_byte* buff , size_t buff_len, bds_strings_map* map) {
	bds_decoder* decoder = (bds_decoder*)bdf_malloc(sizeof(bds_decoder));
	decoder->fp = NULL;
	decoder->pos = 0;
	decoder->buff = buff;
	decoder->buffPos = 0;
	decoder->buffSize = buff_len;
	decoder->map = map;
	decoder->fp_created_by_us = false;
	return decoder;
}

/// Closes and frees the reading stream.
void bdf_decoder_destroy(bds_decoder* decoder) {
	if (decoder->fp) {
		bdf_free(decoder->buff);
		if(decoder->fp_created_by_us)
			fclose(decoder->fp);
	}	
	bdf_free(decoder);
}


/// Reads one byte from stream.
bool bdf_decoder_read(bds_decoder* decoder, bdt_byte* b, int inc) {
	if (decoder->buffPos >= decoder->buffSize) {
		// overflow
		if (!decoder->fp)
			return false;

		// read next chunk from file
		if (fread(decoder->buff, 1, decoder->buffSize, decoder->fp) <= 0)
			return false;

		decoder->buffPos = 0;
	}

	*b = decoder->buff[decoder->buffPos];
	decoder->buffPos += inc;

	return true;
}

/// Reads one byte from the stream.
bool bdf_read_byte(bds_decoder* decoder, bdt_byte* b) {
	return bdf_decoder_read(decoder, b, 1);
}

/// Reads 0x00 from the stream.
bool bdf_read_zero(bds_decoder* decoder) {
	bdt_byte b;
	return bdf_decoder_read(decoder, &b, 1) && (b == 0);
}

/// Gets next byte from the stream but doesn't change the stream position.
bool bdf_read_next(bds_decoder* decoder, bdt_byte* b) {
	return bdf_decoder_read(decoder, b, 0);
}

/// Reads 32-bits.
bool bdf_read_32(bds_decoder* decoder, bdu_64* i) {
	return bdf_read_byte(decoder, &i->b[0]) && bdf_read_byte(decoder, &i->b[1])
		&& bdf_read_byte(decoder, &i->b[2]) && bdf_read_byte(decoder, &i->b[3]);
}

/// Reads 64-bits.
bool bdf_read_64(bds_decoder* decoder, bdu_64* i) {
	return bdf_read_byte(decoder, &i->b[7]) && bdf_read_byte(decoder, &i->b[6])
		&& bdf_read_byte(decoder, &i->b[5]) && bdf_read_byte(decoder, &i->b[4])
		&& bdf_read_byte(decoder, &i->b[3]) && bdf_read_byte(decoder, &i->b[2])
		&& bdf_read_byte(decoder, &i->b[1]) && bdf_read_byte(decoder, &i->b[0]);
}

/// Reads UTF-8 string with known size.
bool bdf_read_str(bds_decoder* decoder, char** p) {
	char* b;
	bdu_64 u;
	bdt_int32 i;

	// read size
	if (!bdf_read_32(decoder, &u))
		return false;

	// read data
	b = (char*)bdf_malloc(u.i32[0]);
	for (i = 0; i < u.i32[0]; i++)
		if (!bdf_read_byte(decoder, (bdt_byte*)&b[i])) {
			bdf_free(b);
			return false;
		}

	// cache
	if (decoder->map) {
		*p = bdf_strings_map_add(decoder->map, b);
		if (*p != b)
			bdf_free(b);
		return true;
	} else { // or not
		*p = b;
		return true;
	}	
}

/// Reads UTF-8 string with unknown size (until 0x00 byte).
bool bdf_read_cstr(bds_decoder* decoder, char** p) {
	bdt_int32 i;
	char buff[BD_MAX_CSTR_LEN];

	// read data	
	i = 0;
	while (1) {
		if (i >= BD_MAX_CSTR_LEN || !bdf_read_byte(decoder, (bdt_byte*)&buff[i]))
			return false;

		// until
		if (!buff[i])
			break;
		i++;
	}

	// cache
	if (decoder->map)
		*p = bdf_strings_map_add(decoder->map, buff);
	else // or not
		*p = bdf_strdup(buff);

	return true;
}

/// Reads element from stream, recursive.
bool bdf_read_elem(bds_decoder* decoder, bds_elem* e) {
	bdt_byte t;
	if (!bdf_read_byte(decoder, &t))
		return false;

	switch (t) {
	case 0x01:
		e->type = E_DOUBLE;
		return bdf_read_cstr(decoder, &e->name) && bdf_read_64(decoder, &e->data);
	case 0x02:
		e->type = E_STR;
		return bdf_read_cstr(decoder, &e->name) && bdf_read_str(decoder, &e->data.str);
	case 0x03:
		e->type = E_DOC;
		return bdf_read_cstr(decoder, &e->name) && bdf_read_doc(decoder, &e->data.node);
	case 0x04:
		e->type = E_ARRAY;
		return bdf_read_cstr(decoder, &e->name) && bdf_read_doc(decoder, &e->data.node);
	case 0x08:
		e->type = E_BOOL;
		return bdf_read_cstr(decoder, &e->name) && bdf_read_byte(decoder, &e->data.b[0]);
	case 0x10:
		e->type = E_INT32;
		return bdf_read_cstr(decoder, &e->name) && bdf_read_32(decoder, &e->data);
	case 0x09:
	case 0x11:
	case 0x12:
		e->type = E_INT64;
		return bdf_read_cstr(decoder, &e->name) && bdf_read_64(decoder, &e->data);
	case 0x0A:
		e->type = E_NUL;
		e->data.i64 = 0;
		return bdf_read_cstr(decoder, &e->name);
	default:
		return false;
	}
}

/// Dumps element with indent, recursive. skipName is used for arrays.
bool bdf_elem_dump(FILE* fp, bds_elem* elem, int indent, bool skipName) {
	if (elem) {
		bool ret = false;
		char* e_name = bdf_json_escape(elem->name);

		switch (elem->type) {
		case E_STR: {
			char* e_val = bdf_json_escape(elem->data.str);
			ret = bdf_write_indent(fp, indent)
				&& (fprintf(fp, "\"%s\": \"%s\" ", e_name, e_val) > 0);
			bdf_free(e_val);
			break;
		}
		case E_INT32:
			ret = bdf_write_indent(fp, indent)
				&& (fprintf(fp, "\"%s\": %d ", e_name, elem->data.i32[0]) > 0);
			break;
		case E_INT64:
			ret = bdf_write_indent(fp, indent)
				&& (fprintf(fp, "\"%s\": %lld ", e_name, elem->data.i64) > 0);
			break;
		case E_DOUBLE:
			ret = bdf_write_indent(fp, indent)
				&& (fprintf(fp, "\"%s\": %f ", e_name, elem->data.d) > 0);
			break;
		case E_BOOL:
			ret = bdf_write_indent(fp, indent)
				&& (fprintf(fp, "\"%s\": %s ", e_name, elem->data.b[0] ? "true" : "false") > 0);
			break;
		case E_NUL:
			ret = bdf_write_indent(fp, indent)
				&& (fprintf(fp, "\"%s\": null ", e_name) > 0);
			break;
		case E_ARRAY:
			ret = bdf_write_indent(fp, indent)
				&& (fprintf(fp, "\"%s\": [\n", e_name) > 0)
				&& bdf_node_dump(fp, elem->data.node, indent + 1, true)
				&& (fputc(']', fp) != EOF);
			break;
		case E_DOC:
			if (skipName)
				ret = bdf_write_indent(fp, indent)
				&& (fputc('{', fp) != EOF)
				&& (fputc('\n', fp) != EOF)
				&& bdf_node_dump(fp, elem->data.node, indent + 1, false)
				&& (fputc('}', fp) != EOF);
			else
				ret = bdf_write_indent(fp, indent)
				&& (fprintf(fp, "\"%s\": {\n", e_name) > 0)
				&& bdf_node_dump(fp, elem->data.node, indent + 1, false)
				&& (fputc('}', fp) != EOF);
			break;
		}

		bdf_free(e_name);
		return ret;
	}	
	return true;
}

/// Gets node type.
bde_type bdf_elem_type(bds_node* n) {
	return n ? n->elem.type : E_ERR;
}

/// Gets the node child with the specified name or NULL if no such child.
bds_node* bdf_elem_child(bds_node* n, const char* name) {
	if (bdf_elem_type(n) != E_DOC)
		return NULL;

	n = n->elem.data.node;
	while (n) {
		if (strcmp(n->elem.name, name) == 0)
			return n;
		n = n->next;
	}

	return NULL;
}

/// Gets node name.
bool bdf_elem_name(bds_node* n, char** str) {
	if (n) {
		*str = n->elem.name;
		return true;
	}
	return false;
}

/// Gets 32-bit integer value from node. Content type check included.
bool bdf_elem_int32(bds_node* n, bdt_int32* i) {
	if (n && n->elem.type == E_INT32) {
		*i = n->elem.data.i32[0];
		return true;
	}
	return false;
}

/// Gets 64-bit integer value from node. Content type check included.
bool bdf_elem_int64(bds_node* n, bdt_int64* i) {
	if (n) {
		switch (n->elem.type) {
		case E_INT32:
			*i = n->elem.data.i32[0];
			return true;
		case E_INT64:
			*i = n->elem.data.i64;
			return true;
		default:
			return false;
		}
	}
	return false;
}

/// Gets UTF-8 string value from node. Content type check included.
bool bdf_elem_str(bds_node* n, char** str) {
	if (n && n->elem.type == E_STR) {
		*str = n->elem.data.str;
		return true;
	}
	return false;
}

/// Gets double value from node. Content type check included.
bool bdf_elem_double(bds_node* n, double* d) {
	if (n && n->elem.type == E_DOUBLE) {
		*d = n->elem.data.d;
		return true;
	}
	return false;
}

/// Gets boolean value from node. Content type check included.
bool bdf_elem_bool(bds_node* n, bool* b) {
	if (n && n->elem.type == E_BOOL) {
		*b = (n->elem.data.b[0] != 0);
		return true;
	}
	return false;
}

/// Gets document or array value from node. Content type check included.
bool bdf_elem_node(bds_node* n, bds_node** n2) {
	if (n && (n->elem.type == E_DOC || n->elem.type == E_ARRAY)) {
		*n2 = n->elem.data.node;
		return true;
	}
	return false;
}

/// Allocates and initializes empty node.
bds_node* bdf_node_create() {
	bds_node* n = (bds_node*)bdf_malloc(sizeof(bds_node));
	n->elem.name = NULL;
	n->elem.data.i64 = 0;
	n->elem.type = E_ERR;
	n->next = NULL;
	return n;
}

/// Deallocates node, free_strings should be set to false if strings map is used.
void bdf_node_destroy(bds_node* n, bool free_strings) {
	bds_node* prev;
	while (n) {
		if (free_strings && n->elem.name)
			bdf_free(n->elem.name);
		if (n->elem.data.str) {
			switch (n->elem.type) {
			case E_DOC:
			case E_ARRAY:
				bdf_node_destroy(n->elem.data.node, free_strings);
				break;
			case E_STR:
				if (free_strings)
					bdf_free(n->elem.data.str);
				break;
			}
		}
		prev = n;
		n = n->next;
		bdf_free(prev);
	}
}

/// Reads list from stream.
bool bdf_read_list(bds_decoder* decoder, bds_node** n) {
	bdt_byte b;
	while(1) {
		*n = NULL;
		if (bdf_read_next(decoder, &b) && b == 0)
			return true;

		*n = bdf_node_create();
		if (!bdf_read_elem(decoder, &(*n)->elem))
			return false;

		n = &(*n)->next;
	}
}

/// Reads doc from stream.
bool bdf_read_doc(bds_decoder* decoder, bds_node** n) {
	bdu_64 i;
	return bdf_read_32(decoder, &i) && bdf_read_list(decoder, n) && bdf_read_zero(decoder);
}

/// Dumps node with indent, recursive. skipName is used for arrays.
bool bdf_node_dump(FILE* fp, bds_node* n, int indent, bool skipName) {
	while (n) {
		if (!bdf_elem_dump(fp, &n->elem, indent + 1, skipName))
			return false;

		if (n->next) {
			if (fputc(',', fp) == EOF || fputc('\n', fp) == EOF)
				return false;
		}
		n = n->next;
	}
	return true;
}

/// Dumps root node to file.
bool bdf_json_dump(FILE* fp, bds_node* n) {
	return (fputc('{', fp) != EOF) && bdf_node_dump(fp, n, 0, false) && (fputc('}', fp) != EOF);
}

/// Creates empty strings map/cache.
bds_strings_map* bdf_strings_map_create(int size) {
	bds_strings_map* m;
	int i;
	m = (bds_strings_map*)bdf_malloc(sizeof(bds_strings_map));
	m->size = size;
	m->map = (bds_strings_map_elem**)bdf_malloc(sizeof(bds_strings_map_elem*)* size);
	for (i = 0; i < size; i++)
		m->map[i] = NULL;
	return m;
}

/// Frees the content of map (all strings).
void bdf_strings_map_destroy(bds_strings_map* m) {
	int i;
	bds_strings_map_elem* next;
	for (i = 0; i < m->size; i++) {
		while (m->map[i]) {
			next = m->map[i]->next;
			bdf_free(m->map[i]->str);
			bdf_free(m->map[i]);
			m->map[i] = next;
		}
	}
	bdf_free(m->map);
	bdf_free(m);
}

/// Hashing function used in strings hash map/cache.
unsigned int bdf_string_hash(const char* str) {
	unsigned int hash = 5381;
	char c;
	while (c = *str++)
		hash = ((hash << 5) + hash) + c;
	return hash;
}

/// Adds the string to the hash map if its not already there.
/// Returns new/existing string address.
char* bdf_strings_map_add(bds_strings_map* m, char* str) {
	int index = bdf_string_hash(str) % m->size;
	if (!m->map[index]) {
		m->map[index] = (bds_strings_map_elem*)bdf_malloc(sizeof(bds_strings_map_elem));
		m->map[index]->next = NULL;
		m->map[index]->str = bdf_strdup(str);
		return m->map[index]->str;
	}
	else {
		bds_strings_map_elem* e = m->map[index];
		if (strcmp(e->str, str) == 0)
			return e->str;
		while (e->next) {
			e = e->next;
			if (strcmp(e->str, str) == 0)
				return e->str;
		}
		e->next = (bds_strings_map_elem*)bdf_malloc(sizeof(bds_strings_map_elem));
		e->next->next = NULL;
		e->next->str = bdf_strdup(str);
		return e->next->str;
	}
}
