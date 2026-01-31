from machine import I2C
from time import sleep_ms, ticks_ms
from micropython import const

########################### Adresses registres RAM VL53L0X ###############################
SYSRANGE_START                              = const(0x00)
SYSTEM_THRESH_HIGH                          = const(0x0C)
SYSTEM_THRESH_LOW                           = const(0x0E)
SYSTEM_SEQUENCE_CONFIG                      = const(0x01)
SYSTEM_RANGE_CONFIG                         = const(0x09)
SYSTEM_INTERMEASUREMENT_PERIOD              = const(0x04)
SYSTEM_INTERRUPT_CONFIG_GPIO                = const(0x0A)
GPIO_HV_MUX_ACTIVE_HIGH                     = const(0x84)
SYSTEM_INTERRUPT_CLEAR                      = const(0x0B)
RESULT_INTERRUPT_STATUS                     = const(0x13)
RESULT_RANGE_STATUS                         = const(0x14)
RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN       = const(0xBC)
RESULT_CORE_RANGING_TOTAL_EVENTS_RTN        = const(0xC0)
RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF       = const(0xD0)
RESULT_CORE_RANGING_TOTAL_EVENTS_REF        = const(0xD4)
RESULT_PEAK_SIGNAL_RATE_REF                 = const(0xB6)
ALGO_PART_TO_PART_RANGE_OFFSET_MM           = const(0x28)
I2C_SLAVE_DEVICE_ADDRESS                    = const(0x8A)
MSRC_CONFIG_CONTROL                         = const(0x60)
PRE_RANGE_CONFIG_MIN_SNR                    = const(0x27)
PRE_RANGE_CONFIG_VALID_PHASE_LOW            = const(0x56)
PRE_RANGE_CONFIG_VALID_PHASE_HIGH           = const(0x57)
PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT          = const(0x64)
FINAL_RANGE_CONFIG_MIN_SNR                  = const(0x67)
FINAL_RANGE_CONFIG_VALID_PHASE_LOW          = const(0x47)
FINAL_RANGE_CONFIG_VALID_PHASE_HIGH         = const(0x48)
FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = const(0x44)
PRE_RANGE_CONFIG_SIGMA_THRESH_HI            = const(0x61)
PRE_RANGE_CONFIG_SIGMA_THRESH_LO            = const(0x62)
PRE_RANGE_CONFIG_VCSEL_PERIOD               = const(0x50)
PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI          = const(0x51)
PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO          = const(0x52)
SYSTEM_HISTOGRAM_BIN                        = const(0x81)
HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT       = const(0x33)
HISTOGRAM_CONFIG_READOUT_CTRL               = const(0x55)
FINAL_RANGE_CONFIG_VCSEL_PERIOD             = const(0x70)
FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI        = const(0x71)
FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO        = const(0x72)
CROSSTALK_COMPENSATION_PEAK_RATE_MCPS       = const(0x20)
MSRC_CONFIG_TIMEOUT_MACROP                  = const(0x46)
SOFT_RESET_GO2_SOFT_RESET_N                 = const(0xBF)
IDENTIFICATION_MODEL_ID                     = const(0xC0)
IDENTIFICATION_REVISION_ID                  = const(0xC2)
OSC_CALIBRATE_VAL                           = const(0xF8)
GLOBAL_CONFIG_VCSEL_WIDTH                   = const(0x32)
GLOBAL_CONFIG_SPAD_ENABLES_REF_0            = const(0xB0)
GLOBAL_CONFIG_SPAD_ENABLES_REF_1            = const(0xB1)
GLOBAL_CONFIG_SPAD_ENABLES_REF_2            = const(0xB2)
GLOBAL_CONFIG_SPAD_ENABLES_REF_3            = const(0xB3)
GLOBAL_CONFIG_SPAD_ENABLES_REF_4            = const(0xB4)
GLOBAL_CONFIG_SPAD_ENABLES_REF_5            = const(0xB5)
GLOBAL_CONFIG_REF_EN_START_SELECT           = const(0xB6)
DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD         = const(0x4E)
DYNAMIC_SPAD_REF_EN_START_OFFSET            = const(0x4F)
POWER_MANAGEMENT_GO1_POWER_FORCE            = const(0x80)
VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV           = const(0x89)
ALGO_PHASECAL_LIM                           = const(0x30)
ALGO_PHASECAL_CONFIG_TIMEOUT                = const(0x30)

########################### constantes ###############################
StartOverhead      = const(1910)
EndOverhead        = const(960)
MsrcOverhead       = const(660)
TccOverhead        = const(590)
DssOverhead        = const(690)
PreRangeOverhead   = const(660)
FinalRangeOverhead = const(550)
VcselPeriodPreRange   = 0
VcselPeriodFinalRange = 1

######################################################################

class VL53L0X():

    def __init__(self, i2c, addr=0x29):

        self.addr = addr
        self.i2c = i2c
        self.io_timeout = 1000  # ms
        self.did_timeout = False
        self.enables = { 'tcc'        :False
                       , 'msrc'       :False
                       , 'dss'        :True
                       , 'pre_range'  :True
                       , 'final_range':True
                       }
        self.timeouts = { 'pre_range_vcsel_period_pclks'  :0
                        , 'final_range_vcsel_period_pclks':0
                        , 'msrc_dss_tcc_mclks'            :0
                        , 'pre_range_mclks'               :0
                        , 'final_range_mclks'             :0
                        , 'msrc_dss_tcc_us'               :0
                        , 'pre_range_us'                  :0
                        , 'final_range_us'                :0
                        }

    def  writeReg(self, RegAddr, value):
        '''Write a 8-bit register'''
        a = bytearray(1)
        a[0] = value & 0xff
        self.i2c.writeto_mem(self.addr, RegAddr, a)

    def writeReg16Bit(self, RegAddr, value):
        '''Write a 16-bit register'''
        a = bytearray(2)
        a[1] = value & 0xff
        a[0] = value >> 8
        self.i2c.writeto_mem(self.addr, RegAddr, a)

    def writeReg32Bit(self, RegAddr, value):
        '''Write a 32-bit register'''
        a = bytearray(4)
        a[3] = value & 0xff
        value = value >> 8
        a[2] = value & 0xff
        value = value >> 8
        a[1] = value & 0xff
        value = value >> 8
        a[0] = value & 0xff
        self.i2c.writeto_mem(self.addr, RegAddr, a)

    def readReg(self, RegAddr):
        '''Read an 8-bit register'''
        res = bytearray(1)
        self.i2c.readfrom_mem_into(self.addr, RegAddr, res)
        return res[0]

    def readReg16Bit(self, RegAddr):
        '''Read an 16-bit register'''
        res = bytearray(2)
        self.i2c.readfrom_mem_into(self.addr, RegAddr, res)
        return (res[0] << 8) + res[1]

    def readReg32Bit(self, RegAddr):
        '''Read an 32-bit register'''
        res = bytearray(4)
        self.i2c.readfrom_mem_into(self.addr, RegAddr, res)
        return (res[0] << 24) + (res[1] << 16) + (res[2] << 8) + res[3]

    def writeMulti(self, RegAddr, src):
        '''Write contiguous memory addresses'''
        self.i2c.writeto_mem(self.addr, RegAddr, src)

    def readMulti(self, RegAddr, buf):
        '''Read contiguous memory addresses'''
        self.i2c.readfrom_mem_into(self.addr, RegAddr, buf)

    def setAdress(self, new_addr):
        '''Change device I2C address'''
        self.writeReg( I2C_SLAVE_DEVICE_ADDRESS, new_addr & 0x7F)
        self.addr = new_addr

    def init(self, io_2v8 = True):
        '''Initialize sensor using sequence based on VL53L0X_DataInit(),
           VL53L0X_StaticInit(), and VL53L0X_PerformRefCalibration().
           This function does not perform reference SPAD calibration
           (VL53L0X_PerformRefSpadManagement()), since the API user manual says that it
           is performed by ST on the bare modules; it seems like that should work well
           enough unless a cover glass is added.
           If io_2v8 (optional) is true or not given, the sensor is configured for 2V8
           mode. '''
        if self.readReg(IDENTIFICATION_MODEL_ID) != 0xEE:
            print("init: identification failed")
            return False
        # VL53L0X_DataInit() begin
        # sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
        if io_2v8:
            self.writeReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV,
                          self.readReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01) # set bit 0
        # Set I2C standard mode
        self.writeReg(0x88, 0x00)
        self.writeReg(0x80, 0x01)
        self.writeReg(0xFF, 0x01)
        self.writeReg(0x00, 0x00)
        self.stop_variable = self.readReg(0x91)
        self.writeReg(0x00, 0x01)
        self.writeReg(0xFF, 0x00)
        self.writeReg(0x80, 0x00)

        # disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
        self.writeReg(MSRC_CONFIG_CONTROL, self.readReg(MSRC_CONFIG_CONTROL) | 0x12)
        # set final range signal rate limit to 0.25 MCPS (million counts per second)
        self.setSignalRateLimit(0.25)
        self.writeReg(SYSTEM_SEQUENCE_CONFIG, 0xFF)
        # VL53L0X_DataInit() end

        # VL53L0X_StaticInit() begin

        if not self.getSpadInfo():
            print("init: getSpadInfo failed")
            return False

        # The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
        # the API, but the same data seems to be more easily readable from
        # GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there

        ref_spad_map = bytearray(6)
        self.readMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map)

        # -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

        self.writeReg(0xFF, 0x01)
        self.writeReg(DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00)
        self.writeReg(DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C)
        self.writeReg(0xFF, 0x00)
        self.writeReg(GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4)

        first_spad_to_enable = 12 if self.type_is_aperture else 0 # 12 is the first aperture spad
        spads_enabled = 0

        for i in range(48):
            if i < first_spad_to_enable or spads_enabled == self.spad_count:
                # This bit is lower than the first one that should be enabled, or
                # (reference_spad_count) bits have already been enabled, so zero this bit
                ref_spad_map[i // 8] &= ~(1 << (i % 8))
            elif ((ref_spad_map[i // 8] >> (i % 8)) & 0x1):
                spads_enabled += 1

        self.writeMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map)

        # -- VL53L0X_set_reference_spads() end

        # -- VL53L0X_load_tuning_settings() begin
        # DefaultTuningSettings from vl53l0x_tuning.h

        self.writeReg(0xFF, 0x01)
        self.writeReg(0x00, 0x00)

        self.writeReg(0xFF, 0x00)
        self.writeReg(0x09, 0x00)
        self.writeReg(0x10, 0x00)
        self.writeReg(0x11, 0x00)

        self.writeReg(0x24, 0x01)
        self.writeReg(0x25, 0xFF)
        self.writeReg(0x75, 0x00)

        self.writeReg(0xFF, 0x01)
        self.writeReg(0x4E, 0x2C)
        self.writeReg(0x48, 0x00)
        self.writeReg(0x30, 0x20)

        self.writeReg(0xFF, 0x00)
        self.writeReg(0x30, 0x09)
        self.writeReg(0x54, 0x00)
        self.writeReg(0x31, 0x04)
        self.writeReg(0x32, 0x03)
        self.writeReg(0x40, 0x83)
        self.writeReg(0x46, 0x25)
        self.writeReg(0x60, 0x00)
        self.writeReg(0x27, 0x00)
        self.writeReg(0x50, 0x06)
        self.writeReg(0x51, 0x00)
        self.writeReg(0x52, 0x96)
        self.writeReg(0x56, 0x08)
        self.writeReg(0x57, 0x30)
        self.writeReg(0x61, 0x00)
        self.writeReg(0x62, 0x00)
        self.writeReg(0x64, 0x00)
        self.writeReg(0x65, 0x00)
        self.writeReg(0x66, 0xA0)

        self.writeReg(0xFF, 0x01)
        self.writeReg(0x22, 0x32)
        self.writeReg(0x47, 0x14)
        self.writeReg(0x49, 0xFF)
        self.writeReg(0x4A, 0x00)

        self.writeReg(0xFF, 0x00)
        self.writeReg(0x7A, 0x0A)
        self.writeReg(0x7B, 0x00)
        self.writeReg(0x78, 0x21)

        self.writeReg(0xFF, 0x01)
        self.writeReg(0x23, 0x34)
        self.writeReg(0x42, 0x00)
        self.writeReg(0x44, 0xFF)
        self.writeReg(0x45, 0x26)
        self.writeReg(0x46, 0x05)
        self.writeReg(0x40, 0x40)
        self.writeReg(0x0E, 0x06)
        self.writeReg(0x20, 0x1A)
        self.writeReg(0x43, 0x40)

        self.writeReg(0xFF, 0x00)
        self.writeReg(0x34, 0x03)
        self.writeReg(0x35, 0x44)

        self.writeReg(0xFF, 0x01)
        self.writeReg(0x31, 0x04)
        self.writeReg(0x4B, 0x09)
        self.writeReg(0x4C, 0x05)
        self.writeReg(0x4D, 0x04)

        self.writeReg(0xFF, 0x00)
        self.writeReg(0x44, 0x00)
        self.writeReg(0x45, 0x20)
        self.writeReg(0x47, 0x08)
        self.writeReg(0x48, 0x28)
        self.writeReg(0x67, 0x00)
        self.writeReg(0x70, 0x04)
        self.writeReg(0x71, 0x01)
        self.writeReg(0x72, 0xFE)
        self.writeReg(0x76, 0x00)
        self.writeReg(0x77, 0x00)

        self.writeReg(0xFF, 0x01)
        self.writeReg(0x0D, 0x01)

        self.writeReg(0xFF, 0x00)
        self.writeReg(0x80, 0x01)
        self.writeReg(0x01, 0xF8)

        self.writeReg(0xFF, 0x01)
        self.writeReg(0x8E, 0x01)
        self.writeReg(0x00, 0x01)
        self.writeReg(0xFF, 0x00)
        self.writeReg(0x80, 0x00)

        # -- VL53L0X_load_tuning_settings() end

        # "Set interrupt config to new sample ready"
        # -- VL53L0X_SetGpioConfig() begin

        self.writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04)
        self.writeReg(GPIO_HV_MUX_ACTIVE_HIGH, self.readReg(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10) # active low
        self.writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01)

        # -- VL53L0X_SetGpioConfig() end

        self.measurement_timing_budget_us = self.getMeasurementTimingBudget()

        # "Disable MSRC and TCC by default"
        # MSRC = Minimum Signal Rate Check
        # TCC = Target CentreCheck
        # -- VL53L0X_SetSequenceStepEnable() begin

        self.writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8)

        # -- VL53L0X_SetSequenceStepEnable() end

        # "Recalculate timing budget"
        self.setMeasurementTimingBudget(self.measurement_timing_budget_us)

        # VL53L0X_StaticInit() end

        # VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())

        # -- VL53L0X_perform_vhv_calibration() begin

        self.writeReg(SYSTEM_SEQUENCE_CONFIG, 0x01)
        if not self.performSingleRefCalibration(0x40):
            print("init: performSingleRefCalibration(0x40) failed")
            return False

        # -- VL53L0X_perform_vhv_calibration() end

        # -- VL53L0X_perform_phase_calibration() begin
        self.writeReg(SYSTEM_SEQUENCE_CONFIG, 0x02)
        if not self.performSingleRefCalibration(0x00):
            print("init: performSingleRefCalibration(0x00) failed")
            return False

        # -- VL53L0X_perform_phase_calibration() end

        # "restore the previous Sequence Config"
        self.writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8)

        # VL53L0X_PerformRefCalibration() end

        return True
    
    def setLongRange(self):
        '''For distance above ~ 1200 mm'''
        # save current mode (single, continuous, continuous & timed)
        mode = self.readReg(SYSRANGE_START)
        period_ms  = self.readReg32Bit(SYSTEM_INTERMEASUREMENT_PERIOD)
        # lower the return signal rate limit (default is 0.25 MCPS)
        self.setSignalRateLimit(0.1)
        # increase laser pulse periods (defaults are 14 and 10 PCLKs)
        self.setVcselPulsePeriod(VcselPeriodPreRange, 18)
        self.setVcselPulsePeriod(VcselPeriodFinalRange, 14)
        # restore current mode
        self.writeReg32Bit(SYSTEM_INTERMEASUREMENT_PERIOD, period_ms)
        self.writeReg(SYSRANGE_START, mode)
    
    def setDefaultRange(self):
        '''For distance below ~ 1200 mm'''
        # save current mode (single, continuous, continuous & timed)
        mode = self.readReg(SYSRANGE_START)
        period_ms  = self.readReg32Bit(SYSTEM_INTERMEASUREMENT_PERIOD)
        # restore the return signal rate limit to 0.25 MCPS
        self.setSignalRateLimit(0.25)
        # restore laser pulse periods to defaults
        self.setVcselPulsePeriod(VcselPeriodPreRange, 14)
        self.setVcselPulsePeriod(VcselPeriodFinalRange, 10)
        # restore current mode
        self.writeReg32Bit(SYSTEM_INTERMEASUREMENT_PERIOD, period_ms)
        self.writeReg(SYSRANGE_START, mode)
    
    def setHighSpeed(self):
        '''Reduce timing budget to minimum'''
        # reduce timing budget to 20 ms (default is about 33 ms)
        self.setMeasurementTimingBudget(20000)
    
    def setHighAccuracy(self):
        '''Increase ranging accuracy'''
        # increase timing budget to 200 ms
        self.setMeasurementTimingBudget(200000)

    def setSignalRateLimit(self, limit_Mcps):
        '''Set the return signal rate limit check value in units of MCPS (mega counts
           per second). "This represents the amplitude of the signal reflected from the
           target and detected by the device"; setting this limit presumably determines
           the minimum measurement necessary for the sensor to report a valid reading.
           Setting a lower limit increases the potential range of the sensor but also
           seems to increase the likelihood of getting an inaccurate reading because of
           unwanted reflections from objects other than the intended target.
           Defaults to 0.25 MCPS as initialized by the ST API and this library.'''

        if limit_Mcps < 0 or limit_Mcps > 511.99:
            return False
        # Q9.7 fixed point format (9 integer bits, 7 fractional bits)
        self.writeReg16Bit(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, int(limit_Mcps * (1 << 7)))
        return True

    def getSignalRateLimit(self):
        '''Get the return signal rate limit check value in MCPS'''
        return self.readReg16Bit(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT) / (1 << 7)

    def setMeasurementTimingBudget(self, budget_us):
        '''Set the measurement timing budget in microseconds, which is the time allowed
           for one measurement; the ST API and this library take care of splitting the
           timing budget among the sub-steps in the ranging sequence. A longer timing
           budget allows for more accurate measurements. Increasing the budget by a
           factor of N decreases the range measurement standard deviation by a factor of
           sqrt(N). Defaults to about 33 milliseconds; the minimum is 20 ms.
           Based on VL53L0X_set_measurement_timing_budget_micro_seconds()'''
        
        used_budget_us = StartOverhead + EndOverhead

        self.getSequenceStepEnables()
        self.getSequenceStepTimeouts()

        if self.enables['tcc']:
            used_budget_us += (self.timeouts['msrc_dss_tcc_us'] + TccOverhead)
        if self.enables['dss']:
            used_budget_us += 2 * (self.timeouts['msrc_dss_tcc_us'] + DssOverhead)
        elif self.enables['msrc']:
            used_budget_us += (self.timeouts['msrc_dss_tcc_us'] + MsrcOverhead)
        if self.enables['pre_range']:
            used_budget_us += (self.timeouts['pre_range_us'] + PreRangeOverhead)
        if self.enables['final_range']:
            used_budget_us += FinalRangeOverhead

        # "Note that the final range timeout is determined by the timing
        # budget and the sum of all other timeouts within the sequence.
        # If there is no room for the final range timeout, then an error
        # will be set. Otherwise the remaining time will be applied to
        # the final range."

        if used_budget_us > budget_us:
            # "Requested timeout too big."
            return False

        final_range_timeout_us = budget_us - used_budget_us

        # set_sequence_step_timeout() begin
        # (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)
        # "For the final range timeout, the pre-range timeout
        #  must be added. To do this both final and pre-range
        #  timeouts must be expressed in macro periods MClks
        #  because they have different vcsel periods."

        final_range_timeout_mclks = self.timeoutMicrosecondsToMclks(final_range_timeout_us,
                                                        self.timeouts['final_range_vcsel_period_pclks'])

        if self.enables['pre_range']:
            final_range_timeout_mclks += self.timeouts['pre_range_mclks']

        self.writeReg16Bit(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI, self.encodeTimeout(final_range_timeout_mclks))

        # set_sequence_step_timeout() end

        self.measurement_timing_budget_us = budget_us   # store for internal reuse

        return True

    def getMeasurementTimingBudget(self):
        '''Get the measurement timing budget in microseconds
           Based on VL53L0X_get_measurement_timing_budget_micro_seconds()
           in us'''
        
        budget_us = StartOverhead + EndOverhead

        self.getSequenceStepEnables()
        self.getSequenceStepTimeouts()

        if self.enables['tcc']:
            budget_us += (self.timeouts['msrc_dss_tcc_us'] + TccOverhead)
        if self.enables['dss']:
            budget_us += 2 * (self.timeouts['msrc_dss_tcc_us'] + DssOverhead)
        elif self.enables['msrc']:
            budget_us += (self.timeouts['msrc_dss_tcc_us'] + MsrcOverhead)
        if self.enables['pre_range']:
            budget_us += (self.timeouts['pre_range_us'] + PreRangeOverhead)
        if self.enables['final_range']:
            budget_us += (self.timeouts['final_range_us'] + FinalRangeOverhead)

        self.measurement_timing_budget_us = budget_us  # store for internal reuse

        return budget_us

    def setVcselPulsePeriod(self, type, period_pclks):
        '''Set the VCSEL (vertical cavity surface emitting laser) pulse period for the
           given period type (pre-range or final range) to the given value in PCLKs.
           Longer periods seem to increase the potential range of the sensor.
           Valid values are (even numbers only):
             pre:  12 to 18 (initialized default: 14)
             final: 8 to 14 (initialized default: 10)
           Based on VL53L0X_set_vcsel_pulse_period()'''

        vcsel_period_reg = self.encodeVcselPeriod(period_pclks)

        self.getSequenceStepEnables()
        self.getSequenceStepTimeouts()

        # "Apply specific settings for the requested clock period"
        # "Re-calculate and apply timeouts, in macro periods"

        # "When the VCSEL period for the pre or final range is changed,
        # the corresponding timeout must be read from the device using
        # the current VCSEL period, then the new VCSEL period can be
        # applied. The timeout then must be written back to the device
        # using the new VCSEL period.
        #
        # For the MSRC timeout, the same applies - this timeout being
        # dependant on the pre-range vcsel period."

        if type == VcselPeriodPreRange:
            # "Set phase check limits"
            if period_pclks == 12:
                self.writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x18)
            elif period_pclks == 14:
                self.writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x30)
            elif period_pclks == 16:
                self.writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x40)
            elif period_pclks == 18:
                self.writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x50)
            else:   # invalid period
                return False

            self.writeReg(PRE_RANGE_CONFIG_VALID_PHASE_LOW, 0x08)

            # apply new VCSEL period
            self.writeReg(PRE_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg)
            # update timeouts
            # set_sequence_step_timeout() begin
            # (SequenceStepId == VL53L0X_SEQUENCESTEP_PRE_RANGE)
            new_pre_range_timeout_mclks = self.timeoutMicrosecondsToMclks(
                                                self.timeouts['pre_range_us'], period_pclks)
            self.writeReg16Bit(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI, self.encodeTimeout(new_pre_range_timeout_mclks))

            # set_sequence_step_timeout() end
            # set_sequence_step_timeout() begin
            # (SequenceStepId == VL53L0X_SEQUENCESTEP_MSRC)

            new_msrc_timeout_mclks = self.timeoutMicrosecondsToMclks(
                                         self.timeouts['msrc_dss_tcc_us'], period_pclks)

            self.writeReg(MSRC_CONFIG_TIMEOUT_MACROP,
                      255 if new_msrc_timeout_mclks > 256 else new_msrc_timeout_mclks - 1)

            # set_sequence_step_timeout() end
        elif type == VcselPeriodFinalRange:
            if period_pclks == 8:
                self.writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x10)
                self.writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08)
                self.writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x02)
                self.writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C)
                self.writeReg(0xFF, 0x01)
                self.writeReg(ALGO_PHASECAL_LIM, 0x30)
                self.writeReg(0xFF, 0x00)
            elif period_pclks == 10:
                self.writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x28)
                self.writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08)
                self.writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03)
                self.writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09)
                self.writeReg(0xFF, 0x01)
                self.writeReg(ALGO_PHASECAL_LIM, 0x20)
                self.writeReg(0xFF, 0x00)
            elif period_pclks == 12:
                self.writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38)
                self.writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08)
                self.writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03)
                self.writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08)
                self.writeReg(0xFF, 0x01)
                self.writeReg(ALGO_PHASECAL_LIM, 0x20)
                self.writeReg(0xFF, 0x00)
            elif period_pclks == 14:
                self.writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x48)
                self.writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08)
                self.writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03)
                self.writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07)
                self.writeReg(0xFF, 0x01)
                self.writeReg(ALGO_PHASECAL_LIM, 0x20)
                self.writeReg(0xFF, 0x00)
            else:
                return False   # invalid period

            # apply new VCSEL period
            self.writeReg(FINAL_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg)

            # update timeouts

            # set_sequence_step_timeout() begin
            # (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

            # "For the final range timeout, the pre-range timeout
            #  must be added. To do this both final and pre-range
            #  timeouts must be exprwriteReg(SYSTEM_INTERRUPT_CLEAR, 0x01)essed in macro periods MClks
            #  because they have different vcsel periods."
            new_final_range_timeout_mclks = self.timeoutMicrosecondsToMclks(
                                               self.timeouts['final_range_us'], period_pclks)


            if self.enables['pre_range']:
                new_final_range_timeout_mclks += self.timeouts['pre_range_mclks']

            self.writeReg16Bit(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
                               self.encodeTimeout(new_final_range_timeout_mclks))

            # set_sequence_step_timeout end
        else:
            return False

        # "Finally, the timing budget must be re-applied"

        self.setMeasurementTimingBudget(self.measurement_timing_budget_us)

        # "Perform the phase calibration. This is needed after changing on vcsel period."
        # VL53L0X_perform_phase_calibration() begin

        sequence_config = self.readReg(SYSTEM_SEQUENCE_CONFIG)
        self.writeReg(SYSTEM_SEQUENCE_CONFIG, 0x02)
        self.performSingleRefCalibration(0x0)
        self.writeReg(SYSTEM_SEQUENCE_CONFIG, sequence_config)

        # VL53L0X_perform_phase_calibration() end

        return True

    def getVcselPulsePeriod(self, type):
        '''Get the VCSEL pulse period in PCLKs for the given period type.
           Based on VL53L0X_get_vcsel_pulse_period()'''

        if type == VcselPeriodPreRange:
            return self.decodeVcselPeriod(self.readReg(PRE_RANGE_CONFIG_VCSEL_PERIOD))
        elif type == VcselPeriodFinalRange:
            return self.decodeVcselPeriod(self.readReg(FINAL_RANGE_CONFIG_VCSEL_PERIOD))
        else:
            return 255

    def startContinuous(self, period_ms):
        '''Start continuous ranging measurements. If period_ms (optional) is 0 or not
           given, continuous back-to-back mode is used (the sensor takes measurements as
           often as possible); otherwise, continuous timed mode is used, with the given
           inter-measurement period in milliseconds determining how often the sensor
           takes a measurement.
           Based on VL53L0X_StartMeasurement()'''

        self.writeReg(0x80, 0x01)
        self.writeReg(0xFF, 0x01)
        self.writeReg(0x00, 0x00)
        self.writeReg(0x91, self.stop_variable)
        self.writeReg(0x00, 0x01)
        self.writeReg(0xFF, 0x00)
        self.writeReg(0x80, 0x00)

        if period_ms != 0:
            # continuous timed mode
            # VL53L0X_SetInterMeasurementPeriodMilliSeconds() begin
            osc_calibrate_val = self.readReg16Bit(OSC_CALIBRATE_VAL)

            if osc_calibrate_val != 0:
                period_ms *= osc_calibrate_val

            self.writeReg32Bit(SYSTEM_INTERMEASUREMENT_PERIOD, period_ms)

            # VL53L0X_SetInterMeasurementPeriodMilliSeconds() end

            self.writeReg(SYSRANGE_START, 0x04) # VL53L0X_REG_SYSRANGE_MODE_TIMED

        else:
            # continuous back-to-back mode
            self.writeReg(SYSRANGE_START, 0x02) # VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK

    def stopContinuous(self):
        '''Stop continuous measurements
           Based on VL53L0X_StopMeasurement()'''
        self.writeReg(SYSRANGE_START, 0x01) # VL53L0X_REG_SYSRANGE_MODE_SINGLESHOT

        self.writeReg(0xFF, 0x01)
        self.writeReg(0x00, 0x00)
        self.writeReg(0x91, 0x00)
        self.writeReg(0x00, 0x01)
        self.writeReg(0xFF, 0x00)

    def readRangeContinuousMillimeters(self):
        '''Returns a range reading in millimeters when continuous mode is active
           (readRangeSingleMillimeters() also calls this function after starting a
           single-shot range measurement)'''

        self.startTimeout()
        while (self.readReg(RESULT_INTERRUPT_STATUS) & 0x07) == 0:
            if self.checkTimeoutExpired():
                self.did_timeout = True
                return 65535
        # assumptions: Linearity Corrective Gain is 1000 (default);
        # fractional ranging is not enabled
        range = self.readReg16Bit(RESULT_RANGE_STATUS + 10)
        self.writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01)

        return range

    def readRangeSingleMillimeters(self):
        '''Performs a single-shot range measurement and returns the reading in
           millimeters
           Based on VL53L0X_PerformSingleRangingMeasurement()'''

        self.writeReg(0x80, 0x01)
        self.writeReg(0xFF, 0x01)
        self.writeReg(0x00, 0x00)
        self.writeReg(0x91, self.stop_variable)
        self.writeReg(0x00, 0x01)
        self.writeReg(0xFF, 0x00)
        self.writeReg(0x80, 0x00)
        self.writeReg(SYSRANGE_START, 0x01)

        # "Wait until start bit has been cleared"
        self.startTimeout()
        while self.readReg(SYSRANGE_START) & 0x01:
            if self.checkTimeoutExpired():
                self.did_timeout = True
                return 65535

        return self.readRangeContinuousMillimeters()
    
    def getRangeStatus(self):
        '''Return last range status'''
        st = (self.readReg(RESULT_RANGE_STATUS) & 0x78) >> 3
        if st in (1, 2, 3):
            status = 5
        elif st in (6, 9):
            status = 4
        elif st in (8, 10):
            status = 3
        elif st == 4:
            status = 2
        else:
            status = 0
        return status
    
    def getRangeStatusString(self):
        '''Return last range status as a string'''
        status_string = {
                          0 : "Range Valid"
                        , 2 : "Signal Fail"
                        , 3 : "Min range Fail"
                        , 4 : "Phase Fail"
                        , 5 : "Hardware Fail"
                        }
        status = self.getRangeStatus()
        return status_string[status]

    def timeoutOccurred(self):
        '''Did a timeout occur in one of the read functions since the last call to
           timeoutOccurred()?'''
        tmp = self.did_timeout
        self.did_timeout = False
        return tmp

    def getSpadInfo(self):
        '''Get reference SPAD (single photon avalanche diode) count and type
           Based on VL53L0X_get_info_from_device(),
           but only gets reference SPAD count and type'''

        self.writeReg(0x80, 0x01)
        self.writeReg(0xFF, 0x01)
        self.writeReg(0x00, 0x00)
        self.writeReg(0xFF, 0x06)
        self.writeReg(0x83, self.readReg(0x83) | 0x04)
        self.writeReg(0xFF, 0x07)
        self.writeReg(0x81, 0x01)
        self.writeReg(0x80, 0x01)
        self.writeReg(0x94, 0x6b)
        self.writeReg(0x83, 0x00)

        self.startTimeout()
        while self.readReg(0x83) == 0x00:
            if self.checkTimeoutExpired():
                return False
        self.writeReg(0x83, 0x01)
        tmp = self.readReg(0x92)

        self.spad_count = tmp & 0x7f
        self.type_is_aperture = (tmp >> 7) & 0x01

        self.writeReg(0x81, 0x00)
        self.writeReg(0xFF, 0x06)
        self.writeReg(0x83, self.readReg(0x83)  & ~0x04)
        self.writeReg(0xFF, 0x01)
        self.writeReg(0x00, 0x01)
        self.writeReg(0xFF, 0x00)
        self.writeReg(0x80, 0x00)

        return True

    def getSequenceStepEnables(self):
        '''Get sequence step enables
           Based on VL53L0X_GetSequenceStepEnables()'''

        sequence_config = self.readReg(SYSTEM_SEQUENCE_CONFIG)

        self.enables['tcc']         = (sequence_config >> 4) & 0x1
        self.enables['dss']         = (sequence_config >> 3) & 0x1
        self.enables['msrc']        = (sequence_config >> 2) & 0x1
        self.enables['pre_range']   = (sequence_config >> 6) & 0x1
        self.enables['final_range'] = (sequence_config >> 7) & 0x1

    def getSequenceStepTimeouts(self):
        '''Get sequence step timeouts
           Based on get_sequence_step_timeout(),
           but gets all timeouts instead of just the requested one, and also stores
           intermediate values'''

        self.timeouts['pre_range_vcsel_period_pclks'] = self.getVcselPulsePeriod(VcselPeriodPreRange)
        self.timeouts['msrc_dss_tcc_mclks']           = self.readReg(MSRC_CONFIG_TIMEOUT_MACROP) + 1
        self.timeouts['msrc_dss_tcc_us'] = self.timeoutMclksToMicroseconds(
                                                self.timeouts['msrc_dss_tcc_mclks'],
                                                self.timeouts['pre_range_vcsel_period_pclks'])
        self.timeouts['pre_range_mclks'] = self.decodeTimeout(self.readReg16Bit(
                                                     PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI))
        self.timeouts['pre_range_us'] = self.timeoutMclksToMicroseconds(
                                               self.timeouts['pre_range_mclks'],
                                               self.timeouts['pre_range_vcsel_period_pclks'])
        self.timeouts['final_range_vcsel_period_pclks'] = self.getVcselPulsePeriod(VcselPeriodFinalRange)
        self.timeouts['final_range_mclks'] = self.decodeTimeout(
                                                 self.readReg16Bit(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI))

        if self.enables['pre_range']:
            self.timeouts['final_range_mclks'] -= self.timeouts['pre_range_mclks']

        self.timeouts['final_range_us'] =  self.timeoutMclksToMicroseconds(
                                                      self.timeouts['final_range_mclks'],
                                                      self.timeouts['final_range_vcsel_period_pclks'])

    def decodeTimeout(self, reg_val):
        '''Decode sequence step timeout in MCLKs from register value
           Based on VL53L0X_decode_timeout()
           Note: the original function returned a uint32_t, but the return value is
           always stored in a uint16_t.'''
        # format: "(LSByte * 2^MSByte) + 1"

        return ((reg_val & 0x00FF) << ((reg_val & 0xFF00) >> 8)) + 1

    def encodeTimeout(self, timeout_mclks):
        '''Encode sequence step timeout register value from timeout in MCLKs
           Based on VL53L0X_encode_timeout()'''
        # format: "(LSByte * 2^MSByte) + 1"

        ls_byte = 0
        ms_byte = 0

        if timeout_mclks > 0:
            ls_byte = timeout_mclks - 1

            while (ls_byte & 0xFFFFFF00) > 0:
                ls_byte >>= 1
                ms_byte += 1

            return (ms_byte << 8) | (ls_byte & 0xFF)
        else:
            return 0

    def timeoutMclksToMicroseconds(self, timeout_period_mclks, vcsel_period_pclks):
        '''Convert sequence step timeout from MCLKs to microseconds with given VCSEL period in PCLKs
           Based on VL53L0X_calc_timeout_us()'''

        macro_period_ns = self.calcMacroPeriod(vcsel_period_pclks)
        return int(((timeout_period_mclks * macro_period_ns) + 500) / 1000)


    def timeoutMicrosecondsToMclks(self, timeout_period_us, vcsel_period_pclks):
        '''Convert sequence step timeout from microseconds to MCLKs with given VCSEL period in PCLKs
           Based on VL53L0X_calc_timeout_mclks()'''

        macro_period_ns = self.calcMacroPeriod(vcsel_period_pclks)
        return int(((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns)

    def performSingleRefCalibration(self, vhv_init_byte):
        '''Based on VL53L0X_perform_single_ref_calibration()'''

        self.writeReg(SYSRANGE_START, 0x01 | vhv_init_byte)  # VL53L0X_REG_SYSRANGE_MODE_START_STOP
        self.startTimeout()

        while (self.readReg(RESULT_INTERRUPT_STATUS) & 0x07) == 0:
            if self.checkTimeoutExpired():
                print("performSingleRefCalibration ", ticks_ms() - self.timeout_start_ms, "ms")
                return False
        self.writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01)
        self.writeReg(SYSRANGE_START, 0x00)

        return True

    def startTimeout(self):
        self.timeout_start_ms = ticks_ms()

    def checkTimeoutExpired(self):
        return (self.io_timeout > 0) & ((ticks_ms() - self.timeout_start_ms) > self.io_timeout)

    def decodeVcselPeriod(self, reg_val):
        return ((reg_val) + 1) << 1

    def encodeVcselPeriod(self, period_pclks):
        return ((period_pclks) >> 1) - 1

    def calcMacroPeriod(self, vcsel_period_pclks):
        return ((2304 * (vcsel_period_pclks) * 1655) + 500) / 1000
    
    def setTimeout(self, timeout):
        self.io_timeout = timeout
    
    def getTimeout(self):
        return self.io_timeout