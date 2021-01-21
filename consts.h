#ifndef CONSTS_H
#define CONSTS_H

#include "QString"
#include <map>

typedef struct{
    QString type;
    int     hextype;
    QString iotype;
    int     iCount;
    int     oCount;
}module;
module createModule(QString t,int ht, QString iot, int ic, int oc){
    module M = {t,ht,iot,ic,oc};
    return M;
}
std::map <unsigned short, module> modules {
    { 1, createModule("SS-01"     , 0x0010, "none", 0, 0)},
    { 2, createModule("USB/RS-485", 0x0020, "D"   , 1, 0)},
    { 3, createModule("ID-8k"     , 0x4002, "D"   , 1, 0)},
    { 4, createModule("ID-16k"    , 0x6002, "D"   , 2, 0)},
    { 5, createModule("OD-5k"     , 0x1502, "D"   , 0, 1)},
    { 6, createModule("OD-16k"    , 0x1600, "D"   , 4, 1)},
    { 7, createModule("OA-4k"     , 0x3002, "A"   , 4, 4)},
    { 8, createModule("IA-4k"     , 0x2002, "A"   , 4, 1)},
    { 9, createModule("IA-8k"     , 0x2800, "A"   , 8, 0)},
    {10, createModule("IF-1k"     , 0x1900, "A"   , 1, 1)},
    {11, createModule("IF-3k"     , 0x1903, "A"   , 3, 1)},
    {12, createModule("D-1k"      , 0x1A00, "A"   , 4, 2)},
    {13, createModule("БУ ВНА"    , 0x1A10, "A"   , 4, 1)},
};

std::map <unsigned char, QString> module_states {
    { 0, "ERRM_ZERO"       },
    { 1, "ERRM_1_NX_LOC"   },
    { 2, "ERRM_2_NX_REM"   },
    { 3, "ERRM_3_BAN_LOC"  },
    { 4, "ERRM_4_BAN_REM"  },
    { 5, "ERRM_5_HLT_LOC"  },
    { 6, "ERRM_6_HLT_REM"  },
    { 7, "ERRM_7_FRZ_LOC"  },
    { 8, "ERRM_8_FRZ_REM"  },
    { 9, "ERRM_9_UNPLUG"   },
    {10, "ERRM_10_LOST"    },
    {11, "ERRM_11_WRONG"   },
    {12, "ERRM_12_ZEROIN"  },
    {13, "ERRM_13_INIT"    },
    {14, "ERRM_14_PORTFAIL"},
    {15, "ERRM_15_LAN"     },
    {16, "ERRM_20_OK"      },
    {17, "ERRM_20_OK"      },
    {18, "ERRM_20_OK"      },
    {19, "ERRM_20_OK"      },
    {20, "ERRM_20_OK"      }
};

std::map <unsigned char, QString> opc_state {
    {0x00, "OPCQ_BAD_ZERO"      },
    {0x04, "OPCQ_CONFIG"        },
    {0x08, "OPCQ_NOT_CONNECTED" },
    {0x0C, "OPCQ_DEVICE_FAILURE"},
    {0x10, "OPCQ_SENSOR_FAILURE"},
    {0x14, "OPCQ_LAST_KNOWN"    },
    {0x18, "OPCQ_COMM_FAILURE"  },
    {0x1C, "OPCQ_OUT_OF_SERVICE"},
    {0x20, "OPCQ_INIT"          },
    {0xC0, "OPCQ_GOOD"          }
};

std::map <unsigned char, QString> valid_state {
    {0,"VALID_EXISTS"},
    {1,"NONE"},
    {2,"VALID_VERA"},
    {3,"NONE"},
    {4,"NONE"},
    {5,"NONE"},
    {6,"NONE"},
    {7,"NONE"}
};

#endif // CONSTS_H
