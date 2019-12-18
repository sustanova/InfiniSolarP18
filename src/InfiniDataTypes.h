#ifndef INFINI_DATA_TYPES_H
#define INFINI_DATA_TYPES_H

#include "InfiniCommon.h"

namespace INFI {

  enum MPPT_CHARGER_STATUS { Abnormal, NormalNotCharged, Charging };
  const char MpptChargerStatus[3][24] = {
    "Abnormal", "NormalNotCharged", "Charging"
  };
  
  struct GeneralStatus {
    float gridVolt;
    float gridFreq;
    float acOutVolt;
    float acOutFreq;
    unsigned int acOutApparentPow;
    unsigned int acOutActivePow;
    BYTE outLoadPct;
    float battVolt;
    float battVoltSCC;
    float battVoltSCC2;
    unsigned int battDischargeCurr;
    unsigned int battChargeCurr;
    BYTE battCapacity;
    BYTE invHeatSinkTemp;
    BYTE mppt1ChrgrTemp;
    BYTE mppt2ChrgrTemp;
    WORD pv1InPow;
    WORD pv2InPow;
    float pv1InVolt;
    float pv2InVolt;
    bool settingsChanged; // setting value configuration state
    BYTE mppt1ChrgrStatus;
    BYTE mppt2ChrgrStatus;
    bool loadConnection;
    BYTE battPowDir;
    BYTE dcACPowDir;
    BYTE linePowDir;
    BYTE localParallelId;
  };
  
}

#endif
