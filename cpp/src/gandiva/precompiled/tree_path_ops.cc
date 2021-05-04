
// String functions
#include "arrow/util/value_parsing.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./types.h"


const gdv_int16 EXTRA_SIZE = 8;
const gdv_int16 GID_SIZE = 8;
const gdv_int16 POS_SIZE = 8;
const gdv_int16 GID_AND_POS_SIZE = 16;
const gdv_int16 GID_AND_DEF_SIZE = 16;
const gdv_int16 PATH_SEGMENT_SIZE = 40;

const gdv_int16 DEF_ID_OFFSET = 8;
const gdv_int16 NAME_ID_OFFSET = 16;
const gdv_int16 POS_OFFSET = 24;
const gdv_int16 LABEL_OFFSET = 32;
const gdv_int16 FIELD_OFFSET = 34;
const gdv_int16 INDEX_OFFSET = 36;

const gdv_uint16 BITS_IN_BYTE = 8;

/* ========================================================================= */
/* ========================================================================= */

inline __attribute__((always_inline))
bool cmp_uint16_bytes_at_index(uint16_t target, const char* bytes, int32_t idx) {
  uint16_t temp;
  memcpy(&temp, &bytes[idx], 2);
  return temp == target;
}

inline __attribute__((always_inline))
bool cmp_int64_bytes_at_index(int64_t target, const char* bytes, int32_t idx) {
  int64_t temp;
  memcpy(&temp, &bytes[idx], 8);
  return temp == target;
}

/* ========================================================================= */
/* ========================================================================= */

template<
  bool NEGATE, 
  bool MATCH_LABEL, 
  bool MATCH_NAME, 
  bool MATCH_FIELD, 
  bool MATCH_INDEX,
  uint16_t MATCH_STEPS_OP
>
inline __attribute__((always_inline))
int32_t match_inner(
  int32_t anchor_idx,
  int32_t start_idx,
  const char* path,
  const int32_t path_len,
  const uint16_t label,
  const int64_t name,
  const uint16_t field,
  const uint16_t index,
  const uint16_t steps
) {
  if constexpr (MATCH_STEPS_OP == 3) {
    start_idx += (steps - 1) * PATH_SEGMENT_SIZE;
  } else if constexpr (MATCH_STEPS_OP == 2) {
    start_idx += steps * PATH_SEGMENT_SIZE;
  }

  for (int32_t p_idx = start_idx; p_idx < path_len; p_idx += PATH_SEGMENT_SIZE) {
    if constexpr (MATCH_STEPS_OP == 1) {  // <
      if (((p_idx  / PATH_SEGMENT_SIZE) - (anchor_idx / PATH_SEGMENT_SIZE)) >= steps) {
        continue;
      }
    } else if constexpr (MATCH_STEPS_OP == 2) {  // >
      if (((p_idx  / PATH_SEGMENT_SIZE) - (anchor_idx / PATH_SEGMENT_SIZE)) <= steps) {
        continue;
      }
    } else if constexpr (MATCH_STEPS_OP == 3) {  // ==
      if (((p_idx  / PATH_SEGMENT_SIZE) - (anchor_idx / PATH_SEGMENT_SIZE)) != steps) {
        continue;
      }
    }

    if constexpr (!NEGATE && MATCH_NAME) {
      if (!cmp_int64_bytes_at_index(name, path, p_idx + NAME_ID_OFFSET)) {
        continue;
      }
    } else if constexpr (NEGATE && MATCH_NAME) {
      if (cmp_int64_bytes_at_index(name, path, p_idx + NAME_ID_OFFSET)) {
        continue;
      }
    }

    if constexpr (!NEGATE && MATCH_LABEL) {
      if (!cmp_uint16_bytes_at_index(label, path, p_idx + LABEL_OFFSET)) {
        continue;
      }
    } else if constexpr (NEGATE && MATCH_LABEL) {
      if (cmp_uint16_bytes_at_index(label, path, p_idx + LABEL_OFFSET)) {
        continue;
      }
    }

    if constexpr (!NEGATE && MATCH_FIELD) {
      if (!cmp_uint16_bytes_at_index(field, path, p_idx + FIELD_OFFSET)) {
        continue;
      }
    } else if constexpr (NEGATE && MATCH_FIELD) {
      if (cmp_uint16_bytes_at_index(field, path, p_idx + FIELD_OFFSET)) {
        continue;
      }
    }

    if constexpr (!NEGATE && MATCH_INDEX) {
      if (!cmp_uint16_bytes_at_index(index, path, p_idx + INDEX_OFFSET)) {
        continue;
      }
    } else if constexpr (NEGATE && MATCH_INDEX) {
      if (cmp_uint16_bytes_at_index(index, path, p_idx + INDEX_OFFSET)) {
        continue;
      }
    }

    return p_idx;
  }

  return -1;
}

/* ========================================================================= */
/* ========================================================================= */

template<
  bool NEGATE_1, 
  bool MATCH_LABEL_1, 
  bool MATCH_NAME_1, 
  bool MATCH_FIELD_1, 
  bool MATCH_INDEX_1,
  uint16_t MATCH_STEPS_OP_1
>
inline __attribute__((always_inline))
char* match_1(
  int64_t context,
  const int64_t extra,
  const char* path,
  const int32_t path_len,
  const uint16_t label1,
  const int64_t name1,
  const uint16_t field1,
  const uint16_t index1,
  const uint16_t steps1,
  int32_t* out_len
) {
  char* ret = nullptr;
  
  int32_t idx1 = match_inner<
    NEGATE_1, MATCH_LABEL_1, MATCH_NAME_1, MATCH_FIELD_1, MATCH_INDEX_1, MATCH_STEPS_OP_1
  >(
    0, 0, path, path_len, label1, name1, field1, index1, steps1
  );

  if (idx1 == -1) {
    goto no_match;
  }

  *out_len = EXTRA_SIZE + PATH_SEGMENT_SIZE;
  ret = reinterpret_cast<char*>(gdv_fn_context_arena_malloc(context, *out_len));
  if (ret == nullptr) {
    gdv_fn_context_set_error_msg(context,
                                  "Could not allocate memory for output bytes.");
    *out_len = 0;
    return nullptr;
  }

  memcpy(ret, static_cast<const void*>(&extra), EXTRA_SIZE);
  memcpy(&ret[EXTRA_SIZE], &path[idx1], PATH_SEGMENT_SIZE);
  return ret;

no_match:
  *out_len = 0;
  return ret;
}

template<
  bool NEGATE_1, 
  bool MATCH_LABEL_1, 
  bool MATCH_NAME_1, 
  bool MATCH_FIELD_1, 
  bool MATCH_INDEX_1,
  uint16_t MATCH_STEPS_OP_1,
  bool NEGATE_2, 
  bool MATCH_LABEL_2, 
  bool MATCH_NAME_2, 
  bool MATCH_FIELD_2, 
  bool MATCH_INDEX_2,
  uint16_t MATCH_STEPS_OP_2
>
inline __attribute__((always_inline))
char* match_2(
    int64_t context,
    const int64_t extra,
    const char* path,
    const int32_t path_len,
    const uint16_t label1,
    const int64_t name1,
    const uint16_t field1,
    const uint16_t index1,
    const uint16_t steps1,
    const uint16_t label2,
    const int64_t name2,
    const uint16_t field2,
    const uint16_t index2,
    const uint16_t steps2,
    int32_t* out_len
) {

  // (0) Setup
  char* ret = nullptr;

  int32_t idx1 = 0, idx2 = 0;

  int32_t pos = 0;
  int32_t last_pos = 0;
    
try_match_one:
  // (1a) Bounds check
  if (pos >= path_len) {
    goto no_match;
  }

  // (1b) Scan (starting at pos) and see if we have a match
  idx1 = match_inner<
    NEGATE_1, MATCH_LABEL_1, MATCH_NAME_1, MATCH_FIELD_1, MATCH_INDEX_1, MATCH_STEPS_OP_1
  >(0, pos, path, path_len, label1, name1, field1, index1, steps1);

  // (1c) No match, no chance we have a "complete" match. Quit now.
  if (idx1 == -1) {
    goto no_match;
  }
  
  // (1d) Match, so advance and search for part 2
  pos = idx1 + PATH_SEGMENT_SIZE;
  last_pos = idx1 + PATH_SEGMENT_SIZE;

try_match_two:
  // (2a) Bounds check
  if (pos >= path_len) {
    goto no_match;
  }

  // (2b) Scan (starting at pos) and see if we match
  idx2 = match_inner<
    NEGATE_2, MATCH_LABEL_2, MATCH_NAME_2, MATCH_FIELD_2, MATCH_INDEX_2, MATCH_STEPS_OP_2
  >(idx1, pos, path, path_len, label2, name2, field2, index2, steps2);

  // (2c) No match, go back and re-try match (1)
  if (idx2 == -1) {
    idx1 = -1;      // No longer have a 1st match
    pos = last_pos; // Reset pos to right after last match
    goto try_match_one;
  }

  // (2d) Matched the whole thing!
  *out_len = EXTRA_SIZE + PATH_SEGMENT_SIZE * 2;
  ret = reinterpret_cast<char*>(gdv_fn_context_arena_malloc(context, *out_len));
  if (ret == nullptr) {
    gdv_fn_context_set_error_msg(context,
                                  "Could not allocate memory for output bytes.");
    *out_len = 0;
    return nullptr;
  }

  // (2e) Copy over matches!
  memcpy(ret, static_cast<const void*>(&extra), EXTRA_SIZE);
  memcpy(&ret[EXTRA_SIZE], &path[idx1], PATH_SEGMENT_SIZE);
  memcpy(&ret[EXTRA_SIZE+PATH_SEGMENT_SIZE], &path[idx2], PATH_SEGMENT_SIZE);
  return ret;

no_match:
  *out_len = 0;
  return ret;
}

template<
  bool NEGATE_1, 
  bool MATCH_LABEL_1, 
  bool MATCH_NAME_1, 
  bool MATCH_FIELD_1, 
  bool MATCH_INDEX_1,
  uint16_t MATCH_STEPS_OP_1,
  bool NEGATE_2, 
  bool MATCH_LABEL_2, 
  bool MATCH_NAME_2, 
  bool MATCH_FIELD_2, 
  bool MATCH_INDEX_2,
  uint16_t MATCH_STEPS_OP_2,
  bool NEGATE_3, 
  bool MATCH_LABEL_3, 
  bool MATCH_NAME_3, 
  bool MATCH_FIELD_3, 
  bool MATCH_INDEX_3,
  uint16_t MATCH_STEPS_OP_3
>
inline __attribute__((always_inline))
char* match_3(
  int64_t context,
  const int64_t extra,
  const char* path,
  const int32_t path_len,
  const uint16_t label1,
  const int64_t name1,
  const uint16_t field1,
  const uint16_t index1,
  const uint16_t steps1,
  const uint16_t label2,
  const int64_t name2,
  const uint16_t field2,
  const uint16_t index2,
  const uint16_t steps2,
  const uint16_t label3,
  const int64_t name3,
  const uint16_t field3,
  const uint16_t index3,
  const uint16_t steps3,
  int32_t* out_len
) {

  // (0) Setup
  char* ret = nullptr;

  int32_t idx1 = 0, idx2 = 0, idx3 = 0;

  int32_t pos = 0;
  int32_t last_pos_1 = 0;
  int32_t last_pos_2 = 0;
    
try_match_one:
  // (1a) Bounds check
  if (pos >= path_len) {
    goto no_match;
  }

  // (1b) Scan (starting at pos) and see if we have a match
  idx1 = match_inner<
    NEGATE_1, MATCH_LABEL_1, MATCH_NAME_1, MATCH_FIELD_1, MATCH_INDEX_1, MATCH_STEPS_OP_1
  >(0, pos, path, path_len, label1, name1, field1, index1, steps1);

  // (1c) No match, no chance we have a "complete" match. Quit now.
  if (idx1 == -1) {
    goto no_match;
  }
  
  // (1d) Match, so advance and search for part 2
  pos = idx1 + PATH_SEGMENT_SIZE;
  last_pos_1 = idx1 + PATH_SEGMENT_SIZE;

try_match_two:
  // (2a) Bounds check
  if (pos >= path_len) {
    goto no_match;
  }

  // (2b) Scan (starting at pos) and see if we match
  idx2 = match_inner<
    NEGATE_2, MATCH_LABEL_2, MATCH_NAME_2, MATCH_FIELD_2, MATCH_INDEX_2, MATCH_STEPS_OP_2
  >(idx1, pos, path, path_len, label2, name2, field2, index2, steps2);

  // (2c) No match, go back and re-try match (1)
  if (idx2 == -1) {
    idx1 = -1;      // No longer have a 1st match
    pos = last_pos_1; // Reset pos to right after last match
    goto try_match_one;
  }
  
  // (2d) Match, so advance and search for part 3
  pos = idx2 + PATH_SEGMENT_SIZE;
  last_pos_2 = idx2 + PATH_SEGMENT_SIZE;

try_match_three:
  // (3a) Bounds check
  if (pos >= path_len) {
    goto no_match;
  }

  // (3b) Scan (starting at pos) and see if we match
  idx3 = match_inner<
    NEGATE_3, MATCH_LABEL_3, MATCH_NAME_3, MATCH_FIELD_3, MATCH_INDEX_3, MATCH_STEPS_OP_3
  >(idx2, pos, path, path_len, label3, name3, field3, index3, steps3);

  // (3c) No match, go back and re-try match (1)
  if (idx3 == -1) {
    idx2 = -1;      // No longer have a 2nd match
    pos = last_pos_2; // Reset pos to right after last match
    goto try_match_two;
  }

  // (3d) Matched the whole thing!
  *out_len = EXTRA_SIZE + PATH_SEGMENT_SIZE * 3;
  ret = reinterpret_cast<char*>(gdv_fn_context_arena_malloc(context, *out_len));
  if (ret == nullptr) {
    gdv_fn_context_set_error_msg(context,
                                  "Could not allocate memory for output bytes.");
    *out_len = 0;
    return nullptr;
  }

  // (3e) Copy over matches!
  memcpy(ret, static_cast<const void*>(&extra), EXTRA_SIZE);
  memcpy(&ret[EXTRA_SIZE], &path[idx1], PATH_SEGMENT_SIZE);
  memcpy(&ret[EXTRA_SIZE+PATH_SEGMENT_SIZE], &path[idx2], PATH_SEGMENT_SIZE);
  memcpy(&ret[EXTRA_SIZE+PATH_SEGMENT_SIZE*2], &path[idx3], PATH_SEGMENT_SIZE);
  return ret;

no_match:
  *out_len = 0;
  return nullptr;
}

/* ========================================================================= */
/* ========================================================================= */

extern "C" {
const char* match_tree_path_1_111111(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, true, true, 1>(
    context, extra, path, path_len, label1, name1, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_111112(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, true, true, 2>(
    context, extra, path, path_len, label1, name1, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_111113(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, true, true, 3>(
    context, extra, path, path_len, label1, name1, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_111101(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, true, false, 1>(
    context, extra, path, path_len, label1, name1, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_111102(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, true, false, 2>(
    context, extra, path, path_len, label1, name1, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_111103(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, true, false, 3>(
    context, extra, path, path_len, label1, name1, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_111011(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, false, true, 1>(
    context, extra, path, path_len, label1, name1, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_111012(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, false, true, 2>(
    context, extra, path, path_len, label1, name1, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_111013(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, false, true, 3>(
    context, extra, path, path_len, label1, name1, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_111001(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, false, false, 1>(
    context, extra, path, path_len, label1, name1, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_111002(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, false, false, 2>(
    context, extra, path, path_len, label1, name1, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_111003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, true, false, false, 3>(
    context, extra, path, path_len, label1, name1, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_110111(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, true, true, 1>(
    context, extra, path, path_len, label1, 0, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_110112(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, true, true, 2>(
    context, extra, path, path_len, label1, 0, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_110113(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, true, true, 3>(
    context, extra, path, path_len, label1, 0, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_110101(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, true, false, 1>(
    context, extra, path, path_len, label1, 0, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_110102(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, true, false, 2>(
    context, extra, path, path_len, label1, 0, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_110103(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, true, false, 3>(
    context, extra, path, path_len, label1, 0, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_110011(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, false, true, 1>(
    context, extra, path, path_len, label1, 0, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_110012(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, false, true, 2>(
    context, extra, path, path_len, label1, 0, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_110013(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, false, true, 3>(
    context, extra, path, path_len, label1, 0, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_110001(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, false, false, 1>(
    context, extra, path, path_len, label1, 0, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_110002(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, false, false, 2>(
    context, extra, path, path_len, label1, 0, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_110003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<true, true, false, false, false, 3>(
    context, extra, path, path_len, label1, 0, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_011110(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, true, true, 0>(
    context, extra, path, path_len, label1, name1, field1, index1, 0, out_len
  );
}
const char* match_tree_path_1_011111(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, true, true, 1>(
    context, extra, path, path_len, label1, name1, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_011112(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, true, true, 2>(
    context, extra, path, path_len, label1, name1, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_011113(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, true, true, 3>(
    context, extra, path, path_len, label1, name1, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_011100(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, true, false, 0>(
    context, extra, path, path_len, label1, name1, field1, 0, 0, out_len
  );
}
const char* match_tree_path_1_011101(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, true, false, 1>(
    context, extra, path, path_len, label1, name1, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_011102(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, true, false, 2>(
    context, extra, path, path_len, label1, name1, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_011103(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, true, false, 3>(
    context, extra, path, path_len, label1, name1, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_011010(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 index1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, false, true, 0>(
    context, extra, path, path_len, label1, name1, 0, index1, 0, out_len
  );
}
const char* match_tree_path_1_011011(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, false, true, 1>(
    context, extra, path, path_len, label1, name1, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_011012(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, false, true, 2>(
    context, extra, path, path_len, label1, name1, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_011013(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, false, true, 3>(
    context, extra, path, path_len, label1, name1, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_011000(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, false, false, 0>(
    context, extra, path, path_len, label1, name1, 0, 0, 0, out_len
  );
}
const char* match_tree_path_1_011001(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, false, false, 1>(
    context, extra, path, path_len, label1, name1, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_011002(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, false, false, 2>(
    context, extra, path, path_len, label1, name1, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_011003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, true, false, false, 3>(
    context, extra, path, path_len, label1, name1, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_010110(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, true, true, 0>(
    context, extra, path, path_len, label1, 0, field1, index1, 0, out_len
  );
}
const char* match_tree_path_1_010111(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, true, true, 1>(
    context, extra, path, path_len, label1, 0, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_010112(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, true, true, 2>(
    context, extra, path, path_len, label1, 0, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_010113(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, true, true, 3>(
    context, extra, path, path_len, label1, 0, field1, index1, steps1, out_len
  );
}
const char* match_tree_path_1_010100(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, true, false, 0>(
    context, extra, path, path_len, label1, 0, field1, 0, 0, out_len
  );
}
const char* match_tree_path_1_010101(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, true, false, 1>(
    context, extra, path, path_len, label1, 0, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_010102(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, true, false, 2>(
    context, extra, path, path_len, label1, 0, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_010103(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, true, false, 3>(
    context, extra, path, path_len, label1, 0, field1, 0, steps1, out_len
  );
}
const char* match_tree_path_1_010010(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 index1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, false, true, 0>(
    context, extra, path, path_len, label1, 0, 0, index1, 0, out_len
  );
}
const char* match_tree_path_1_010011(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, false, true, 1>(
    context, extra, path, path_len, label1, 0, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_010012(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, false, true, 2>(
    context, extra, path, path_len, label1, 0, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_010013(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 index1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, false, true, 3>(
    context, extra, path, path_len, label1, 0, 0, index1, steps1, out_len
  );
}
const char* match_tree_path_1_010000(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, false, false, 0>(
    context, extra, path, path_len, label1, 0, 0, 0, 0, out_len
  );
}
const char* match_tree_path_1_010001(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, false, false, 1>(
    context, extra, path, path_len, label1, 0, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_010002(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, false, false, 2>(
    context, extra, path, path_len, label1, 0, 0, 0, steps1, out_len
  );
}
const char* match_tree_path_1_010003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 steps1,
  gdv_int32* out_len
) {
  return match_1<false, true, false, false, false, 3>(
    context, extra, path, path_len, label1, 0, 0, 0, steps1, out_len
  );
}

const char* match_tree_path_3_010100_011003_010003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 label2,
  const gdv_int64 name2,
  const gdv_uint16 steps2,
  const gdv_uint16 label3,
  const gdv_uint16 steps3,
  gdv_int32* out_len
) {
  return match_3<false, true, false, true, false, 0, false, true, true, false, false, 3, false, true, false, false, false, 3>(
    context, extra, path, path_len, label1, 0, field1, 0, 0, label2, name2, 0, 0, steps2, label3, 0, 0, 0, steps3, out_len
  );
}
const char* match_tree_path_3_010100_011003_110003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 label2,
  const gdv_int64 name2,
  const gdv_uint16 steps2,
  const gdv_uint16 label3,
  const gdv_uint16 steps3,
  gdv_int32* out_len
) {
  return match_3<false, true, false, true, false, 0, false, true, true, false, false, 3, true, true, false, false, false, 3>(
    context, extra, path, path_len, label1, 0, field1, 0, 0, label2, name2, 0, 0, steps2, label3, 0, 0, 0, steps3, out_len
  );
}
const char* match_tree_path_3_010100_011103_010003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 label2,
  const gdv_int64 name2,
  const gdv_uint16 field2,
  const gdv_uint16 steps2,
  const gdv_uint16 label3,
  const gdv_uint16 steps3,
  gdv_int32* out_len
) {
  return match_3<false, true, false, true, false, 0, false, true, true, true, false, 3, false, true, false, false, false, 3>(
    context, extra, path, path_len, label1, 0, field1, 0, 0, label2, name2, field2, 0, steps2, label3, 0, 0, 0, steps3, out_len
  );
}
const char* match_tree_path_3_010100_011103_110003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 label2,
  const gdv_int64 name2,
  const gdv_uint16 field2,
  const gdv_uint16 steps2,
  const gdv_uint16 label3,
  const gdv_uint16 steps3,
  gdv_int32* out_len
) {
  return match_3<false, true, false, true, false, 0, false, true, true, true, false, 3, true, true, false, false, false, 3>(
    context, extra, path, path_len, label1, 0, field1, 0, 0, label2, name2, field2, 0, steps2, label3, 0, 0, 0, steps3, out_len
  );
}
const char* match_tree_path_3_010100_010003_010003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 label2,
  const gdv_uint16 steps2,
  const gdv_uint16 label3,
  const gdv_uint16 steps3,
  gdv_int32* out_len
) {
  return match_3<false, true, false, true, false, 0, false, true, false, false, false, 3, false, true, false, false, false, 3>(
    context, extra, path, path_len, label1, 0, field1, 0, 0, label2, 0, 0, 0, steps2, label3, 0, 0, 0, steps3, out_len
  );
}
const char* match_tree_path_3_010100_010003_010000(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 label2,
  const gdv_uint16 steps2,
  const gdv_uint16 label3,
  gdv_int32* out_len
) {
  return match_3<false, true, false, true, false, 0, false, true, false, false, false, 3, false, true, false, false, false, 0>(
    context, extra, path, path_len, label1, 0, field1, 0, 0, label2, 0, 0, 0, steps2, label3, 0, 0, 0, 0, out_len
  );
}
const char* match_tree_path_2_010100_011003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 label2,
  const gdv_int64 name2,
  const gdv_uint16 steps2,
  gdv_int32* out_len
) {
  return match_2<false, true, false, true, false, 0, false, true, true, false, false, 3>(
    context, extra, path, path_len, label1, 0, field1, 0, 0, label2, name2, 0, 0, steps2, out_len
  );
}
const char* match_tree_path_2_011000_010003(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_uint16 label1,
  const gdv_int64 name1,
  const gdv_uint16 label2,
  const gdv_uint16 steps2,
  gdv_int32* out_len
) {
  return match_2<false, true, true, false, false, 0, false, true, false, false, false, 3>(
    context, extra, path, path_len, label1, name1, 0, 0, 0, label2, 0, 0, 0, steps2, out_len
  );
}



}  // extern "C"
