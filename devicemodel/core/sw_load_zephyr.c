/*-
 * Copyright (c) 2019 Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NETAPP, INC ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NETAPP, INC OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "dm.h"
#include "vmmapi.h"
#include "sw_load.h"
#include "acpi.h"

#define ZEPHYR_LOAD_OFF(ctx)		(1*MB)

static char zephyr_path[STR_LEN];
static size_t zephyr_size;

int
acrn_parse_zephyr(char *arg)
{
	size_t len = strlen(arg);

	if (len < STR_LEN) {
		strncpy(zephyr_path, arg, len + 1);
		assert(check_image(zephyr_path, 8*MB, &zephyr_size) == 0);

		zephyr_file_name = zephyr_path;

		printf("SW_LOAD: get zephyr filename %s\n", zephyr_path);
		return 0;
	} else {
		return -1;
	}
}

static int
acrn_prepare_zephyr(struct vmctx *ctx)
{
	FILE *fp;
	int len, read;

	fp = fopen(zephyr_path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "SW_LOAD ERR: could not open zephyr file: %s\n", zephyr_path);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	read = fread(ctx->baseaddr + ZEPHYR_LOAD_OFF(ctx), sizeof(char), len, fp);

	if (read < len) {
		fprintf(stderr, "SW_LOAD ERR: could not read whole zephyr file\n");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	printf("SW_LOAD: zephyr file %s size %ld copy to guest 0x%lx\n",
		zephyr_path, zephyr_size, ZEPHYR_LOAD_OFF(ctx));

	return 0;
}

int
acrn_sw_load_zephyr(struct vmctx *ctx)
{
	int ret;

	ret = acrn_prepare_zephyr(ctx);
	if (ret) {
		fprintf(stderr, "ERR to prepare zephyr, error: %d\n", ret);
		return ret;
	}

	ctx->bsp_regs.vcpu_regs.rip = ZEPHYR_LOAD_OFF(ctx);

	/* CR0_ET | CR0_NE | CR0_PE */
	ctx->bsp_regs.vcpu_regs.cr0 = 0x31U;
	ctx->bsp_regs.vcpu_regs.cr3 = 0UL;
	ctx->bsp_regs.vcpu_regs.cr4 = 0UL;

	ctx->bsp_regs.vcpu_regs.cs_sel = 0x10U;
	ctx->bsp_regs.vcpu_regs.cs_ar = 0xC09BU;
	ctx->bsp_regs.vcpu_regs.cs_limit = 0xFFFFFFFFU;
	ctx->bsp_regs.vcpu_regs.ds_sel = 0x18U;
	ctx->bsp_regs.vcpu_regs.ss_sel = 0x18U;
	ctx->bsp_regs.vcpu_regs.es_sel = 0x18U;

	return 0;
}
