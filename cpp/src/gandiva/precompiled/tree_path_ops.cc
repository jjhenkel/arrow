
// String functions
#include "arrow/util/value_parsing.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./types.h"


#define READ_UINT16_AT_OFFSET(IDX, OFFSET) \
  ((static_cast<uint8_t>(path[IDX + OFFSET + 1]) << BITS_IN_BYTE) | static_cast<uint8_t>(path[IDX + OFFSET]))

#define READ_INT64_AT_OFFSET(IDX, OFFSET) \
  ((static_cast<int64_t>(path[IDX + OFFSET + 3]) << BITS_IN_BYTE*3) | (static_cast<int64_t>(path[IDX + OFFSET + 2]) << BITS_IN_BYTE*2) | (static_cast<int64_t>(path[IDX + OFFSET + 1]) << BITS_IN_BYTE*1) | static_cast<int64_t>(path[IDX + OFFSET]))

#define VALIDATE_PATH(NUM_LABELS) \
  if (path_len <= (NUM_LABELS) * PATH_SEGMENT_SIZE) { \
    *out_len = 0; \
    return ""; \
  }


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
    const char* path,
    const int32_t path_len,
    const uint16_t label,
    const int64_t name,
    const uint16_t field,
    const uint16_t index,
    const uint16_t steps
) {
    for (int32_t p_idx = 0; p_idx < path_len; p_idx += PATH_SEGMENT_SIZE) {
        if constexpr (!NEGATE && MATCH_NAME) {
            if (name != READ_INT64_AT_OFFSET(p_idx, NAME_ID_OFFSET)) {
                continue;
            }
        } else if constexpr (NEGATE && MATCH_NAME) {
            if (name == READ_INT64_AT_OFFSET(p_idx, NAME_ID_OFFSET)) {
                continue;
            }
        }

        if constexpr (!NEGATE && MATCH_LABEL) {
            if (label != READ_UINT16_AT_OFFSET(p_idx, LABEL_OFFSET)) {
                continue;
            }
        } else if constexpr (NEGATE && MATCH_LABEL) {
            if (label == READ_UINT16_AT_OFFSET(p_idx, LABEL_OFFSET)) {
                continue;
            }
        }

        if constexpr (!NEGATE && MATCH_FIELD) { 
            if (field != READ_UINT16_AT_OFFSET(p_idx, FIELD_OFFSET)) {
                continue;
            }
        } else if constexpr (NEGATE && MATCH_FIELD) {
            if (field == READ_UINT16_AT_OFFSET(p_idx, FIELD_OFFSET)) {
                continue;
            }
        }

        if constexpr (!NEGATE && MATCH_INDEX) {
            if (index != READ_UINT16_AT_OFFSET(p_idx, INDEX_OFFSET)) {
                continue;
            }
        } else if constexpr (NEGATE && MATCH_INDEX) {
            if (index == READ_UINT16_AT_OFFSET(p_idx, INDEX_OFFSET)) {
                continue;
            }
        }

        if constexpr (MATCH_STEPS_OP == 1) {        // <
            if ((p_idx / PATH_SEGMENT_SIZE) >= steps) {
                continue;
            }
        } else if constexpr (MATCH_STEPS_OP == 2) { // >
            if ((p_idx / PATH_SEGMENT_SIZE) <= steps) {
                continue;
            }
        } else if constexpr (MATCH_STEPS_OP == 3) { // ==
            if ((p_idx / PATH_SEGMENT_SIZE) != steps) {
                continue;
            }
        }

        return p_idx;
    }

    return -1;
}

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
    int32_t idx1 = 0;
    
    const char * current = path;
    int32_t curr_len = path_len;

    while (idx1 < curr_len && curr_len > 0) {
        idx1 = match_inner<
            NEGATE_1, MATCH_LABEL_1, MATCH_NAME_1, MATCH_FIELD_1, MATCH_INDEX_1, MATCH_STEPS_OP_1
        >(current, curr_len, label1, name1, field1, index1, steps1);

        if (idx1 != -1) {
          *out_len = 32;
          char* ret = reinterpret_cast<char*>(gdv_fn_context_arena_malloc(context, 32));
          if (ret == nullptr) {
            gdv_fn_context_set_error_msg(context, "Could not allocate memory for output bytes.");
            *out_len = 0;
            return nullptr;
          }

          memcpy(ret, static_cast<const void*>(&extra), EXTRA_SIZE);
          memcpy(&ret[EXTRA_SIZE], &current[idx1], GID_AND_DEF_SIZE);
          memcpy(&ret[EXTRA_SIZE + GID_AND_POS_SIZE], &current[idx1+POS_OFFSET], POS_SIZE);
          return ret;
        }

        current = &current[idx1 + PATH_SEGMENT_SIZE];
        curr_len = curr_len - idx1 - PATH_SEGMENT_SIZE;
        idx1 = -1;
    }

  *out_len = 0;
  return nullptr;
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
    int32_t idx1 = 0;
    int32_t idx2 = 0;
    
    const char * current = path;
    int32_t curr_len = path_len;

    while (idx1 < curr_len && curr_len > 0) {
        idx1 = match_inner<
            NEGATE_1, MATCH_LABEL_1, MATCH_NAME_1, MATCH_FIELD_1, MATCH_INDEX_1, MATCH_STEPS_OP_1
        >(current, curr_len, label1, name1, field1, index1, steps1);
    
        idx2 = match_inner<
            NEGATE_2, MATCH_LABEL_2, MATCH_NAME_2, MATCH_FIELD_2, MATCH_INDEX_2, MATCH_STEPS_OP_2
        >(&current[idx1], curr_len - idx1, label2, name2, field2, index2, steps2);

        if (idx1 != -1 && idx2 != -1) {
          *out_len = 56;
          char* ret = reinterpret_cast<char*>(gdv_fn_context_arena_malloc(context, 56));
          if (ret == nullptr) {
            gdv_fn_context_set_error_msg(context, "Could not allocate memory for output bytes.");
            *out_len = 0;
            return nullptr;
          }

          memcpy(ret, static_cast<const void*>(&extra), EXTRA_SIZE);
          memcpy(&ret[EXTRA_SIZE], &current[idx1], GID_AND_DEF_SIZE);
          memcpy(&ret[EXTRA_SIZE + GID_AND_POS_SIZE], &current[idx1+POS_OFFSET], POS_SIZE);
          memcpy(&ret[32], &current[idx2], GID_AND_DEF_SIZE);
          memcpy(&ret[48], &current[idx2+POS_OFFSET], POS_SIZE);
          return ret;
        }

        current = &current[idx1 + PATH_SEGMENT_SIZE];
        curr_len = curr_len - idx1 - PATH_SEGMENT_SIZE;
        idx1 = -1;
        idx2 = -1;
    }

  *out_len = 0;
  return nullptr;
}


extern "C" {

const char* match_1_010000(
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

const char* match_1_110003(
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

const char* match_1_010003(
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

}  // extern "C"
