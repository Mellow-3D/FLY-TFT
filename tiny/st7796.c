// SPDX-License-Identifier: GPL-2.0+
/*
 * DRM driver for display panels connected to a Sitronix ST7796
 * display controller in SPI mode.
 *
 * Copyright 2023 XiaoK <xiaok@zxkxz.cn>
 *
 * Based on st7735r.c:
 * Copyright 2017 David Lechner <david@lechnology.com>
 * Copyright (C) 2019 Glider bvba
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/dma-buf.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/spi/spi.h>
#include <linux/version.h>
#include <video/mipi_display.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_drv.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 1)
#include <drm/drm_fb_helper.h>
#else
#include <drm/drm_fbdev_generic.h>
#endif
#include <drm/drm_gem_atomic_helper.h>
#include <drm/drm_gem_dma_helper.h>
#include <drm/drm_managed.h>
#include <drm/drm_mipi_dbi.h>

#define ST7796_NOP 0x00
#define ST7796_SWRESET 0x01
#define ST7796_RDDID 0x04
#define ST7796_RDDST 0x09

#define ST7796_SLPIN 0x10
#define ST7796_SLPOUT 0x11
#define ST7796_PTLON 0x12
#define ST7796_NORON 0x13

#define ST7796_RDMODE 0x0A
#define ST7796_RDMADCTL 0x0B
#define ST7796_RDPIXFMT 0x0C
#define ST7796_RDIMGFMT 0x0A
#define ST7796_RDSELFDIAG 0x0F

#define ST7796_INVOFF 0x20
#define ST7796_INVON 0x21

#define ST7796_DISPOFF 0x28
#define ST7796_DISPON 0x29

#define ST7796_CASET 0x2A
#define ST7796_PASET 0x2B
#define ST7796_RAMWR 0x2C
#define ST7796_RAMRD 0x2E

#define ST7796_PTLAR 0x30
#define ST7796_VSCRDEF 0x33
#define ST7796_MADCTL 0x36
#define ST7796_VSCRSADD 0x37
#define ST7796_PIXFMT 0x3A

#define ST7796_WRDISBV 0x51
#define ST7796_RDDISBV 0x52
#define ST7796_WRCTRLD 0x53

#define ST7796_IFMODE 0xB0
#define ST7796_FRMCTR1 0xB1
#define ST7796_FRMCTR2 0xB2
#define ST7796_FRMCTR3 0xB3
#define ST7796_INVCTR 0xB4
#define ST7796_BPCCTR 0xB5
#define ST7796_DFUNCTR 0xB6
#define ST7796_EMSET 0xB7

#define ST7796_PWCTR1 0xC0
#define ST7796_PWCTR2 0xC1
#define ST7796_PWCTR3 0xC2

#define ST7796_VMCTR1 0xC5
#define ST7796_VMCOFF 0xC6

#define ST7796_RDID4 0xD3

#define ST7796_GMCTRP1 0xE0
#define ST7796_GMCTRN1 0xE1
#define ST7796_DOCACTL 0xE8

#define ST7796_CSCONCTL 0xF0

#define ST7796_MADCTL_MH BIT(2)
#define ST7796_MADCTL_BGR BIT(3)
#define ST7796_MADCTL_ML BIT(4)
#define ST7796_MADCTL_MV BIT(5)
#define ST7796_MADCTL_MX BIT(6)
#define ST7796_MADCTL_MY BIT(7)

struct st7796_cfg
{
	const struct drm_display_mode mode;
	unsigned int left_offset;
	unsigned int top_offset;
	unsigned int write_only : 1;
	unsigned int rgb : 1; /* RGB (vs. BGR) */
};

struct st7796_priv
{
	struct mipi_dbi_dev dbidev; /* Must be first for .release() */
	const struct st7796_cfg *cfg;
};

static void st7796_pipe_enable(struct drm_simple_display_pipe *pipe,
							   struct drm_crtc_state *crtc_state,
							   struct drm_plane_state *plane_state)
{
	struct mipi_dbi_dev *dbidev = drm_to_mipi_dbi_dev(pipe->crtc.dev);
	struct mipi_dbi *dbi = &dbidev->dbi;
	int ret, idx;
	u8 addr_mode;

	if (!drm_dev_enter(pipe->crtc.dev, &idx))
		return;

	DRM_DEBUG_KMS("\n");

	ret = mipi_dbi_poweron_reset(dbidev);
	if (ret)
		goto out_exit;

	msleep(150);

	mipi_dbi_command(dbi, MIPI_DCS_SET_DISPLAY_OFF);
	msleep(120);

	/* Software reset */
	mipi_dbi_command(dbi, ST7796_SWRESET);
	msleep(120);

	/* Sleep exit */
	mipi_dbi_command(dbi, ST7796_SLPOUT);
	msleep(120);

	/* Command Set control */
	mipi_dbi_command(dbi, ST7796_CSCONCTL, 0xC3); // Enable extension command 2 partI

	/* Command Set control */
	mipi_dbi_command(dbi, ST7796_CSCONCTL, 0x96); // Enable extension command 2 partII

	/* Memory Data Access Control MX, MY, RGB mode */
	mipi_dbi_command(dbi, ST7796_MADCTL, 0x48); // X-Mirror, Top-Left to right-Buttom, RGB

	/* Interface Pixel Format */
	mipi_dbi_command(dbi, ST7796_PIXFMT, 0x05); // Control interface color format set to 16

	mipi_dbi_command(dbi, ST7796_IFMODE, 0x80);
	mipi_dbi_command(dbi, ST7796_DFUNCTR, 0x00, 0x02);
	mipi_dbi_command(dbi, ST7796_BPCCTR, 0x02, 0x03, 0x00, 0x04);
	mipi_dbi_command(dbi, ST7796_FRMCTR1, 0x80, 0x10);
	mipi_dbi_command(dbi, ST7796_INVCTR, 0x00);
	mipi_dbi_command(dbi, ST7796_EMSET, 0xC6);
	mipi_dbi_command(dbi, ST7796_VMCTR1, 0x24);
	mipi_dbi_command(dbi, 0xE4, 0x31);

	/* Display Output Ctrl Adjust */
	mipi_dbi_command(dbi, ST7796_DOCACTL, 0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33);

	/* Power control2 */
	mipi_dbi_command(dbi, ST7796_PWCTR2, 0x06); // VAP(GVDD)=3.85+( vcom+vcom offset), VAN(GVCL)=-3.85+( vcom+vcom offset)

	/* Power control 3 */
	mipi_dbi_command(dbi, ST7796_PWCTR3, 0xA7); // Source driving current level=low, Gamma driving current level=High

	/* VCOM Control */
	mipi_dbi_command(dbi, ST7796_VMCTR1, 0x18); // VCOM=0.9
	msleep(120);

	/* ST7796 Gamma Sequence */
	/* Gamma"+" */
	mipi_dbi_command(dbi, ST7796_GMCTRP1, 0xF0, 0x09, 0x0b, 0x06, 0x04, 0x15, 0x2F, 0x54, 0x42, 0x3C, 0x17, 0x14, 0x18, 0x1B);
	/* Gamma"-" */
	mipi_dbi_command(dbi, ST7796_GMCTRN1, 0xE0, 0x09, 0x0B, 0x06, 0x04, 0x03, 0x2B, 0x43, 0x42, 0x3B, 0x16, 0x14, 0x17, 0x1B);
	msleep(120);

	switch (dbidev->rotation)
	{
	default:
		addr_mode = ST7796_MADCTL_MX;
		break;
	case 90:
		addr_mode = ST7796_MADCTL_MV;
		break;
	case 180:
		addr_mode = ST7796_MADCTL_MY;
		break;
	case 270:
		addr_mode = ST7796_MADCTL_MV | ST7796_MADCTL_MY |
					ST7796_MADCTL_MX;
		break;
	}
	addr_mode |= ST7796_MADCTL_BGR;
	mipi_dbi_command(dbi, MIPI_DCS_SET_ADDRESS_MODE, addr_mode);

	/* Command Set control */
	mipi_dbi_command(dbi, ST7796_CSCONCTL, 0x3C); // Disable extension command 2 partI
	mipi_dbi_command(dbi, ST7796_CSCONCTL, 0x69); // Disable extension command 2 partII
	msleep(100);

	mipi_dbi_command(dbi, MIPI_DCS_SET_DISPLAY_ON);
	msleep(100);
	mipi_dbi_enable_flush(dbidev, crtc_state, plane_state);
out_exit:
	drm_dev_exit(idx);
}

static const struct drm_simple_display_pipe_funcs st7796_pipe_funcs = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 1)
	.mode_valid = mipi_dbi_pipe_mode_valid,
	.enable = st7796_pipe_enable,
	.disable = mipi_dbi_pipe_disable,
	.update = mipi_dbi_pipe_update,
#else
	DRM_MIPI_DBI_SIMPLE_DISPLAY_PIPE_FUNCS(st7796_pipe_enable),
#endif
};

static const struct st7796_cfg fly_tft_v2_cfg = {
	.mode = {DRM_SIMPLE_MODE(320, 480, 56, 84)},
	.write_only = true,
};

DEFINE_DRM_GEM_DMA_FOPS(st7796_fops);

static const struct drm_driver st7796_driver = {
	.driver_features = DRIVER_GEM | DRIVER_MODESET | DRIVER_ATOMIC,
	.fops = &st7796_fops,
	DRM_GEM_DMA_DRIVER_OPS_VMAP,
	.debugfs_init = mipi_dbi_debugfs_init,
	.name = "st7796",
	.desc = "Sitronix ST7796",
	.date = "20231011",
	.major = 1,
	.minor = 1,
};

static const struct of_device_id st7796_of_match[] = {
	{.compatible = "mellow,fly-tft-v2", .data = &fly_tft_v2_cfg},
	{},
};
MODULE_DEVICE_TABLE(of, st7796_of_match);

static const struct spi_device_id st7796_id[] = {
	{"fly-tft-v2", (uintptr_t)&fly_tft_v2_cfg},
	{},
};
MODULE_DEVICE_TABLE(spi, st7796_id);

static int st7796_probe(struct spi_device *spi)
{
	struct device *dev = &spi->dev;
	const struct st7796_cfg *cfg;
	struct mipi_dbi_dev *dbidev;
	struct st7796_priv *priv;
	struct drm_device *drm;
	struct mipi_dbi *dbi;
	struct gpio_desc *dc;
	struct gpio_desc *th_rst;
	u32 rotation = 0;
	int ret;

	cfg = device_get_match_data(&spi->dev);
	if (!cfg)
		cfg = (void *)spi_get_device_id(spi)->driver_data;

	priv = devm_drm_dev_alloc(dev, &st7796_driver,
							  struct st7796_priv, dbidev.drm);
	if (IS_ERR(priv))
		return PTR_ERR(priv);

	dbidev = &priv->dbidev;
	priv->cfg = cfg;

	dbi = &dbidev->dbi;
	drm = &dbidev->drm;

	dbi->reset = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(dbi->reset))
		return dev_err_probe(dev, PTR_ERR(dbi->reset), "Failed to get GPIO 'reset'\n");

	dc = devm_gpiod_get(dev, "dc", GPIOD_OUT_LOW);
	if (IS_ERR(dc))
		return dev_err_probe(dev, PTR_ERR(dc), "Failed to get GPIO 'dc'\n");

	th_rst = devm_gpiod_get(dev, "thrst", GPIOD_OUT_HIGH);
	if (IS_ERR(th_rst))
		return dev_err_probe(dev, PTR_ERR(th_rst), "Failed to get GPIO 'th_rst'\n");
	gpiod_direction_output(th_rst, 1);

	dbidev->backlight = devm_of_find_backlight(dev);
	if (IS_ERR(dbidev->backlight))
		return PTR_ERR(dbidev->backlight);

	device_property_read_u32(dev, "rotation", &rotation);

	ret = mipi_dbi_spi_init(spi, dbi, dc);
	if (ret)
		return ret;

	if (cfg->write_only)
		dbi->read_commands = NULL;

	dbidev->left_offset = cfg->left_offset;
	dbidev->top_offset = cfg->top_offset;

	ret = mipi_dbi_dev_init(dbidev, &st7796_pipe_funcs, &cfg->mode,
							rotation);
	if (ret)
		return ret;

	drm_mode_config_reset(drm);

	ret = drm_dev_register(drm, 0);
	if (ret)
		return ret;

	spi_set_drvdata(spi, drm);

	drm_fbdev_generic_setup(drm, 0);

	return 0;
}

static void st7796_remove(struct spi_device *spi)
{
	struct drm_device *drm = spi_get_drvdata(spi);

	drm_dev_unplug(drm);
	drm_atomic_helper_shutdown(drm);
}

static void st7796_shutdown(struct spi_device *spi)
{
	drm_atomic_helper_shutdown(spi_get_drvdata(spi));
}

static struct spi_driver st7796_spi_driver = {
	.driver = {
		.name = "st7796",
		.of_match_table = st7796_of_match,
	},
	.id_table = st7796_id,
	.probe = st7796_probe,
	.remove = st7796_remove,
	.shutdown = st7796_shutdown,
};
module_spi_driver(st7796_spi_driver);

MODULE_DESCRIPTION("Sitronix ST7796 DRM driver");
MODULE_AUTHOR("XiaoK <xiaok@zxkxz.cn>");
MODULE_INFO(intree, "Y");
MODULE_LICENSE("GPL");
