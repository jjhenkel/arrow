// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "gandiva/function_registry_tree_path.h"

#include "gandiva/function_registry_common.h"

namespace gandiva {

std::vector<NativeFunction> GetTreePathFunctionRegistry() {
  static std::vector<NativeFunction> tree_path_fn_registry_ = {

        NativeFunction(
      "match_tree_path_1_111111", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111111",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111112", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111112",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111113", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111113",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111101", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111101",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111102", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111102",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111103", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111103",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111011", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111011",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111012", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111012",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111013", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111013",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111001", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111001",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111002", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111002",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_111003", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_111003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110111", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110111",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110112", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110112",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110113", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110113",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110101", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110101",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110102", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110102",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110103", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110103",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110011", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110011",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110012", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110012",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110013", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110013",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110001", {},
      DataTypeVector{int64(), binary(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110001",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110002", {},
      DataTypeVector{int64(), binary(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110002",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_110003", {},
      DataTypeVector{int64(), binary(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_110003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011110", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011110",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011111", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011111",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011112", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011112",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011113", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011113",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011100", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011100",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011101", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011101",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011102", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011102",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011103", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011103",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011010", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011010",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011011", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011011",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011012", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011012",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011013", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011013",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011000", {},
      DataTypeVector{int64(), binary(), uint16(), int64()},
      binary(), kResultNullIfNull, "match_tree_path_1_011000",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011001", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011001",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011002", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011002",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_011003", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_011003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010110", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010110",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010111", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010111",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010112", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010112",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010113", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010113",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010100", {},
      DataTypeVector{int64(), binary(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010100",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010101", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010101",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010102", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010102",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010103", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010103",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010010", {},
      DataTypeVector{int64(), binary(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010010",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010011", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010011",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010012", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010012",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010013", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010013",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010000", {},
      DataTypeVector{int64(), binary(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010000",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010001", {},
      DataTypeVector{int64(), binary(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010001",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010002", {},
      DataTypeVector{int64(), binary(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010002",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_1_010003", {},
      DataTypeVector{int64(), binary(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_1_010003",
      NativeFunction::kNeedsContext),

    NativeFunction(
      "match_tree_path_3_010100_011003_010003", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), int64(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_3_010100_011003_010003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_3_010100_011003_110003", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), int64(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_3_010100_011003_110003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_3_010100_011103_010003", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), int64(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_3_010100_011103_010003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_3_010100_011103_110003", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), int64(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_3_010100_011103_110003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_3_010100_010003_010003", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_3_010100_010003_010003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_3_010100_010003_010000", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_3_010100_010003_010000",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_2_010100_011003", {},
      DataTypeVector{int64(), binary(), uint16(), uint16(), uint16(), int64(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_2_010100_011003",
      NativeFunction::kNeedsContext),
    NativeFunction(
      "match_tree_path_2_011000_010003", {},
      DataTypeVector{int64(), binary(), uint16(), int64(), uint16(), uint16()},
      binary(), kResultNullIfNull, "match_tree_path_2_011000_010003",
      NativeFunction::kNeedsContext),
  };

  return tree_path_fn_registry_;
}

}  // namespace gandiva
