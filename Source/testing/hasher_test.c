/**
 * @file hasher_test.h
 * 
 * @brief Tests for the hasher_t classes.
 * 
 */

/*
 * Copyright (C) 2005 Jan Hutter, Martin Willi
 * Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <string.h>

#include "hasher_test.h"


/* 
 * described in Header-File
 */
void test_md5_hasher(protected_tester_t *tester)
{
	/*
	 * Test vectors from RFC1321:
	 * MD5 ("") = d41d8cd98f00b204e9800998ecf8427e
	 * MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661
	 * MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72
	 * MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
	 * MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
	 * 
	 * currently testing "", "abc", "abcdefghijklmnopqrstuvwxyz"
	 */	
	hasher_t *hasher = hasher_create(HASH_MD5);
	u_int8_t hash_buffer[16];
	chunk_t empty, abc, abcd, hash_chunk;
	
	u_int8_t hash_empty[] = {
		0xd4,0x1d,0x8c,0xd9,
		0x8f,0x00,0xb2,0x04,
		0xe9,0x80,0x09,0x98,
		0xec,0xf8,0x42,0x7e
	};
		
	u_int8_t hash_abc[] = {
		0x90,0x01,0x50,0x98,
		0x3c,0xd2,0x4f,0xb0,
		0xd6,0x96,0x3f,0x7d,
		0x28,0xe1,0x7f,0x72
	};
		
	u_int8_t hash_abcd[] = {
		0xc3,0xfc,0xd3,0xd7,
		0x61,0x92,0xe4,0x00,
		0x7d,0xfb,0x49,0x6c,
		0xca,0x67,0xe1,0x3b
	};

	empty.ptr = "";
	empty.len = 0;
	abc.ptr = "abc";
	abc.len = 3;
	abcd.ptr = "abcdefghijklmnopqrstuvwxyz";
	abcd.len = strlen(abcd.ptr);
	
	tester->assert_true(tester, hasher->get_block_size(hasher) == 16, "block size");
	
	/* simple hashing, using empty */
	hasher->get_hash(hasher, empty, hash_buffer);
	tester->assert_false(tester, memcmp(hash_buffer, hash_empty, 16), "hash for empty");
	
	/* simple hashing, using "abc" */
	hasher->get_hash(hasher, abc, hash_buffer);
	tester->assert_false(tester, memcmp(hash_buffer, hash_abc, 16), "hash for abc");
	
	/* with allocation, using "abcdb..." */
	hasher->reset(hasher);
	hasher->allocate_hash(hasher, abcd, &hash_chunk);
	tester->assert_true(tester, hash_chunk.len == 16, "hash len");
	tester->assert_false(tester, memcmp(hash_chunk.ptr, hash_abcd, hash_chunk.len), "hash for abcd...");
	free(hash_chunk.ptr);
	hasher->destroy(hasher);
}

/* 
 * described in Header-File
 */
void test_sha1_hasher(protected_tester_t *tester)
{
	/*
	* 	Test Vectors (from FIPS PUB 180-1)
	* 	"abc"
	*	A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
	* 	"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
	*   84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
	* 	A million repetitions of "a"
	*   34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
	*/	
	hasher_t *hasher = hasher_create(HASH_SHA1);
	u_int8_t hash_buffer[20];
	chunk_t abc, abcdb, aaa, hash_chunk;
	u_int32_t i;
	u_int8_t hash_abc[] = {	
		0xA9,0x99,0x3E,0x36,
		0x47,0x06,0x81,0x6A,
		0xBA,0x3E,0x25,0x71,
		0x78,0x50,0xC2,0x6C,
		0x9C,0xD0,0xD8,0x9D
	};   	
	u_int8_t hash_abcdb[] = {	
		0x84,0x98,0x3E,0x44,
		0x1C,0x3B,0xD2,0x6E,
		0xBA,0xAE,0x4A,0xA1,
		0xF9,0x51,0x29,0xE5,
		0xE5,0x46,0x70,0xF1
	}; 	
	u_int8_t hash_aaa[] = {	
		0x34,0xAA,0x97,0x3C,
		0xD4,0xC4,0xDA,0xA4,
		0xF6,0x1E,0xEB,0x2B,
		0xDB,0xAD,0x27,0x31,
		0x65,0x34,0x01,0x6F
	};
	abc.ptr = "abc";
	abc.len = 3;
	abcdb.ptr = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	abcdb.len = strlen(abcdb.ptr);
	aaa.ptr = "aaaaaaaaaa"; /* 10 a's */
	aaa.len = 10;
	
	tester->assert_true(tester, hasher->get_block_size(hasher) == 20, "block size");
	
	/* simple hashing, using "abc" */
	hasher->get_hash(hasher, abc, hash_buffer);
	tester->assert_false(tester, memcmp(hash_buffer, hash_abc, 20), "hash for abc");
	
	/* with allocation, using "abcdb..." */
	hasher->reset(hasher);
	hasher->allocate_hash(hasher, abcdb, &hash_chunk);
	tester->assert_true(tester, hash_chunk.len == 20, "chunk len");
	tester->assert_false(tester, memcmp(hash_chunk.ptr, hash_abcdb, hash_chunk.len), "hash for abcdb...");
	free(hash_chunk.ptr);
	
	/* updating, using "aaaaaaa..." */
	hasher->reset(hasher);
	for(i=0; i<100000; i++)
	{
		if (i != 99999)
		{
			hasher->get_hash(hasher, aaa, NULL);
		}
		else
		{
			hasher->get_hash(hasher, aaa, hash_buffer);
		} 
	}
	tester->assert_false(tester, memcmp(hash_buffer, hash_aaa, 20), "hash for aaa...");
	

	hasher->destroy(hasher);
}
