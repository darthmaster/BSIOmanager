#ifndef CONSTS_H
#define CONSTS_H

#include "QString"
#include <map>

std::map <unsigned short, QString> module_types {
    { 1, "SS-01" },     //0x0010 -IN- -OUT-
    { 2, "USB/RS-485"}, //0x0020 DIN1 -OUT-
    { 3, "ID-8k" },     //0x4002 DIN1 -OUT-
    { 4, "ID-16k"},     //0x6002 DIN2 -OUT-
    { 5, "OD-5k" },     //0x1502 -IN- DOUT1
    { 6, "OD-16k"},     //0x1600 DIN4 DOUT1
    { 7, "OA-4k" },     //0x3002 AIN4 AOUT4
    { 8, "IA-4k" },     //0x2002 AIN4 DOUT1*
    { 9, "IA-8k" },     //0x2800 AIN8 -OUT-
    {10, "IF-1k" },     //0x1900 AIN1 DOUT1*
    {11, "IF-3k" },     //0x1903 AIN3 DOUT1*
    {12, "D-1k"  },     //0x1A00 AIN4 AOUT2
    {13, "БУ ВНА"}      //0x1A10 AIN4 AOUT1
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
