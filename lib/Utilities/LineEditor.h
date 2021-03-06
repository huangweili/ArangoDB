////////////////////////////////////////////////////////////////////////////////
/// @brief abstract line editor
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
/// @author Dr. Frank Celler
/// @author Copyright 2011-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_UTILITIES_LINE_EDITOR_H
#define TRIAGENS_UTILITIES_LINE_EDITOR_H 1

#include "Basics/Common.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                  class LineEditor
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor
////////////////////////////////////////////////////////////////////////////////

class LineEditor {
  LineEditor (LineEditor const&);
  LineEditor& operator= (LineEditor const&);

// -----------------------------------------------------------------------------
// --SECTION--                                                  public constants
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief number of history entries
////////////////////////////////////////////////////////////////////////////////

    static const int MAX_HISTORY_ENTRIES = 1000;

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

    LineEditor (std::string const& history);

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

    virtual ~LineEditor ();

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor open
////////////////////////////////////////////////////////////////////////////////

    virtual bool open (bool autoComplete);

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor shutdown
////////////////////////////////////////////////////////////////////////////////

    virtual bool close ();

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor prompt
////////////////////////////////////////////////////////////////////////////////

    virtual char* prompt (char const* prompt);

////////////////////////////////////////////////////////////////////////////////
/// @brief get the history file path
///
/// The path is "$HOME" plus _historyFilename, if $HOME is set. Else
/// the local file _historyFilename.
////////////////////////////////////////////////////////////////////////////////

    virtual std::string historyPath ();

////////////////////////////////////////////////////////////////////////////////
/// @brief add to history
////////////////////////////////////////////////////////////////////////////////

    virtual void addHistory (const char*);

// -----------------------------------------------------------------------------
// --SECTION--                                                 protected methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief check if line is complete
////////////////////////////////////////////////////////////////////////////////

    virtual bool isComplete (std::string const&, size_t lineno, size_t column) = 0;

// -----------------------------------------------------------------------------
// --SECTION--                                                     private types
// -----------------------------------------------------------------------------

  private:

////////////////////////////////////////////////////////////////////////////////
/// @brief state of the console
////////////////////////////////////////////////////////////////////////////////

    typedef enum {
      STATE_NONE = 0,
      STATE_OPENED,
      STATE_CLOSED
    }
    console_state_e;

// -----------------------------------------------------------------------------
// --SECTION--                                               protected variables
// -----------------------------------------------------------------------------

  protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief current text
////////////////////////////////////////////////////////////////////////////////

    std::string _current;

////////////////////////////////////////////////////////////////////////////////
/// @brief history filename
////////////////////////////////////////////////////////////////////////////////

    std::string _historyFilename;

////////////////////////////////////////////////////////////////////////////////
/// @brief current console state
////////////////////////////////////////////////////////////////////////////////

    console_state_e _state;
};

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
