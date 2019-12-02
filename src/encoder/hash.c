#include "include/uthash.h"
#include "include/encoder.h"
#include "include/codeTables.h"
#include "include/flushTables.h"
#include <string.h>

codeWord *hashedCodeWord0 = NULL, *hashedCodeWord1 = NULL, *hashedCodeWord2 = NULL, *hashedCodeWord3 = NULL, *hashedCodeWord4 = NULL, *hashedCodeWord5 = NULL, *hashedCodeWord6 = NULL, *hashedCodeWord7 = NULL, 
    *hashedCodeWord8 = NULL, *hashedCodeWord9 = NULL, *hashedCodeWord10 = NULL, *hashedCodeWord11 = NULL, *hashedCodeWord12 = NULL, *hashedCodeWord13 = NULL, *hashedCodeWord14 = NULL, *hashedCodeWord15 = NULL;

codeWord *hashedFlushTable0 = NULL, *hashedFlushTable1 = NULL, *hashedFlushTable2 = NULL, *hashedFlushTable3 = NULL, *hashedFlushTable4 = NULL, *hashedFlushTable5 = NULL, *hashedFlushTable6 = NULL, *hashedFlushTable7 = NULL, 
    *hashedFlushTable8 = NULL, *hashedFlushTable9 = NULL, *hashedFlushTable10 = NULL, *hashedFlushTable11 = NULL, *hashedFlushTable12 = NULL, *hashedFlushTable13 = NULL, *hashedFlushTable14 = NULL, *hashedFlushTable15 = NULL;

//Hash all values in codetables according to codeWord struct.
//It is a very ugly solution but it was a quick one.
void hashCodeTableValues() {
    codeWord *tmp = NULL;
    for (uint16_t i = 0; i < sizeof(codeTable0)/sizeof(codeTable0[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable0[i];
        tmp->bitSize = bitSizes0[i];
        tmp->codeWordValue = outPutCodes0[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord0, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable1)/sizeof(codeTable1[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable1[i];
        tmp->bitSize = bitSizes1[i];
        tmp->codeWordValue = outPutCodes1[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord1, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable2)/sizeof(codeTable2[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable2[i];
        tmp->bitSize = bitSizes2[i];
        tmp->codeWordValue = outPutCodes2[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord2, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable3)/sizeof(codeTable3[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable3[i];
        tmp->bitSize = bitSizes3[i];
        tmp->codeWordValue = outPutCodes3[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord3, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable4)/sizeof(codeTable4[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable4[i];
        tmp->bitSize = bitSizes4[i];
        tmp->codeWordValue = outPutCodes4[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord4, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable5)/sizeof(codeTable5[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable5[i];
        tmp->bitSize = bitSizes5[i];
        tmp->codeWordValue = outPutCodes5[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord5, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable6)/sizeof(codeTable6[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable6[i];
        tmp->bitSize = bitSizes6[i];
        tmp->codeWordValue = outPutCodes6[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord6, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable7)/sizeof(codeTable7[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable7[i];
        tmp->bitSize = bitSizes7[i];
        tmp->codeWordValue = outPutCodes7[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord7, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable8)/sizeof(codeTable8[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable8[i];
        tmp->bitSize = bitSizes8[i];
        tmp->codeWordValue = outPutCodes8[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord8, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable9)/sizeof(codeTable9[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable9[i];
        tmp->bitSize = bitSizes9[i];
        tmp->codeWordValue = outPutCodes9[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedCodeWord9, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable10)/sizeof(codeTable10[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable10[i];
        tmp->bitSize = bitSize10[i];
        tmp->codeWordValue = outputCodes10[i];
        tmp->ifReverse = shouldReverse10[i];
        HASH_ADD_KEYPTR(hh, hashedCodeWord10, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable11)/sizeof(codeTable11[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable11[i];
        tmp->bitSize = bitSize11[i];
        tmp->codeWordValue = outputCodes11[i];
        tmp->ifReverse = shouldReverse11[i];
        HASH_ADD_KEYPTR(hh, hashedCodeWord11, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable12)/sizeof(codeTable12[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable12[i];
        tmp->bitSize = bitSize12[i];
        tmp->codeWordValue = outputCodes12[i];
        tmp->ifReverse = shouldReverse12[i];
        HASH_ADD_KEYPTR(hh, hashedCodeWord12, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable13)/sizeof(codeTable13[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable13[i];
        tmp->bitSize = bitSize13[i];
        tmp->codeWordValue = outputCodes13[i];
        tmp->ifReverse = shouldReverse13[i];
        HASH_ADD_KEYPTR(hh, hashedCodeWord13, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable14)/sizeof(codeTable14[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable14[i];
        tmp->bitSize = bitSize14[i];
        tmp->codeWordValue = outputCodes14[i];
        tmp->ifReverse = shouldReverse14[i];
        HASH_ADD_KEYPTR(hh, hashedCodeWord14, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(codeTable15)/sizeof(codeTable15[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = codeTable15[i];
        tmp->bitSize = bitSize15[i];
        tmp->codeWordValue = outputCodes15[i];
        tmp->ifReverse = shouldReverse15[i];
        HASH_ADD_KEYPTR(hh, hashedCodeWord15, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
}

codeWord *searchHash(char * searchString, int table) {
    codeWord *tmp = NULL;
    if(table == 0) {
    }
    switch (table) {
    case 0:
        HASH_FIND_STR(hashedCodeWord0, searchString, tmp);
        break;
    case 1:
        HASH_FIND_STR(hashedCodeWord1, searchString, tmp);
        break;
    case 2:
        HASH_FIND_STR(hashedCodeWord2, searchString, tmp);
        break;
    case 3:
        HASH_FIND_STR(hashedCodeWord3, searchString, tmp);
        break;
    case 4:
        HASH_FIND_STR(hashedCodeWord4, searchString, tmp);
        break;
    case 5:
        HASH_FIND_STR(hashedCodeWord5, searchString, tmp);
        break;
    case 6:
        HASH_FIND_STR(hashedCodeWord6, searchString, tmp);
        break;
    case 7:
        HASH_FIND_STR(hashedCodeWord7, searchString, tmp);
        break;
    case 8:
        HASH_FIND_STR(hashedCodeWord8, searchString, tmp);
        break;
    case 9:
        HASH_FIND_STR(hashedCodeWord9, searchString, tmp);
        break;
    case 10:
        HASH_FIND_STR(hashedCodeWord10, searchString, tmp);
        break;
    case 11:
        HASH_FIND_STR(hashedCodeWord11, searchString, tmp);
        break;
    case 12:
        HASH_FIND_STR(hashedCodeWord12, searchString, tmp);
        break;
    case 13:
        HASH_FIND_STR(hashedCodeWord13, searchString, tmp);
        break;
    case 14:
        HASH_FIND_STR(hashedCodeWord14, searchString, tmp);
        break;
    case 15:
        HASH_FIND_STR(hashedCodeWord15, searchString, tmp);
        break;
    }
    return tmp;
}

void hashFlushCodes() {
    codeWord *tmp = NULL;
    for (uint16_t i = 0; i < sizeof(activePrefix0)/sizeof(activePrefix0[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix0[i];
        tmp->bitSize = bitSizeFlush0[i];
        tmp->codeWordValue = flushWord0[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable0, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix1)/sizeof(activePrefix1[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix1[i];
        tmp->bitSize = bitSizeFlush1[i];
        tmp->codeWordValue = flushWord1[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable1, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix2)/sizeof(activePrefix2[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix2[i];
        tmp->bitSize = bitSizeFlush2[i];
        tmp->codeWordValue = flushWord2[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable2, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix3)/sizeof(activePrefix3[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix3[i];
        tmp->bitSize = bitSizeFlush3[i];
        tmp->codeWordValue = flushWord3[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable3, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix4)/sizeof(activePrefix4[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix4[i];
        tmp->bitSize = bitSizeFlush4[i];
        tmp->codeWordValue = flushWord4[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable4, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix5)/sizeof(activePrefix5[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix5[i];
        tmp->bitSize = bitSizeFlush5[i];
        tmp->codeWordValue = flushWord5[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable5, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix6)/sizeof(activePrefix6[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix6[i];
        tmp->bitSize = bitSizeFlush6[i];
        tmp->codeWordValue = flushWord6[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable6, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix7)/sizeof(activePrefix7[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix7[i];
        tmp->bitSize = bitSizeFlush7[i];
        tmp->codeWordValue = flushWord7[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable7, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix8)/sizeof(activePrefix8[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix8[i];
        tmp->bitSize = bitSizeFlush8[i];
        tmp->codeWordValue = flushWord8[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable8, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix9)/sizeof(activePrefix9[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix9[i];
        tmp->bitSize = bitSizeFlush9[i];
        tmp->codeWordValue = flushWord9[i];
        tmp->ifReverse = 0;
        HASH_ADD_KEYPTR(hh, hashedFlushTable9, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix10)/sizeof(activePrefix10[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix10[i];
        tmp->bitSize = bitSize10[i];
        tmp->codeWordValue = outputCodes10[i];
        tmp->ifReverse = shouldReverse10[i];
        HASH_ADD_KEYPTR(hh, hashedFlushTable10, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix11)/sizeof(activePrefix11[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix11[i];
        tmp->bitSize = bitSize11[i];
        tmp->codeWordValue = outputCodes11[i];
        tmp->ifReverse = shouldReverse11[i];
        HASH_ADD_KEYPTR(hh, hashedFlushTable11, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix12)/sizeof(activePrefix12[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix12[i];
        tmp->bitSize = bitSize12[i];
        tmp->codeWordValue = outputCodes12[i];
        tmp->ifReverse = shouldReverse12[i];
        HASH_ADD_KEYPTR(hh, hashedFlushTable12, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix13)/sizeof(activePrefix13[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix13[i];
        tmp->bitSize = bitSize13[i];
        tmp->codeWordValue = outputCodes13[i];
        tmp->ifReverse = shouldReverse13[i];
        HASH_ADD_KEYPTR(hh, hashedFlushTable13, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix14)/sizeof(activePrefix14[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix14[i];
        tmp->bitSize = bitSize14[i];
        tmp->codeWordValue = outputCodes14[i];
        tmp->ifReverse = shouldReverse14[i];
        HASH_ADD_KEYPTR(hh, hashedFlushTable14, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
    for (uint16_t i = 0; i < sizeof(activePrefix15)/sizeof(activePrefix15[0]); i++)
    {
        tmp = (codeWord *)malloc(sizeof *tmp);
        tmp->inputCodeword = activePrefix15[i];
        tmp->bitSize = bitSize15[i];
        tmp->codeWordValue = outputCodes15[i];
        tmp->ifReverse = shouldReverse15[i];
        HASH_ADD_KEYPTR(hh, hashedFlushTable15, tmp->inputCodeword, strlen(tmp->inputCodeword), tmp);
        
    }
}


codeWord *searchFlushHash(char * searchString, int table) {
    codeWord *tmp = NULL;
    switch (table)
    {
    case 0:
        HASH_FIND_STR(hashedFlushTable0, searchString, tmp);
        break;
    case 1:
        HASH_FIND_STR(hashedFlushTable1, searchString, tmp);
        break;
    case 2:
        HASH_FIND_STR(hashedFlushTable2, searchString, tmp);
        break;
    case 3:
        HASH_FIND_STR(hashedFlushTable3, searchString, tmp);
        break;
    case 4:
        HASH_FIND_STR(hashedFlushTable4, searchString, tmp);
        break;
    case 5:
        HASH_FIND_STR(hashedFlushTable5, searchString, tmp);
        break;
    case 6:
        HASH_FIND_STR(hashedFlushTable6, searchString, tmp);
        break;
    case 7:
        HASH_FIND_STR(hashedFlushTable7, searchString, tmp);
        break;
    case 8:
        HASH_FIND_STR(hashedFlushTable8, searchString, tmp);
        break;
    case 9:
        HASH_FIND_STR(hashedFlushTable9, searchString, tmp);
        break;
    case 10:
        HASH_FIND_STR(hashedFlushTable10, searchString, tmp);
        break;
    case 11:
        HASH_FIND_STR(hashedFlushTable11, searchString, tmp);
        break;
    case 12:
        HASH_FIND_STR(hashedFlushTable12, searchString, tmp);
        break;
    case 13:
        HASH_FIND_STR(hashedFlushTable13, searchString, tmp);
        break;
    case 14:
        HASH_FIND_STR(hashedFlushTable14, searchString, tmp);
        break;
    case 15:
        HASH_FIND_STR(hashedFlushTable15, searchString, tmp);
        break;
    default:
        break;
    }
    return tmp;
}
