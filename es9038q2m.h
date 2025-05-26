/* SPDX-License-Identifier: GPL-2.0 
 *
 * ALSA SoC codec driver for ESS ES9038Q2M
 *
 * Copyright 2024 mahaudio
 *
 * Author: Moreno Hassem <moreno.hassem@mahaudio.com.br>
 * 
 */

#ifndef __ES9038Q2M_H__
#define __ES9038Q2M_H__

/* Device I2C addresses depending on ADDR pin */
#define ES9038Q2M_I2C_ADDR_LOW   0x48
#define ES9038Q2M_I2C_ADDR_HIGH  0x49

/* Register Map */
#define ES9038Q2M_REG_SYSTEM             0x00
#define ES9038Q2M_REG_INPUT_SEL          0x01
#define ES9038Q2M_REG_MIXING             0x02
#define ES9038Q2M_REG_SPDIF_CFG          0x03
#define ES9038Q2M_REG_AMUTE_TIME         0x04
#define ES9038Q2M_REG_AMUTE_LEVEL        0x05
#define ES9038Q2M_REG_DEEMP_VOLRAMP      0x06
#define ES9038Q2M_REG_FILTER_SHAPE       0x07
#define ES9038Q2M_REG_GPIO_CFG           0x08
#define ES9038Q2M_REG_RESERVED_09        0x09
#define ES9038Q2M_REG_MASTER_MODE        0x0A
#define ES9038Q2M_REG_SPDIF_SELECT       0x0B
#define ES9038Q2M_REG_DPLL_BW            0x0C
#define ES9038Q2M_REG_THD_BYPASS         0x0D
#define ES9038Q2M_REG_SOFT_START         0x0E
#define ES9038Q2M_REG_VOL_CH1            0x0F
#define ES9038Q2M_REG_VOL_CH2            0x10
#define ES9038Q2M_REG_MTRIM_0            0x11
#define ES9038Q2M_REG_MTRIM_1            0x12
#define ES9038Q2M_REG_MTRIM_2            0x13
#define ES9038Q2M_REG_MTRIM_3            0x14
#define ES9038Q2M_REG_GPIO_IN_SEL        0x15
#define ES9038Q2M_REG_THD_C2_0           0x16
#define ES9038Q2M_REG_THD_C2_1           0x17
#define ES9038Q2M_REG_THD_C3_0           0x18
#define ES9038Q2M_REG_THD_C3_1           0x19
#define ES9038Q2M_REG_RESERVED_1A        0x1A
#define ES9038Q2M_REG_GEN_CFG            0x1B
#define ES9038Q2M_REG_RESERVED_1C        0x1C
#define ES9038Q2M_REG_GPIO_INV           0x1D
#define ES9038Q2M_REG_CP_CLK_0           0x1E
#define ES9038Q2M_REG_CP_CLK_1           0x1F
#define ES9038Q2M_REG_RESERVED_20        0x20
#define ES9038Q2M_REG_INTR_MASK          0x21
#define ES9038Q2M_REG_NCO_0              0x22
#define ES9038Q2M_REG_NCO_1              0x23
#define ES9038Q2M_REG_NCO_2              0x24
#define ES9038Q2M_REG_NCO_3              0x25
#define ES9038Q2M_REG_RESERVED_26        0x26
#define ES9038Q2M_REG_GEN_CFG_2          0x27
#define ES9038Q2M_REG_FIR_ADDR           0x28
#define ES9038Q2M_REG_FIR_DATA_0         0x29
#define ES9038Q2M_REG_FIR_DATA_1         0x2A
#define ES9038Q2M_REG_FIR_DATA_2         0x2B
#define ES9038Q2M_REG_FIR_CONFIG         0x2C
#define ES9038Q2M_REG_LOW_PWR_CALIB      0x2D
#define ES9038Q2M_REG_ADC_CONFIG         0x2E

/* ADC filter config */
#define ES9038Q2M_REG_ADC_FTR_SCALE_0    0x2F
#define ES9038Q2M_REG_ADC_FTR_SCALE_1    0x30
#define ES9038Q2M_REG_ADC_FBQ1_0         0x31
#define ES9038Q2M_REG_ADC_FBQ1_1         0x32
#define ES9038Q2M_REG_ADC_FBQ2_0         0x33
#define ES9038Q2M_REG_ADC_FBQ2_1         0x34

/* Read-only registers */
#define ES9038Q2M_REG_CHIP_ID            0x40
#define ES9038Q2M_REG_GPIO_READBACK      0x41
#define ES9038Q2M_REG_DPLL_NUM_0         0x42
#define ES9038Q2M_REG_DPLL_NUM_1         0x43
#define ES9038Q2M_REG_DPLL_NUM_2         0x44
#define ES9038Q2M_REG_DPLL_NUM_3         0x45
#define ES9038Q2M_REG_SPDIF_STATUS_BASE  0x46  /* up to 0x5F */
#define ES9038Q2M_REG_INPUT_STATUS       0x60
#define ES9038Q2M_REG_ADC_READBACK_0     0x64
#define ES9038Q2M_REG_ADC_READBACK_1     0x65
#define ES9038Q2M_REG_ADC_READBACK_2     0x66

#define ES9038Q2M_NUM_REGISTERS          0x67

/* Register bit definitions */

/* =========================
 * REG_SYSTEM (0x00)
 * ========================= */
#define ES9038Q2M_OSC_DRV_FULL_BIAS      0x00  /* 0000 << 4 */
#define ES9038Q2M_OSC_DRV_3_4_BIAS       0x80  /* 1000 << 4 */
#define ES9038Q2M_OSC_DRV_HALF_BIAS      0xC0  /* 1100 << 4 */
#define ES9038Q2M_OSC_DRV_1_4_BIAS       0xE0  /* 1110 << 4 */
#define ES9038Q2M_OSC_DRV_SHUTDOWN       0xF0  /* 1111 << 4 */

#define ES9038Q2M_CLK_GEAR_DIV1          0x00  /* 00 << 2 */
#define ES9038Q2M_CLK_GEAR_DIV2          0x04  /* 01 << 2 */
#define ES9038Q2M_CLK_GEAR_DIV4          0x08  /* 10 << 2 */
#define ES9038Q2M_CLK_GEAR_DIV8          0x0C  /* 11 << 2 */

#define ES9038Q2M_SOFT_RESET             0x01  /* Bit 0 */

/* =========================
 * REG_INPUT_SEL (0x01)
 * ========================= */
#define ES9038Q2M_SERIAL_LEN_MASK        0xC0
#define ES9038Q2M_SERIAL_LEN_16BIT       0x00
#define ES9038Q2M_SERIAL_LEN_24BIT       0x40
#define ES9038Q2M_SERIAL_LEN_32BIT_2     0x80
#define ES9038Q2M_SERIAL_LEN_32BIT       0xC0

#define ES9038Q2M_SERIAL_MODE_MASK       0x30
#define ES9038Q2M_SERIAL_MODE_I2S        0x00
#define ES9038Q2M_SERIAL_MODE_LJ         0x10
#define ES9038Q2M_SERIAL_MODE_RJ         0x20
#define ES9038Q2M_SERIAL_MODE_RJ2        0x30

#define ES9038Q2M_AUTOSEL_MASK           0x0C
#define ES9038Q2M_AUTOSEL_DISABLED       0x00
#define ES9038Q2M_AUTOSEL_DSD_SERIAL     0x04
#define ES9038Q2M_AUTOSEL_SPDIF_SERIAL   0x08
#define ES9038Q2M_AUTOSEL_ALL            0x0C

#define ES9038Q2M_INPUT_SEL_SERIAL       0x00
#define ES9038Q2M_INPUT_SEL_SPDIF        0x01
#define ES9038Q2M_INPUT_SEL_RESERVED     0x02
#define ES9038Q2M_INPUT_SEL_DSD          0x03

/* =========================
 * REG_MIXING (0x02)
 * ========================= */
#define ES9038Q2M_AUTOMUTE_NORMAL        0x00
#define ES9038Q2M_AUTOMUTE_MUTE          0x40
#define ES9038Q2M_AUTOMUTE_GND           0x80
#define ES9038Q2M_AUTOMUTE_MUTE_GND      0xC0

#define ES9038Q2M_CH2_MIX_CH1            0x00
#define ES9038Q2M_CH2_MIX_CH2            0x04

#define ES9038Q2M_CH1_MIX_CH1            0x00
#define ES9038Q2M_CH1_MIX_CH2            0x01

/* =========================
 * REG_SPDIF_CFG (0x03)
 * ========================= */
#define ES9038Q2M_SPDIF_USER_BITS        0x08
#define ES9038Q2M_SPDIF_IGNORE_DATA      0x04
#define ES9038Q2M_SPDIF_IGNORE_VALID     0x02

/* =========================
 * REG_AMUTE_TIME (0x04)
 * ========================= */
// Full byte used, 0 disables automute
// Time = 2096896 * AMUTE_TIME / FSR (s)

/* =========================
 * REG_AMUTE_LEVEL (0x05)
 * ========================= */
// Bits [6:0] set level in dB, default 104 = -104dB

/* =========================
 * REG_DEEMP_VOLRAMP (0x06)
 * ========================= */
#define ES9038Q2M_AUTO_DEEMPH            0x80
#define ES9038Q2M_DEEMPH_BYPASS          0x40

#define ES9038Q2M_DEEMPH_32KHZ           0x00
#define ES9038Q2M_DEEMPH_44KHZ           0x10
#define ES9038Q2M_DEEMPH_48KHZ           0x20

#define ES9038Q2M_DOP_ENABLE             0x08

/* Volume Ramp Rate (bits [2:0]): rate = (2^x * FSR) / 512 dB/s */
#define ES9038Q2M_VOLRAMP_RATE_0         0x00
#define ES9038Q2M_VOLRAMP_RATE_1         0x01
#define ES9038Q2M_VOLRAMP_RATE_2         0x02
#define ES9038Q2M_VOLRAMP_RATE_3         0x03
#define ES9038Q2M_VOLRAMP_RATE_4         0x04
#define ES9038Q2M_VOLRAMP_RATE_5         0x05
#define ES9038Q2M_VOLRAMP_RATE_6         0x06
#define ES9038Q2M_VOLRAMP_RATE_7         0x07

/* =========================
 * REG_FILTER_SHAPE (0x07)
 * ========================= */
#define ES9038Q2M_FILTER_SHAPE_FAST_LIN      0x00
#define ES9038Q2M_FILTER_SHAPE_SLOW_LIN      0x20
#define ES9038Q2M_FILTER_SHAPE_FAST_MIN      0x40
#define ES9038Q2M_FILTER_SHAPE_SLOW_MIN      0x60
#define ES9038Q2M_FILTER_SHAPE_APOD_FAST     0x80  /* Default */
#define ES9038Q2M_FILTER_SHAPE_RESERVED_1    0xA0
#define ES9038Q2M_FILTER_SHAPE_CORR_MIN_FAST 0xC0
#define ES9038Q2M_FILTER_SHAPE_BRICK_WALL    0xE0

#define ES9038Q2M_BYPASS_OSF             0x08
#define ES9038Q2M_MUTE                   0x01

/* =========================
 * REG_GPIO_CFG (0x08)
 * ========================= */
// gpio1_cfg = bits [3:0], gpio2_cfg = bits [7:4]
// Use values 0–15 for function, per datasheet table

/* =========================
 * REG_MASTER_MODE (0x0A)
 * ========================= */
#define ES9038Q2M_MASTER_MODE_MASK       0x80
#define ES9038Q2M_MASTER_MODE_ENABLE     0x80

#define ES9038Q2M_MASTER_DIV_2           0x00
#define ES9038Q2M_MASTER_DIV_4           0x20
#define ES9038Q2M_MASTER_DIV_8           0x40
#define ES9038Q2M_MASTER_DIV_16          0x60

#define ES9038Q2M_128FS_MODE_ENABLE      0x10

// Lock speed [3:0] from 0–15: see datasheet for details

/* =========================
 * REG_SPDIF_SELECT (0x0B)
 * ========================= */
// bits [7:4] selects DATA_CLK, DATA1, DATA2, GPIO1, GPIO2

/* =========================
 * REG_DPLL_BW (0x0C)
 * ========================= */
// [7:4] = serial mode BW (0 = off, 1 = narrow, ..., 15 = wide)
// [3:0] = DSD mode BW (idem)

/* =========================
 * REG_THD_BYPASS (0x0D)
 * ========================= */
#define ES9038Q2M_THD_ENABLE             0x00
#define ES9038Q2M_THD_DISABLE            0x40

/* =========================
 * REG_SOFT_START (0x0E)
 * ========================= */
#define ES9038Q2M_SOFT_START_MASK        0x80
#define ES9038Q2M_SOFT_START_ENABLE      0x80
#define ES9038Q2M_SOFT_START_DISABLE     0x00

// Time value in [4:0] → 0 to 20; see datasheet formula

/* =========================
 * REG_VOL_CH1/CH2 (0x0F–0x10)
 * ========================= */
// Values from 0x00 (-127.5 dB) to 0xFE (0 dB), step 0.5 dB


/* =========================
 * REG_GEN_CFG (0x1B)
 * ========================= */
#define ES9038Q2M_ASRC_ENABLE            0x80

#define ES9038Q2M_CH1_VOLUME_SHARED      0x08
#define ES9038Q2M_LATCH_VOLUME           0x04

#define ES9038Q2M_GAIN_NONE              0x00
#define ES9038Q2M_GAIN_CH1_ONLY          0x01
#define ES9038Q2M_GAIN_CH2_ONLY          0x02
#define ES9038Q2M_GAIN_BOTH              0x03

/* =========================
 * REG_GPIO_INV (0x1D)
 * ========================= */
#define ES9038Q2M_GPIO_INV_NONE          0x00
#define ES9038Q2M_GPIO_INV_1             0x40
#define ES9038Q2M_GPIO_INV_2             0x80
#define ES9038Q2M_GPIO_INV_BOTH          0xC0

/* =========================
 * REG_GEN_CFG_2 (0x27)
 * ========================= */
#define ES9038Q2M_AMP_PDB_SS             0x80
#define ES9038Q2M_AMP_PDB                0x40

#define ES9038Q2M_SW_CTRL_EXT            0x00
#define ES9038Q2M_SW_CTRL_LOW            0x01
#define ES9038Q2M_SW_CTRL_HIGH           0x03

/* =========================
 * REG_LOW_PWR_CALIB (0x2D)
 * ========================= */
#define ES9038Q2M_CALIB_ENABLE           0x20
#define ES9038Q2M_CALIB_LATCH            0x10
#define ES9038Q2M_BIAS_CTRL_ENABLE       0x01

/* =========================
 * REG_ADC_CONFIG (0x2E)
 * ========================= */
#define ES9038Q2M_ADC_ORDER_1ST          0x00
#define ES9038Q2M_ADC_ORDER_2ND          0x40

#define ES9038Q2M_ADC_CLK_DIV1           0x00
#define ES9038Q2M_ADC_CLK_DIV2           0x10
#define ES9038Q2M_ADC_CLK_DIV4           0x20
#define ES9038Q2M_ADC_CLK_DIV8           0x30

#define ES9038Q2M_ADC_DITHER_DISABLE     0x04
#define ES9038Q2M_ADC_PDB_ENABLE         0x01

/* =========================
 * REG_CHIP_ID (0x40)
 * ========================= */
#define ES9038Q2M_CHIP_ID                0xFC
#define ES9038Q2M_AUTOMUTE_STATUS        0x02
#define ES9038Q2M_DPLL_LOCK_STATUS       0x01

#endif // __ES9038Q2M_H__