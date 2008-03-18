/* -*- C++ -*-
 * 
 *  ScriptHandler.h - Script manipulation class
 *
 *  Copyright (c) 2001-2005 Ogapee. All rights reserved.
 *
 *  ogapee@aqua.dti2.ne.jp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Modified by Haeleth, Autumn 2006, to better support OS X packaging.

#ifndef __SCRIPT_HANDLER_H__
#define __SCRIPT_HANDLER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BaseReader.h"

#define VARIABLE_RANGE 4096

#define IS_TWO_BYTE(x) \
        ( ((x) & 0xe0) == 0xe0 || ((x) & 0xe0) == 0x80 )

typedef unsigned char uchar3[3];

class ScriptHandler
{
public:
    enum { END_NONE       = 0,
           END_COMMA      = 1,
           END_1BYTE_CHAR = 2
    };
    struct LabelInfo{
        char *name;
        char *label_header;
        char *start_address;
        int  start_line;
        int  num_of_lines;
    };

    struct ArrayVariable{
        struct ArrayVariable* next;
        int no;
        int num_dim;
        int dim[20];
        int *data;
        ArrayVariable(){
            next = NULL;
            data = NULL;
        };
        ~ArrayVariable(){
            if (data) delete[] data;
        };
        ArrayVariable& operator=(const ArrayVariable& av){
            no = av.no;
            num_dim = av.num_dim;

            int total_dim = 1;
            for (int i=0 ; i<20 ; i++){
                dim[i] = av.dim[i];
                total_dim *= dim[i];
            }

            if (data) delete[] data;
            data = NULL;
            if (av.data){
                data = new int[total_dim];
                memcpy(data, av.data, sizeof(int)*total_dim);
            }

            return *this;
        };
    };

    enum { VAR_NONE  = 0,
           VAR_INT   = 1,  // integer
           VAR_ARRAY = 2,  // array
           VAR_STR   = 4,  // string
           VAR_CONST = 8,  // direct value or alias, not variable
           VAR_PTR   = 16  // poiter to a variable, e.g. i%0, s%0
    };
    struct VariableInfo{
        int type;
        int var_no;   // for integer(%), array(?), string($) variable
        ArrayVariable array; // for array(?)
    };

    ScriptHandler();
    ~ScriptHandler();

    void reset();
    FILE *fopen( const char *path, const char *mode, const bool save = false );
    void setKeyTable( const unsigned char *key_table );

    // basic parser function
    const char *readToken();
    const char *readLabel();
    void readVariable( bool reread_flag=false );
    const char *readStr();
    int  readInt();
    int  parseInt( char **buf );
    void skipToken();

    // function for string access
    inline char *getStringBuffer(){ return string_buffer; };
    char *saveStringBuffer();
    void addStringBuffer( char ch );
    
    // function for direct manipulation of script address 
    inline char *getCurrent(){ return current_script; };
    inline char *getNext(){ return next_script; };
    void setCurrent(char *pos);
    void pushCurrent( char *pos );
    void popCurrent();

    int  getOffset( char *pos );
    char *getAddress( int offset );
    int  getLineByAddress( char *address );
    char *getAddressByLine( int line );
    LabelInfo getLabelByAddress( char *address );
    LabelInfo getLabelByLine( int line );

    bool isName( const char *name );
    bool isText();
    bool compareString( const char *buf );
    inline int getEndStatus(){ return end_status; };
    void skipLine( int no=1 );
    void setLinepage( bool val );

    // function for kidoku history
    bool isKidoku();
    void markAsKidoku( char *address=NULL );
    void setKidokuskip( bool kidokuskip_flag );
    void saveKidokuData();
    void loadKidokuData();

    void addStrVariable(char **buf);
    void addIntVariable(char **buf);
    void declareDim();

    void enableTextgosub(bool val);
    void setClickstr( const char *list );
    int  checkClickstr(const char *buf, bool recursive_flag=false);

    void setInt( VariableInfo *var_info, int val, int offset=0 );
    void setNumVariable( int no, int val );
    void pushVariable();
    int  getIntVariable( VariableInfo *var_info=NULL );

    int  getStringFromInteger( char *buffer, int no, int num_column, bool is_zero_inserted=false );

    int  readScriptSub( FILE *fp, char **buf, int encrypt_mode );
    int  readScript( char *path );
    int  labelScript();

    LabelInfo lookupLabel( const char* label );
    LabelInfo lookupLabelNext( const char* label );
    void errorAndExit( char *str );

    ArrayVariable *getRootArrayVariable();
    void loadArrayVariable( FILE *fp );
    
    void addNumAlias( const char *str, int no );
    void addStrAlias( const char *str1, const char *str2 );

    typedef enum { LABEL_LOG = 0,
                   FILE_LOG = 1
    };
    struct LogLink{
        LogLink *next;
        char *name;

        LogLink(){
            next = NULL;
            name = NULL;
        };
        ~LogLink(){
            if ( name ) delete[] name;
        };
    };
    struct LogInfo{
        LogLink root_log;
        LogLink *current_log;
        int num_logs;
        char *filename;
    } log_info[2];
    LogLink *findAndAddLog( LogInfo &info, const char *name, bool add_flag );
    void resetLog( LogInfo &info );
    
    /* ---------------------------------------- */
    /* Variable */
    struct VariableData{
        int num;
        bool num_limit_flag;
        int num_limit_upper;
        int num_limit_lower;
        char *str;

        VariableData(){
            str = NULL;
            reset(true);
        };
        void reset(bool limit_reset_flag){
            num = 0;
            if (limit_reset_flag)
                num_limit_flag = false;
            if (str){
                delete[] str;
                str = NULL;
            }
        };
    } *variable_data;
    
    VariableInfo current_variable, pushed_variable;
    
    int screen_size;
    enum { SCREEN_SIZE_640x480 = 0,
           SCREEN_SIZE_800x600 = 1,
           SCREEN_SIZE_400x300 = 2,
           SCREEN_SIZE_320x240 = 3
    };
    int global_variable_border;

/* Haeleth */
    char *game_identifier;
    char *save_path;
/* /Haeleth */

    BaseReader *cBR;
    
private:
    enum { OP_INVALID = 0, // 000
           OP_PLUS    = 2, // 010
           OP_MINUS   = 3, // 011
           OP_MULT    = 4, // 100
           OP_DIV     = 5, // 101
           OP_MOD     = 6  // 110
    };
    
    struct Alias{
        struct Alias *next;
        char *alias;
        int  num;
        char *str;

        Alias(){
            next = NULL;
            alias = NULL;
            str = NULL;
        };
        Alias( const char *name, int num ){
            next = NULL;
            alias = new char[ strlen(name) + 1];
            strcpy( alias, name );
            str = NULL;
            this->num = num;
        };
        Alias( const char *name, const char *str ){
            next = NULL;
            alias = new char[ strlen(name) + 1];
            strcpy( alias, name );
            this->str = new char[ strlen(str) + 1];
            strcpy( this->str, str );
        };
        ~Alias(){
            if (alias) delete[] alias;
            if (str)   delete[] str;
        };
    };
    
    int findLabel( const char* label );

    char *checkComma( char *buf );
    void parseStr( char **buf );
    int  parseIntExpression( char **buf );
    void readNextOp( char **buf, int *op, int *num );
    int  calcArithmetic( int num1, int op, int num2 );
    int  parseArray( char **buf, ArrayVariable &array );
    int  *getArrayPtr( int no, ArrayVariable &array, int offset );

    /* ---------------------------------------- */
    /* Variable */
    Alias root_num_alias, *last_num_alias;
    Alias root_str_alias, *last_str_alias;
    
    ArrayVariable *root_array_variable, *current_array_variable;

    char *archive_path;
    int  script_buffer_length;
    char *script_buffer;
    char *tmp_script_buf;
    
    char *string_buffer; // update only be readToken
    int  string_counter;
    char *saved_string_buffer; // updated only by saveStringBuffer
    char *str_string_buffer; // updated only by readStr

    LabelInfo *label_info;
    int num_of_labels;

    bool skip_enabled;
    bool kidokuskip_flag;
    char *kidoku_buffer;

    bool text_flag; // true if the current token is text
    int  end_status;
    bool linepage_flag;
    bool textgosub_flag;
    char *clickstr_list;

    char *current_script;
    char *next_script;

    char *pushed_current_script;
    char *pushed_next_script;

    unsigned char key_table[256];
    bool key_table_flag;
};

#endif // __SCRIPT_HANDLER_H__