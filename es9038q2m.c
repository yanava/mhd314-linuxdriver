/*
 * ALSA SoC codec driver for ESS ES9038Q2M
 *
 * Copyright 2024 mahaudio
 *
 * Author: Moreno Hassem <moreno.hassem@mahaudio.com.br>
 * 
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <sound/soc-dapm.h>

#include "es9038q2m.h"

#define ES9038Q2M_CHIP_ID_NBR  (0x70)

#define ES9038Q2M_NUM_SUPPLIES  (3)
static const char * const es9038q2m_supply_names[ES9038Q2M_NUM_SUPPLIES] = {
	"DVCC", // Digital Power Supply
	"VCCA", // Analog Power Supply for PLL
	"AVCC", // Analog Power Supply for DAC
};

struct es9038q2m_priv {
	struct i2c_client *i2c;
    struct regmap *regmap;
    struct regulator_bulk_data supplies[ES9038Q2M_NUM_SUPPLIES];
    struct notifier_block supply_nb[ES9038Q2M_NUM_SUPPLIES];
	unsigned int mclk;
    int fmt;
	unsigned int rate;
	unsigned int width;
    int mute;
	int is_master;
    struct mutex lock;
    unsigned int bclk_ratio;
};

static const struct reg_default es9038q2m_reg_defaults[] = {
    /* Default values for the DAC registers */
	{ ES9038Q2M_REG_SYSTEM,        0x00 },
	{ ES9038Q2M_REG_INPUT_SEL,     0xCC },
	{ ES9038Q2M_REG_MIXING,        0x2C },
	{ ES9038Q2M_REG_SPDIF_CFG,     0x40 },
	{ ES9038Q2M_REG_AMUTE_TIME,    0x00 },
	{ ES9038Q2M_REG_AMUTE_LEVEL,   0x68 },
	{ ES9038Q2M_REG_DEEMP_VOLRAMP, 0x42 },
	{ ES9038Q2M_REG_FILTER_SHAPE,  0x84 },
	{ ES9038Q2M_REG_GPIO_CFG,      0xDD },
	{ ES9038Q2M_REG_RESERVED_09,   0x22 },
	{ ES9038Q2M_REG_MASTER_MODE,   0x02 },
	{ ES9038Q2M_REG_SPDIF_SELECT,  0x00 },
	{ ES9038Q2M_REG_DPLL_BW,       0x5A },
	{ ES9038Q2M_REG_THD_BYPASS,    0x40 },
	{ ES9038Q2M_REG_SOFT_START,    0x8A },
	{ ES9038Q2M_REG_VOL_CH1,       0x50 },
	{ ES9038Q2M_REG_VOL_CH2,       0x50 },
	{ ES9038Q2M_REG_MTRIM_0,       0xFF },
	{ ES9038Q2M_REG_MTRIM_1,       0xFF },
	{ ES9038Q2M_REG_MTRIM_2,       0xFF },
	{ ES9038Q2M_REG_MTRIM_3,       0x7F },
	{ ES9038Q2M_REG_GPIO_IN_SEL,   0x00 },
	{ ES9038Q2M_REG_THD_C2_0,      0x00 },
	{ ES9038Q2M_REG_THD_C2_1,      0x00 },
	{ ES9038Q2M_REG_THD_C3_0,      0x00 },
	{ ES9038Q2M_REG_THD_C3_1,      0x00 },
	{ ES9038Q2M_REG_RESERVED_1A,   0x00 },
	{ ES9038Q2M_REG_GEN_CFG,       0xD4 },
	{ ES9038Q2M_REG_RESERVED_1C,   0xF0 },
	{ ES9038Q2M_REG_GPIO_INV,      0x00 },
	{ ES9038Q2M_REG_CP_CLK_0,      0x00 },
	{ ES9038Q2M_REG_CP_CLK_1,      0x00 },
	{ ES9038Q2M_REG_RESERVED_20,   0x00 },
	{ ES9038Q2M_REG_INTR_MASK,     0xF0 },
	{ ES9038Q2M_REG_NCO_0,         0x00 },
	{ ES9038Q2M_REG_NCO_1,         0x00 },
	{ ES9038Q2M_REG_NCO_2,         0x00 },
	{ ES9038Q2M_REG_NCO_3,         0x00 },
	{ ES9038Q2M_REG_RESERVED_26,   0x00 },
	{ ES9038Q2M_REG_GEN_CFG_2,     0x00 },
	{ ES9038Q2M_REG_FIR_ADDR,      0x00 },
	{ ES9038Q2M_REG_FIR_DATA_0,    0x00 },
	{ ES9038Q2M_REG_FIR_DATA_1,    0x00 },
	{ ES9038Q2M_REG_FIR_DATA_2,    0x00 },
	{ ES9038Q2M_REG_FIR_CONFIG,    0x00 },
	{ ES9038Q2M_REG_LOW_PWR_CALIB, 0x20 },
	{ ES9038Q2M_REG_ADC_CONFIG,    0x55 },

    /* ADC Filter Config (Registers 0x2F–0x34) */
	{ ES9038Q2M_REG_ADC_FTR_SCALE_0, 0xE0 },
	{ ES9038Q2M_REG_ADC_FTR_SCALE_1, 0x03 },
	{ ES9038Q2M_REG_ADC_FBQ1_0,      0x00 },
	{ ES9038Q2M_REG_ADC_FBQ1_1,      0x04 },
	{ ES9038Q2M_REG_ADC_FBQ2_0,      0x00 },
	{ ES9038Q2M_REG_ADC_FBQ2_1,      0x04 },
};

static const DECLARE_TLV_DB_SCALE(dac_tlv, -12750, 50, 1);

static const char * const es9038q2m_filter_texts[] = {
	"Fast Linear", "Slow Linear", "Fast Minimum",
	"Slow Minimum", "Apodizing Fast", 
	"Corrected Minimum", "Brick Wall"
};

static const unsigned int es9038q2m_filter_values[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

static const struct soc_enum es9038q2m_filter_enum = SOC_VALUE_ENUM_SINGLE(ES9038Q2M_REG_FILTER_SHAPE, 
	5, 
	7, 
	ARRAY_SIZE(es9038q2m_filter_texts), 
	es9038q2m_filter_texts, 
	es9038q2m_filter_values);

static const struct snd_kcontrol_new es9038q2m_snd_controls[] = {
	SOC_DOUBLE_R_TLV("DAC Playback Volume", ES9038Q2M_REG_VOL_CH1, ES9038Q2M_REG_VOL_CH2, 0, 255, 1, dac_tlv),
	SOC_SINGLE("DAC Mute", ES9038Q2M_REG_FILTER_SHAPE, 0, 1, 0),
	SOC_ENUM("DAC Filter", es9038q2m_filter_enum),
};

static const struct snd_soc_component_driver es9038q2m_codec_driver = {
	.controls           = es9038q2m_snd_controls,
	.num_controls       = ARRAY_SIZE(es9038q2m_snd_controls),
};

static bool es9038q2m_writable_reg(struct device *dev, unsigned int reg)
{
	return (reg < ES9038Q2M_REG_CHIP_ID);
}


static bool es9038q2m_readable_reg(struct device *dev, unsigned int reg)
{
	return (reg < ES9038Q2M_NUM_REGISTERS);
}

static bool es9038q2m_volatile_reg(struct device *dev, unsigned int reg)
{
    /* Read-only status registers are volatile */
    if (reg >= ES9038Q2M_REG_CHIP_ID || reg == ES9038Q2M_REG_SYSTEM)
        return true;

    return false;
}

static const struct regmap_config es9038q2m_regmap_config = {
	.reg_bits         = 8,
	.val_bits         = 8,
	.max_register     = ES9038Q2M_NUM_REGISTERS - 1,
	.reg_defaults     = es9038q2m_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(es9038q2m_reg_defaults),
    .use_single_read  = true,
	.use_single_write = true,
	.writeable_reg    = es9038q2m_writable_reg,
	.readable_reg     = es9038q2m_readable_reg,
	.volatile_reg     = es9038q2m_volatile_reg,
	.cache_type       = REGCACHE_RBTREE,
};

static int es9038q2m_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct es9038q2m_priv *es9038 = snd_soc_component_get_drvdata(component);
	unsigned int rate = params_rate(params);
	unsigned int width = params_width(params);
	unsigned int regval, ret;
	unsigned int is_dsd = 0;

	/* disables soft start */
    ret = regmap_update_bits(es9038->regmap, ES9038Q2M_REG_SOFT_START, ES9038Q2M_SOFT_START_MASK, ES9038Q2M_SOFT_START_DISABLE);
	if (ret) {
		dev_err(component->dev, "Failed to disable soft start: %d\n", ret);
		return ret;
	}

	switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_S16_LE:
			regval = ES9038Q2M_SERIAL_LEN_16BIT;
			break;
		case SNDRV_PCM_FORMAT_S24_LE:
			regval = ES9038Q2M_SERIAL_LEN_24BIT;
			break;
		case SNDRV_PCM_FORMAT_S32_LE:
			regval = ES9038Q2M_SERIAL_LEN_32BIT;
			break;
		case SNDRV_PCM_FORMAT_DSD_U8:
			is_dsd = 1;
			break;
		case SNDRV_PCM_FORMAT_DSD_U16_LE:
			is_dsd = 1;
			break;
		default:
			dev_err(component->dev, "Unsupported sound format: \n");
			return -EINVAL;
	}

	/* Updates the serial length bits */
	ret = regmap_update_bits(es9038->regmap, ES9038Q2M_REG_INPUT_SEL, ES9038Q2M_SERIAL_LEN_MASK, regval);
	if (ret) {
		dev_err(component->dev, "Failed to set serial length: %d\n", ret);
		return ret;
	}

	/* If master needs to calculate the NCO register from MCLK */
	if(es9038->is_master){
		unsigned int nco = 0;
		unsigned int mclk = es9038->mclk;
		
		/* Selects between DSD and PCM if in master mode, as required by datasheet */
		ret = regmap_update_bits(es9038->regmap, ES9038Q2M_REG_INPUT_SEL, ES9038Q2M_SERIAL_MODE_MASK, is_dsd ? ES9038Q2M_INPUT_SEL_DSD : ES9038Q2M_INPUT_SEL_SERIAL);
		if (ret) {
			dev_err(component->dev, "Failed to set input select: %d\n", ret);
			return ret;
		}

		/* Calculates NCO according to datasheet, page 36 */
		nco = rate * 0x100000000 / mclk;

		/* Writes to  NCO0-3 registers */
		regmap_write(es9038->regmap, ES9038Q2M_REG_NCO_0, nco & 0xFF);
		regmap_write(es9038->regmap, ES9038Q2M_REG_NCO_1, (nco >> 8) & 0xFF);
		regmap_write(es9038->regmap, ES9038Q2M_REG_NCO_2, (nco >> 16) & 0xFF);
		regmap_write(es9038->regmap, ES9038Q2M_REG_NCO_3, (nco >> 24) & 0xFF);	

		dev_info(component->dev, "NCO set to: %d\n", nco);
	}

	/* enables soft start */
    ret = regmap_update_bits(es9038->regmap, ES9038Q2M_REG_SOFT_START, ES9038Q2M_SOFT_START_MASK, ES9038Q2M_SOFT_START_ENABLE);
	if (ret) {
		dev_err(component->dev, "Failed to enable soft start: %d\n", ret);
		return ret;
	}
		
	/* Saves info if succeeded */						  
	es9038->rate = rate;	
	es9038->width = width;

	dev_info(component->dev, "HW Params set to: %dHz, %d bits. DSD = %d\n", rate, width, is_dsd);
	
	return 0; 
}

static int es9038q2m_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_component *component = dai->component;
	struct es9038q2m_priv *es9038 = snd_soc_component_get_drvdata(component);
	unsigned int daifmt = 0, modefmt = 0, ismaster = 0, ret;
	
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
		case SND_SOC_DAIFMT_I2S:
			daifmt |= ES9038Q2M_SERIAL_MODE_I2S; 
			break;
		case SND_SOC_DAIFMT_LEFT_J:
			daifmt |= ES9038Q2M_SERIAL_MODE_LJ;
			break;
		case SND_SOC_DAIFMT_RIGHT_J:
			daifmt |= ES9038Q2M_SERIAL_MODE_RJ;
			break;
		default:
			dev_err(component->dev, "Unsupported DAI format: 0x%x\n", fmt & SND_SOC_DAIFMT_FORMAT_MASK);
			return (-EINVAL);
	}

	/* Sets master mode, depending on configuration*/
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
		case SND_SOC_DAIFMT_CBM_CFM:
			modefmt |= ES9038Q2M_MASTER_MODE_ENABLE;
			ismaster = 1;
			break;
		case SND_SOC_DAIFMT_CBS_CFS:
			modefmt &= ~(ES9038Q2M_MASTER_MODE_ENABLE);
			ismaster = 0;
			break;
		default:
			dev_err(component->dev, "Unsupported CODEC mode: 0x%x\n", fmt & SND_SOC_DAIFMT_MASTER_MASK);
			return -EINVAL;
	}

	/* Turns off autosel if in master mode */
	if(ismaster)
	{
		dev_info(component->dev, "Disabling AUTOSEL in master mode\n");
		ret = regmap_update_bits(es9038->regmap, ES9038Q2M_REG_INPUT_SEL, ES9038Q2M_AUTOSEL_MASK, ES9038Q2M_AUTOSEL_DISABLED);
		if (ret) {
			dev_err(component->dev, "Failed to disable AUTOSEL in master mode: %d\n", ret);
			return ret;
		}
	}

	/* Sets interface format */
	ret = regmap_update_bits(es9038->regmap, ES9038Q2M_REG_INPUT_SEL, ES9038Q2M_SERIAL_MODE_MASK, daifmt);
	if (ret) {
		dev_err(component->dev, "Failed to set DAI format: %d\n", ret);
		return ret;
	}

	/* Sets master/slave mode */
	ret = regmap_update_bits(es9038->regmap, ES9038Q2M_REG_MASTER_MODE, ES9038Q2M_MASTER_MODE_MASK, modefmt);
	if (ret) {
		dev_err(component->dev, "Failed to set master/slave mode: %d\n", ret);
		return ret;
	}

	/* If everything successful saves the	 format */
	es9038->is_master = ismaster;
	es9038->fmt = fmt;

	dev_info(component->dev, "DAI format set to: 0x%x, master: %d\n", fmt, ismaster);
	
	return 0; 
}

static const struct snd_soc_dai_ops es9038q2m_dai_ops = {
	.hw_params = es9038q2m_hw_params,
	.set_fmt   = es9038q2m_set_dai_fmt,
};


static struct snd_soc_dai_driver es9038q2m_dai = {
	.name = "es9038q2m",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_CONTINUOUS,
		.rate_min = 8000,
		.rate_max = 1536000,
        .formats = SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE |
                  SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FMTBIT_DSD_U8 |
                  SNDRV_PCM_FMTBIT_DSD_U16_LE,	},
	.ops = &es9038q2m_dai_ops,
};

static int es9038q2m_i2c_probe(struct i2c_client *i2c)
{
    struct es9038q2m_priv *es9038q2m;
	int ret, chip_id;
	unsigned int regval;
    struct device *dev = &i2c->dev;
	struct device_node *np = dev->of_node;

	es9038q2m = devm_kzalloc(dev, sizeof(struct es9038q2m_priv), GFP_KERNEL);
	if (!es9038q2m)
		return -ENOMEM;

    i2c_set_clientdata(i2c, es9038q2m);
	
    es9038q2m->i2c = i2c;
	mutex_init(&es9038q2m->lock);
	es9038q2m->regmap = devm_regmap_init_i2c(i2c, &es9038q2m_regmap_config);
	if (IS_ERR(es9038q2m->regmap)) {
		ret = PTR_ERR(es9038q2m->regmap);
		dev_err(dev, "Failed to init regmap: %d\n", ret);
		return ret;
	}

	/* Gets MCLK info from Device Tree */
	ret = of_property_read_u32(np, "clock-frequency", &es9038q2m->mclk);
    if (ret) {
            dev_err(dev, "Failed to retrieve MCLK frequency for the codec: %d\n", ret);
            return ret;
    }
	dev_info(dev, "MCLK frequency set to: %d\n", es9038q2m->mclk);

	/* Reads CHIP_ID reg */
	ret = regmap_read(es9038q2m->regmap, ES9038Q2M_REG_CHIP_ID, &regval);
	if (ret) {
		dev_err(dev, "Failed to read CHIP_ID register: %d\n", ret);
		return ret;
	}

	/* Applies proper mask to get Chip ID */
	chip_id = regval & ES9038Q2M_CHIP_ID;

	/* If wrong chip, report and reject*/
	if(chip_id != ES9038Q2M_CHIP_ID_NBR) {
		dev_err(dev, "Invalid CHIP_ID: 0x%02X\n", chip_id);
		return -ENODEV;
	}

	/* Print the detected chip ID */
	dev_info(dev, "ES9038Q2M detected, CHIP_ID = 0x%02X \n", chip_id);

	/* Register with ASoC */
	ret = devm_snd_soc_register_component(dev, &es9038q2m_codec_driver,
					      &es9038q2m_dai, 1);
	if (ret) {
		dev_err(dev, "Failed to register ES9038Q2M with ASoC: %d\n", ret);
		return ret;
	}

	/* Confirmation */
	dev_info(dev, "ES9038Q2M successfully registered with ASoC\n");

	return 0;
}

static const struct i2c_device_id es9038q2m_i2c_id[] = {
	{ "es9038q2m", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, es9038q2m_i2c_id);

static const struct of_device_id es9038q2m_of_match[] = {
	{ .compatible = "ess,es9038q2m" },
	{ }
};
MODULE_DEVICE_TABLE(of, es9038q2m_of_match);

static struct i2c_driver es9038q2m_i2c_driver = {
	.driver = {
		.name = "es9038q2m",
		.of_match_table = of_match_ptr(es9038q2m_of_match),
	},
	.probe = es9038q2m_i2c_probe,  // ✅ Essencial
	.id_table = es9038q2m_i2c_id,
};

module_i2c_driver(es9038q2m_i2c_driver);

MODULE_AUTHOR("Moreno Hassem <moreno.hassem@mahaudio.com.br>");
MODULE_DESCRIPTION("ESS ES9038Q2M ALSA SoC Codec Driver");
MODULE_LICENSE("GPL");
