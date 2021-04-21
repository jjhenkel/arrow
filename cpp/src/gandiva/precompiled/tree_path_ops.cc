
// String functions
#include "arrow/util/value_parsing.h"
extern "C" {

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./types.h"


const gdv_int16 EXTRA_SIZE = 8;
const gdv_int16 GID_SIZE = 8;
const gdv_int16 POS_SIZE = 8;
const gdv_int16 GID_AND_POS_SIZE = 16;
const gdv_int16 PATH_SEGMENT_SIZE = 40;

const gdv_int16 DEF_ID_OFFSET = 8;
const gdv_int16 NAME_ID_OFFSET = 16;
const gdv_int16 POS_OFFSET = 24;
const gdv_int16 LABEL_OFFSET = 32;
const gdv_int16 FIELD_OFFSET = 34;
const gdv_int16 INDEX_OFFSET = 36;

const gdv_uint16 BITS_IN_BYTE = 8;


#define READ_UINT16_AT_OFFSET(IDX, OFFSET) \
  ((static_cast<uint8_t>(path[IDX + OFFSET + 1]) << BITS_IN_BYTE) | static_cast<uint8_t>(path[IDX + OFFSET]))

#define VALIDATE_PATH(NUM_LABELS) \
  if (path_len <= (NUM_LABELS) * PATH_SEGMENT_SIZE) { \
    *out_len = 0; \
    return ""; \
  }

FORCE_INLINE
const char* match_tree_path_n1(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_boolean negate,
  const gdv_boolean capture1,
  const gdv_boolean match_field1,
  const gdv_boolean match_index1,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  gdv_int32* out_len
) {
  const gdv_int16 NUM_LABELS = 1;
  const gdv_int16 NUM_CAPTURES = capture1 ? 1 : 0;

  if (path_len < NUM_LABELS * PATH_SEGMENT_SIZE) {
    *out_len = 0;
    return "";
  }

  gdv_int32 match_1_idx = -1;
  for (gdv_int32 p_idx = 0; p_idx < path_len; p_idx += PATH_SEGMENT_SIZE) {
    if (label1 != READ_UINT16_AT_OFFSET(p_idx, LABEL_OFFSET)) {
        continue;
    }
    if (match_field1 && field1 != READ_UINT16_AT_OFFSET(p_idx, FIELD_OFFSET)) {
        continue;
    }
    if (match_index1 && index1 != READ_UINT16_AT_OFFSET(p_idx, INDEX_OFFSET)) {
        continue;
    }
    match_1_idx = p_idx;
    break;
  }

  if ((negate && match_1_idx == -1) || (!negate && match_1_idx != -1)) {
    // Allocate num capture * size per capture + extra bytes
    *out_len = NUM_CAPTURES * GID_AND_POS_SIZE + EXTRA_SIZE;
    gdv_binary ret = reinterpret_cast<gdv_binary>(gdv_fn_context_arena_malloc(context, *out_len));
    if (ret == nullptr) {
        gdv_fn_context_set_error_msg(context, "Could not allocate memory for output bytes.");
        *out_len = 0;
        return "";
    }

    // Copy over extra + maybe capture
    memcpy(ret, static_cast<const void*>(&extra), EXTRA_SIZE);
    if (capture1) {
        memcpy(&ret[EXTRA_SIZE], &path[match_1_idx], GID_SIZE);
        memcpy(&ret[EXTRA_SIZE+GID_SIZE], &path[match_1_idx+POS_OFFSET], POS_SIZE);
    }
    
    return ret;
  }

  *out_len = 0;
  return "";
}


FORCE_INLINE
const char* match_tree_path_n2(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_boolean negate,
  const gdv_boolean capture1,
  const gdv_boolean match_field1,
  const gdv_boolean match_index1,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps_1_to_2,
  const gdv_boolean capture2,
  const gdv_boolean match_field2,
  const gdv_boolean match_index2,
  const gdv_uint16 label2,
  const gdv_uint16 field2,
  const gdv_uint16 index2,
  gdv_int32* out_len
) {
  const gdv_int16 NUM_LABELS = 2;
  const gdv_int16 NUM_CAPTURES = (
      static_cast<gdv_int16>(capture1) + static_cast<gdv_int16>(capture2)
  );

  if (path_len < NUM_LABELS * PATH_SEGMENT_SIZE) {
    *out_len = 0;
    return "";
  }

  gdv_int32 match_1_idx = -1;
  gdv_int32 match_2_idx = -1;
  for (gdv_int32 p_idx = 0; p_idx < path_len; p_idx += PATH_SEGMENT_SIZE) {
    match_2_idx = -1;

    if (label1 != READ_UINT16_AT_OFFSET(p_idx, LABEL_OFFSET)) {
        continue;
    }
    if (match_field1 && field1 != READ_UINT16_AT_OFFSET(p_idx, FIELD_OFFSET)) {
        continue;
    }
    if (match_index1 && index1 != READ_UINT16_AT_OFFSET(p_idx, INDEX_OFFSET)) {
        continue;
    }

    gdv_int32 steps_taken_1_2 = 0;
    for (gdv_int32 p_idx_2 = p_idx + PATH_SEGMENT_SIZE; p_idx_2 < path_len; p_idx_2 += PATH_SEGMENT_SIZE) {
        steps_taken_1_2 += 1;
        if (label2 != READ_UINT16_AT_OFFSET(p_idx_2, LABEL_OFFSET)) {
            continue;
        }
        if (match_field2 && field2 != READ_UINT16_AT_OFFSET(p_idx_2, FIELD_OFFSET)) {
            continue;
        }
        if (match_index2 && index2 != READ_UINT16_AT_OFFSET(p_idx_2, INDEX_OFFSET)) {
            continue;
        }
        match_2_idx = p_idx_2;
        break;
    }

    if (match_2_idx == -1 || (steps_1_to_2 != 0 && steps_taken_1_2 != steps_1_to_2)) {
        continue;
    }
    
    match_1_idx = p_idx;

    break;
  }

  if ((negate && match_1_idx == -1 && match_2_idx == -1) || (!negate && match_1_idx != -1 && match_2_idx != -1)) {
    // Allocate num capture * size per capture + extra bytes
    *out_len = NUM_CAPTURES * GID_AND_POS_SIZE + EXTRA_SIZE;
    gdv_binary ret = reinterpret_cast<gdv_binary>(gdv_fn_context_arena_malloc(context, *out_len));
    if (ret == nullptr) {
        gdv_fn_context_set_error_msg(context, "Could not allocate memory for output bytes.");
        *out_len = 0;
        return "";
    }

    // Copy over extra + maybe capture
    gdv_int16 offset = EXTRA_SIZE;
    memcpy(ret, static_cast<const void*>(&extra), EXTRA_SIZE);
    if (capture1) {
        memcpy(&ret[offset], &path[match_1_idx], GID_SIZE);
        memcpy(&ret[offset+GID_SIZE], &path[match_1_idx+POS_OFFSET], POS_SIZE);
        offset += GID_AND_POS_SIZE;
    }
    if (capture2) {
        memcpy(&ret[offset], &path[match_2_idx], GID_SIZE);
        memcpy(&ret[offset+GID_SIZE], &path[match_2_idx+POS_OFFSET], POS_SIZE);
    }
    
    return ret;
  }

  *out_len = 0;
  return "";
}


const char* match_tree_path_n3(
  gdv_int64 context,
  const gdv_int64 extra,
  const gdv_binary path,
  const gdv_int32 path_len,
  const gdv_boolean negate,
  const gdv_boolean capture1,
  const gdv_boolean match_field1,
  const gdv_boolean match_index1,
  const gdv_uint16 label1,
  const gdv_uint16 field1,
  const gdv_uint16 index1,
  const gdv_uint16 steps_1_to_2,
  const gdv_boolean capture2,
  const gdv_boolean match_field2,
  const gdv_boolean match_index2,
  const gdv_uint16 label2,
  const gdv_uint16 field2,
  const gdv_uint16 index2,
  const gdv_uint16 steps_2_to_3,
  const gdv_boolean capture3,
  const gdv_boolean match_field3,
  const gdv_boolean match_index3,
  const gdv_uint16 label3,
  const gdv_uint16 field3,
  const gdv_uint16 index3,
  gdv_int32* out_len
) {
  const gdv_int16 NUM_LABELS = 3;
  const gdv_int16 NUM_CAPTURES = (
      static_cast<gdv_int16>(capture1) + static_cast<gdv_int16>(capture2) + static_cast<gdv_int16>(capture3)
  );

  if (path_len < NUM_LABELS * PATH_SEGMENT_SIZE) {
    *out_len = 0;
    return "";
  }

  gdv_int32 match_1_idx = -1;
  gdv_int32 match_2_idx = -1;
  gdv_int32 match_3_idx = -1;
  for (gdv_int32 p_idx = 0; p_idx < path_len; p_idx += PATH_SEGMENT_SIZE) {
    match_2_idx = -1;
    match_3_idx = -1;

    if (label1 != READ_UINT16_AT_OFFSET(p_idx, LABEL_OFFSET)) {
        continue;
    }
    if (match_field1 && field1 != READ_UINT16_AT_OFFSET(p_idx, FIELD_OFFSET)) {
        continue;
    }
    if (match_index1 && index1 != READ_UINT16_AT_OFFSET(p_idx, INDEX_OFFSET)) {
        continue;
    }

    gdv_int32 steps_taken_1_2 = 0;
    for (gdv_int32 p_idx_2 = p_idx + PATH_SEGMENT_SIZE; p_idx_2 < path_len; p_idx_2 += PATH_SEGMENT_SIZE) {
        steps_taken_1_2 += 1;
        match_3_idx = -1;

        if (label2 != READ_UINT16_AT_OFFSET(p_idx_2, LABEL_OFFSET)) {
            continue;
        }
        if (match_field2 && field2 != READ_UINT16_AT_OFFSET(p_idx_2, FIELD_OFFSET)) {
            continue;
        }
        if (match_index2 && index2 != READ_UINT16_AT_OFFSET(p_idx_2, INDEX_OFFSET)) {
            continue;
        }

        gdv_int32 steps_taken_2_3 = 0;
        for (gdv_int32 p_idx_3 = p_idx_2 + PATH_SEGMENT_SIZE; p_idx_3 < path_len; p_idx_3 += PATH_SEGMENT_SIZE) {
            steps_taken_2_3 += 1;
            if (label3 != READ_UINT16_AT_OFFSET(p_idx_3, LABEL_OFFSET)) {
                continue;
            }
            if (match_field3 && field3 != READ_UINT16_AT_OFFSET(p_idx_3, FIELD_OFFSET)) {
                continue;
            }
            if (match_index3 && index3 != READ_UINT16_AT_OFFSET(p_idx_3, INDEX_OFFSET)) {
                continue;
            }
            match_3_idx = p_idx_3;
            break;
        }

        if (match_3_idx == -1 || (steps_2_to_3 != 0 && steps_taken_2_3 != steps_2_to_3)) {
            continue;
        }

        match_2_idx = p_idx_2;
        break;
    }

    if (match_2_idx == -1 || (steps_1_to_2 != 0 && steps_taken_1_2 != steps_1_to_2)) {
        continue;
    }
    
    match_1_idx = p_idx;

    break;
  }

  if ((negate && match_1_idx == -1 && match_2_idx == -1 && match_3_idx == -1) || (!negate && match_1_idx != -1 && match_2_idx != -1 && match_3_idx != -1)) {
    // Allocate num capture * size per capture + extra bytes
    *out_len = NUM_CAPTURES * GID_AND_POS_SIZE + EXTRA_SIZE;
    gdv_binary ret = reinterpret_cast<gdv_binary>(gdv_fn_context_arena_malloc(context, *out_len));
    if (ret == nullptr) {
        gdv_fn_context_set_error_msg(context, "Could not allocate memory for output bytes.");
        *out_len = 0;
        return "";
    }

    // Copy over extra + maybe capture
    gdv_int16 offset = EXTRA_SIZE;
    memcpy(ret, static_cast<const void*>(&extra), EXTRA_SIZE);
    if (capture1) {
        memcpy(&ret[offset], &path[match_2_idx], GID_SIZE);
        memcpy(&ret[GID_SIZE+offset], &path[match_2_idx+POS_OFFSET], POS_SIZE);
        offset += GID_AND_POS_SIZE;
    }
    if (capture2) {
        memcpy(&ret[offset], &path[match_2_idx], GID_SIZE);
        memcpy(&ret[GID_SIZE+offset], &path[match_2_idx+POS_OFFSET], POS_SIZE);
        offset += GID_AND_POS_SIZE;
    }
    if (capture3) {
        memcpy(&ret[offset], &path[match_3_idx], GID_SIZE);
        memcpy(&ret[GID_SIZE+offset], &path[match_3_idx+POS_OFFSET], POS_SIZE);
    }
    
    return ret;
  }

  *out_len = 0;
  return "";
}


}  // extern "C"
