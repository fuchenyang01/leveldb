// Copyright (c) 2012 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <cstdio>

#include "leveldb/dumpfile.h"
#include "leveldb/env.h"
#include "leveldb/status.h"
#include "leveldb/db.h"
#include "leveldb/options.h"

namespace leveldb {
namespace {

class StdoutPrinter : public WritableFile {
 public:
  Status Append(const Slice& data) override {
    fwrite(data.data(), 1, data.size(), stdout);
    return Status::OK();
  }
  Status Close() override { return Status::OK(); }
  Status Flush() override { return Status::OK(); }
  Status Sync() override { return Status::OK(); }
};

bool HandleDumpCommand(Env* env, char** files, int num) {
  StdoutPrinter printer;
  bool ok = true;
  for (int i = 0; i < num; i++) {
    Status s = DumpFile(env, files[i], &printer);
    if (!s.ok()) {
      std::fprintf(stderr, "%s\n", s.ToString().c_str());
      ok = false;
    }
  }
  return ok;
}

}  // namespace
}  // namespace leveldb

static void Usage() {
  std::fprintf(
      stderr,
      "Usage: leveldbutil command...\n"
      "   dump files...         -- dump contents of specified files\n");
}

int main(int argc, char** argv) {
  leveldb::Options options;
  options.create_if_missing = true;
  options.zlib_compression_level = 1;
  options.zstd_compression_level = 1;
  options.level_compression = {
      {0, leveldb::CompressionType::kNoCompression},
      {1, leveldb::CompressionType::kSnappyCompression},
      {2, leveldb::CompressionType::kSnappyCompression},
      {3, leveldb::CompressionType::kSnappyCompression},
      {4, leveldb::CompressionType::kZlibCompression},
      {5, leveldb::CompressionType::kZlibCompression},
      {6, leveldb::CompressionType::kZstdCompression},
  };

  leveldb::DB* dbzlib;
  leveldb::DB::Open(options, "testdb", &dbzlib);

  // 执行写入操作
  dbzlib->Put(leveldb::WriteOptions(), "key1", "value1");
  dbzlib->Put(leveldb::WriteOptions(), "key2", "value2");

  delete dbzlib;
  delete options.controller;
  return 0;
}
