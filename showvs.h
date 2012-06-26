#ifndef  showvs_INC
#define  showvs_INC
/*
 *显示vs中的内容
 */

#include <stdio.h>
#include <stdlib.h>
#include "luahead.h"

#ifdef DEBUG
    #define SHOW_VSL(exp) exp; printf(#exp"\n"); stackDump(L)
    #define DEBUG_LOG(exp) exp;
#else
    #define SHOW_VSL(exp) exp; 
    #define DEBUG_LOG(exp) 
#endif

#ifdef __cplusplus
    extern "C" {
#endif

    void stackDump(lua_State *L);

#ifdef __cplusplus
    }
#endif

#endif   /* ----- #ifndef showvs_INC  ----- */
