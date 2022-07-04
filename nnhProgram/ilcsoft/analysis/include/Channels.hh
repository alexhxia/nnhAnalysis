/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#pragma once

#include <map>

#include <TColor.h> // https://root.cern.ch/doc/master/classTColor.html

enum Channel {
    SIGNAL,             // signal
    OTHER_H_WW,         // ?
    OTHER_H_NOT_WW,     // ?
    OTHER_H_BB,         // ?
    OTHER_H_NOT_BB,     // ?
    F2_L,               // 2 leptonic fermions  
    F2_H,               // 2 hadronic fermions  
    F4_L,               // 4 leptonic fermions  
    F4_H,               // 4 hadronic fermions  
    F4_SL,              // 4 semi-leptonic fermions  
    BKG                 // background 
};

struct ChannelInfo {
    std::string displayName = "";
    Color_t color = 0;
};

const std::map<Channel, ChannelInfo> CHANNEL_PARAMS = 
        {{SIGNAL, {"Signal", 1}},
        {OTHER_H_WW, {"Other higgs->WW*", 401}},
        {OTHER_H_NOT_WW, {"Other higgs", 417}},
        {OTHER_H_BB, {"Other higgs->bb", 401}},
        {OTHER_H_NOT_BB, {"Other higgs", 417}},
        {F2_L, {"2 fermions leptonic", 435}},
        {F2_H, {"2 fermions hadronic", 633}},
        {F4_L, {"4 fermions leptonic", 433}},
        {F4_H, {"4 fermions hadronic", 625}},
        {F4_SL, {"4 fermions semileptonic", 591}},
        {BKG, {"Background", 63}}
};
