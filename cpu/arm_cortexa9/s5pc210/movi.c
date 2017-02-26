#include <common.h>
#if defined(CONFIG_SECURE)
#include "movi.h"
#else
#include <movi.h>
#endif
#include <asm/io.h>
#include <mmc.h>

extern raw_area_t raw_area_control;

typedef u32 (*copy_sd_mmc_to_mem) \
	(u32 start_block, u32 block_count, u32* dest_addr);

typedef u32 (*copy_emmc_to_mem) \
	(u32 block_size, u32 *buffer);

typedef u32 (*copy_emmc441_to_mem) \
	(u32 block_size, void *buffer);

typedef u32 (*emmc441_endboot_op)();


/*     function:    This function copies the data of SD and MMC type device to destination
	 params:      u32 SrcBlock, u32 NumofSrcBlock, void * DstByte
	 Return type:
	                    True=1, False=0
	 Address:
	              0x02020030
 */
typedef u32 (*SDMMC_ReadBlocks)(u32 SrcBlock, u32 NumofSrcBlock, void * DstByte);

/*     function:    This function copies BL2 of the boot area data of eMMC 4.3 to internal RAM
	 params:      u32 SrcBlock, u32* DstByte
	 Return type:
	                    True=1, False=0
	 Address:
	              0x0202003C
 */
typedef u32 (*LoadBL2FromEmmc43Ch0)(u32 SrcBlock, u32* DstByte);

void movi_write_env(ulong addr)
{
	movi_write(raw_area_control.image[2].start_blk, 
		raw_area_control.image[2].used_blk, addr);
}

void movi_read_env(ulong addr)
{
	movi_read(raw_area_control.image[2].start_blk,
		raw_area_control.image[2].used_blk, addr);
}

void movi_write_bl1(ulong addr)
{
	int i;
	ulong checksum;
	ulong src;
	ulong tmp;

	src = addr;
	
	for(i = 0, checksum = 0;i < (14 * 1024) - 4;i++)
	{
		checksum += *(u8*)addr++;
	}

	tmp = *(ulong*)addr;
	*(ulong*)addr = checksum;
			
	movi_write(raw_area_control.image[1].start_blk, 
		raw_area_control.image[1].used_blk, src);

	*(ulong*)addr = tmp;
}

void movi_calc_checksum_bl1(ulong addr)
{
	int i;
	ulong checksum;
	ulong src;
#ifdef CONFIG_SECURE
	src = addr+(16*1024); /* from BL2 */
#else
	src = addr; /* from BL2 */
#endif
	
	for(i = 0, checksum = 0;i < (14 * 1024) - 4;i++)
	{
		checksum += *(u8*)addr++;
	}

	*(ulong*)addr = checksum;
}

void emmc441_uboot_copy(void)
{
	int i;
	char *ptemp;

#ifdef CONFIG_EVT1
	copy_emmc441_to_mem copy_bl2 = (copy_emmc441_to_mem)*(u32 *)(0x02020044);	//MSH_ReadFromFIFO_eMMC
	emmc441_endboot_op end_bootop = (emmc441_endboot_op)*(u32 *)(0x02020048);	//MSH_EndBootOp_eMMC
#else
	copy_emmc441_to_mem copy_bl2 = (copy_emmc441_to_mem)*(u32 *)(0x00007974);	//MSH_ReadFromFIFO_eMMC
	emmc441_endboot_op end_bootop = (emmc441_endboot_op)*(u32 *)(0x000082c8);	//MSH_EndBootOp_eMMC
#endif

#ifdef CONFIG_SECURE
	copy_bl2(0x10, MOVI_UBOOT_BLKCNT-MOVI_BL1_BLKCNT-MOVI_BL1_BLKCNT, CFG_PHY_UBOOT_BASE);
#else
	copy_bl2(/*0x8,*/ MOVI_UBOOT_BLKCNT, CFG_PHY_UBOOT_BASE); //mj
#endif
	/* stop bootop */
	end_bootop();
}

void movi_uboot_copy(void)
{
#ifdef CONFIG_EVT1
	SDMMC_ReadBlocks copy_bl2 = (SDMMC_ReadBlocks)*(u32 *)(0x02020030);
#else
	copy_sd_mmc_to_mem copy_bl2 = (copy_sd_mmc_to_mem)(0x00002488);
#endif
#if defined(CONFIG_SECURE)
	copy_bl2(MOVI_UBOOT_POS, MOVI_UBOOT_BLKCNT, CFG_PHY_UBOOT_BASE);
#else
	copy_bl2(MOVI_UBOOT_POS, MOVI_UBOOT_BLKCNT, CFG_PHY_UBOOT_BASE);
#endif
}


