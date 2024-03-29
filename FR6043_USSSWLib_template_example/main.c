
#include <msp430.h>
#include "ussSwLib.h"
#include "USS_userConfig.h"
#include "lcd_display.h"
#include "algorithmsSonic.h"

/*******************************************************************************
 * Enables Signal gain calibration. It is recommmended to enable this feature
 * once USS_START_CAPTURE_SEC has been configured and UPS and DNS captures have
 * a valid signal
 ******************************************************************************/
#define APPLICATION_ENABLE_SIGNAL_GAIN_CALIBRATION               false

/*******************************************************************************
 * For Single tone application using USS_Alg_AbsToF_Calculation_Option_lobeWide
 * Algorithm it  might be required to scale down binary it the algorithm when
 * the capturing a valid signal but USS_runAlgorithms is returning the following
 * error message
 * USS_message_code_algorithm_error_no_signal_detected_ups_dns_channel
 * Please refer to the library User's Guide for further detail
 ******************************************************************************/
#define APPLICATION_ENABLE_BINARY_PATTERN_SIZE_SCALING      false
#if (APPLICATION_ENABLE_BINARY_PATTERN_SIZE_SCALING == true)
#define APPLICATION_BINARY_PATTERN_SCALE_FACTOR             4
#endif

/*******************************************************************************
 *  Channel Swap configuration
 ******************************************************************************/
#define APPLICATION_ENABLE_CHANNEL_SWAP                     false
#if (APPLICATION_ENABLE_CHANNEL_SWAP == true)
#define APPLICATION_CHANNEL_SWAP_INTERVAL                   1
#endif

/*******************************************************************************
 * Abstof and DToF Offset calculation configuration
 ******************************************************************************/
#define APPLICATION_ENABLE_ABSTOF_DTOF_OFFSET_CALIBRATION               false
#if (APPLICATION_ENABLE_ABSTOF_DTOF_OFFSET_CALIBRATION == true)
#define APPLICATION_ABSTOF_DTOF_OFFSET_UPDATE_INTERVAL      255
#define APPLICATION_ABSTOF_DTOF_OFFSET_LPM_CAPTURE          true
#define APPLICATION_ABSTOF_DTOF_OFFSET_CALC_UPS_ABSTOF      true
#define APPLICATION_ABSTOF_DTOF_OFFSET_CALC_DNS_ABSTOF      false
#define APPLICATION_ABSTOF_DTOF_OFFSET_CALC_DTOF            false
#define APPLICATION_ABSTOF_REFERENCE                        (32.00e-6f)
#endif


USS_Algorithms_Results algorithms_Results;
Sonic_Results_fixed_point Sonic_Results;

static void checkCode(USS_message_code code, USS_message_code expectedCode);
static void handlePllUnlockEvent(void);
static void disableApplicationInterrupts(void);
static void enableApplicationInterrupts(void);

int main(void)
{
#if (APPLICATION_ENABLE_CHANNEL_SWAP == true)
    uint16_t appSwapInterval = APPLICATION_CHANNEL_SWAP_INTERVAL;
#endif

#if (APPLICATION_ENABLE_ABSTOF_DTOF_OFFSET_CALIBRATION == true)
    USS_dTof_absTof_offset_results abstoFDtofTestResults;
    USS_dTof_absTof_offset_test_config abstoFDtofTestConfig =
    {
         .numOfTestIterations        = APPLICATION_ABSTOF_DTOF_OFFSET_UPDATE_INTERVAL,
         .isUseLPMCapture            = APPLICATION_ABSTOF_DTOF_OFFSET_LPM_CAPTURE,
         .isCalculateUpsAbsTofOffset = APPLICATION_ABSTOF_DTOF_OFFSET_CALC_UPS_ABSTOF,
         .isCalculateDnsAbsTofOffset = APPLICATION_ABSTOF_DTOF_OFFSET_CALC_DNS_ABSTOF,
         .isCalculateDToFOffset      = APPLICATION_ABSTOF_DTOF_OFFSET_CALC_DTOF,
    };
#endif

    volatile USS_message_code code;
    USS_Algorithms_Results_fixed_point algResultsFixedPoint;
    //USS_Algorithms_Results algResults;
    USS_calibration_hspll_results testResults;
    LcdInit();

    // Register PLL unlock event
    USS_registerHSPLLInterruptCallback(USS_HSPLL_Interrupt_PLLUNLOCK,
                                       &handlePllUnlockEvent);

    code = USS_configureUltrasonicMeasurement(&gUssSWConfig);
    checkCode(code, USS_message_code_no_error);

#if((USS_ALG_ABS_TOF_COMPUTATION_MODE == USS_ALG_ABS_TOF_COMPUTATION_MODE_LOBE_WIDE) || \
    (USS_ALG_ABS_TOF_COMPUTATION_MODE == USS_ALG_ABS_TOF_COMPUTATION_MODE_HILBERT_WIDE))
    // Reference binary pattern are only needed by
    // USS_Alg_AbsToF_Calculation_Option_lobeWide and
    // USS_Alg_AbsToF_Calculation_Option_hilbertWide AbsToF computation options
    if((USS_Alg_AbsToF_Calculation_Option_lobeWide ==
            gUssSWConfig.algorithmsConfig->absToFOption)
       || (USS_Alg_AbsToF_Calculation_Option_hilbertWide ==
               gUssSWConfig.algorithmsConfig->absToFOption))
    {
#if defined(__MSP430_HAS_SAPH_A__)
        if(USS_measurement_pulse_generation_mode_multi_tone ==
                gUssSWConfig.measurementConfig->pulseConfig->pulseGenMode)
        {
            code = USS_generateMultiToneBinaryPattern(&gUssSWConfig);
            checkCode(code, USS_message_code_no_error);
        }
#endif
        if(USS_measurement_pulse_generation_mode_multi_tone !=
                gUssSWConfig.measurementConfig->pulseConfig->pulseGenMode)
        {
            code = USS_generateMonoDualToneBinaryPattern(&gUssSWConfig);
            checkCode(code, USS_message_code_no_error);
        }
    }

#if (APPLICATION_ENABLE_BINARY_PATTERN_SIZE_SCALING == true)
    gUssSWConfig.algorithmsConfig->binaryPatternLength =
            (gUssSWConfig.captureConfig->sampleSize / APPLICATION_BINARY_PATTERN_SCALE_FACTOR);
#endif

#endif


    // Application must ensure no application level interrupts occur while
    // verifying HSPLL Frequency
    disableApplicationInterrupts();

    code = USS_verifyHSPLLFrequency(&gUssSWConfig, &testResults);

    // Application can re-enable interrupts after HSPLL verification
    enableApplicationInterrupts();

    checkCode(code, USS_message_code_no_error);

   gUssSWConfig.algorithmsConfig->clockRelativeError = _IQ27div((int32_t)(testResults.actualTestCount -
   testResults.expectedResult),testResults.expectedResult);

    code = USS_initAlgorithms(&gUssSWConfig);
    checkCode(code, USS_message_code_no_error);

#if (APPLICATION_ENABLE_SIGNAL_GAIN_CALIBRATION == true)
    code = USS_calibrateSignalGain(&gUssSWConfig);
    checkCode(code, USS_message_code_Signal_Gain_Calibration_successful);
#endif

#if (APPLICATION_ENABLE_ABSTOF_DTOF_OFFSET_CALIBRATION == true)
    code = USS_calculateOffsets(&gUssSWConfig, &abstoFDtofTestResults,
                                &abstoFDtofTestConfig);
    checkCode(code, USS_message_code_no_error);

    code = USS_updateAdditionalCaptureDelay(&gUssSWConfig,
          ((abstoFDtofTestResults.upsAbsToFOffset + abstoFDtofTestResults.dnsAbsToFOffset) /2.0f) -
          APPLICATION_ABSTOF_REFERENCE);
    checkCode(code, USS_message_code_no_error);

    code = USS_updateDtoFOffset(&gUssSWConfig, (-1.0f *abstoFDtofTestResults.dToFOffset));
    checkCode(code, USS_message_code_no_error);

#endif




    while(1)
    {

      code = USS_startLowPowerUltrasonicCapture(&gUssSWConfig);
       checkCode(code, USS_message_code_no_error);

       code = USS_runAlgorithmsFixedPoint(&gUssSWConfig,&algResultsFixedPoint);
       LcdDisplayTopRow(code);
       LcdDisplayBottomRow(algResultsFixedPoint.iq44DeltaTOF/2650);
       LcdDisplayMiddleRow(algResultsFixedPoint.iq16VolumeFlowRate/1000);

      // code = USS_runAlgorithms(&gUssSWConfig,&algResults);
       //checkCode(code, USS_message_code_valid_results);




        // Generate a 1 sec LPM3 delay
        USS_generateLPMDelay(&gUssSWConfig,
                             USS_low_power_mode_option_low_power_mode_3,
                             32768);
    }
}


void handlePllUnlockEvent(void)
{
    // If USS PLL unlock event is detected rest USS Module and reconfigure
    // measurement
    USS_resetUSSModule(&gUssSWConfig, true);
}


// This is a place holder for the application to disable interrupts which might
// be triggered during HSPLL verification
void disableApplicationInterrupts(void){

}

// This is a place holder for the application to enable interrupts once
// HSPLL verification has completed
void enableApplicationInterrupts(void){

}

void checkCode(USS_message_code code, USS_message_code expectedCode)
{
    LcdDisplayTopRow(code);
    if(code != expectedCode)
    {
        // Trap code
        while(1);
    }
}
