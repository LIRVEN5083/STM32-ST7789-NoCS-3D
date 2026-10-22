#include "controller.hpp"

uint32_t ADC_ReadChannel(uint32_t Channel) {
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, Channel);
    
    LL_ADC_SetChannelSamplingTime(ADC1, Channel, LL_ADC_SAMPLINGTIME_56CYCLES);

    LL_ADC_ClearFlag_EOCS(ADC1);
    
    LL_ADC_REG_StartConversionSWStart(ADC1);
    
    uint32_t timeout = 10000;
    while (!LL_ADC_IsActiveFlag_EOCS(ADC1) && (--timeout > 0));

    return LL_ADC_REG_ReadConversionData12(ADC1);
}