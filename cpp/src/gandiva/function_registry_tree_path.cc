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
          "match_tree_path_1_010000", {},
          DataTypeVector{int64(), binary(), uint16()},
          binary(), kResultNullIfNull, "match_1_010000",
          NativeFunction::kNeedsContext),
      NativeFunction(
          "match_tree_path_1_110003", {},
          DataTypeVector{int64(), binary(), uint16(), uint16()},
          binary(), kResultNullIfNull, "match_1_110003",
          NativeFunction::kNeedsContext),
      NativeFunction(
          "match_tree_path_1_010003", {},
          DataTypeVector{int64(), binary(), uint16(), uint16()},
          binary(), kResultNullIfNull, "match_1_010003",
          NativeFunction::kNeedsContext),

  };

  return tree_path_fn_registry_;
}

}  // namespace gandiva
