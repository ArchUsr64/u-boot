/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  Copyright © 2000-2010 David Woodhouse <dwmw2@infradead.org>
 *                        Steven J. Hill <sjhill@realitydiluted.com>
 *		          Thomas Gleixner <tglx@linutronix.de>
 *
 * Info:
 *	Contains standard defines and IDs for NAND flash devices
 *
 * Changelog:
 *	See git changelog.
 */
#ifndef __LINUX_MTD_RAWNAND_H
#define __LINUX_MTD_RAWNAND_H

#include <config.h>

#include <dm/device.h>
#include <linux/bitops.h>
#include <linux/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/flashchip.h>
#include <linux/mtd/bbm.h>
#include <asm/cache.h>

struct mtd_info;
struct nand_chip;
struct nand_flash_dev;
struct device_node;

/* Get the flash and manufacturer id and lookup if the type is supported. */
int nand_detect(struct nand_chip *chip, int *maf_id, int *dev_id,
		struct nand_flash_dev *type);

/* Scan and identify a NAND device */
int nand_scan(struct mtd_info *mtd, int max_chips);
/*
 * Separate phases of nand_scan(), allowing board driver to intervene
 * and override command or ECC setup according to flash type.
 */
int nand_scan_ident(struct mtd_info *mtd, int max_chips,
			   struct nand_flash_dev *table);
int nand_scan_tail(struct mtd_info *mtd);

/* Free resources held by the NAND device */
void nand_release(struct mtd_info *mtd);

/* Internal helper for board drivers which need to override command function */
void nand_wait_ready(struct mtd_info *mtd);

/*
 * This constant declares the max. oobsize / page, which
 * is supported now. If you add a chip with bigger oobsize/page
 * adjust this accordingly.
 */
#define NAND_MAX_OOBSIZE       1664
#define NAND_MAX_PAGESIZE      16384

/*
 * Constants for hardware specific CLE/ALE/NCE function
 *
 * These are bits which can be or'ed to set/clear multiple
 * bits in one go.
 */
/* Select the chip by setting nCE to low */
#define NAND_NCE		0x01
/* Select the command latch by setting CLE to high */
#define NAND_CLE		0x02
/* Select the address latch by setting ALE to high */
#define NAND_ALE		0x04

#define NAND_CTRL_CLE		(NAND_NCE | NAND_CLE)
#define NAND_CTRL_ALE		(NAND_NCE | NAND_ALE)
#define NAND_CTRL_CHANGE	0x80

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_PARAM		0xec
#define NAND_CMD_GET_FEATURES	0xee
#define NAND_CMD_SET_FEATURES	0xef
#define NAND_CMD_RESET		0xff

#define NAND_CMD_LOCK		0x2a
#define NAND_CMD_UNLOCK1	0x23
#define NAND_CMD_UNLOCK2	0x24

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15

/* Extended commands for AG-AND device */
/*
 * Note: the command for NAND_CMD_DEPLETE1 is really 0x00 but
 *       there is no way to distinguish that from NAND_CMD_READ0
 *       until the remaining sequence of commands has been completed
 *       so add a high order bit and mask it off in the command.
 */
#define NAND_CMD_DEPLETE1	0x100
#define NAND_CMD_DEPLETE2	0x38
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_STATUS_ERROR	0x72
/* multi-bank error status (banks 0-3) */
#define NAND_CMD_STATUS_ERROR0	0x73
#define NAND_CMD_STATUS_ERROR1	0x74
#define NAND_CMD_STATUS_ERROR2	0x75
#define NAND_CMD_STATUS_ERROR3	0x76
#define NAND_CMD_STATUS_RESET	0x7f
#define NAND_CMD_STATUS_CLEAR	0xff

#define NAND_CMD_NONE		-1

/* Status bits */
#define NAND_STATUS_FAIL	0x01
#define NAND_STATUS_FAIL_N1	0x02
#define NAND_STATUS_TRUE_READY	0x20
#define NAND_STATUS_READY	0x40
#define NAND_STATUS_WP		0x80

#define NAND_DATA_IFACE_CHECK_ONLY	-1

/*
 * There are different places where the manufacturer stores the factory bad
 * block markers.
 *
 * Position within the block: Each of these pages needs to be checked for a
 * bad block marking pattern.
 */
#define NAND_BBM_FIRSTPAGE	BIT(24)
#define NAND_BBM_SECONDPAGE	BIT(25)
#define NAND_BBM_LASTPAGE	BIT(26)

/*
 * Constants for ECC_MODES
 */
typedef enum {
	NAND_ECC_NONE,
	NAND_ECC_SOFT,
	NAND_ECC_HW,
	NAND_ECC_HW_SYNDROME,
	NAND_ECC_HW_OOB_FIRST,
	NAND_ECC_SOFT_BCH,
} nand_ecc_modes_t;

enum nand_ecc_algo {
	NAND_ECC_UNKNOWN,
	NAND_ECC_HAMMING,
	NAND_ECC_BCH,
};

/*
 * Constants for Hardware ECC
 */
/* Reset Hardware ECC for read */
#define NAND_ECC_READ		0
/* Reset Hardware ECC for write */
#define NAND_ECC_WRITE		1
/* Enable Hardware ECC before syndrome is read back from flash */
#define NAND_ECC_READSYN	2

/*
 * Enable generic NAND 'page erased' check. This check is only done when
 * ecc.correct() returns -EBADMSG.
 * Set this flag if your implementation does not fix bitflips in erased
 * pages and you want to rely on the default implementation.
 */
#define NAND_ECC_GENERIC_ERASED_CHECK	BIT(0)
#define NAND_ECC_MAXIMIZE		BIT(1)
/*
 * If your controller already sends the required NAND commands when
 * reading or writing a page, then the framework is not supposed to
 * send READ0 and SEQIN/PAGEPROG respectively.
 */
#define NAND_ECC_CUSTOM_PAGE_ACCESS	BIT(2)

/* Bit mask for flags passed to do_nand_read_ecc */
#define NAND_GET_DEVICE		0x80

/*
 * Option constants for bizarre disfunctionality and real
 * features.
 */
/* Buswidth is 16 bit */
#define NAND_BUSWIDTH_16	0x00000002
/* Device supports partial programming without padding */
#define NAND_NO_PADDING		0x00000004
/* Chip has cache program function */
#define NAND_CACHEPRG		0x00000008
/* Chip has copy back function */
#define NAND_COPYBACK		0x00000010
/*
 * Chip requires ready check on read (for auto-incremented sequential read).
 * True only for small page devices; large page devices do not support
 * autoincrement.
 */
#define NAND_NEED_READRDY	0x00000100

/* Chip does not allow subpage writes */
#define NAND_NO_SUBPAGE_WRITE	0x00000200

/* Device is one of 'new' xD cards that expose fake nand command set */
#define NAND_BROKEN_XD		0x00000400

/* Device behaves just like nand, but is readonly */
#define NAND_ROM		0x00000800

/* Device supports subpage reads */
#define NAND_SUBPAGE_READ	0x00001000

/*
 * Some MLC NANDs need data scrambling to limit bitflips caused by repeated
 * patterns.
 */
#define NAND_NEED_SCRAMBLING	0x00002000

/* Device needs 3rd row address cycle */
#define NAND_ROW_ADDR_3		0x00004000

/* Options valid for Samsung large page devices */
#define NAND_SAMSUNG_LP_OPTIONS NAND_CACHEPRG

/* Macros to identify the above */
#define NAND_HAS_CACHEPROG(chip) ((chip->options & NAND_CACHEPRG))
#define NAND_HAS_SUBPAGE_READ(chip) ((chip->options & NAND_SUBPAGE_READ))
#define NAND_HAS_SUBPAGE_WRITE(chip) !((chip)->options & NAND_NO_SUBPAGE_WRITE)

/* Non chip related options */
/* This option skips the bbt scan during initialization. */
#define NAND_SKIP_BBTSCAN	0x00010000
/*
 * This option is defined if the board driver allocates its own buffers
 * (e.g. because it needs them DMA-coherent).
 */
#define NAND_OWN_BUFFERS	0x00020000
/* Chip may not exist, so silence any errors in scan */
#define NAND_SCAN_SILENT_NODEV	0x00040000
/*
 * Autodetect nand buswidth with readid/onfi.
 * This suppose the driver will configure the hardware in 8 bits mode
 * when calling nand_scan_ident, and update its configuration
 * before calling nand_scan_tail.
 */
#define NAND_BUSWIDTH_AUTO      0x00080000
/*
 * This option could be defined by controller drivers to protect against
 * kmap'ed, vmalloc'ed highmem buffers being passed from upper layers
 */
#define NAND_USE_BOUNCE_BUFFER	0x00100000
/*
 * Whether the NAND chip is a boot medium. Drivers might use this information
 * to select ECC algorithms supported by the boot ROM or similar restrictions.
 */
#define NAND_IS_BOOT_MEDIUM	0x00400000

/*
 * Do not try to tweak the timings at runtime. This is needed when the
 * controller initializes the timings on itself or when it relies on
 * configuration done by the bootloader.
 */
#define NAND_KEEP_TIMINGS	0x00800000

/* Options set by nand scan */
/* bbt has already been read */
#define NAND_BBT_SCANNED	0x40000000
/* Nand scan has allocated controller struct */
#define NAND_CONTROLLER_ALLOC	0x80000000

/* Cell info constants */
#define NAND_CI_CHIPNR_MSK	0x03
#define NAND_CI_CELLTYPE_MSK	0x0C
#define NAND_CI_CELLTYPE_SHIFT	2

/* ONFI features */
#define ONFI_FEATURE_16_BIT_BUS		(1 << 0)
#define ONFI_FEATURE_EXT_PARAM_PAGE	(1 << 7)

/* ONFI timing mode, used in both asynchronous and synchronous mode */
#define ONFI_TIMING_MODE_0		(1 << 0)
#define ONFI_TIMING_MODE_1		(1 << 1)
#define ONFI_TIMING_MODE_2		(1 << 2)
#define ONFI_TIMING_MODE_3		(1 << 3)
#define ONFI_TIMING_MODE_4		(1 << 4)
#define ONFI_TIMING_MODE_5		(1 << 5)
#define ONFI_TIMING_MODE_UNKNOWN	(1 << 6)

/* ONFI feature address */
#define ONFI_FEATURE_ADDR_TIMING_MODE	0x1

/* Vendor-specific feature address (Micron) */
#define ONFI_FEATURE_ADDR_READ_RETRY	0x89

/* ONFI subfeature parameters length */
#define ONFI_SUBFEATURE_PARAM_LEN	4

/* ONFI optional commands SET/GET FEATURES supported? */
#define ONFI_OPT_CMD_SET_GET_FEATURES	(1 << 2)

struct nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	u8 sig[4];
	__le16 revision;
	__le16 features;
	__le16 opt_cmd;
	u8 reserved0[2];
	__le16 ext_param_page_length; /* since ONFI 2.1 */
	u8 num_of_param_pages;        /* since ONFI 2.1 */
	u8 reserved1[17];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	u8 jedec_id;
	__le16 date_code;
	u8 reserved2[13];

	/* memory organization block */
	__le32 byte_per_page;
	__le16 spare_bytes_per_page;
	__le32 data_bytes_per_ppage;
	__le16 spare_bytes_per_ppage;
	__le32 pages_per_block;
	__le32 blocks_per_lun;
	u8 lun_count;
	u8 addr_cycles;
	u8 bits_per_cell;
	__le16 bb_per_lun;
	__le16 block_endurance;
	u8 guaranteed_good_blocks;
	__le16 guaranteed_block_endurance;
	u8 programs_per_page;
	u8 ppage_attr;
	u8 ecc_bits;
	u8 interleaved_bits;
	u8 interleaved_ops;
	u8 reserved3[13];

	/* electrical parameter block */
	u8 io_pin_capacitance_max;
	__le16 async_timing_mode;
	__le16 program_cache_timing_mode;
	__le16 t_prog;
	__le16 t_bers;
	__le16 t_r;
	__le16 t_ccs;
	__le16 src_sync_timing_mode;
	u8 src_ssync_features;
	__le16 clk_pin_capacitance_typ;
	__le16 io_pin_capacitance_typ;
	__le16 input_pin_capacitance_typ;
	u8 input_pin_capacitance_max;
	u8 driver_strength_support;
	__le16 t_int_r;
	__le16 t_adl;
	u8 reserved4[8];

	/* vendor */
	__le16 vendor_revision;
	u8 vendor[88];

	__le16 crc;
} __packed;

#define ONFI_CRC_BASE	0x4F4E

/* Extended ECC information Block Definition (since ONFI 2.1) */
struct onfi_ext_ecc_info {
	u8 ecc_bits;
	u8 codeword_size;
	__le16 bb_per_lun;
	__le16 block_endurance;
	u8 reserved[2];
} __packed;

#define ONFI_SECTION_TYPE_0	0	/* Unused section. */
#define ONFI_SECTION_TYPE_1	1	/* for additional sections. */
#define ONFI_SECTION_TYPE_2	2	/* for ECC information. */
struct onfi_ext_section {
	u8 type;
	u8 length;
} __packed;

#define ONFI_EXT_SECTION_MAX 8

/* Extended Parameter Page Definition (since ONFI 2.1) */
struct onfi_ext_param_page {
	__le16 crc;
	u8 sig[4];             /* 'E' 'P' 'P' 'S' */
	u8 reserved0[10];
	struct onfi_ext_section sections[ONFI_EXT_SECTION_MAX];

	/*
	 * The actual size of the Extended Parameter Page is in
	 * @ext_param_page_length of nand_onfi_params{}.
	 * The following are the variable length sections.
	 * So we do not add any fields below. Please see the ONFI spec.
	 */
} __packed;

struct jedec_ecc_info {
	u8 ecc_bits;
	u8 codeword_size;
	__le16 bb_per_lun;
	__le16 block_endurance;
	u8 reserved[2];
} __packed;

/* JEDEC features */
#define JEDEC_FEATURE_16_BIT_BUS	(1 << 0)

struct nand_jedec_params {
	/* rev info and features block */
	/* 'J' 'E' 'S' 'D'  */
	u8 sig[4];
	__le16 revision;
	__le16 features;
	u8 opt_cmd[3];
	__le16 sec_cmd;
	u8 num_of_param_pages;
	u8 reserved0[18];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	u8 jedec_id[6];
	u8 reserved1[10];

	/* memory organization block */
	__le32 byte_per_page;
	__le16 spare_bytes_per_page;
	u8 reserved2[6];
	__le32 pages_per_block;
	__le32 blocks_per_lun;
	u8 lun_count;
	u8 addr_cycles;
	u8 bits_per_cell;
	u8 programs_per_page;
	u8 multi_plane_addr;
	u8 multi_plane_op_attr;
	u8 reserved3[38];

	/* electrical parameter block */
	__le16 async_sdr_speed_grade;
	__le16 toggle_ddr_speed_grade;
	__le16 sync_ddr_speed_grade;
	u8 async_sdr_features;
	u8 toggle_ddr_features;
	u8 sync_ddr_features;
	__le16 t_prog;
	__le16 t_bers;
	__le16 t_r;
	__le16 t_r_multi_plane;
	__le16 t_ccs;
	__le16 io_pin_capacitance_typ;
	__le16 input_pin_capacitance_typ;
	__le16 clk_pin_capacitance_typ;
	u8 driver_strength_support;
	__le16 t_adl;
	u8 reserved4[36];

	/* ECC and endurance block */
	u8 guaranteed_good_blocks;
	__le16 guaranteed_block_endurance;
	struct jedec_ecc_info ecc_info[4];
	u8 reserved5[29];

	/* reserved */
	u8 reserved6[148];

	/* vendor */
	__le16 vendor_rev_num;
	u8 reserved7[88];

	/* CRC for Parameter Page */
	__le16 crc;
} __packed;

/**
 * struct nand_hw_control - Control structure for hardware controller (e.g ECC generator) shared among independent devices
 * @lock:               protection lock
 * @active:		the mtd device which holds the controller currently
 * @wq:			wait queue to sleep on if a NAND operation is in
 *			progress used instead of the per chip wait queue
 *			when a hw controller is available.
 */
struct nand_hw_control {
	spinlock_t lock;
	struct nand_chip *active;
};

static inline void nand_hw_control_init(struct nand_hw_control *nfc)
{
	nfc->active = NULL;
	spin_lock_init(&nfc->lock);
	init_waitqueue_head(&nfc->wq);
}

/* The maximum expected count of bytes in the NAND ID sequence */
#define NAND_MAX_ID_LEN 8

/**
 * struct nand_id - NAND id structure
 * @data: buffer containing the id bytes.
 * @len: ID length.
 */
struct nand_id {
	u8 data[NAND_MAX_ID_LEN];
	int len;
};

/**
 * struct nand_ecc_step_info - ECC step information of ECC engine
 * @stepsize: data bytes per ECC step
 * @strengths: array of supported strengths
 * @nstrengths: number of supported strengths
 */
struct nand_ecc_step_info {
	int stepsize;
	const int *strengths;
	int nstrengths;
};

/**
 * struct nand_ecc_caps - capability of ECC engine
 * @stepinfos: array of ECC step information
 * @nstepinfos: number of ECC step information
 * @calc_ecc_bytes: driver's hook to calculate ECC bytes per step
 */
struct nand_ecc_caps {
	const struct nand_ecc_step_info *stepinfos;
	int nstepinfos;
	int (*calc_ecc_bytes)(int step_size, int strength);
};

/* a shorthand to generate struct nand_ecc_caps with only one ECC stepsize */
#define NAND_ECC_CAPS_SINGLE(__name, __calc, __step, ...)	\
static const int __name##_strengths[] = { __VA_ARGS__ };	\
static const struct nand_ecc_step_info __name##_stepinfo = {	\
	.stepsize = __step,					\
	.strengths = __name##_strengths,			\
	.nstrengths = ARRAY_SIZE(__name##_strengths),		\
};								\
static const struct nand_ecc_caps __name = {			\
	.stepinfos = &__name##_stepinfo,			\
	.nstepinfos = 1,					\
	.calc_ecc_bytes = __calc,				\
}

/**
 * struct nand_ecc_ctrl - Control structure for ECC
 * @mode:	ECC mode
 * @algo:	ECC algorithm
 * @steps:	number of ECC steps per page
 * @size:	data bytes per ECC step
 * @bytes:	ECC bytes per step
 * @strength:	max number of correctible bits per ECC step
 * @total:	total number of ECC bytes per page
 * @prepad:	padding information for syndrome based ECC generators
 * @postpad:	padding information for syndrome based ECC generators
 * @options:	ECC specific options (see NAND_ECC_XXX flags defined above)
 * @layout:	ECC layout control struct pointer
 * @priv:	pointer to private ECC control data
 * @hwctl:	function to control hardware ECC generator. Must only
 *		be provided if an hardware ECC is available
 * @calculate:	function for ECC calculation or readback from ECC hardware
 * @correct:	function for ECC correction, matching to ECC generator (sw/hw).
 *		Should return a positive number representing the number of
 *		corrected bitflips, -EBADMSG if the number of bitflips exceed
 *		ECC strength, or any other error code if the error is not
 *		directly related to correction.
 *		If -EBADMSG is returned the input buffers should be left
 *		untouched.
 * @read_page_raw:	function to read a raw page without ECC. This function
 *			should hide the specific layout used by the ECC
 *			controller and always return contiguous in-band and
 *			out-of-band data even if they're not stored
 *			contiguously on the NAND chip (e.g.
 *			NAND_ECC_HW_SYNDROME interleaves in-band and
 *			out-of-band data).
 * @write_page_raw:	function to write a raw page without ECC. This function
 *			should hide the specific layout used by the ECC
 *			controller and consider the passed data as contiguous
 *			in-band and out-of-band data. ECC controller is
 *			responsible for doing the appropriate transformations
 *			to adapt to its specific layout (e.g.
 *			NAND_ECC_HW_SYNDROME interleaves in-band and
 *			out-of-band data).
 * @read_page:	function to read a page according to the ECC generator
 *		requirements; returns maximum number of bitflips corrected in
 *		any single ECC step, 0 if bitflips uncorrectable, -EIO hw error
 * @read_subpage:	function to read parts of the page covered by ECC;
 *			returns same as read_page()
 * @write_subpage:	function to write parts of the page covered by ECC.
 * @write_page:	function to write a page according to the ECC generator
 *		requirements.
 * @write_oob_raw:	function to write chip OOB data without ECC
 * @read_oob_raw:	function to read chip OOB data without ECC
 * @read_oob:	function to read chip OOB data
 * @write_oob:	function to write chip OOB data
 */
struct nand_ecc_ctrl {
	nand_ecc_modes_t mode;
	enum nand_ecc_algo algo;
	int steps;
	int size;
	int bytes;
	int total;
	int strength;
	int prepad;
	int postpad;
	unsigned int options;
	struct nand_ecclayout	*layout;
	void *priv;
	void (*hwctl)(struct mtd_info *mtd, int mode);
	int (*calculate)(struct mtd_info *mtd, const uint8_t *dat,
			uint8_t *ecc_code);
	int (*correct)(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc,
			uint8_t *calc_ecc);
	int (*read_page_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			uint8_t *buf, int oob_required, int page);
	int (*write_page_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			const uint8_t *buf, int oob_required, int page);
	int (*read_page)(struct mtd_info *mtd, struct nand_chip *chip,
			uint8_t *buf, int oob_required, int page);
	int (*read_subpage)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offs, uint32_t len, uint8_t *buf, int page);
	int (*write_subpage)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offset, uint32_t data_len,
			const uint8_t *data_buf, int oob_required, int page);
	int (*write_page)(struct mtd_info *mtd, struct nand_chip *chip,
			const uint8_t *buf, int oob_required, int page);
	int (*write_oob_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
	int (*read_oob_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
	int (*read_oob)(struct mtd_info *mtd, struct nand_chip *chip, int page);
	int (*write_oob)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
};

static inline int nand_standard_page_accessors(struct nand_ecc_ctrl *ecc)
{
	return !(ecc->options & NAND_ECC_CUSTOM_PAGE_ACCESS);
}

/**
 * struct nand_buffers - buffer structure for read/write
 * @ecccalc:	buffer pointer for calculated ECC, size is oobsize.
 * @ecccode:	buffer pointer for ECC read from flash, size is oobsize.
 * @databuf:	buffer pointer for data, size is (page size + oobsize).
 *
 * Do not change the order of buffers. databuf and oobrbuf must be in
 * consecutive order.
 */
struct nand_buffers {
	uint8_t	ecccalc[ALIGN(NAND_MAX_OOBSIZE, ARCH_DMA_MINALIGN)];
	uint8_t	ecccode[ALIGN(NAND_MAX_OOBSIZE, ARCH_DMA_MINALIGN)];
	uint8_t databuf[ALIGN(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE,
			      ARCH_DMA_MINALIGN)];
};

/**
 * struct nand_sdr_timings - SDR NAND chip timings
 *
 * This struct defines the timing requirements of a SDR NAND chip.
 * These information can be found in every NAND datasheets and the timings
 * meaning are described in the ONFI specifications:
 * www.onfi.org/~/media/ONFI/specs/onfi_3_1_spec.pdf (chapter 4.15 Timing
 * Parameters)
 *
 * All these timings are expressed in picoseconds.
 *
 * @tBERS_max: Block erase time
 * @tCCS_min: Change column setup time
 * @tPROG_max: Page program time
 * @tR_max: Page read time
 * @tALH_min: ALE hold time
 * @tADL_min: ALE to data loading time
 * @tALS_min: ALE setup time
 * @tAR_min: ALE to RE# delay
 * @tCEA_max: CE# access time
 * @tCEH_min: CE# high hold time
 * @tCH_min:  CE# hold time
 * @tCHZ_max: CE# high to output hi-Z
 * @tCLH_min: CLE hold time
 * @tCLR_min: CLE to RE# delay
 * @tCLS_min: CLE setup time
 * @tCOH_min: CE# high to output hold
 * @tCS_min: CE# setup time
 * @tDH_min: Data hold time
 * @tDS_min: Data setup time
 * @tFEAT_max: Busy time for Set Features and Get Features
 * @tIR_min: Output hi-Z to RE# low
 * @tITC_max: Interface and Timing Mode Change time
 * @tRC_min: RE# cycle time
 * @tREA_max: RE# access time
 * @tREH_min: RE# high hold time
 * @tRHOH_min: RE# high to output hold
 * @tRHW_min: RE# high to WE# low
 * @tRHZ_max: RE# high to output hi-Z
 * @tRLOH_min: RE# low to output hold
 * @tRP_min: RE# pulse width
 * @tRR_min: Ready to RE# low (data only)
 * @tRST_max: Device reset time, measured from the falling edge of R/B# to the
 *	      rising edge of R/B#.
 * @tWB_max: WE# high to SR[6] low
 * @tWC_min: WE# cycle time
 * @tWH_min: WE# high hold time
 * @tWHR_min: WE# high to RE# low
 * @tWP_min: WE# pulse width
 * @tWW_min: WP# transition to WE# low
 */
struct nand_sdr_timings {
	u64 tBERS_max;
	u32 tCCS_min;
	u64 tPROG_max;
	u64 tR_max;
	u32 tALH_min;
	u32 tADL_min;
	u32 tALS_min;
	u32 tAR_min;
	u32 tCEA_max;
	u32 tCEH_min;
	u32 tCH_min;
	u32 tCHZ_max;
	u32 tCLH_min;
	u32 tCLR_min;
	u32 tCLS_min;
	u32 tCOH_min;
	u32 tCS_min;
	u32 tDH_min;
	u32 tDS_min;
	u32 tFEAT_max;
	u32 tIR_min;
	u32 tITC_max;
	u32 tRC_min;
	u32 tREA_max;
	u32 tREH_min;
	u32 tRHOH_min;
	u32 tRHW_min;
	u32 tRHZ_max;
	u32 tRLOH_min;
	u32 tRP_min;
	u32 tRR_min;
	u64 tRST_max;
	u32 tWB_max;
	u32 tWC_min;
	u32 tWH_min;
	u32 tWHR_min;
	u32 tWP_min;
	u32 tWW_min;
};

/**
 * enum nand_data_interface_type - NAND interface timing type
 * @NAND_SDR_IFACE:	Single Data Rate interface
 */
enum nand_data_interface_type {
	NAND_SDR_IFACE,
};

/**
 * struct nand_data_interface - NAND interface timing
 * @type:	type of the timing
 * @timings:	The timing, type according to @type
 */
struct nand_data_interface {
	enum nand_data_interface_type type;
	union {
		struct nand_sdr_timings sdr;
	} timings;
};

/**
 * nand_get_sdr_timings - get SDR timing from data interface
 * @conf:	The data interface
 */
static inline const struct nand_sdr_timings *
nand_get_sdr_timings(const struct nand_data_interface *conf)
{
	if (conf->type != NAND_SDR_IFACE)
		return ERR_PTR(-EINVAL);

	return &conf->timings.sdr;
}

/**
 * struct nand_manufacturer_ops - NAND Manufacturer operations
 * @detect: detect the NAND memory organization and capabilities
 * @init: initialize all vendor specific fields (like the ->read_retry()
 *	  implementation) if any.
 */
struct nand_manufacturer_ops {
	void (*detect)(struct nand_chip *chip);
	int (*init)(struct nand_chip *chip);
};

/**
 * struct nand_chip - NAND Private Flash Chip Data
 * @mtd:		MTD device registered to the MTD framework
 * @IO_ADDR_R:		[BOARDSPECIFIC] address to read the 8 I/O lines of the
 *			flash device
 * @IO_ADDR_W:		[BOARDSPECIFIC] address to write the 8 I/O lines of the
 *			flash device.
 * @flash_node:		[BOARDSPECIFIC] device node describing this instance
 * @read_byte:		[REPLACEABLE] read one byte from the chip
 * @read_word:		[REPLACEABLE] read one word from the chip
 * @write_byte:		[REPLACEABLE] write a single byte to the chip on the
 *			low 8 I/O lines
 * @write_buf:		[REPLACEABLE] write data from the buffer to the chip
 * @read_buf:		[REPLACEABLE] read data from the chip into the buffer
 * @select_chip:	[REPLACEABLE] select chip nr
 * @block_bad:		[REPLACEABLE] check if a block is bad, using OOB markers
 * @block_markbad:	[REPLACEABLE] mark a block bad
 * @cmd_ctrl:		[BOARDSPECIFIC] hardwarespecific function for controlling
 *			ALE/CLE/nCE. Also used to write command and address
 * @dev_ready:		[BOARDSPECIFIC] hardwarespecific function for accessing
 *			device ready/busy line. If set to NULL no access to
 *			ready/busy is available and the ready/busy information
 *			is read from the chip status register.
 * @cmdfunc:		[REPLACEABLE] hardwarespecific function for writing
 *			commands to the chip.
 * @waitfunc:		[REPLACEABLE] hardwarespecific function for wait on
 *			ready.
 * @setup_read_retry:	[FLASHSPECIFIC] flash (vendor) specific function for
 *			setting the read-retry mode. Mostly needed for MLC NAND.
 * @ecc:		[BOARDSPECIFIC] ECC control structure
 * @buffers:		buffer structure for read/write
 * @buf_align:		minimum buffer alignment required by a platform
 * @hwcontrol:		platform-specific hardware control structure
 * @erase:		[REPLACEABLE] erase function
 * @scan_bbt:		[REPLACEABLE] function to scan bad block table
 * @chip_delay:		[BOARDSPECIFIC] chip dependent delay for transferring
 *			data from array to read regs (tR).
 * @state:		[INTERN] the current state of the NAND device
 * @oob_poi:		"poison value buffer," used for laying out OOB data
 *			before writing
 * @page_shift:		[INTERN] number of address bits in a page (column
 *			address bits).
 * @phys_erase_shift:	[INTERN] number of address bits in a physical eraseblock
 * @bbt_erase_shift:	[INTERN] number of address bits in a bbt entry
 * @chip_shift:		[INTERN] number of address bits in one chip
 * @options:		[BOARDSPECIFIC] various chip options. They can partly
 *			be set to inform nand_scan about special functionality.
 *			See the defines for further explanation.
 * @bbt_options:	[INTERN] bad block specific options. All options used
 *			here must come from bbm.h. By default, these options
 *			will be copied to the appropriate nand_bbt_descr's.
 * @badblockpos:	[INTERN] position of the bad block marker in the oob
 *			area.
 * @badblockbits:	[INTERN] minimum number of set bits in a good block's
 *			bad block marker position; i.e., BBM == 11110111b is
 *			not bad when badblockbits == 7
 * @bits_per_cell:	[INTERN] number of bits per cell. i.e., 1 means SLC.
 * @ecc_strength_ds:	[INTERN] ECC correctability from the datasheet.
 *			Minimum amount of bit errors per @ecc_step_ds guaranteed
 *			to be correctable. If unknown, set to zero.
 * @ecc_step_ds:	[INTERN] ECC step required by the @ecc_strength_ds,
 *                      also from the datasheet. It is the recommended ECC step
 *			size, if known; if unknown, set to zero.
 * @onfi_timing_mode_default: [INTERN] default ONFI timing mode. This field is
 *			      set to the actually used ONFI mode if the chip is
 *			      ONFI compliant or deduced from the datasheet if
 *			      the NAND chip is not ONFI compliant.
 * @numchips:		[INTERN] number of physical chips
 * @chipsize:		[INTERN] the size of one chip for multichip arrays
 * @pagemask:		[INTERN] page number mask = number of (pages / chip) - 1
 * @pagebuf:		[INTERN] holds the pagenumber which is currently in
 *			data_buf.
 * @pagebuf_bitflips:	[INTERN] holds the bitflip count for the page which is
 *			currently in data_buf.
 * @subpagesize:	[INTERN] holds the subpagesize
 * @onfi_version:	[INTERN] holds the chip ONFI version (BCD encoded),
 *			non 0 if ONFI supported.
 * @jedec_version:	[INTERN] holds the chip JEDEC version (BCD encoded),
 *			non 0 if JEDEC supported.
 * @onfi_params:	[INTERN] holds the ONFI page parameter when ONFI is
 *			supported, 0 otherwise.
 * @jedec_params:	[INTERN] holds the JEDEC parameter page when JEDEC is
 *			supported, 0 otherwise.
 * @read_retries:	[INTERN] the number of read retry modes supported
 * @onfi_set_features:	[REPLACEABLE] set the features for ONFI nand
 * @onfi_get_features:	[REPLACEABLE] get the features for ONFI nand
 * @setup_data_interface: [OPTIONAL] setup the data interface and timing. If
 *			  chipnr is set to %NAND_DATA_IFACE_CHECK_ONLY this
 *			  means the configuration should not be applied but
 *			  only checked.
 * @bbt:		[INTERN] bad block table pointer
 * @bbt_td:		[REPLACEABLE] bad block table descriptor for flash
 *			lookup.
 * @bbt_md:		[REPLACEABLE] bad block table mirror descriptor
 * @badblock_pattern:	[REPLACEABLE] bad block scan pattern used for initial
 *			bad block scan.
 * @controller:		[REPLACEABLE] a pointer to a hardware controller
 *			structure which is shared among multiple independent
 *			devices.
 * @priv:		[OPTIONAL] pointer to private chip data
 * @write_page:		[REPLACEABLE] High-level page write function
 * @manufacturer:	[INTERN] Contains manufacturer information
 */

struct nand_chip {
	struct mtd_info mtd;
	struct nand_id id;

	void __iomem *IO_ADDR_R;
	void __iomem *IO_ADDR_W;

	ofnode flash_node;

	uint8_t (*read_byte)(struct mtd_info *mtd);
	u16 (*read_word)(struct mtd_info *mtd);
	void (*write_byte)(struct mtd_info *mtd, uint8_t byte);
	void (*write_buf)(struct mtd_info *mtd, const uint8_t *buf, int len);
	void (*read_buf)(struct mtd_info *mtd, uint8_t *buf, int len);
	void (*select_chip)(struct mtd_info *mtd, int chip);
	int (*block_bad)(struct mtd_info *mtd, loff_t ofs);
	int (*block_markbad)(struct mtd_info *mtd, loff_t ofs);
	void (*cmd_ctrl)(struct mtd_info *mtd, int dat, unsigned int ctrl);
	int (*dev_ready)(struct mtd_info *mtd);
	void (*cmdfunc)(struct mtd_info *mtd, unsigned command, int column,
			int page_addr);
	int(*waitfunc)(struct mtd_info *mtd, struct nand_chip *this);
	int (*erase)(struct mtd_info *mtd, int page);
	int (*scan_bbt)(struct mtd_info *mtd);
	int (*write_page)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offset, int data_len, const uint8_t *buf,
			int oob_required, int page, int raw);
	int (*onfi_set_features)(struct mtd_info *mtd, struct nand_chip *chip,
			int feature_addr, uint8_t *subfeature_para);
	int (*onfi_get_features)(struct mtd_info *mtd, struct nand_chip *chip,
			int feature_addr, uint8_t *subfeature_para);
	int (*setup_read_retry)(struct mtd_info *mtd, int retry_mode);
	int (*setup_data_interface)(struct mtd_info *mtd, int chipnr,
				    const struct nand_data_interface *conf);

	int chip_delay;
	unsigned int options;
	unsigned int bbt_options;

	int page_shift;
	int phys_erase_shift;
	int bbt_erase_shift;
	int chip_shift;
	int numchips;
	uint64_t chipsize;
	int pagemask;
	int pagebuf;
	unsigned int pagebuf_bitflips;
	int subpagesize;
	uint8_t bits_per_cell;
	uint16_t ecc_strength_ds;
	uint16_t ecc_step_ds;
	int onfi_timing_mode_default;
	int badblockpos;
	int badblockbits;

	int onfi_version;
	int jedec_version;
	struct nand_onfi_params	onfi_params;
	struct nand_jedec_params jedec_params;

	struct nand_data_interface *data_interface;

	int read_retries;

	flstate_t state;

	uint8_t *oob_poi;
	struct nand_hw_control *controller;
	struct nand_ecclayout *ecclayout;

	struct nand_ecc_ctrl ecc;
	struct nand_buffers *buffers;
	unsigned long buf_align;
	struct nand_hw_control hwcontrol;

	uint8_t *bbt;
	struct nand_bbt_descr *bbt_td;
	struct nand_bbt_descr *bbt_md;

	struct nand_bbt_descr *badblock_pattern;
	int cur_cs;

	void *priv;

	struct {
		const struct nand_manufacturer *desc;
		void *priv;
	} manufacturer;
};

static inline void nand_set_flash_node(struct nand_chip *chip,
				       ofnode node)
{
	chip->flash_node = node;
}

static inline ofnode nand_get_flash_node(struct nand_chip *chip)
{
	return chip->flash_node;
}

static inline struct nand_chip *mtd_to_nand(struct mtd_info *mtd)
{
	return container_of(mtd, struct nand_chip, mtd);
}

static inline struct mtd_info *nand_to_mtd(struct nand_chip *chip)
{
	return &chip->mtd;
}

static inline void *nand_get_controller_data(struct nand_chip *chip)
{
	return chip->priv;
}

static inline void nand_set_controller_data(struct nand_chip *chip, void *priv)
{
	chip->priv = priv;
}

static inline void nand_set_manufacturer_data(struct nand_chip *chip,
					      void *priv)
{
	chip->manufacturer.priv = priv;
}

static inline void *nand_get_manufacturer_data(struct nand_chip *chip)
{
	return chip->manufacturer.priv;
}

/*
 * NAND Flash Manufacturer ID Codes
 */
#define NAND_MFR_TOSHIBA	0x98
#define NAND_MFR_SAMSUNG	0xec
#define NAND_MFR_FUJITSU	0x04
#define NAND_MFR_NATIONAL	0x8f
#define NAND_MFR_RENESAS	0x07
#define NAND_MFR_STMICRO	0x20
#define NAND_MFR_HYNIX		0xad
#define NAND_MFR_MICRON		0x2c
#define NAND_MFR_AMD		0x01
#define NAND_MFR_MACRONIX	0xc2
#define NAND_MFR_EON		0x92
#define NAND_MFR_SANDISK	0x45
#define NAND_MFR_INTEL		0x89
#define NAND_MFR_ATO		0x9b

/* The maximum expected count of bytes in the NAND ID sequence */
#define NAND_MAX_ID_LEN 8

/*
 * A helper for defining older NAND chips where the second ID byte fully
 * defined the chip, including the geometry (chip size, eraseblock size, page
 * size). All these chips have 512 bytes NAND page size.
 */
#define LEGACY_ID_NAND(nm, devid, chipsz, erasesz, opts)          \
	{ .name = (nm), {{ .dev_id = (devid) }}, .pagesize = 512, \
	  .chipsize = (chipsz), .erasesize = (erasesz), .options = (opts) }

/*
 * A helper for defining newer chips which report their page size and
 * eraseblock size via the extended ID bytes.
 *
 * The real difference between LEGACY_ID_NAND and EXTENDED_ID_NAND is that with
 * EXTENDED_ID_NAND, manufacturers overloaded the same device ID so that the
 * device ID now only represented a particular total chip size (and voltage,
 * buswidth), and the page size, eraseblock size, and OOB size could vary while
 * using the same device ID.
 */
#define EXTENDED_ID_NAND(nm, devid, chipsz, opts)                      \
	{ .name = (nm), {{ .dev_id = (devid) }}, .chipsize = (chipsz), \
	  .options = (opts) }

#define NAND_ECC_INFO(_strength, _step)	\
			{ .strength_ds = (_strength), .step_ds = (_step) }
#define NAND_ECC_STRENGTH(type)		((type)->ecc.strength_ds)
#define NAND_ECC_STEP(type)		((type)->ecc.step_ds)

/**
 * struct nand_flash_dev - NAND Flash Device ID Structure
 * @name: a human-readable name of the NAND chip
 * @dev_id: the device ID (the second byte of the full chip ID array)
 * @mfr_id: manufecturer ID part of the full chip ID array (refers the same
 *          memory address as @id[0])
 * @dev_id: device ID part of the full chip ID array (refers the same memory
 *          address as @id[1])
 * @id: full device ID array
 * @pagesize: size of the NAND page in bytes; if 0, then the real page size (as
 *            well as the eraseblock size) is determined from the extended NAND
 *            chip ID array)
 * @chipsize: total chip size in MiB
 * @erasesize: eraseblock size in bytes (determined from the extended ID if 0)
 * @options: stores various chip bit options
 * @id_len: The valid length of the @id.
 * @oobsize: OOB size
 * @ecc: ECC correctability and step information from the datasheet.
 * @ecc.strength_ds: The ECC correctability from the datasheet, same as the
 *                   @ecc_strength_ds in nand_chip{}.
 * @ecc.step_ds: The ECC step required by the @ecc.strength_ds, same as the
 *               @ecc_step_ds in nand_chip{}, also from the datasheet.
 *               For example, the "4bit ECC for each 512Byte" can be set with
 *               NAND_ECC_INFO(4, 512).
 * @onfi_timing_mode_default: the default ONFI timing mode entered after a NAND
 *			      reset. Should be deduced from timings described
 *			      in the datasheet.
 *
 */
struct nand_flash_dev {
	char *name;
	union {
		struct {
			uint8_t mfr_id;
			uint8_t dev_id;
		};
		uint8_t id[NAND_MAX_ID_LEN];
	};
	unsigned int pagesize;
	unsigned int chipsize;
	unsigned int erasesize;
	unsigned int options;
	uint16_t id_len;
	uint16_t oobsize;
	struct {
		uint16_t strength_ds;
		uint16_t step_ds;
	} ecc;
	int onfi_timing_mode_default;
};

/**
 * struct nand_manufacturer - NAND Flash Manufacturer ID Structure
 * @name:	Manufacturer name
 * @id:		manufacturer ID code of device.
 * @ops:	manufacturer operations
*/
struct nand_manufacturer {
	int id;
	char *name;
	const struct nand_manufacturer_ops *ops;
};

extern struct nand_flash_dev nand_flash_ids[];
extern struct nand_manufacturer nand_manuf_ids[];

extern const struct nand_manufacturer_ops toshiba_nand_manuf_ops;
extern const struct nand_manufacturer_ops samsung_nand_manuf_ops;
extern const struct nand_manufacturer_ops hynix_nand_manuf_ops;
extern const struct nand_manufacturer_ops micron_nand_manuf_ops;
extern const struct nand_manufacturer_ops amd_nand_manuf_ops;
extern const struct nand_manufacturer_ops macronix_nand_manuf_ops;

int nand_default_bbt(struct mtd_info *mtd);
int nand_markbad_bbt(struct mtd_info *mtd, loff_t offs);
int nand_isreserved_bbt(struct mtd_info *mtd, loff_t offs);
int nand_isbad_bbt(struct mtd_info *mtd, loff_t offs, int allowbbt);
int nand_erase_nand(struct mtd_info *mtd, struct erase_info *instr,
			   int allowbbt);
int nand_do_read(struct mtd_info *mtd, loff_t from, size_t len,
			size_t *retlen, uint8_t *buf);

/*
* Constants for oob configuration
*/
#define NAND_SMALL_BADBLOCK_POS		5
#define NAND_LARGE_BADBLOCK_POS		0

/**
 * struct platform_nand_chip - chip level device structure
 * @nr_chips:		max. number of chips to scan for
 * @chip_offset:	chip number offset
 * @nr_partitions:	number of partitions pointed to by partitions (or zero)
 * @partitions:		mtd partition list
 * @chip_delay:		R/B delay value in us
 * @options:		Option flags, e.g. 16bit buswidth
 * @bbt_options:	BBT option flags, e.g. NAND_BBT_USE_FLASH
 * @part_probe_types:	NULL-terminated array of probe types
 */
struct platform_nand_chip {
	int nr_chips;
	int chip_offset;
	int nr_partitions;
	struct mtd_partition *partitions;
	int chip_delay;
	unsigned int options;
	unsigned int bbt_options;
	const char **part_probe_types;
};

/* Keep gcc happy */
struct platform_device;

/**
 * struct platform_nand_ctrl - controller level device structure
 * @probe:		platform specific function to probe/setup hardware
 * @remove:		platform specific function to remove/teardown hardware
 * @hwcontrol:		platform specific hardware control structure
 * @dev_ready:		platform specific function to read ready/busy pin
 * @select_chip:	platform specific chip select function
 * @cmd_ctrl:		platform specific function for controlling
 *			ALE/CLE/nCE. Also used to write command and address
 * @write_buf:		platform specific function for write buffer
 * @read_buf:		platform specific function for read buffer
 * @read_byte:		platform specific function to read one byte from chip
 * @priv:		private data to transport driver specific settings
 *
 * All fields are optional and depend on the hardware driver requirements
 */
struct platform_nand_ctrl {
	int (*probe)(struct platform_device *pdev);
	void (*remove)(struct platform_device *pdev);
	void (*hwcontrol)(struct mtd_info *mtd, int cmd);
	int (*dev_ready)(struct mtd_info *mtd);
	void (*select_chip)(struct mtd_info *mtd, int chip);
	void (*cmd_ctrl)(struct mtd_info *mtd, int dat, unsigned int ctrl);
	void (*write_buf)(struct mtd_info *mtd, const uint8_t *buf, int len);
	void (*read_buf)(struct mtd_info *mtd, uint8_t *buf, int len);
	unsigned char (*read_byte)(struct mtd_info *mtd);
	void *priv;
};

/**
 * struct platform_nand_data - container structure for platform-specific data
 * @chip:		chip level chip structure
 * @ctrl:		controller level device structure
 */
struct platform_nand_data {
	struct platform_nand_chip chip;
	struct platform_nand_ctrl ctrl;
};

#ifdef CONFIG_SYS_NAND_ONFI_DETECTION
/* return the supported features. */
static inline int onfi_feature(struct nand_chip *chip)
{
	return chip->onfi_version ? le16_to_cpu(chip->onfi_params.features) : 0;
}

/* return the supported asynchronous timing mode. */
static inline int onfi_get_async_timing_mode(struct nand_chip *chip)
{
	if (!chip->onfi_version)
		return ONFI_TIMING_MODE_UNKNOWN;
	return le16_to_cpu(chip->onfi_params.async_timing_mode);
}

/* return the supported synchronous timing mode. */
static inline int onfi_get_sync_timing_mode(struct nand_chip *chip)
{
	if (!chip->onfi_version)
		return ONFI_TIMING_MODE_UNKNOWN;
	return le16_to_cpu(chip->onfi_params.src_sync_timing_mode);
}
#else
static inline int onfi_feature(struct nand_chip *chip)
{
	return 0;
}

static inline int onfi_get_async_timing_mode(struct nand_chip *chip)
{
	return ONFI_TIMING_MODE_UNKNOWN;
}

static inline int onfi_get_sync_timing_mode(struct nand_chip *chip)
{
	return ONFI_TIMING_MODE_UNKNOWN;
}
#endif

int onfi_init_data_interface(struct nand_chip *chip,
			     struct nand_data_interface *iface,
			     enum nand_data_interface_type type,
			     int timing_mode);

/*
 * Check if it is a SLC nand.
 * The !nand_is_slc() can be used to check the MLC/TLC nand chips.
 * We do not distinguish the MLC and TLC now.
 */
static inline bool nand_is_slc(struct nand_chip *chip)
{
	return chip->bits_per_cell == 1;
}

/**
 * Check if the opcode's address should be sent only on the lower 8 bits
 * @command: opcode to check
 */
static inline int nand_opcode_8bits(unsigned int command)
{
	switch (command) {
	case NAND_CMD_READID:
	case NAND_CMD_PARAM:
	case NAND_CMD_GET_FEATURES:
	case NAND_CMD_SET_FEATURES:
		return 1;
	default:
		break;
	}
	return 0;
}

/* return the supported JEDEC features. */
static inline int jedec_feature(struct nand_chip *chip)
{
	return chip->jedec_version ? le16_to_cpu(chip->jedec_params.features)
		: 0;
}

/* Standard NAND functions from nand_base.c */
void nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len);
void nand_write_buf16(struct mtd_info *mtd, const uint8_t *buf, int len);
void nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len);
void nand_read_buf16(struct mtd_info *mtd, uint8_t *buf, int len);
uint8_t nand_read_byte(struct mtd_info *mtd);

/* get timing characteristics from ONFI timing mode. */
const struct nand_sdr_timings *onfi_async_timing_mode_to_sdr_timings(int mode);
/* get data interface from ONFI timing mode 0, used after reset. */
const struct nand_data_interface *nand_get_default_data_interface(void);

int nand_check_erased_ecc_chunk(void *data, int datalen,
				void *ecc, int ecclen,
				void *extraoob, int extraooblen,
				int threshold);

int nand_check_ecc_caps(struct nand_chip *chip,
			const struct nand_ecc_caps *caps, int oobavail);

int nand_match_ecc_req(struct nand_chip *chip,
		       const struct nand_ecc_caps *caps,  int oobavail);

int nand_maximize_ecc(struct nand_chip *chip,
		      const struct nand_ecc_caps *caps, int oobavail);

/* Reset and initialize a NAND device */
int nand_reset(struct nand_chip *chip, int chipnr);

/* NAND operation helpers */
int nand_reset_op(struct nand_chip *chip);
int nand_readid_op(struct nand_chip *chip, u8 addr, void *buf,
		   unsigned int len);
int nand_status_op(struct nand_chip *chip, u8 *status);
int nand_exit_status_op(struct nand_chip *chip);
int nand_erase_op(struct nand_chip *chip, unsigned int eraseblock);
int nand_read_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, void *buf, unsigned int len);
int nand_change_read_column_op(struct nand_chip *chip,
			       unsigned int offset_in_page, void *buf,
			       unsigned int len, bool force_8bit);
int nand_read_oob_op(struct nand_chip *chip, unsigned int page,
		     unsigned int offset_in_page, void *buf, unsigned int len);
int nand_prog_page_begin_op(struct nand_chip *chip, unsigned int page,
			    unsigned int offset_in_page, const void *buf,
			    unsigned int len);
int nand_prog_page_end_op(struct nand_chip *chip);
int nand_prog_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, const void *buf,
		      unsigned int len);
int nand_change_write_column_op(struct nand_chip *chip,
				unsigned int offset_in_page, const void *buf,
				unsigned int len, bool force_8bit);
int nand_read_data_op(struct nand_chip *chip, void *buf, unsigned int len,
		      bool force_8bit);
int nand_write_data_op(struct nand_chip *chip, const void *buf,
		       unsigned int len, bool force_8bit);

/* Default extended ID decoding function */
void nand_decode_ext_id(struct nand_chip *chip);

#endif /* __LINUX_MTD_RAWNAND_H */
