////////////////////////////////////////////////////////////////////////////////
/// @brief general cursors
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2013 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2012-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "VocBase/general-cursor.h"

#include "BasicsC/json.h"
#include "BasicsC/logging.h"
#include "BasicsC/vector.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                      cursor store
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                   private defines
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief delete at most this number of cursors during a gc cycle
////////////////////////////////////////////////////////////////////////////////

#define CURSOR_MAX_DELETE 256

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                cursor result sets
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief create a cursor result set
////////////////////////////////////////////////////////////////////////////////

TRI_general_cursor_result_t* TRI_CreateCursorResult (void* data,
  void (*freeData)(TRI_general_cursor_result_t*),
  TRI_general_cursor_row_t (*getAt)(TRI_general_cursor_result_t const*, const TRI_general_cursor_length_t),
  TRI_general_cursor_length_t (*getLength)(TRI_general_cursor_result_t const*)) {

  TRI_general_cursor_result_t* result;

  if (data == NULL) {
    return NULL;
  }

  result = (TRI_general_cursor_result_t*) TRI_Allocate(TRI_UNKNOWN_MEM_ZONE, sizeof(TRI_general_cursor_result_t), false);

  if (result == NULL) {
    return NULL;
  }

  result->_data     = data;
  result->_freed    = false;

  result->freeData  = freeData;
  result->getAt     = getAt;
  result->getLength = getLength;

  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destroy a cursor result set but do not free the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_DestroyCursorResult (TRI_general_cursor_result_t* const result) {
  assert(result);

  if (! result->_freed) {
    result->freeData(result);
    result->_freed = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief free a cursor result set
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeCursorResult (TRI_general_cursor_result_t* const result) {
  if (result != NULL) {
    TRI_DestroyCursorResult(result);
    TRI_Free(TRI_UNKNOWN_MEM_ZONE, result);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                           cursors
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief hashes an element in the ids index
////////////////////////////////////////////////////////////////////////////////

static uint64_t HashKeyId (TRI_associative_pointer_t* array, void const* k) {
  return (uint64_t) *((TRI_voc_tick_t*) k);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief hashes an element in the ids index
////////////////////////////////////////////////////////////////////////////////

static uint64_t HashElementId (TRI_associative_pointer_t* array, void const* e) {
  TRI_general_cursor_t const* element = e;

  return (uint64_t) element->_id;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief tests if two elements are equal
////////////////////////////////////////////////////////////////////////////////

static bool EqualKeyId (TRI_associative_pointer_t* array, void const* k, void const* e) {
  TRI_general_cursor_t const* element = e;
  TRI_voc_tick_t key = *((TRI_voc_tick_t*) k);

  return (key == element->_id);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the next element
////////////////////////////////////////////////////////////////////////////////

static inline TRI_general_cursor_row_t NextGeneralCursor (TRI_general_cursor_t* const cursor) {
  if (cursor->_currentRow < cursor->_length) {
    return cursor->_result->getAt(cursor->_result, cursor->_currentRow++);
  }

  if (! cursor->_result->_freed) {
    cursor->_result->_freed = true;
    cursor->_result->freeData(cursor->_result);
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if the cursor is exhausted
////////////////////////////////////////////////////////////////////////////////

static bool HasNextGeneralCursor (const TRI_general_cursor_t* const cursor) {
  return cursor->_currentRow < cursor->_length;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns if the count flag is set for the cursor
////////////////////////////////////////////////////////////////////////////////

static bool HasCountGeneralCursor (const TRI_general_cursor_t* const cursor) {
  return cursor->_hasCount;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the maximum number of results per transfer
////////////////////////////////////////////////////////////////////////////////

static TRI_general_cursor_length_t GetBatchSizeGeneralCursor (const TRI_general_cursor_t* const cursor) {
  return cursor->_batchSize;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the cursor's extra data
////////////////////////////////////////////////////////////////////////////////

static TRI_json_t* GetExtraGeneralCursor (const TRI_general_cursor_t* const cursor) {
  return cursor->_extra;
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief frees a cursor
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeGeneralCursor (TRI_general_cursor_t* cursor) {
  if (cursor->_extra != NULL) {
    TRI_FreeJson(TRI_UNKNOWN_MEM_ZONE, cursor->_extra);
  }
  
  TRI_FreeCursorResult(cursor->_result);

  TRI_DestroySpin(&cursor->_lock);
  TRI_Free(TRI_UNKNOWN_MEM_ZONE, cursor);

  LOG_TRACE("destroyed general cursor");
}

////////////////////////////////////////////////////////////////////////////////
/// @brief create a cursor
////////////////////////////////////////////////////////////////////////////////

TRI_general_cursor_t* TRI_CreateGeneralCursor (TRI_vocbase_t* vocbase,
                                               TRI_general_cursor_result_t* result,
                                               const bool doCount,
                                               const TRI_general_cursor_length_t batchSize,
                                               TRI_json_t* extra) {
  TRI_general_cursor_t* cursor;
  
  assert(vocbase != NULL);

  cursor = (TRI_general_cursor_t*) TRI_Allocate(TRI_UNKNOWN_MEM_ZONE, sizeof(TRI_general_cursor_t), false);

  if (cursor == NULL) {
    return NULL;
  }

  cursor->_vocbase     = vocbase;
  cursor->_store       = vocbase->_cursors;

  cursor->_result      = result;
  cursor->_extra       = extra; // might be NULL
  
  cursor->_expires     = TRI_microtime() + 3600; // default lifetime: 1h
  cursor->_id          = TRI_NewTickServer();

  // state
  cursor->_currentRow  = 0;
  cursor->_length      = result->getLength(result);
  cursor->_hasCount    = doCount;
  cursor->_batchSize   = batchSize;
  cursor->_usage._refCount  = 0;
  cursor->_usage._isDeleted = false;

  // assign functions
  cursor->next         = NextGeneralCursor;
  cursor->hasNext      = HasNextGeneralCursor;
  cursor->hasCount     = HasCountGeneralCursor;
  cursor->getBatchSize = GetBatchSizeGeneralCursor;
  cursor->getExtra     = GetExtraGeneralCursor;
  cursor->free         = TRI_FreeGeneralCursor;

  TRI_InitSpin(&cursor->_lock);

  TRI_LockSpin(&vocbase->_cursors->_lock);
  // TODO: check for errors here
  TRI_InsertKeyAssociativePointer(&vocbase->_cursors->_ids, &cursor->_id, cursor, true);
  TRI_UnlockSpin(&vocbase->_cursors->_lock);

  LOG_TRACE("created general cursor");

  return cursor;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief exclusively lock a general cursor
////////////////////////////////////////////////////////////////////////////////

void TRI_LockGeneralCursor (TRI_general_cursor_t* const cursor) {
  TRI_LockSpin(&cursor->_lock);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief unlock a general cursor
////////////////////////////////////////////////////////////////////////////////

void TRI_UnlockGeneralCursor (TRI_general_cursor_t* const cursor) {
  TRI_UnlockSpin(&cursor->_lock);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief reference-count a general cursor
////////////////////////////////////////////////////////////////////////////////

TRI_general_cursor_t* TRI_UseGeneralCursor (TRI_general_cursor_t* cursor) {
  TRI_general_cursor_store_t* store = cursor->_store;

  TRI_LockSpin(&store->_lock);
  cursor = TRI_LookupByKeyAssociativePointer(&store->_ids, &cursor->_id);

  if (cursor != NULL) {
    if (cursor->_usage._isDeleted) {
      cursor = NULL;
    }
    else {
      ++cursor->_usage._refCount;
    }
  }
  TRI_UnlockSpin(&store->_lock);

  return cursor;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief de-reference-count a general cursor
////////////////////////////////////////////////////////////////////////////////

void TRI_ReleaseGeneralCursor (TRI_general_cursor_t* cursor) {
  TRI_general_cursor_store_t* store = cursor->_store;

  TRI_LockSpin(&store->_lock);
  cursor = TRI_LookupByKeyAssociativePointer(&store->_ids, &cursor->_id);
  if (cursor != NULL) {
    --cursor->_usage._refCount;
  }
  TRI_UnlockSpin(&store->_lock);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief mark as cursor as deleted
////////////////////////////////////////////////////////////////////////////////

bool TRI_DropGeneralCursor (TRI_general_cursor_t* cursor) {
  TRI_general_cursor_store_t* store = cursor->_store;
  bool result;

  TRI_LockSpin(&store->_lock);
  cursor = TRI_LookupByKeyAssociativePointer(&store->_ids, &cursor->_id);
  if (cursor != NULL && ! cursor->_usage._isDeleted) {
    cursor->_usage._isDeleted = true;
    result = true;
  }
  else {
    result = false;
  }
  TRI_UnlockSpin(&store->_lock);

  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief return the cursor id
////////////////////////////////////////////////////////////////////////////////

TRI_voc_tick_t TRI_IdGeneralCursor (TRI_general_cursor_t* cursor) {
  return cursor->_id;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief return the cursor length
////////////////////////////////////////////////////////////////////////////////

size_t TRI_CountGeneralCursor (TRI_general_cursor_t* cursor) {
  return (size_t) cursor->_length;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief persist the cursor by setting a timeout
////////////////////////////////////////////////////////////////////////////////

void TRI_PersistGeneralCursor (TRI_general_cursor_t* cursor,
                               double ttl) {
  cursor->_expires = TRI_microtime() + ttl;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief lookup a cursor by its id
////////////////////////////////////////////////////////////////////////////////

TRI_general_cursor_t* TRI_FindGeneralCursor (TRI_vocbase_t* vocbase, 
                                             TRI_voc_tick_t id) {
  TRI_general_cursor_store_t* store = vocbase->_cursors;
  TRI_general_cursor_t* cursor;

  TRI_LockSpin(&store->_lock);
  cursor = TRI_LookupByKeyAssociativePointer(&store->_ids, &id);
  if (cursor == NULL || cursor->_usage._isDeleted) {
    cursor = NULL;
  }
  TRI_UnlockSpin(&store->_lock);

  return cursor;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief mark as cursor as deleted
////////////////////////////////////////////////////////////////////////////////

bool TRI_RemoveGeneralCursor (TRI_vocbase_t* vocbase, 
                              TRI_voc_tick_t id) {
  TRI_general_cursor_store_t* store = vocbase->_cursors;
  TRI_general_cursor_t* cursor;
  bool result;

  TRI_LockSpin(&store->_lock);
  cursor = TRI_LookupByKeyAssociativePointer(&store->_ids, &id);
  if (cursor == NULL || cursor->_usage._isDeleted) {
    result = false;
  }
  else {
    cursor->_usage._isDeleted = true;
    result = true;
  }
  TRI_UnlockSpin(&store->_lock);

  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief create data store for cursors
////////////////////////////////////////////////////////////////////////////////

TRI_general_cursor_store_t* TRI_CreateStoreGeneralCursor (void) {
  TRI_general_cursor_store_t* store;
  int res;

  store = TRI_Allocate(TRI_UNKNOWN_MEM_ZONE, sizeof(TRI_general_cursor_t), false);

  if (store == NULL) {
    return NULL;
  }

  res = TRI_InitAssociativePointer(&store->_ids,
                                   TRI_UNKNOWN_MEM_ZONE,
                                   HashKeyId,
                                   HashElementId,
                                   EqualKeyId,
                                   NULL);

  if (res != TRI_ERROR_NO_ERROR) {
    TRI_Free(TRI_UNKNOWN_MEM_ZONE, store);

    return NULL;
  }

  TRI_InitSpin(&store->_lock);

  return store;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief free the data store for cursors
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeStoreGeneralCursor (TRI_general_cursor_store_t* store) {
  // force deletion of all remaining cursors
  TRI_CleanupGeneralCursor(store, true);

  TRI_DestroySpin(&store->_lock);
  TRI_DestroyAssociativePointer(&store->_ids);
  TRI_Free(TRI_UNKNOWN_MEM_ZONE, store);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief enumerate all cursors and remove them if
/// - their refcount is 0 and they are transient
/// - their refcount is 0 and they are expired
/// - the force flag is set
////////////////////////////////////////////////////////////////////////////////

void TRI_CleanupGeneralCursor (TRI_general_cursor_store_t* store,
                               bool force) {
  double compareStamp = TRI_microtime(); 
  size_t deleteCount = 0;

  // we need an exclusive lock on the index
  TRI_LockSpin(&store->_lock);

  if (store->_ids._nrUsed == 0) {
    // store is empty, nothing to do!
    TRI_UnlockSpin(&store->_lock);
    return;
  }

  LOG_TRACE("cleaning shadows. in store: %ld", (unsigned long) store->_ids._nrUsed);

  // loop until there's nothing to delete or
  // we have deleted CURSOR_MAX_DELETE elements
  while (deleteCount++ < CURSOR_MAX_DELETE || force) {
    bool deleted = false;
    size_t i;

    for (i = 0; i < store->_ids._nrAlloc; i++) {
      // enum all cursors
      TRI_general_cursor_t* cursor = (TRI_general_cursor_t*) store->_ids._table[i];

      if (cursor == NULL) {
        continue;
      }

      TRI_LockSpin(&cursor->_lock);

      if (force ||
          (cursor->_usage._refCount == 0 &&
           (cursor->_usage._isDeleted || cursor->_expires < compareStamp))) {
        LOG_TRACE("cleaning cursor %p, id: %llu, rc: %d, expires: %d, deleted: %d",
                  cursor,
                  (unsigned long long) cursor->_id,
                  (int) cursor->_usage._refCount,
                  (int) cursor->_expires,
                  (int) cursor->_usage._isDeleted);

        TRI_RemoveKeyAssociativePointer(&store->_ids, &cursor->_id);
        TRI_UnlockSpin(&cursor->_lock);
        TRI_FreeGeneralCursor(cursor);
        deleted = true;
          
        // the remove might reposition elements in the container.
        // therefore break here and start iteration anew
        break;
      }
        
      TRI_UnlockSpin(&cursor->_lock);
    }
  
    if (! deleted) {
      // we did not find anything to delete, so give up
      break;
    }
  }

  // release lock
  TRI_UnlockSpin(&store->_lock);
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
