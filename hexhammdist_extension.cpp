#define DUCKDB_EXTENSION_MAIN
#include <iostream>
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/parser/parser_extension.hpp"
#include "duckdb/parser/parsed_data/create_table_function_info.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/parser/parsed_data/create_scalar_function_info.hpp"
#include "duckdb/parser/parsed_data/create_type_info.hpp"
#include "duckdb/catalog/catalog_entry/type_catalog_entry.hpp"
using namespace duckdb;
/*
 * hexhammdist - hamming distance between same-length hex strings in SQLite
 * Copyright (c) 2016, Daniel Roethlisberger <daniel@roe.ch>
 * Copyright (c) 2022, Salvador Pardiñas <darkfm@vera.com.uy> (Port to DuckDB)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

static const unsigned char unhextab[256] = {
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
					    0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define BST2(n)      n,       n+1,       n+1,       n+2
#define BST4(n) BST2(n), BST2(n+1), BST2(n+1), BST2(n+2)
#define BST6(n) BST4(n), BST4(n+1), BST4(n+1), BST4(n+2)
static const unsigned char bitssettab[256] = {
					      BST6(0), BST6(1), BST6(1), BST6(2)
};
#undef BST2
#undef BST4
#undef BST6

int64_t hammdist(string_t a, string_t b) {
  if(a.GetSize() != b.GetSize()) throw OutOfRangeException("Strings must have same size!");
  int64_t res = 0;
  auto aString = a.GetString();
  auto bString = b.GetString();
  auto len = a.GetSize();
  for(size_t i = 0; i < len; i++) {
    res += bitssettab[unhextab[aString[i]] ^ unhextab[bString[i]]];
  }
  return res;
}

void hammdist_vectorized(DataChunk &args, ExpressionState &state, Vector &result) {
  result.SetVectorType(VectorType::FLAT_VECTOR);

  auto result_data = FlatVector::GetData<int64_t>(result);

  auto ldata = FlatVector::GetData<string_t>(args.data[0]);
  auto rdata = FlatVector::GetData<string_t>(args.data[1]);

  ValidityMask val = FlatVector::Validity(args.data[0]);
  val.Combine(FlatVector::Validity(args.data[1]), args.size());
  FlatVector::SetValidity(result, val);
  idx_t maxLen = 0;
  size_t *lens = (size_t*)malloc(args.size() * sizeof(size_t));

  for (idx_t i = 0; i < args.size(); i++) {
    lens[i] = 0;
    if(!val.RowIsValid(i)) continue;
    auto &aString = ldata[i];
    auto &bString = rdata[i];
    if(aString.GetSize() != bString.GetSize()) {
      free(lens);
      throw OutOfRangeException("Strings must have same size!");
    }
    lens[i] = aString.GetSize();
    maxLen = std::max(maxLen, aString.GetSize());
  }
  
  uint8_t *tmp = (uint8_t*)calloc(args.size(), maxLen);

  for (idx_t i = 0; i < args.size(); i++) {
    auto aString = ldata[i].GetDataUnsafe();
    auto bString = rdata[i].GetDataUnsafe();
    auto len = lens[i];

    for(idx_t c = 0; c < len; c++) {
      tmp[i * maxLen + c] = bitssettab[unhextab[aString[c]] ^ unhextab[bString[c]]];
    }
  }
  size_t sz = args.size();

  memset(result_data, 0, sizeof(int64_t) * sz);

  for (size_t i = 0; i < sz; i++) {
    for(size_t c = 0; c < maxLen; c++)
      result_data[i] += tmp[i * maxLen + c];
  }

  free(tmp);
  free(lens);
}

//===--------------------------------------------------------------------===//
// Extension load + setup
//===--------------------------------------------------------------------===//
extern "C" {
DUCKDB_EXTENSION_API void hexhammdist_extension_init(duckdb::DatabaseInstance &db) {
	Connection con(db);
	con.BeginTransaction();
	con.CreateScalarFunction<int64_t, string_t, string_t>("hexhammdist", {LogicalType(LogicalTypeId::VARCHAR),LogicalType(LogicalTypeId::VARCHAR)},
	                                             LogicalType(LogicalTypeId::BIGINT), &hammdist);
	con.CreateVectorizedFunction<int64_t, string_t, string_t>("hexhammdist_vec", &hammdist_vectorized);
	con.Commit();
}

DUCKDB_EXTENSION_API const char *hexhammdist_extension_version() {
	return DuckDB::LibraryVersion();
}
}
