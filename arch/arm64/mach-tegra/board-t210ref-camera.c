/*
 * arch/arm/mach-tegra/board-t210ref-sensors.c
 *
 * Copyright (c) 2013-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <media/imx214.h>
#include <media/soc_camera.h>
#include <media/soc_camera_platform.h>
#include <media/tegra_v4l2_camera.h>
#include "board-t210.h"
/*
 * Soc Camera platform driver for testing
 */
#if IS_ENABLED(CONFIG_SOC_CAMERA_PLATFORM)
static struct platform_device *t210ref_pdev;
static int t210ref_soc_camera_add(struct soc_camera_device *icd);
static void t210ref_soc_camera_del(struct soc_camera_device *icd);

static int t210ref_soc_camera_set_capture(struct soc_camera_platform_info *info,
		int enable)
{
	/* TODO: probably add clk opertaion here */
	return 0; /* camera sensor always enabled */
}

static struct soc_camera_platform_info t210ref_soc_camera_info = {
	.format_name = "RGB4",
	.format_depth = 32,
	.format = {
		.code = V4L2_MBUS_FMT_RGBA8888_4X8_LE,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.field = V4L2_FIELD_NONE,
		.width = 1280,
		.height = 720,
	},
	.set_capture = t210ref_soc_camera_set_capture,
};

static struct tegra_camera_platform_data t210ref_camera_platform_data = {
	.port                   = TEGRA_CAMERA_PORT_CSI_A,
	.lanes                  = 4,
};

static struct soc_camera_link t210ref_soc_camera_link = {
	.bus_id         = 0, /* This must match the .id of tegra_vi01_device */
	.add_device     = t210ref_soc_camera_add,
	.del_device     = t210ref_soc_camera_del,
	.module_name    = "soc_camera_platform",
	.priv		= &t210ref_camera_platform_data,
	.dev_priv	= &t210ref_soc_camera_info,
};

static void t210ref_soc_camera_release(struct device *dev)
{
	soc_camera_platform_release(&t210ref_pdev);
}

static int t210ref_soc_camera_add(struct soc_camera_device *icd)
{
	return soc_camera_platform_add(icd, &t210ref_pdev,
			&t210ref_soc_camera_link,
			t210ref_soc_camera_release, 0);
}

static void t210ref_soc_camera_del(struct soc_camera_device *icd)
{
	soc_camera_platform_del(icd, t210ref_pdev, &t210ref_soc_camera_link);
}

static struct platform_device t210ref_soc_camera_device = {
	.name   = "soc-camera-pdrv",
	.dev    = {
		.platform_data = &t210ref_soc_camera_link,
	},
};
#endif

#if IS_ENABLED(CONFIG_SOC_CAMERA_IMX214)
static int t210ref_imx214_power(struct device *dev, int enable)
{
	return 0;
}

static struct imx214_platform_data t210ref_imx214_data;

static struct i2c_board_info t210ref_imx214_camera_i2c_device = {
	I2C_BOARD_INFO("imx214_v4l2", 0x1a),
	.platform_data = &t210ref_imx214_data,
};

static struct tegra_camera_platform_data t210ref_imx214_camera_platform_data = {
	.flip_v			= 0,
	.flip_h			= 0,
	.port			= TEGRA_CAMERA_PORT_CSI_A,
	.lanes			= 4,
	.continuous_clk		= 0,
};

static struct soc_camera_link imx214_iclink = {
	.bus_id		= 0, /* This must match the .id of tegra_vi01_device */
	.board_info	= &t210ref_imx214_camera_i2c_device,
	.module_name	= "imx214_v4l2",
	.i2c_adapter_id	= 6, /* VI2 I2C controller */
	.power		= t210ref_imx214_power,
	.priv		= &t210ref_imx214_camera_platform_data,
};

static struct platform_device t210ref_imx214_soc_camera_device = {
	.name	= "soc-camera-pdrv",
	.id	= 0,
	.dev	= {
		.platform_data = &imx214_iclink,
	},
};
#endif

int t210ref_camera_init(void)
{
	pr_debug("%s: ++\n", __func__);
#if IS_ENABLED(CONFIG_SOC_CAMERA_PLATFORM)
	platform_device_register(&t210ref_soc_camera_device);
#endif

#if IS_ENABLED(CONFIG_SOC_CAMERA_IMX214)
	platform_device_register(&t210ref_imx214_soc_camera_device);
#endif

	return 0;
}