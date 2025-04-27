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
  options.controller =
      new leveldb::SimpleController(100, 1024 * 1024);  // 每100ms或1MB刷盘

  leveldb::DB* db;
  leveldb::Status s = leveldb::DB::Open(options, "testdb", &db);

  // 执行写入操作
  db->Put(leveldb::WriteOptions(), "key1", "value1");
  db->Put(leveldb::WriteOptions(), "key2", "value2");

  delete db;
  delete options.controller;
  return 0;
}
