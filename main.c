/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file 		fmapi.c
 *
 * @brief 		Code file for CXL Fabric Management API commands
 *
 * @details 	As per CXL specification, all registers are little-endian
 *
 * @copyright 	Copyright (C) 2024 Jackrabbit Founders LLC. All rights reserved.
 *
 * @date 		Jan 2024
 * @author 		Barrett Edwards <code@jrlabs.io>
 *
 */
/* INCLUDES ==================================================================*/

/* printf()
 */
#include <stdio.h>

/* Return error codes from functions
 */
#include <errno.h>

/* memcpy()
 */
#include <string.h>

#include "main.h"

/* MACROS ====================================================================*/

#define MCMT_CXLCCI 	0x08

/* ENUMERATIONS ==============================================================*/

/* STRUCTS ===================================================================*/

/* GLOBAL VARIABLES ==========================================================*/

/**
 * String representations of FM API Command Message types (MT)
 * 
 * CXL 2.0 v1.0 Table 84
 */
const char *STR_FMMT[] = {
	"Request",		// FMMT_REQ		= 0
	"Response",		// FMMT_RESP	= 1
};

const char *STR_FMOP_1[] = {
	"Identify Switch Device",					// FMOP_PSC_ID					= 0x5100,
	"Get Physical Port State",					// FMOP_PSC_PORT				= 0x5101,
	"Physical Port Control",					// FMOP_PSC_PORT_CTRL			= 0x5102,
	"Send PPB CXL.io Configuration Request"		// FMOP_PSC_CFG					= 0x5103,
};

const char *STR_FMOP_2[] = {
	"Get Virtual CXL Switch Info",				// FMOP_VSC_INFO				= 0x5200,
	"Bind vPPB",								// FMOP_VSC_BIND				= 0x5201,
	"Unbind vPPB",								// FMOP_VSC_UNBIND				= 0x5202,
	"Generate AER Event"						// FMOP_VSC_AER					= 0x5203,
};

const char *STR_FMOP_3[] = {
	"Tunnel Management Command",				// FMOP_MPC_TMC					= 0x5300,
	"Send LD CXL.io Configuration Request",		// FMOP_MPC_CFG					= 0x5301,
	"Send LD CXL.io Memory Request"				// FMOP_MPC_MEM					= 0x5302,
};

const char *STR_FMOP_4[] = {
	"Get LD Info",								// FMOP_MCC_INFO				= 0x5400,
	"Get LD Allocations",						// FMOP_MCC_ALLOC_GET			= 0x5401,
	"Set LD Allocations",						// FMOP_MCC_ALLOC_SET			= 0x5402,
	"Get QOS Control",							// FMOP_MCC_QOS_CTRL_GE			= 0x5403,
	"Set QOS Control",							// FMOP_MCC_QOS_CTRL_SE			= 0x5404,
	"Get QOS Status",							// FMOP_MCC_QOS_STAT			= 0x5405,
	"Get QOS Allocated BW",						// FMOP_MCC_QOS_BW_ALLOC_GET	= 0x5406,
	"Set QOS Allocated BW",						// FMOP_MCC_QOS_BW_ALLOC_SET	= 0x5407,
	"Get QOS BW Limit",							// FMOP_MCC_QOS_BW_LIMIT_GET	= 0x5408,
	"Set QOS BW Limit"							// FMOP_MCC_QOS_BW_LIMIT_SET	= 0x5409,
};

/**
 * String representations of CXL Command Return Codes (RC)
 *
 * CXL 2.0 v1 Table 150
 */
const char *STR_FMRC[] = {
	"Success",									// FMRC_SUCCESS 						= 0x0000,
	"Background operation started", 			// FMRC_BACKGROUND_OP_STARTED			= 0x0001,
	"Invalid input",							// FMRC_INVALID_INPUT					= 0x0002, 
	"Unsupported",								// FMRC_UNSUPPORTED 					= 0x0003,
	"Internal error", 							// FMRC_INTERNAL_ERROR					= 0x0004,
	"Retry required",							// FMRC_RETRY_REQUIRED 					= 0x0005,
	"Busy",										// FMRC_BUSY							= 0x0006,
	"Media disabled",							// FMRC_MEDIA_DISABLED					= 0x0007,
	"FW transfer in progress",					// FMRC_FW_TXFR_IN_PROGRESS				= 0x0008,
	"FW transfer out of order", 				// FMRC_FW_TXFR_OUT_OF_ORDER			= 0x0009,
	"FW authentication failed",					// FMRC_FW_AUTH_FAILED					= 0x000A,
	"FW invalid slot",							// FMRC_FW_INVALID_SLOT					= 0x000B,
	"FW activation failed, FW rolled back",		// FMRC_FW_ACTV_FAILED_ROLLED_BACK		= 0x000C,
	"FW activation failed, reset requested",	// FMRC_FW_ACTV_FAILED_RESET_REQ		= 0x000D,
	"Invalid handle",							// FMRC_INVALID_HANDLE 					= 0x000E,
	"Invalid physical address",					// FMRC_INVALID_PHY_ADDR				= 0x000F,
	"Inject poison limit reached",				// FMRC_POISON_LIMIT_REACHED			= 0x0010,
	"Media failure",							// FMRC_MEDIA_FAILURE 					= 0x0011, 
	"Aborted",									// FMRC_ABORTED 						= 0x0012, 
	"Invalid security state",					// FMRC_INVALID_SECURITY_STATE			= 0x0013, 
	"Incorrect passphrase",						// FMRC_INCORRECT_PASSPHRASE 			= 0x0014, 
	"Unsupported mailbox",						// FMRC_UNSUPPORTED_MAILBOX				= 0x0015,
	"Invalid payload length"					// FMRC_INVALID_PAYLOAD_LEN				= 0x0016,
};

/**
 * String representations of CXL Port States (PS)
 *
 * CXL 2.0 v1.0 Table 92
 */
const char *STR_FMPS[] = {
	"Disabled", 		// FMPS_DISABLED	= 0
	"Binding", 			// FMPS_BINDING		= 1
	"Unbinding", 		// FMPS_UNBINDING	= 2 
	"Downstream", 				// FMPS_DSP			= 3
	"Upstream", 				// FMPS_USP			= 4
	"Fabric", 			// FMPS_FABRIC		= 5 
	"", 				//					= 6
	"", 				//					= 7
	"", 				//					= 8
	"", 				//					= 9
	"", 				//					= 0x0A
	"", 				//					= 0x0B
	"", 				//					= 0x0C
	"", 				//					= 0x0D
	"", 				//					= 0x0E
	"Invalid"			// FMPS_INVALID		= 0x0F
};

/**
 * String representation of Connected Device CXL version (DV)
 *
 * CXL 2.0 v1.0 Table 92
 */
const char *STR_FMDV[] = {
	"NC", 	// FMDV_NOT_CXL	= 0,
	"1.1",	// FMDV_CXL1_1	= 1,
	"2.0"	// FMDV_CXL2_0	= 2,
};

/**
 * String representation of Connected Device Type (DT)
 *
 * CXL 2.0 v1.0 Table 92
 */
const char *STR_FMDT[] = {
	"None",		// FMDT_NONE				= 0,
	"PCIe",		// FMDT_PCIE				= 1,
	"T1",		// FMDT_CXL_TYPE_1			= 2,
	"T2",		// FMDT_CXL_TYPE_2			= 3,
	"T3-SLD",	// FMDT_CXL_TYPE_3			= 4,
	"T3-MLD",	// FMDT_CXL_TYPE_3_POOLED	= 5,
	"Switch"	// FMDT_CXL_SWITCH			= 6,
};

/*
 * String representation of Connected CXL Version (VC) 
 *
 * CXL 2.0 v1.0 Table 92
 */
const char *STR_FMVC[] = {
	"1.1",		// FMCV_CXL1_1 	= 0x01,
	"2.0" 		// FMCV_CXL2_0 	= 0x02
};

/**
 * Negotiated Link Width (NW)
 *
 * CXL 2.0 v1.0 Table 92
 * Also defined in linux/pci_regs.h
 * - PCI_EXP_LNKSTA_NLW_X1
 */
const char *STR_FMNW[] = {
	"X1", 		// FMNW_X1			= PCI_EXP_LNKSTA_NLW_X1, // 0x0010
	"X2", 		// FMNW_X2			= PCI_EXP_LNKSTA_NLW_X2, // 0x0020
	"X4",		// FMNW_X4 			= PCI_EXP_LNKSTA_NLW_X4, // 0x0040 
	"X8"		// FMNW_X8			= PCI_EXP_LNKSTA_NLW_X8	 // 0x0080
};

/**
 * String representation of PCIe Supported Link Speeds (SS) - Bitmask fields
 *
 * CXL 2.0 v1.0 Table 92 
 */
const char *STR_FMSS[] = {
	"PCIe1", 	// FMSS_PCIE1 		= PCI_EXP_LNKCAP2_SLS_2_5GB, 	// 0x02
	"PCIe2",	// FMSS_PCIE2 		= PCI_EXP_LNKCAP2_SLS_5_0GB, 	// 0x04
	"PCIe3",	// FMSS_PCIE3 		= PCI_EXP_LNKCAP2_SLS_8_0GB, 	// 0x08
	"PCIe4",	// FMSS_PCIE4 		= PCI_EXP_LNKCAP2_SLS_16_0GB,	// 0x10
	"PCIe5",	// FMSS_PCIE5 		= PCI_EXP_LNKCAP2_SLS_32_0GB,	// 0x20
	"PCIe6"	// FMSS_PCIE6		= PCI_EXP_LNKCAP2_SLS_64_0GB	// 0x40
};

/**
 * String representation of PCIe Max Link Speed (MS)
 *
 * CXL 2.0 v1.0 Table 92
 */
const char *STR_FMMS[] = {
	"None",			// 				= 0,
	"1.0",		// FMMS_PCIE1	= 1,
	"2.0",		// FMMS_PCIE2	= 2,
	"3.0",		// FMMS_PCIE3	= 3,
	"4.0",		// FMMS_PCIE4	= 4,
	"5.0",		// FMMS_PCIE5	= 5,
	"6.0"		// FMMS_PCIE6	= 6,
};

/**
 * String representation of PCIe Link state (LS)
 *
 * CXL 2.0 v1.0 Table 92
 */
const char *STR_FMLS[] = {
	"Detect",			// FMLS_DETECT 			= 0,
	"Polling",			// FMLS_POOLING			= 1,
	"Configuration",	// FMLS_CONFIGURATION	= 2,
	"Recovery",			// FMLS_RECOVERY		= 3,
	"L0",				// FMLS_L0				= 4,
	"L0s",				// FMLS_L0S				= 5, 
	"L1",				// FMLS_L1				= 6, 
	"L2",				// FMLS_L2				= 7,
	"Disabled",			// FMLS_DISABLED 		= 8,
	"Loopback",			// FMLS_LOOPBACK		= 9, 
	"Hot Reset"			// FMLS_HOT_RESET		= 10,
};

/**
 * String representation of Link State Flags (LF) - Bitmask Flags
 *
 * CXL 2.0 v1.0 Table 92
 */
const char *STR_FMLF[] = {
	"Lane Reversal", 	// FMLF_LANE_REVERSAL_BIT 		= 0x01,
	"PERST", 			// FMLF_PERST_STATE_BIT 		= 0x02,
	"PRSNT", 			// FMLF_PRSNT_STATE_BIT 		= 0x04,
	"PWRCTL"			// FMLF_PWRCTL_STATE_BIT 		= 0x08
};

/**
 * String representation of PCIe Lane Reverse Ordering State (LO)
 *
 * CXL 2.0 v1.0 Table 92
 */
const char *STR_FMLO[] = {
	"Lane Standard ordering", 	// FMLO_LANE_STANDARD_ORDERING = 0,
	"Lane Reverse Ordering" 	// FMLO_LANE_REVERSE_ORDERING 	= 1
};

/**
 * String representationf of Port Opcode - Actions for Physical Port Control (PO)
 *
 * CXL 2.0 v1.0 Table 93
 */
const char *STR_FMPO[] = {
	"Assert", 	// FMPO_ASSERT_PERST	= 0x00,
	"Deassert", // FMPO_DEASSERT_PERST	= 0x01,
	"Reset"		// FMPO_RESET_PPB		= 0x02,
};

/**
 * String representation of VCS State (VS)
 *
 * CXL 2.0 v1.0 Table 99
 */
const char *STR_FMVS[] = {
	"Disabled",			// FMVS_DISABLED 	= 0,
	"Enabled",			// FMVS_ENABLED		= 1,
	"Invalid"			// FMVS_INVALID		= 0xFF
};

/**
 * String representation of PBB Binding Status (BS)
 *
 * CXL 2.0 v1.0 Table 99
 */
const char *STR_FMBS[] = {
	"Unbound",				// FMBS_UNBOUND 	= 0,
	"In Progress",			// FMBS_INPROGRESS	= 1, 
	"Bound Physical Port",	// FMBS_BOUND_PORT	= 2, 
	"Bound LD"				// FMBS_BOUND_LD	= 3,
};

/**
 * String representation of Unbind option - For Unbind vPPB Command (UB)
 *
 * CXL 2.0 v1.0 Table 101
 */
const char *STR_FMUB[] = {
	"Wait", 				// FMUB_WAIT					= 0, 
	"Managed Hot Remove", 	// FMUB_MANAGED_HOT_REMOVE		= 1,
	"Surprise Hot Remove"	// FMUB_SURPRISE_HOT_REMOVE 	= 2,
};

/**
 * String representation of Memory Granularity - For Get LD Allocation Command (MG)
 *
 * CXL 2.0 v1.0 Table 112
 */
const char *STR_FMMG[] = {
	"256MB", 	// FMMG_256MB 	= 0,
	"512MB",	// FMMG_512MB	= 1, 
	"1GB"		// FMMG_1GB		= 2,
};

/**
 * String representation of QoS Telemetry Control (QT) - Bitmask
 *
 * CXL 2.0 v1.0 Table 116
 */
const char *STR_FMQT[] = {
	"Egress Port Congestion",			// FMQT_EGRESS_PORT_CONGESTION_BIT 		= 0x01,
	"Temporary Throughput Reduction"	// FMQT_TEMP_THROUGHPUT_REDUCTION_BIT 	= 0x02
};

/**
 * String representation of Physical Switch Event Record - Event Type (ET)
 *
 * CXL 2.0 v1.0 Table 120 
 */
const char *STR_FMET[] = {
	"Link Status Change",			// FMET_LINK_STATUS_CHANGE				= 0,
	"Slot Status Register Updated"	// FMET_SLOT_STATUS_REGISTER_UPDATED	= 1,
};

/**
 * String representation of Virtual CXL Switch Event Record - Event Type (VT)
 *
 * CXL 2.0 v1.0 Table 121 
 */
const char *STR_FMVT[] = {
	"Binding Change",					// FMVT_BINDING_CHANGE					= 0,
	"Secondary Bus Reset",				// FMVT_SECONDARY_BUS_RESET				= 1,
	"Link Control Register Updated",	// FMVT_LINK_CONTROL_REGISTER_UPDATED	= 2, 
	"Slot Control Register Updatead"	// FMVT_SLOT_CONTROL_REGISTER_UPDATED	= 3,
};

/**
 * String REpresentation of MLD Port Event Record - Event Type (MR)
 *
 * CXL 2.0 v1.0 Table 121 
 */
const char *STR_FMMR[] = {
	"Correctable Message Received",	// FMMR_CORRECTABLE_MSG_RECEIVED 	= 0,
	"Non-Fatal Message Received",	// FMMR_NONFATAL_MSG_RECEIVED		= 1,
	"Fatal Message Received"		// FMMR_FATAL_MSG_RECEIVED 			= 2,
};

/**
 * String representation of CXL.io Configuration Request Type (CT)
 *
 * CXL 2.0 v1.0 Table 94
 */
const char *STR_FMCT[] = {
	"Read",	// FMCT_READ 	= 0,
	"Write"	// FMCT_WRITE 	= 1
};

/* PROTOTYPES ================================================================*/

void fmapi_prnt_hdr(void *ptr);
void fmapi_prnt_isc_bos(void *ptr);
void fmapi_prnt_isc_id_rsp(void *ptr);
void fmapi_prnt_isc_msg_limit(void *ptr);
void fmapi_prnt_psc_id_rsp(void *ptr);
void fmapi_prnt_psc_port_req(void *ptr);
void fmapi_prnt_psc_port_info(void *ptr);
void fmapi_prnt_psc_port_rsp(void *ptr);
void fmapi_prnt_psc_port_ctrl_req(void *ptr);
void fmapi_prnt_psc_cfg_req(void *ptr);
void fmapi_prnt_psc_cfg_rsp(void *ptr);
void fmapi_prnt_vsc_info_req(void *ptr);
void fmapi_prnt_vsc_ppb_stat_blk(void *ptr);
void fmapi_prnt_vsc_info_blk(void *ptr);
void fmapi_prnt_vsc_info_rsp(void *ptr);
void fmapi_prnt_vsc_bind_req(void *ptr);
void fmapi_prnt_vsc_unbind_req(void *ptr);
void fmapi_prnt_vsc_aer_req(void *ptr);
void fmapi_prnt_mpc_tmc_req(void *ptr);
void fmapi_prnt_mpc_tmc_rsp(void *ptr);
void fmapi_prnt_mpc_cfg_req(void *ptr);
void fmapi_prnt_mpc_cfg_rsp(void *ptr);
void fmapi_prnt_mpc_mem_req(void *ptr);
void fmapi_prnt_mpc_mem_rsp(void *ptr);
void fmapi_prnt_mcc_info_rsp(void *ptr);
void fmapi_prnt_mcc_alloc_blk(void *ptr);
void fmapi_prnt_mcc_alloc_get_req(void *ptr);
void fmapi_prnt_mcc_alloc_get_rsp(void *ptr);
void fmapi_prnt_mcc_alloc_set_req(void *ptr);
void fmapi_prnt_mcc_alloc_set_rsp(void *ptr);
void fmapi_prnt_mcc_qos_ctrl(void *ptr);
void fmapi_prnt_mcc_qos_stat_rsp(void *ptr);
void fmapi_prnt_mcc_qos_bw_alloc_get_req(void *ptr);
void fmapi_prnt_mcc_qos_bw_alloc(void *ptr);
void fmapi_prnt_mcc_qos_bw_limit_get_req(void *ptr);
void fmapi_prnt_mcc_qos_bw_limit(void *ptr);      

/* FUNCTIONS =================================================================*/

/**
 * @brief Convert from a Little Endian byte array to a struct
 * 
 * @param[out] dst void Pointer to destination struct
 * @param[in] src void Pointer to unsigned char array
 * @param[in] type unsigned enum _FMOB representing type of object to deserialize
 * @param[in] param void * to data needed to deserialize the byte stream 
 * (e.g. count of objects to expect in the stream)
 * @return number of bytes consumed. 0 upon error otherwise. 
 */
int fmapi_deserialize(void *dst, __u8 *src, unsigned type, void *param)
{
	int rv;

	rv = 0;

	if ( (dst == NULL) || (type >= FMOB_MAX) )
		return -1;

	switch(type)
	{
		case FMOB_NULL:
			rv = 0;
			break;

		case FMOB_HDR: //!< struct fmapi_hdr
		{
			struct fmapi_hdr *o = (struct fmapi_hdr*) dst;
			o->category 	= (src[0] >> 4) & 0x0F;	
			o->tag 			= (src[1]); 
			o->opcode 		= (src[4] << 8)  | src[3];
			o->len 			= ((src[7] & 0x00F8) << 13) | (src[6] << 8) | src[5] ;
			o->background 	= (src[7] & 0x01);
			o->return_code 	= (src[9] << 8)  | (src[8]);
			o->ext_status 	= (src[11] << 8) | (src[10]);
			rv = FMLN_HDR;
		}
			break;

		case FMOB_ISC_BOS: //!< struct fmapi_isc_bos
		{
			struct fmapi_isc_bos *o = (struct fmapi_isc_bos*) dst;
			o->running =  src[0]        & 0x01;
			o->pcnt    = (src[0] >> 1 ) & 0x7F;
			o->opcode  = (src[3] << 8 ) | (src[2] & 0x00FF);
			o->rc      = (src[5] << 8 ) | (src[4] & 0x00FF);
			o->ext     = (src[7] << 8 ) | (src[6] & 0x00FF);
			rv = FMLN_ISC_BOS;
		}
			break;

		case FMOB_ISC_ID_RSP: //!< struct fmapi_isc_id_rsp
		{
			struct fmapi_isc_id_rsp *o = (struct fmapi_isc_id_rsp*) dst;
			o->vid  = (src[1] << 8) | src[0];
			o->did  = (src[3] << 8) | src[2];
			o->svid = (src[5] << 8) | src[4];
			o->ssid = (src[7] << 8) | src[6];
			o->sn   = 	((__u64)src[15] << 56) | 
						((__u64)src[14] << 48) |
						((__u64)src[13] << 40) | 
						((__u64)src[12] << 32) | 
						((__u64)src[11] << 24) |
						((__u64)src[10] << 16) | 
						((__u64)src[ 9] << 8 ) | 
						((__u64)src[ 8]      );
			o->size = src[16];
			rv = FMLN_ISC_ID_RSP;
		}
			break;

		case FMOB_ISC_MSG_LIMIT: //!< struct fmapi_isc_msg_limit
		{
			struct fmapi_isc_msg_limit *o = (struct fmapi_isc_msg_limit*) dst;
			o->limit = src[0];
			rv = FMLN_ISC_MSG_LIMIT;
		}
			break;

		case FMOB_PSC_ID_RSP: //!< struct fmapi_psc_id_rsp
		{
			struct fmapi_psc_id_rsp *o = (struct fmapi_psc_id_rsp*) dst;
			o->ingress_port 	=  src[0];
			o->num_ports 		=  src[2];
			o->num_vcss 		=  src[3];
			memcpy(o->active_ports, &src[4],  32);
			memcpy(o->active_vcss,  &src[36], 32);
			o->num_vppbs 		= (src[69] << 8) | src[68];
			o->active_vppbs 	= (src[71] << 8) | src[70];
			o->num_decoders 	=  src[72];
			rv = FMLN_PSC_IDENTIFY_SWITCH;
		}
			break;

		case FMOB_PSC_PORT_REQ: //!< struct fmapi_psc_port_req
		{
			struct fmapi_psc_port_req *o = (struct fmapi_psc_port_req*) dst;
			o->num = src[0];
			memcpy(o->ports, &src[1], o->num);
			rv = FMLN_PSC_GET_PHY_PORT_REQ + o->num;
		}
			break;

		case FMOB_PSC_PORT_INFO: //!< struct fmapi_psc_port_info
		{
			struct fmapi_psc_port_info *o = (struct fmapi_psc_port_info*) dst;
			o->ppid		= src[0];
			o->state	= src[1];
			o->dv		= src[2];
			o->dt		= src[4];
			o->cv		= src[5];
			o->mlw		= src[6];
			o->nlw		= src[7];
			o->speeds	= src[8];
			o->mls		= src[9];
			o->cls		= src[10];
			o->ltssm	= src[11];
			o->lane		= src[12];
			o->lane_rev = (src[13] >> FMLF_LANE_REVERSAL_BIT) & 0x01;
			o->perst    = (src[13] >> FMLF_PERST_STATE_BIT  ) & 0x01;
			o->prsnt    = (src[13] >> FMLF_PRSNT_STATE_BIT  ) & 0x01;
			o->pwrctrl  = (src[13] >> FMLF_PWRCTL_STATE_BIT ) & 0x01;
			o->num_ld	= src[15];
			rv = FMLN_PSC_GET_PHY_PORT_INFO;
		}
			break;

		case FMOB_PSC_PORT_RSP: //!< struct fmapi_psc_port_rsp
		{
			struct fmapi_psc_port_rsp *o = (struct fmapi_psc_port_rsp*) dst;	
			o->num = src[0];
			rv = FMLN_PSC_GET_PHY_PORT_RESP;
			for ( int i = 0 ; i < o->num ; i++) 
				rv += fmapi_deserialize(&o->list[i], &src[rv], FMOB_PSC_PORT_INFO, NULL);
		}
			break;

		case FMOB_PSC_PORT_CTRL_REQ: //!< struct fmapi_psc_port_ctrl_req
		{
			struct fmapi_psc_port_ctrl_req *o = (struct fmapi_psc_port_ctrl_req*) dst;
			o->ppid   = src[0];
			o->opcode = src[1];
			rv = FMLN_PSC_PHY_PORT_CTRL;
		}
			break;

		case FMOB_PSC_CFG_REQ: //!< struct fmapi_psc_cfg_req
		{
			struct fmapi_psc_cfg_req *o = (struct fmapi_psc_cfg_req*) dst;
			o->ppid 	= src[0];
			o->reg   	= src[1]; 
			o->ext 		= (src[2]     ) & 0x0F;
			o->fdbe 	= (src[2] >> 4) & 0x0F;
			o->type 	= (src[3] >> 7) & 0x01;
			o->data[0] 	= src[4];
			o->data[1] 	= src[5];
			o->data[2] 	= src[6];
			o->data[3] 	= src[7];
			rv = FMLN_PSC_PPB_IO_CFG_REQ;
		}
			break;

		case FMOB_PSC_CFG_RSP: //!< struct fmapi_psc_cfg_rsp 
		{
			struct fmapi_psc_cfg_rsp *o = (struct fmapi_psc_cfg_rsp*) dst;
			o->data[0] 	= src[0];
			o->data[1] 	= src[1];
			o->data[2] 	= src[2];
			o->data[3] 	= src[3];
			rv = FMLN_PSC_PPB_IO_CFG_RESP;
		}
			break;

		case FMOB_VSC_INFO_REQ: //!< struct fmapi_vsc_info_req
		{
			struct fmapi_vsc_info_req *o = (struct fmapi_vsc_info_req*) dst;

			o->vppbid_start = src[0];
			o->vppbid_limit = src[1];
			o->num          = src[2];
			memcpy(o->vcss, &src[3], o->num);
			rv = FMLN_VSC_GET_INFO_REQ + o->num;
		}
			break;

		case FMOB_VSC_PPB_STAT_BLK: //!< struct fmapi_vsc_ppb_stat_blk
		{
			struct fmapi_vsc_ppb_stat_blk *o = (struct fmapi_vsc_ppb_stat_blk*) dst;
			o->status 	= src[0];
			o->ppid 	= src[1];
			o->ldid 	= src[2];
			rv = FMLN_VSC_PPB_STATUS;
		}
			break;

		case FMOB_VSC_INFO_BLK: //!< struct fmapi_vsc_info_blk
		{
			struct fmapi_vsc_info_blk *o = (struct fmapi_vsc_info_blk*) dst;
			struct fmapi_vsc_info_req *r = (struct fmapi_vsc_info_req*) param;

			// We must have a pointer to the request to deserialize the response 
			if (r == NULL) {
				rv = 0;
				break;
			}

			o->vcsid = src[0];
			o->state = src[1];
			o->uspid = src[2];
			o->total = src[3];

			// Compute number of vPPB blk entries from fields in request 
			o->num = o->total - r->vppbid_start; 
			if (r->vppbid_limit < o->num)
				o->num = r->vppbid_limit;

			rv = FMLN_VSC_INFO;
			for (int i = 0 ; i < o->num ; i++)
				rv += fmapi_deserialize(&o->list[i], &src[rv], FMOB_VSC_PPB_STAT_BLK, NULL);
		}
			break;

		case FMOB_VSC_INFO_RSP: //!< struct fmapi_vsc_info_rsp
		{
			struct fmapi_vsc_info_rsp *o = (struct fmapi_vsc_info_rsp*) dst;
			struct fmapi_vsc_info_req *r = (struct fmapi_vsc_info_req*) param;

			o->num = src[0];

			rv = FMLN_VSC_GET_INFO_RESP;
			for (int i = 0 ; i < o->num ; i++)
				rv += fmapi_deserialize(&o->list[i], &src[rv], FMOB_VSC_INFO_BLK, r);
		}
			break;

		case FMOB_VSC_BIND_REQ: //!< struct fmapi_vsc_bind_req
		{
			struct fmapi_vsc_bind_req *o = (struct fmapi_vsc_bind_req*) dst;
			o->vcsid  = src[0];
			o->vppbid = src[1];
			o->ppid   = src[2];
			o->ldid   = (src[5] << 8) | src[4];
			rv = FMLN_VSC_BIND;
		}
			break;

		case FMOB_VSC_UNBIND_REQ: //!< struct fmapi_vsc_unbind_req
		{
			struct fmapi_vsc_unbind_req *o = (struct fmapi_vsc_unbind_req*) dst;
			o->vcsid  = src[0];
			o->vppbid = src[1]; 
			o->option = src[2]; 
			rv = FMLN_VSC_UNBIND;
		}
			break;

		case FMOB_VSC_AER_REQ: //!< struct fmapi_vsc_aer_req
		{
			struct fmapi_vsc_aer_req *o = (struct fmapi_vsc_aer_req*) dst;
			o->vcsid  = src[0];
			o->vppbid = src[1];
			o->error_type = (src[7] << 24) |  (src[6] << 16) | (src[5] << 8) | src[4];
			memcpy(o->header, &src[8], 32);
			rv = FMLN_VSC_GEN_AER;
		}
			break;

		case FMOB_MPC_TMC_REQ: //!< struct fmapi_mpc_tmc_req
		{
			struct fmapi_mpc_tmc_req *o = (struct fmapi_mpc_tmc_req*) dst;
			o->ppid   = src[0];
			o->len    = ((src[3] << 8 ) | src[2])-1;
			o->type   = src[4];
			memcpy(o->msg, &src[FMLN_MPC_TUNNEL_CMD_REQ], o->len);
			rv = FMLN_MPC_TUNNEL_CMD_REQ + o->len;
		}
			break;

		case FMOB_MPC_TMC_RSP: //!< struct fmapi_mpc_tmc_rsp
		{
			struct fmapi_mpc_tmc_rsp *o = (struct fmapi_mpc_tmc_rsp*) dst;
			o->len  = ((src[1] << 8) | src[0]) - 1;
			o->type = src[4];
			memcpy(o->msg, &src[FMLN_MPC_TUNNEL_CMD_REQ], o->len);
			rv = FMLN_MPC_TUNNEL_CMD_RESP + o->len;
		}
			break;

		case FMOB_MPC_CFG_REQ: //!< struct fmapi_mpc_cfg_req
		{
			struct fmapi_mpc_cfg_req *o = (struct fmapi_mpc_cfg_req*) dst;
			o->ppid 	= src[0]; 
			o->reg 	 	= src[1]; 
			o->ext 		= src[2] & 0x0F;
			o->fdbe		= (src[2] >> 4 ) & 0x0F;
			o->type     = (src[3] >> 7 ) & 0x01;
			o->ldid		= (src[5] << 8) | src[4];
			o->data[0]	= src[8];
			o->data[1]  = src[9];
			o->data[2]  = src[10];
			o->data[3]  = src[11];  
			rv = FMLN_MPC_LD_IO_CFG_REQ;
		}
			break;

		case FMOB_MPC_CFG_RSP: //!< struct fmapi_mpc_cfg_rsp
		{
			struct fmapi_mpc_cfg_rsp *o = (struct fmapi_mpc_cfg_rsp*) dst;
			o->data[0] = src[0];
			o->data[1] = src[1];
			o->data[2] = src[2];
			o->data[3] = src[3];
			rv = FMLN_MPC_LD_IO_CFG_RESP;
		}
			break;

		case FMOB_MPC_MEM_REQ: //!< struct fmapi_mpc_mem_req
		{
			struct fmapi_mpc_mem_req *o = (struct fmapi_mpc_mem_req*) dst;
			o->ppid 	=  src[0];
			o->fdbe 	= (src[2] >> 4 ) & 0x000F;		
			o->ldbe		= (src[3]      ) & 0x000F;
			o->type 	= (src[3] >> 7 ) & 0x0001;
			o->ldid 	= (src[5] << 8) | src[4];
			o->len  	= (src[7] << 8) | src[6];
			o->offset 	= 	((__u64)src[15] << 56) | 
							((__u64)src[14] << 48) |
							((__u64)src[13] << 40) | 
							((__u64)src[12] << 32) | 
							((__u64)src[11] << 24) |
							((__u64)src[10] << 16) | 
							((__u64)src[ 9] << 8 ) | 
							((__u64)src[ 8]      );
			memcpy(o->data, &src[FMLN_MPC_LD_MEM_REQ], o->len);
			rv = FMLN_MPC_LD_MEM_REQ + o->len;
		}
			break;

		case FMOB_MPC_MEM_RSP: //!< struct fmapi_mpc_mem_rsp
		{
			struct fmapi_mpc_mem_rsp *o = (struct fmapi_mpc_mem_rsp*) dst;
			o->len = (src[1] << 8) | src[0];
			memcpy(o->data, &src[FMLN_MPC_LD_MEM_RESP], o->len);
			rv = FMLN_MPC_LD_MEM_RESP + o->len;
		}
			break;

		case FMOB_MCC_INFO_RSP: //!< struct fmapi_mcc_info_rsp
		{
			struct fmapi_mcc_info_rsp *o = (struct fmapi_mcc_info_rsp*) dst;
			o->size = 	((__u64)src[7] << 56) | 
						((__u64)src[6] << 48) |
						((__u64)src[5] << 40) | 
						((__u64)src[4] << 32) | 
						((__u64)src[3] << 24) |
						((__u64)src[2] << 16) | 
						((__u64)src[1] << 8 ) | 
						((__u64)src[0]      );
			o->num = (src[9] << 8) | src[8];
			o->epc = (src[10] >> FMQT_EGRESS_PORT_CONGESTION_BIT   ) & 0x01;
			o->ttr = (src[10] >> FMQT_TEMP_THROUGHPUT_REDUCTION_BIT) & 0x01;
			rv = FMLN_MCC_GET_LD_INFO;
		}
			break;

		case FMOB_MCC_ALLOC_BLK: //!< struct fmapi_mcc_ldalloc_entry
		{
			struct fmapi_mcc_alloc_blk *o = (struct fmapi_mcc_alloc_blk*) dst;
			o->rng1 = 	((__u64)src[7] << 56) | 
						((__u64)src[6] << 48) |
						((__u64)src[5] << 40) | 
						((__u64)src[4] << 32) | 
						((__u64)src[3] << 24) |
						((__u64)src[2] << 16) | 
						((__u64)src[1] << 8 ) | 
						((__u64)src[0]      );
			o->rng2 = 	((__u64)src[15] << 56) | 
						((__u64)src[14] << 48) |
						((__u64)src[13] << 40) | 
						((__u64)src[12] << 32) | 
						((__u64)src[11] << 24) |
						((__u64)src[10] << 16) | 
						((__u64)src[9] << 8 ) | 
						((__u64)src[8]      );
			rv = FMLN_MCC_LD_ALLOC_ENTRY;
		}
			break;

		
		case FMOB_MCC_ALLOC_GET_REQ: //!< struct fmapi_mcc_alloc_get_req
		{
			struct fmapi_mcc_alloc_get_req *o = (struct fmapi_mcc_alloc_get_req*) dst;
			o->start = src[0];
			o->limit = src[1];
			rv = FMLN_MCC_GET_LD_ALLOC_REQ;
		}
			break;

		case FMOB_MCC_ALLOC_GET_RSP: //!< struct fmapi_mcc_alloc_get_rsp
		{
			struct fmapi_mcc_alloc_get_rsp *o = (struct fmapi_mcc_alloc_get_rsp*) dst;
			o->total 		= src[0];
			o->granularity 	= src[1];
			o->start		= src[2];
			o->num 			= src[3];
			rv = FMLN_MCC_GET_LD_ALLOC_RSP;
			for ( int i = 0 ; i < o->num ; i++ )
				rv += fmapi_deserialize(&o->list[i], &src[rv], FMOB_MCC_ALLOC_BLK, NULL);
		}
			break;

		case FMOB_MCC_ALLOC_SET_REQ: //!< struct fmapi_mcc_alloc_set_req
		{
			struct fmapi_mcc_alloc_set_req *o = (struct fmapi_mcc_alloc_set_req*) dst;
			o->num   = src[0];
			o->start = src[1];
			rv = FMLN_MCC_SET_LD_ALLOC_REQ;
			for ( int i = 0 ; i < o->num ; i++ )
				rv += fmapi_deserialize(&o->list[i], &src[rv], FMOB_MCC_ALLOC_BLK, NULL);
		}
			break;

		case FMOB_MCC_ALLOC_SET_RSP: //!< struct fmapi_mcc_alloc_set_rsp
		{
			struct fmapi_mcc_alloc_set_rsp *o = (struct fmapi_mcc_alloc_set_rsp*) dst;
			o->num   = src[0];
			o->start = src[1];
			rv = FMLN_MCC_SET_LD_ALLOC_RSP;
			for ( int i = 0 ; i < o->num ; i++ )
				rv += fmapi_deserialize(&o->list[i], &src[rv], FMOB_MCC_ALLOC_BLK, NULL);
		}
			break;

		case FMOB_MCC_QOS_CTRL: //!< struct fmapi_mcc_qos_ctrl
		{
			struct fmapi_mcc_qos_ctrl *o = (struct fmapi_mcc_qos_ctrl*) dst;
			o->epc_en       = (src[0] >> FMQT_EGRESS_PORT_CONGESTION_BIT   ) & 0x01;
			o->ttr_en = (src[0] >> FMQT_TEMP_THROUGHPUT_REDUCTION_BIT) & 0x01;
			o->egress_mod_pcnt 	= src[1];
			o->egress_sev_pcnt 	= src[2];
			o->sample_interval 	= src[3];
			o->rcb 	= (src[5] << 8 ) | src[4];
			o->comp_interval  	= src[6];
			rv = FMLN_MCC_QOS_CTRL;
		}
			break;

		case FMOB_MCC_QOS_STAT_RSP: //!< struct fmapi_mcc_qos_stat_rsp
		{
			struct fmapi_mcc_qos_stat_rsp *o = (struct fmapi_mcc_qos_stat_rsp*) dst;
			o->bp_avg_pcnt = src[0];
			rv = FMLN_MCC_QOS_STATUS;
		}
			break;

		case FMOB_MCC_QOS_BW_GET_REQ:
		{
			struct fmapi_mcc_qos_bw_alloc_get_req *o = (struct fmapi_mcc_qos_bw_alloc_get_req*) dst;
			o->num 		= src[0];
			o->start 	= src[1];
			rv = FMLN_MCC_GET_QOS_BW_REQ;
		}
			break;

		case FMOB_MCC_QOS_BW_ALLOC: //!< struct fmapi_mcc_qos_bw_alloc
		{
			struct fmapi_mcc_qos_bw_alloc *o = (struct fmapi_mcc_qos_bw_alloc*) dst;
			o->num 	 = src[0];
			o->start = src[1];
			rv = FMLN_MCC_QOS_BW_ALLOC;
			for ( int i = 0 ; i < o->num ; i++ )
				o->list[i] = src[rv + i];
			rv += o->num;
		}
			break;

		case FMOB_MCC_QOS_BW_LIMIT_GET_REQ: //!< struct fmapi_mcc_qos_bw_limit_get_req 
		{
			struct fmapi_mcc_qos_bw_limit_get_req *o = (struct fmapi_mcc_qos_bw_limit_get_req*) dst;
			o->num = src[0];
			o->start = src[1];
			rv = FMLN_MCC_GET_QOS_BW_LIMIT_REQ;
		}
			break;

		case FMOB_MCC_QOS_BW_LIMIT: //!< struct fmapi_mcc_qos_bw_limit
		{
			struct fmapi_mcc_qos_bw_limit *o = (struct fmapi_mcc_qos_bw_limit*) dst;
			o->num 	 = src[0];
			o->start = src[1];
			rv = FMLN_MCC_QOS_BW_LIMIT;
			for ( int i = 0 ; i < o->num ; i++ )
				o->list[i] = src[rv + i];
			rv += o->num;
		}
			break;

		default:
			rv = 0;
			break;
	}

	return rv;
}

/** 
 * Prepare an FM API Message - ISC Background Operation Status
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_isc_bos(struct fmapi_msg *m)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_ISC_BOS;	

	// Set object 

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - ISC Get Message Limit 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_isc_get_msg_limit(struct fmapi_msg *m)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_ISC_MSG_LIMIT_GET;	

	// Set object 

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - ISC Identify
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_isc_id(struct fmapi_msg *m)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_ISC_ID;	

	// Set object 

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - ISC Set Message Limit 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_isc_set_msg_limit(struct fmapi_msg *m, __u8 limit)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_ISC_MSG_LIMIT_SET;	

	// Set object 
	m->obj.isc_msg_limit.limit = limit;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Get LD Alloc
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_get_alloc(struct fmapi_msg *m, int start, int limit)
{
	int rv;

	// Initialize variables
	rv = 1;
	if (limit == 0)
		limit = 255;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_ALLOC_GET;	

	// Set object
	m->obj.mcc_alloc_get_req.start = start;
	m->obj.mcc_alloc_get_req.limit = limit;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Get Info
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_get_info(struct fmapi_msg *m)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_INFO;

	// Fill object 

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Get QoS BW Alloc
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_get_qos_alloc(struct fmapi_msg *m, int start, int limit)
{
	int rv;

	// Initialize variables
	rv = 1;
	if (limit == 0)
		limit = 255;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Fill header 
	m->hdr.opcode = FMOP_MCC_QOS_BW_ALLOC_GET;	

	// Fill object
	m->obj.mcc_qos_bw_alloc_get_req.start = start;
	m->obj.mcc_qos_bw_alloc_get_req.num   = limit;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Get Info
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_get_qos_ctrl(struct fmapi_msg *m)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_QOS_CTRL_GET;	

	// Fill Object

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Get QoS BW Limit
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_get_qos_limit(struct fmapi_msg *m, int start, int limit)
{
	int rv;

	// Initialize variables
	rv = 1;
	if (limit == 0)
		limit = 16;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_QOS_BW_LIMIT_GET;	

	// Set object
	m->obj.mcc_qos_bw_limit_get_req.start = start;
	m->obj.mcc_qos_bw_limit_get_req.num   = limit;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Get QoS Status
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_get_qos_status(struct fmapi_msg *m)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_QOS_STAT;

	// Fill Object

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Set LD Allocations
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_set_alloc(struct fmapi_msg *m, int start, int num, __u64 *rng1, __u64 *rng2)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_ALLOC_SET;	

	// Set object
	m->obj.mcc_alloc_set_req.num = num;
	m->obj.mcc_alloc_set_req.start = start;

	for ( int i = 0 ; i < num ; i++)
	{
		m->obj.mcc_alloc_set_req.list[i].rng1 = rng1[i];
		m->obj.mcc_alloc_set_req.list[i].rng2 = rng2[i];
	}

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Set QoS Allocated
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_set_qos_alloc(struct fmapi_msg *m, int start, int num, __u8 *list)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_QOS_BW_ALLOC_SET;	

	// Set object
	m->obj.mcc_qos_bw_alloc.num   = num;
	m->obj.mcc_qos_bw_alloc.start = start;
	for ( int i = 0 ; i < num ; i++)
		m->obj.mcc_qos_bw_alloc.list[i] = list[i];
		
	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Set QoS Control 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_set_qos_ctrl(struct fmapi_msg *m, int epc, int ttr, int mod, int sev, int si, int rcb, int ci)
{

	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_QOS_CTRL_SET;	

	// Set object
	m->obj.mcc_qos_ctrl.epc_en 				= epc;
	m->obj.mcc_qos_ctrl.ttr_en 				= ttr;
	m->obj.mcc_qos_ctrl.egress_mod_pcnt 	= mod;
	m->obj.mcc_qos_ctrl.egress_sev_pcnt 	= sev;
	m->obj.mcc_qos_ctrl.sample_interval 	= si;
	m->obj.mcc_qos_ctrl.rcb 			 	= rcb;
	m->obj.mcc_qos_ctrl.comp_interval	 	= ci;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MCC Set QoS Limit
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mcc_set_qos_limit(struct fmapi_msg *m, int start, int num, __u8 *list)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MCC_QOS_BW_LIMIT_SET;	

	// Set object
	m->obj.mcc_qos_bw_limit.num   = num;
	m->obj.mcc_qos_bw_limit.start = start;
	for ( int i = 0 ; i < num ; i++)
		m->obj.mcc_qos_bw_limit.list[i] = list[i];
		
	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MPC CXL.io Configg
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mpc_cfg(struct fmapi_msg *m, int ppid, int ldid, int reg, int ext, int fdbe, int type, __u8 *data)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MPC_CFG;	

	// Set object
	m->obj.mpc_cfg_req.ppid   = ppid;
	m->obj.mpc_cfg_req.ldid   = ldid;
	m->obj.mpc_cfg_req.reg    = reg;
	m->obj.mpc_cfg_req.ext    = ext;
	m->obj.mpc_cfg_req.fdbe   = fdbe;
	m->obj.mpc_cfg_req.type   = type;
	memcpy(m->obj.mpc_cfg_req.data, data, 4);

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MPC Memory 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mpc_mem(struct fmapi_msg *m, int ppid, int ldid, __u64 offset, int len, int fdbe, int ldbe,  int type, __u8 *data)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_MPC_MEM;	

	// Set object
	m->obj.mpc_mem_req.ppid   = ppid;
	m->obj.mpc_mem_req.ldid   = ldid;
	m->obj.mpc_mem_req.offset = offset;
	m->obj.mpc_mem_req.len    = len;
	m->obj.mpc_mem_req.fdbe   = fdbe;
	m->obj.mpc_mem_req.ldbe   = ldbe;
	m->obj.mpc_mem_req.type   = type;
	memcpy(m->obj.mpc_mem_req.data, data, len);

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - MPC - Tunnel Management Command
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_mpc_tmc(struct fmapi_msg *m, int ppid, int type, struct fmapi_msg *sub)
{
	int rv, len;

	// Initialize variables
	rv = 1;
	len = 0;
	sub->buf = (struct fmapi_buf*) m->obj.mpc_tmc_req.msg;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOB_MPC_TMC_REQ;	

	// Set object
	{
		// Fill Sub Object 

		// Serialize Sub Object 
		len = fmapi_serialize(sub->buf->payload, &sub->obj, fmapi_fmob_req(sub->hdr.opcode));

		// Fill Sub Header
		m->obj.mpc_tmc_req.len = fmapi_fill_hdr(&sub->hdr, FMMT_REQ, 0, sub->hdr.opcode, 0, len, 0, 0);
	
		// Serialize Sub Header 
		fmapi_serialize((__u8*)&sub->buf->hdr, &sub->hdr, FMOB_HDR);

		// Set TMC Object Type 
		m->obj.mpc_tmc_req.type = type;
	}

	// Prepare TMC Object
	m->obj.mpc_tmc_req.ppid = ppid;	

	// Fill TMC Header
	m->hdr.opcode = FMOP_MPC_TMC;

	rv = 0;

end:

	return rv;
}


/** 
 * Prepare an FM API Message - PSC Identify Switch Device
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_psc_id(struct fmapi_msg *m)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_PSC_ID;	

	// Set object 

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - PSC Get All Port Status 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_psc_get_all_ports(struct fmapi_msg *m)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_PSC_PORT;	

	// Set object
	m->obj.psc_port_req.num = 255;
	for ( int i = 0 ; i < 255 ; i++)
		m->obj.psc_port_req.ports[i] = i;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - PSC Get Port Status 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_psc_get_port(struct fmapi_msg *m, __u8 port)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_PSC_PORT;	

	// Set object
	m->obj.psc_port_req.num = 1;
	m->obj.psc_port_req.ports[0] = port;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - PSC Get Port Status 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_psc_get_ports(struct fmapi_msg *m, int num, __u8 *list)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_PSC_PORT;	

	// Set object
	m->obj.psc_port_req.num = num;
	for ( int i = 0 ; i < num ; i++ )
		m->obj.psc_port_req.ports[i] = list[i];

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - PSC Port CXL.io Config
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_psc_cfg(struct fmapi_msg *m, int ppid, int reg, int ext, int fdbe, int type, __u8 *data)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_PSC_CFG;	

	// Set object
	m->obj.psc_cfg_req.ppid   = ppid;
	m->obj.psc_cfg_req.reg    = reg;
	m->obj.psc_cfg_req.ext    = ext;
	m->obj.psc_cfg_req.fdbe   = fdbe;
	m->obj.psc_cfg_req.type   = type;

	if (data !=NULL)
		memcpy(m->obj.psc_cfg_req.data, data, 4);

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - PSC Port Control 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_psc_port_ctrl(struct fmapi_msg *m, int ppid, int opcode)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_PSC_PORT_CTRL;	

	// Set object
	m->obj.psc_port_ctrl_req.ppid   = ppid;
	m->obj.psc_port_ctrl_req.opcode = opcode;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - VCS AER Event
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_vsc_aer(struct fmapi_msg *m, int vcsid, int vppbid, __u32 type, __u8 *header)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_VSC_AER;	

	// Set object 
	m->obj.vsc_aer_req.vcsid = vcsid;
	m->obj.vsc_aer_req.vppbid = vppbid;
	m->obj.vsc_aer_req.error_type = type;
	memcpy(m->obj.vsc_aer_req.header, header, 32);

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - VCS Bind 
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_vsc_bind(struct fmapi_msg *m, int vcsid, int vppbid, int ppid, int ldid)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_VSC_BIND;	

	// Set object 
	m->obj.vsc_bind_req.vcsid = vcsid;
	m->obj.vsc_bind_req.vppbid = vppbid;
	m->obj.vsc_bind_req.ppid = ppid;
	m->obj.vsc_bind_req.ldid = ldid;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - VSC Get Info
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_vsc_get_vcs(struct fmapi_msg *m, int vcsid, int start, int limit)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_VSC_INFO;	

	// Set object 
	m->obj.vsc_info_req.num = 1;
	m->obj.vsc_info_req.vcss[0] = vcsid;
	m->obj.vsc_info_req.vppbid_start = start;
	m->obj.vsc_info_req.vppbid_limit = limit;

	rv = 0;

end:

	return rv;
}

/** 
 * Prepare an FM API Message - VSC Unbind
 *
 * @param m		fmapi_msg* to fill
 * @return 		0 upon success, non zero otherwise
 */
int fmapi_fill_vsc_unbind(struct fmapi_msg *m, int vcsid, int vppbid, int option)
{
	int rv;

	// Initialize variables
	rv = 1;

	// Validate Inputs 
	if (m == NULL)
		goto end;

	// Clear Header
	memset(&m->hdr, 0, sizeof(struct fmapi_hdr));

	// Set header 
	m->hdr.opcode = FMOP_VSC_UNBIND;	

	// Set object 
	m->obj.vsc_unbind_req.vcsid = vcsid;
	m->obj.vsc_unbind_req.vppbid = vppbid;
	m->obj.vsc_unbind_req.option = option;

	rv = 0;

end:

	return rv;
}
/**
 * @brief Convenience function to populate a fmapi_hdr object 
 *
 * @param[out] 	fh 			struct fmapi_hdr* to fill in
 * @param[in] 	category	__u8 Request / Response [FMMT]
 * @param[in] 	tag			__u8  		
 * @param[in] 	opcode    	__u16 	
 * @param[in] 	background 	__u8  		
 * @param[in] 	len  		__u32 Length of payload in bytes 		
 * @param[in] 	return_code __u16 		
 * @param[in] 	ext_status	__u16 	
 * @return      			Returns the length of the mctp_msg payload (HDR + Obj)
 */
int fmapi_fill_hdr
(
	struct fmapi_hdr *fh, 
	__u8 category,			//!< Type of FM API message [FMMT] (4 bit field)
	__u8 tag,				//!< Tag used to track response messages 
	__u16 opcode,    		//!< enum _FMOP
	__u8 background, 		//!< Run operation in the background (1 bit field)
	__u32 len, 				//!< Payload length in bytes (21 bit field)
	__u16 return_code,		//!< enum _FMRC
	__u16 ext_status		//!< Vendor Specific Extended Status  
)
{
	if (fh == NULL)
		return 0;
	fh->category = category;
	fh->tag = tag;
	fh->opcode = opcode;
	fh->len = len;
	fh->background = background;
	fh->return_code = return_code;
	fh->ext_status = ext_status;
	return FMLN_HDR + len;
}

/**
 * Determine the Request Object Identifier [FMOB] for an FM API Message Opcode [FMOP]
 *
 * @param	opcode 	This is an FM API Opcode [FMOP]
 * @return	int		Returns the FM API Object Identifier [FMOB] used in a Request
 */
int fmapi_fmob_req(unsigned int opcode)
{
	switch (opcode)
	{
		case FMOP_PSC_ID:				return FMOB_NULL;
		case FMOP_PSC_PORT:				return FMOB_PSC_PORT_REQ;
		case FMOP_PSC_PORT_CTRL:		return FMOB_PSC_PORT_CTRL_REQ;
		case FMOP_PSC_CFG:				return FMOB_PSC_CFG_REQ;
		case FMOP_VSC_INFO:				return FMOB_VSC_INFO_REQ;
		case FMOP_VSC_BIND:				return FMOB_VSC_BIND_REQ;;
		case FMOP_VSC_UNBIND:			return FMOB_VSC_UNBIND_REQ;
		case FMOP_VSC_AER:				return FMOB_VSC_AER_REQ;
		case FMOP_MPC_TMC:				return FMOB_MPC_TMC_REQ;
		case FMOP_MPC_CFG:				return FMOB_MPC_CFG_REQ;
		case FMOP_MPC_MEM:				return FMOB_MPC_MEM_REQ;
		case FMOP_MCC_INFO:				return FMOB_NULL;
		case FMOP_MCC_ALLOC_GET:		return FMOB_MCC_ALLOC_GET_REQ;
		case FMOP_MCC_ALLOC_SET:		return FMOB_MCC_ALLOC_SET_REQ;
		case FMOP_MCC_QOS_CTRL_GET:		return FMOB_NULL;
		case FMOP_MCC_QOS_CTRL_SET:		return FMOB_MCC_QOS_CTRL;
		case FMOP_MCC_QOS_STAT:			return FMOB_NULL;
		case FMOP_MCC_QOS_BW_ALLOC_GET:	return FMOB_MCC_QOS_BW_GET_REQ;
		case FMOP_MCC_QOS_BW_ALLOC_SET:	return FMOB_MCC_QOS_BW_ALLOC;
		case FMOP_MCC_QOS_BW_LIMIT_GET:	return FMOB_MCC_QOS_BW_LIMIT_GET_REQ;
		case FMOP_MCC_QOS_BW_LIMIT_SET:	return FMOB_MCC_QOS_BW_LIMIT;
		case FMOP_ISC_ID:				return FMOB_NULL;
		case FMOP_ISC_BOS:				return FMOB_NULL;
		case FMOP_ISC_MSG_LIMIT_GET:	return FMOB_NULL;
		case FMOP_ISC_MSG_LIMIT_SET:	return FMOB_ISC_MSG_LIMIT;
		default: 						return FMOB_NULL;
	}
}

/**
 * Determine the Response Object Identifier [FMOB] for an FM API Message Opcode [FMOP]
 *
 * @param	opcode 	This is an FM API Opcode [FMOP]
 * @return	int		Returns the FM API Object Identifier [FMOB] used in a Response 
 */
int fmapi_fmob_rsp(unsigned int opcode)
{
	switch (opcode)
	{
		case FMOP_PSC_ID:				return FMOB_PSC_ID_RSP;
		case FMOP_PSC_PORT:				return FMOB_PSC_PORT_RSP;
		case FMOP_PSC_PORT_CTRL:		return FMOB_NULL;
		case FMOP_PSC_CFG:				return FMOB_PSC_CFG_RSP;
		case FMOP_VSC_INFO:				return FMOB_VSC_INFO_RSP;
		case FMOP_VSC_BIND:				return FMOB_NULL;
		case FMOP_VSC_UNBIND:			return FMOB_NULL;
		case FMOP_VSC_AER:				return FMOB_NULL;
		case FMOP_MPC_TMC:				return FMOB_MPC_TMC_RSP;
		case FMOP_MPC_CFG:				return FMOB_MPC_CFG_RSP;
		case FMOP_MPC_MEM:				return FMOB_MPC_MEM_RSP;
		case FMOP_MCC_INFO:				return FMOB_MCC_INFO_RSP;
		case FMOP_MCC_ALLOC_GET:		return FMOB_MCC_ALLOC_GET_RSP;
		case FMOP_MCC_ALLOC_SET:		return FMOB_MCC_ALLOC_SET_RSP;
		case FMOP_MCC_QOS_CTRL_GET:		return FMOB_MCC_QOS_CTRL;
		case FMOP_MCC_QOS_CTRL_SET:		return FMOB_MCC_QOS_CTRL;
		case FMOP_MCC_QOS_STAT:			return FMOB_MCC_QOS_STAT_RSP;
		case FMOP_MCC_QOS_BW_ALLOC_GET:	return FMOB_MCC_QOS_BW_ALLOC;
		case FMOP_MCC_QOS_BW_ALLOC_SET:	return FMOB_MCC_QOS_BW_ALLOC;
		case FMOP_MCC_QOS_BW_LIMIT_GET:	return FMOB_MCC_QOS_BW_LIMIT;
		case FMOP_MCC_QOS_BW_LIMIT_SET:	return FMOB_MCC_QOS_BW_LIMIT;
		case FMOP_ISC_ID:				return FMOB_ISC_ID_RSP;
		case FMOP_ISC_BOS:				return FMOB_ISC_BOS;
		case FMOP_ISC_MSG_LIMIT_GET:	return FMOB_ISC_MSG_LIMIT;
		case FMOP_ISC_MSG_LIMIT_SET:	return FMOB_ISC_MSG_LIMIT;
		default: 						return FMOB_NULL;
	}
}

/**
 * @brief Convert an object into Little Endian byte array format
 * 
 * @param[out] dst void Pointer to destination unsigned char array
 * @param[in] src void Pointer to object to serialize
 * @param[in] type unsigned enum _FMOB representing type of object to serialize
 * @return number of serialized bytes, 0 if error 
 */
int fmapi_serialize(__u8 *dst, void *src, unsigned type)
{
	int rv;

	// Initialize variables 
	rv = 0;

	// Validate Inputs 
	if ( (type == FMOB_NULL) || (type >= FMOB_MAX) )
		return 0;

	switch(type)
	{
		case FMOB_HDR: //!< struct fmapi_hdr
		{
			struct fmapi_hdr *o = (struct fmapi_hdr*) src;
			dst[0] = (o->category << 4) & 0xF0;
			dst[1] = o->tag;
			dst[3] = (o->opcode & 0x00FF);
			dst[4] = ((o->opcode >> 8) & 0x00FF);
			dst[5] = (o->len         ) & 0x00FF;
			dst[6] = ( o->len >> 8   ) & 0x00FF;
			dst[7] = ((o->len >> 13  ) & 0x00F8) | (o->background & 0x01);
			dst[8] = (o->return_code     ) & 0x00FF; 
			dst[9] = (o->return_code >> 8) & 0x00FF;
			dst[10]= (o->ext_status     ) & 0x00FF;
			dst[11]= (o->ext_status >> 8) & 0x00FF;
			rv = FMLN_HDR;
		}
			break;

		case FMOB_ISC_BOS: //!< struct fmapi_isc_bos
		{
			struct fmapi_isc_bos *o = (struct fmapi_isc_bos*) src;
			dst[ 0] = (o->pcnt << 1) | (o->running & 0x01);
			dst[ 1] = 0;
			dst[ 2] =  o->opcode        & 0x00FF;
			dst[ 3] = (o->opcode >> 8 ) & 0x00FF;
			dst[ 4] =  o->rc            & 0x00FF;
			dst[ 5] = (o->rc     >> 8 ) & 0x00FF;
			dst[ 6] =  o->ext           & 0x00FF;
			dst[ 7] = (o->ext    >> 8 ) & 0x00FF;
			rv = FMLN_ISC_BOS;
		}
			break;

		case FMOB_ISC_ID_RSP: //!< struct fmapi_isc_id_rsp
		{
			struct fmapi_isc_id_rsp *o = (struct fmapi_isc_id_rsp*) src;
			dst[ 0] = (o->vid       ) & 0x00FF;
			dst[ 1] = (o->vid  >>  8) & 0x00FF;
			dst[ 2] = (o->did       ) & 0x00FF;
			dst[ 3] = (o->did  >>  8) & 0x00FF;
			dst[ 4] = (o->svid      ) & 0x00FF;
			dst[ 5] = (o->svid >>  8) & 0x00FF;
			dst[ 6] = (o->ssid      ) & 0x00FF;
			dst[ 7] = (o->ssid >>  8) & 0x00FF;
			dst[ 8] = (o->sn        ) & 0x00FF;	
			dst[ 9] = (o->sn   >>  8) & 0x00FF;	
			dst[10] = (o->sn   >> 16) & 0x00FF;	
			dst[11] = (o->sn   >> 24) & 0x00FF;	
			dst[12] = (o->sn   >> 32) & 0x00FF;	
			dst[13] = (o->sn   >> 40) & 0x00FF;	
			dst[14] = (o->sn   >> 48) & 0x00FF;	
			dst[15] = (o->sn   >> 56) & 0x00FF;	
			dst[16] =  o->size;
			rv = FMLN_ISC_ID_RSP;
		}
			break;

		case FMOB_ISC_MSG_LIMIT: //!< struct fmapi_isc_msg_limit
		{
			struct fmapi_isc_msg_limit *o = (struct fmapi_isc_msg_limit*) src;
			dst[0] =  o->limit;
			rv = FMLN_ISC_MSG_LIMIT;
		}
			break;

		case FMOB_PSC_ID_RSP: //!< struct fmapi_psc_id_rsp
		{
			struct fmapi_psc_id_rsp *o = (struct fmapi_psc_id_rsp*) src;
			dst[0]  = o->ingress_port;	
			dst[1]  = 0;  //rsvd
			dst[2]  = o->num_ports;	
			dst[3]  = o->num_vcss;
			memcpy(&dst[4], o->active_ports, 32);
			memcpy(&dst[36], o->active_vcss, 32);
			dst[68] = (o->num_vppbs         ) & 0x00FF;
			dst[69] = (o->num_vppbs    >>  8) & 0x00FF;
			dst[70] = (o->active_vppbs      ) & 0x00FF;
			dst[71] = (o->active_vppbs >>  8) & 0x00FF;
			dst[72] = o->num_decoders;
			rv = FMLN_PSC_IDENTIFY_SWITCH;
		}
			break;

		case FMOB_PSC_PORT_REQ: //!< struct fmapi_psc_port_req
		{
			struct fmapi_psc_port_req *o = (struct fmapi_psc_port_req*) src;
			dst[0] = o->num;
			memcpy(&dst[1], o->ports, o->num);
			rv = FMLN_PSC_GET_PHY_PORT_REQ + o->num;
		}
			break;

		case FMOB_PSC_PORT_INFO: //!< struct fmapi_psc_port_info
		{
			struct fmapi_psc_port_info *o = (struct fmapi_psc_port_info*) src;
			dst[0]  =  o->ppid;
			dst[1]  =  o->state;
			dst[2]  =  o->dv;
			dst[4]  =  o->dt;
			dst[5]  =  o->cv;
			dst[6]  =  o->mlw;
			dst[7]  =  o->nlw;
			dst[8]  =  o->speeds;
			dst[9]  =  o->mls;
			dst[10] =  o->cls;
			dst[11] =  o->ltssm;
			dst[12] =  o->lane;
			dst[13] = 0;
			dst[13] |= (o->lane_rev & 0x01) << FMLF_LANE_REVERSAL_BIT; 
			dst[13] |= (o->perst    & 0x01) << FMLF_PERST_STATE_BIT; 
			dst[13] |= (o->prsnt    & 0x01) << FMLF_PRSNT_STATE_BIT; 
			dst[13] |= (o->pwrctrl  & 0x01) << FMLF_PWRCTL_STATE_BIT; 
			dst[14] = 0;
			dst[15] =  o->num_ld;
			rv = FMLN_PSC_GET_PHY_PORT_INFO;
		}
			break;

		case FMOB_PSC_PORT_RSP: //!< struct fmapi_psc_port_rsp
		{
			struct fmapi_psc_port_rsp *o = (struct fmapi_psc_port_rsp*) src;	
			dst[0]  =  o->num;
			rv = FMLN_PSC_GET_PHY_PORT_RESP;
			for ( int i = 0 ; i < o->num ; i++) 
				rv += fmapi_serialize(&dst[rv], &o->list[i], FMOB_PSC_PORT_INFO);
		}
			break;

		case FMOB_PSC_PORT_CTRL_REQ: //!< struct fmapi_psc_port_ctrl_req
		{
			struct fmapi_psc_port_ctrl_req *o = (struct fmapi_psc_port_ctrl_req*) src;
			dst[0] = o->ppid;
			dst[1] = o->opcode;;
			rv = FMLN_PSC_PHY_PORT_CTRL;
		}
			break;

		case FMOB_PSC_CFG_REQ: //!< struct fmapi_psc_cfg_req
		{
			struct fmapi_psc_cfg_req *o = (struct fmapi_psc_cfg_req*) src;
			dst[0] = o->ppid;
			dst[1] = o->reg;
			dst[2] = ((o->fdbe << 4) & 0xF0) | (o->ext & 0x0F);
			dst[3] = ( o->type << 7) & 0x80;
			dst[4] = o->data[0];
			dst[5] = o->data[1];
			dst[6] = o->data[2];
			dst[7] = o->data[3];
			rv = FMLN_PSC_PPB_IO_CFG_REQ;
		}
			break;

		case FMOB_PSC_CFG_RSP: //!< struct fmapi_psc_cfg_rsp 
		{
			struct fmapi_psc_cfg_rsp *o = (struct fmapi_psc_cfg_rsp*) src;
			dst[0] = o->data[0];
			dst[1] = o->data[1];
			dst[2] = o->data[2];
			dst[3] = o->data[3];
			rv = FMLN_PSC_PPB_IO_CFG_RESP;
		}
			break;

		case FMOB_VSC_INFO_REQ: //!< struct fmapi_vsc_info_req
		{
			struct fmapi_vsc_info_req *o = (struct fmapi_vsc_info_req*) src;
			dst[0] = o->vppbid_start;
			dst[1] = o->vppbid_limit;
			dst[2] = o->num;
			memcpy(&dst[3], o->vcss, o->num);
			rv = FMLN_VSC_GET_INFO_REQ + o->num;
		}
		break;

		case FMOB_VSC_PPB_STAT_BLK: //!< struct fmapi_vsc_ppb_stat_blk
		{
			struct fmapi_vsc_ppb_stat_blk *o = (struct fmapi_vsc_ppb_stat_blk*) src;
			dst[0] = o->status;
			dst[1] = o->ppid;
			dst[2] = o->ldid;
			rv = FMLN_VSC_PPB_STATUS;
		}
			break;

		case FMOB_VSC_INFO_BLK: //!< struct fmapi_vsc_info_blk
		{
			struct fmapi_vsc_info_blk *o = (struct fmapi_vsc_info_blk*) src;
			dst[0] = o->vcsid;
			dst[1] = o->state;
			dst[2] = o->uspid;
			dst[3] = o->total;

			rv = FMLN_VSC_INFO;
			for (int i = 0 ; i < o->num ; i++)
				rv += fmapi_serialize(&dst[rv], &o->list[i], FMOB_VSC_PPB_STAT_BLK);
		}
			break;

		case FMOB_VSC_INFO_RSP: //!< struct fmapi_vsc_info_rsp
		{
			struct fmapi_vsc_info_rsp *o = (struct fmapi_vsc_info_rsp*) src;
			dst[0] = o->num;

			rv = FMLN_VSC_GET_INFO_RESP;
			for ( int i = 0 ; i < o->num ; i++)
				rv += fmapi_serialize(&dst[rv], &o->list[i], FMOB_VSC_INFO_BLK);
		}
			break;

		case FMOB_VSC_BIND_REQ: //!< struct fmapi_vsc_bind_req
		{
			struct fmapi_vsc_bind_req *o = (struct fmapi_vsc_bind_req*) src;
			dst[0] = o->vcsid;
			dst[1] = o->vppbid;
			dst[2] = o->ppid;
			dst[4] = (o->ldid      ) & 0x00FF;
			dst[5] = (o->ldid >> 8 ) & 0x00FF;
			rv = FMLN_VSC_BIND;
		}
			break;

		case FMOB_VSC_UNBIND_REQ: //!< struct fmapi_vsc_unbind_req
		{
			struct fmapi_vsc_unbind_req *o = (struct fmapi_vsc_unbind_req*) src;
			dst[0] = o->vcsid;
			dst[1] = o->vppbid;
			dst[2] = o->option;;
			rv = FMLN_VSC_UNBIND;
		}
			break;

		case FMOB_VSC_AER_REQ: //!< struct fmapi_vsc_aer_req
		{
			struct fmapi_vsc_aer_req *o = (struct fmapi_vsc_aer_req*) src;
			dst[0] = o->vcsid;
			dst[1] = o->vppbid;
			dst[4] = (o->error_type  	 )	& 0x00FF;
			dst[5] = (o->error_type >>  8) 	& 0x00FF;
			dst[6] = (o->error_type >> 16) 	& 0x00FF;
			dst[7] = (o->error_type >> 24) 	& 0x00FF;
			memcpy(&dst[8], o->header, 32);
			rv = FMLN_VSC_GEN_AER;
		}
			break;

		case FMOB_MPC_TMC_REQ: //!< struct fmapi_mpc_tmc_req
		{
			struct fmapi_mpc_tmc_req *o = (struct fmapi_mpc_tmc_req*) src;
			dst[0] = o->ppid;
			dst[2] = ((o->len+1)      ) & 0x00FF;
			dst[3] = ((o->len+1) >> 8 ) & 0x00FF;
			dst[4] = o->type;
			memcpy(&dst[FMLN_MPC_TUNNEL_CMD_REQ], o->msg, o->len);
			rv = FMLN_MPC_TUNNEL_CMD_REQ + o->len;
		}
			break;

		case FMOB_MPC_TMC_RSP: //!< struct fmapi_mpc_tmc_rsp
		{
			struct fmapi_mpc_tmc_rsp *o = (struct fmapi_mpc_tmc_rsp*) src;
			dst[0] = ((o->len+1)      ) & 0x00FF;
			dst[1] = ((o->len+1) >> 8 ) & 0x00FF;
			dst[4] = o->type;
			memcpy(&dst[FMLN_MPC_TUNNEL_CMD_REQ], o->msg, o->len);
			rv = FMLN_MPC_TUNNEL_CMD_RESP + o->len;
		}
			break;

		case FMOB_MPC_CFG_REQ: //!< struct fmapi_mpc_cfg_req
		{
			struct fmapi_mpc_cfg_req *o = (struct fmapi_mpc_cfg_req*) src;
			dst[0] = o->ppid;
			dst[1] = o->reg;
			dst[2] = ((o->fdbe << 4) & 0xF0) | (o->ext & 0x0F);
			dst[3] = (o->type  << 7) & 0x80;
			dst[4] = (o->ldid      ) & 0x00FF;
			dst[5] = (o->ldid >> 8 ) & 0x00FF;
			dst[8] = o->data[0];
			dst[9] = o->data[1];
			dst[10] = o->data[2];
			dst[11] = o->data[3];
			rv = FMLN_MPC_LD_IO_CFG_REQ;
		}
			break;

		case FMOB_MPC_CFG_RSP: //!< struct fmapi_mpc_cfg_rsp
		{
			struct fmapi_mpc_cfg_rsp *o = (struct fmapi_mpc_cfg_rsp*) src;
			dst[0] = o->data[0];
			dst[1] = o->data[1];
			dst[2] = o->data[2];
			dst[3] = o->data[3];
			rv = FMLN_MPC_LD_IO_CFG_RESP;
		}
			break;

		case FMOB_MPC_MEM_REQ: //!< struct fmapi_mpc_mem_req
		{
			struct fmapi_mpc_mem_req *o = (struct fmapi_mpc_mem_req*) src;
			dst[ 0] =   o->ppid;
			dst[ 2] =  (o->fdbe   << 4 ) & 0x00F0; 		
			dst[ 3] = ((o->type   << 7 ) & 0x0080) | (o->ldbe & 0x000F); 	
			dst[ 4] =  (o->ldid        ) & 0x00FF;
			dst[ 5] =  (o->ldid   >> 8 ) & 0x00FF;
			dst[ 6] =  (o->len         ) & 0x00FF;
			dst[ 7] =  (o->len    >> 8 ) & 0x00FF;
			dst[ 8] =  (o->offset      ) & 0x00FF;	
			dst[ 9] =  (o->offset >>  8) & 0x00FF;	
			dst[10] =  (o->offset >> 16) & 0x00FF;	
			dst[11] =  (o->offset >> 24) & 0x00FF;	
			dst[12] =  (o->offset >> 32) & 0x00FF;	
			dst[13] =  (o->offset >> 40) & 0x00FF;	
			dst[14] =  (o->offset >> 48) & 0x00FF;	
			dst[15] =  (o->offset >> 56) & 0x00FF;	
			memcpy(&dst[FMLN_MPC_LD_MEM_REQ], o->data, o->len);
			rv = FMLN_MPC_LD_MEM_REQ + o->len;
		}
			break;

		case FMOB_MPC_MEM_RSP: //!< struct fmapi_mpc_mem_rsp
		{
			struct fmapi_mpc_mem_rsp *o = (struct fmapi_mpc_mem_rsp*) src;
			dst[0] =  (o->len        ) & 0x00FF;
			dst[1] =  (o->len   >> 8 ) & 0x00FF;
			memcpy(&dst[FMLN_MPC_LD_MEM_RESP], o->data, o->len);
			rv = FMLN_MPC_LD_MEM_RESP + o->len;
		}
			break;

		case FMOB_MCC_INFO_RSP: //!< struct fmapi_mcc_info_rsp
		{
			struct fmapi_mcc_info_rsp *o = (struct fmapi_mcc_info_rsp*) src;
			dst[0] =  (o->size      ) & 0x00FF;	
			dst[1] =  (o->size >>  8) & 0x00FF;	
			dst[2] =  (o->size >> 16) & 0x00FF;	
			dst[3] =  (o->size >> 24) & 0x00FF;	
			dst[4] =  (o->size >> 32) & 0x00FF;	
			dst[5] =  (o->size >> 40) & 0x00FF;	
			dst[6] =  (o->size >> 48) & 0x00FF;	
			dst[7] =  (o->size >> 56) & 0x00FF;	
			dst[8] =  (o->num      ) & 0x00FF;
			dst[9] =  (o->num >> 8 ) & 0x00FF;
			dst[10]  = (o->epc & 0x01) << FMQT_EGRESS_PORT_CONGESTION_BIT;
			dst[10] |= (o->ttr & 0x01) << FMQT_TEMP_THROUGHPUT_REDUCTION_BIT;
			rv = FMLN_MCC_GET_LD_INFO;
		}
			break;

		case FMOB_MCC_ALLOC_BLK: //!< struct fmapi_mcc_ldalloc_entry
		{
			struct fmapi_mcc_alloc_blk *o = (struct fmapi_mcc_alloc_blk*) src;
			dst[0] =  (o->rng1      ) & 0x00FF;	
			dst[1] =  (o->rng1 >>  8) & 0x00FF;	
			dst[2] =  (o->rng1 >> 16) & 0x00FF;	
			dst[3] =  (o->rng1 >> 24) & 0x00FF;	
			dst[4] =  (o->rng1 >> 32) & 0x00FF;	
			dst[5] =  (o->rng1 >> 40) & 0x00FF;	
			dst[6] =  (o->rng1 >> 48) & 0x00FF;	
			dst[7] =  (o->rng1 >> 56) & 0x00FF;	
			dst[8] =  (o->rng2      ) & 0x00FF;	
			dst[9] =  (o->rng2 >>  8) & 0x00FF;	
			dst[10] =  (o->rng2 >> 16) & 0x00FF;	
			dst[11] =  (o->rng2 >> 24) & 0x00FF;	
			dst[12] =  (o->rng2 >> 32) & 0x00FF;	
			dst[13] =  (o->rng2 >> 40) & 0x00FF;	
			dst[14] =  (o->rng2 >> 48) & 0x00FF;	
			dst[15] =  (o->rng2 >> 56) & 0x00FF;	
			rv = FMLN_MCC_LD_ALLOC_ENTRY;
		}
			break;

		case FMOB_MCC_ALLOC_GET_REQ: //!< struct fmapi_mcc_alloc_get_req
		{
			struct fmapi_mcc_alloc_get_req *o = (struct fmapi_mcc_alloc_get_req*) src;
			dst[0] = o->start;
			dst[1] = o->limit;
			rv = FMLN_MCC_GET_LD_ALLOC_REQ;
		}
			break;

		case FMOB_MCC_ALLOC_GET_RSP: //!< struct fmapi_mcc_alloc_get_rsp
		{
			struct fmapi_mcc_alloc_get_rsp *o = (struct fmapi_mcc_alloc_get_rsp*) src;
			dst[0] = o->total;
			dst[1] = o->granularity;
			dst[2] = o->start;
			dst[3] = o->num;
			rv = FMLN_MCC_GET_LD_ALLOC_RSP;
			for ( int i = 0 ; i < o->num ; i++ )
				rv += fmapi_serialize(&dst[rv], &o->list[i], FMOB_MCC_ALLOC_BLK);
		}
			break;

		case FMOB_MCC_ALLOC_SET_REQ: //!< struct fmapi_mcc_alloc_set_req
		{
			struct fmapi_mcc_alloc_set_req *o = (struct fmapi_mcc_alloc_set_req*) src;
			dst[0] = o->num;
			dst[1] = o->start;
			rv = FMLN_MCC_SET_LD_ALLOC_REQ;
			for ( int i = 0 ; i < o->num ; i++ )
				rv += fmapi_serialize(&dst[rv], &o->list[i], FMOB_MCC_ALLOC_BLK);
		}
			break;

		case FMOB_MCC_ALLOC_SET_RSP: //!< struct fmapi_mcc_alloc_set_rsp
		{
			struct fmapi_mcc_alloc_set_rsp *o = (struct fmapi_mcc_alloc_set_rsp*) src;
			dst[0] = o->num;
			dst[1] = o->start;
			rv = FMLN_MCC_SET_LD_ALLOC_RSP;
			for ( int i = 0 ; i < o->num ; i++ )
				rv += fmapi_serialize(&dst[rv], &o->list[i], FMOB_MCC_ALLOC_BLK);
		}
			break;

		case FMOB_MCC_QOS_CTRL: //!< struct fmapi_mcc_qos_ctrl
		{
			struct fmapi_mcc_qos_ctrl *o = (struct fmapi_mcc_qos_ctrl*) src;
			dst[0] = 0;
			dst[0] |= o->epc_en       << FMQT_EGRESS_PORT_CONGESTION_BIT;
			dst[0] |= o->ttr_en << FMQT_TEMP_THROUGHPUT_REDUCTION_BIT;
			dst[1] = o->egress_mod_pcnt;
			dst[2] = o->egress_sev_pcnt;
			dst[3] = o->sample_interval;
			dst[4] = (o->rcb      ) & 0x00FF;
			dst[5] = (o->rcb >> 8 ) & 0x00FF;
			dst[6] = o->comp_interval;
			rv = FMLN_MCC_QOS_CTRL;
		}
			break;

		case FMOB_MCC_QOS_STAT_RSP: //!< struct fmapi_mcc_qos_stat_rsp
		{
			struct fmapi_mcc_qos_stat_rsp *o = (struct fmapi_mcc_qos_stat_rsp*) src;
			dst[0] = o->bp_avg_pcnt;
			rv = FMLN_MCC_QOS_STATUS;
		}
			break;

		case FMOB_MCC_QOS_BW_GET_REQ:
		{
			struct fmapi_mcc_qos_bw_alloc_get_req *o = (struct fmapi_mcc_qos_bw_alloc_get_req*) src;
			dst[0] = o->num;
			dst[1] = o->start;
			rv = FMLN_MCC_GET_QOS_BW_REQ;
		}
			break;

		case FMOB_MCC_QOS_BW_ALLOC: //!< struct fmapi_mcc_qos_bw_alloc
		{
			struct fmapi_mcc_qos_bw_alloc *o = (struct fmapi_mcc_qos_bw_alloc*) src;
			dst[0] = o->num;
			dst[1] = o->start;
			rv = FMLN_MCC_QOS_BW_ALLOC;
			for ( int i = 0 ; i < o->num ; i++ )
				dst[rv + i] = o->list[i];
			rv += o->num;
		}
			break;

		case FMOB_MCC_QOS_BW_LIMIT_GET_REQ: //!< struct fmapi_mcc_qos_bw_limit_get_req 
		{
			struct fmapi_mcc_qos_bw_limit_get_req *o = (struct fmapi_mcc_qos_bw_limit_get_req*) src;
			dst[0] = o->num;
			dst[1] = o->start;
			rv = FMLN_MCC_GET_QOS_BW_LIMIT_REQ;
		}
			break;

		case FMOB_MCC_QOS_BW_LIMIT: //!< struct fmapi_mcc_qos_bw_limit
		{
			struct fmapi_mcc_qos_bw_limit *o = (struct fmapi_mcc_qos_bw_limit*) src;
			dst[0] = o->num;
			dst[1] = o->start;
			rv = FMLN_MCC_QOS_BW_LIMIT;
			for ( int i = 0 ; i < o->num ; i++ )
				dst[rv + i] = o->list[i];
			rv += o->num;
		}
			break;

		default:
			rv = 0;
			break;
	}
	
	return rv;
};

/* Functions to return a string representation of an object*/
const char *fmbs(unsigned int u)
{
	if (u >= FMBS_MAX) 	return NULL;
	else 				return STR_FMBS[u];	
}

const char *fmct(unsigned int u)
{
	if (u >= FMCT_MAX) 	return NULL;
	else 				return STR_FMCT[u];	
}

const char *fmdt(unsigned int u)
{
	if (u >= FMDT_MAX) 	return NULL;
	else 				return STR_FMDT[u];	
}

const char *fmdv(unsigned int u)
{
	if (u >= FMDV_MAX) 	return NULL;
	else 				return STR_FMDV[u];	
}

const char *fmet(unsigned int u)
{
	if (u >= FMET_MAX) 	return NULL;
	else 				return STR_FMET[u];	
}

const char *fmlf(unsigned int u)
{
	int i;
	switch (u)
	{
		case FMLF_LANE_REVERSAL_BIT: 	i = 0; break; 	// 0x01
		case FMLF_PERST_STATE_BIT: 		i = 1; break;	// 0x02
		case FMLF_PRSNT_STATE_BIT:		i = 2; break;	// 0x04
		case FMLF_PWRCTL_STATE_BIT:		i = 3; break; 	// 0x08
		default: 						return NULL;
	}
	return STR_FMLF[i];	
}

const char *fmlo(unsigned int u)
{
	if (u >= FMLO_MAX) 	return NULL;
	else 				return STR_FMLO[u];	
}

const char *fmls(unsigned int u)
{
	if (u >= FMLS_MAX) 	return NULL;
	else 				return STR_FMLS[u];	
}

const char *fmmg(unsigned int u)
{
	if (u >= FMMG_MAX) 	return NULL;
	else 				return STR_FMMG[u];	
}

const char *fmmr(unsigned int u)
{
	if (u >= FMMR_MAX) 	return NULL;
	else 				return STR_FMMR[u];	
}

const char *fmms(unsigned int u)
{
	if (u >= FMMS_MAX) 	return NULL;
	else 				return STR_FMMS[u];	
}

const char *fmmt(unsigned int u)
{
	if (u >= FMMT_MAX) 	return NULL;
	else 				return STR_FMMT[u];	
}

const char *fmnw(unsigned int u)
{
	int i;
	switch (u)
	{
		case FMNW_X1: i = 0; break;	//  PCI_EXP_LNKSTA_NLW_X1, // 0x0010
		case FMNW_X2: i = 1; break;	//  PCI_EXP_LNKSTA_NLW_X2, // 0x0020
		case FMNW_X4: i = 2; break;	//  PCI_EXP_LNKSTA_NLW_X4, // 0x0040 
		case FMNW_X8: i = 3; break;	//  PCI_EXP_LNKSTA_NLW_X8, // 0x0080
		default:	  i = 4; break; 
	}
	return STR_FMNW[i];	
}

const char *fmop(unsigned int u) 
{
	unsigned int group = (u >> 8) & 0x0F;
	u &= 0x00FF;
	switch (group) 
	{
		case 1:	
			if (u >= 4) 	return NULL;
			else			return STR_FMOP_1[u];	

		case 2:	
			if (u >= 4) 	return NULL;
			else			return STR_FMOP_2[u];	

		case 3:	
			if (u >= 3)		return NULL;
			else 			return STR_FMOP_3[u];	

		case 4:	
			if (u >= 10) 	return NULL;
			else			return STR_FMOP_4[u];	
	}
	return NULL;
}

const char *fmpo(unsigned int u)
{
	if (u >= FMPO_MAX) 	return NULL;
	else 				return STR_FMPO[u];	
}

const char *fmps(unsigned int u)
{
	if (u == FMPS_INVALID)
		return STR_FMPS[FMPS_MAX];

	if (u >= FMPS_MAX) 	return NULL;
	else 				return STR_FMPS[u];	
}

const char *fmqt(unsigned int u)
{
	int i;
	switch (u)
	{
		case FMQT_EGRESS_PORT_CONGESTION_BIT: 	 i = 0; break; // 0x01
		case FMQT_TEMP_THROUGHPUT_REDUCTION_BIT: i = 1; break; // 0x02,
		default: return NULL;
	}
	return STR_FMQT[i];
}

const char *fmrc(unsigned int u)
{
	if (u >= FMRC_MAX) 	return NULL;
	else 				return STR_FMRC[u];	
}

const char *fmss(unsigned int u)
{
	int i;
	switch (u)
	{
		case FMSS_PCIE1: i = 0; break; 	// PCI_EXP_LNKCAP2_SLS_2_5GB, 	// 0x02
		case FMSS_PCIE2: i = 1; break; 	// PCI_EXP_LNKCAP2_SLS_5_0GB, 	// 0x04
		case FMSS_PCIE3: i = 2; break; 	// PCI_EXP_LNKCAP2_SLS_8_0GB, 	// 0x08
		case FMSS_PCIE4: i = 3; break; 	// PCI_EXP_LNKCAP2_SLS_16_0GB,	// 0x10
		case FMSS_PCIE5: i = 4; break; 	// PCI_EXP_LNKCAP2_SLS_32_0GB,	// 0x20
		case FMSS_PCIE6: i = 5; break; 	// PCI_EXP_LNKCAP2_SLS_64_0GB	// 0x40
		default: return NULL;
	}
	return STR_FMSS[i];	
}

const char *fmub(unsigned int u)
{
	if (u >= FMUB_MAX) 	return NULL;
	else 				return STR_FMSS[u];	
}

const char *fmvs(unsigned int u)
{
	int i;
	switch(u)
	{
		case FMVS_DISABLED: 	i = 0; break;; 	// FMVS_DISABLED 	= 0,
		case FMVS_ENABLED: 		i = 1; break;	// FMVS_ENABLED		= 1,
		case FMVS_INVALID:		i = 2; break;	// FMVS_INVALID		= 0xFF
		default:				return NULL;
	}
	return STR_FMVS[i];
}

const char *fmvc(unsigned int u)
{
	int i;
	switch(u)
	{
		case FMCV_CXL1_1: i = 0; break; // 0x01
		case FMCV_CXL2_0: i = 1; break; // 0x02
		default: return NULL;
	}
	return STR_FMVC[i];	
}

const char *fmvt(unsigned int u)
{
	if (u >= FMVT_MAX) 	return NULL;
	else 				return STR_FMVT[u];	
}

/**
 * Print an object to the screen
 *
 * @param ptr A pointer to the object to print 
 * @param type The type of object to be printed from enum _FMOB
 */
void fmapi_prnt(void *ptr, unsigned type)
{
	switch (type)
	{
		case FMOB_HDR:                    	fmapi_prnt_hdr(ptr);						break;
		case FMOB_PSC_ID_RSP:    		  	fmapi_prnt_psc_id_rsp(ptr);					break;
		case FMOB_PSC_PORT_REQ:           	fmapi_prnt_psc_port_req(ptr);				break;
		case FMOB_PSC_PORT_INFO:  	      	fmapi_prnt_psc_port_info(ptr);				break;
		case FMOB_PSC_PORT_RSP:           	fmapi_prnt_psc_port_rsp(ptr);				break;
		case FMOB_PSC_PORT_CTRL_REQ:      	fmapi_prnt_psc_port_ctrl_req(ptr);			break;
		case FMOB_PSC_CFG_REQ:     	      	fmapi_prnt_psc_cfg_req(ptr);				break;
		case FMOB_PSC_CFG_RSP:            	fmapi_prnt_psc_cfg_rsp(ptr);				break;
		case FMOB_VSC_INFO_REQ:           	fmapi_prnt_vsc_info_req(ptr);				break;
		case FMOB_VSC_PPB_STAT_BLK:       	fmapi_prnt_vsc_ppb_stat_blk(ptr);			break;
		case FMOB_VSC_INFO_BLK:           	fmapi_prnt_vsc_info_blk(ptr);				break;
		case FMOB_VSC_INFO_RSP:           	fmapi_prnt_vsc_info_rsp(ptr);				break;
		case FMOB_VSC_BIND_REQ:           	fmapi_prnt_vsc_bind_req(ptr);				break;
		case FMOB_VSC_UNBIND_REQ:         	fmapi_prnt_vsc_unbind_req(ptr);				break;
		case FMOB_VSC_AER_REQ:            	fmapi_prnt_vsc_aer_req(ptr);				break;
		case FMOB_MPC_TMC_REQ:            	fmapi_prnt_mpc_tmc_req(ptr);				break;
		case FMOB_MPC_TMC_RSP:            	fmapi_prnt_mpc_tmc_rsp(ptr);				break;
		case FMOB_MPC_CFG_REQ:            	fmapi_prnt_mpc_cfg_req(ptr);				break;
		case FMOB_MPC_CFG_RSP:     	      	fmapi_prnt_mpc_cfg_rsp(ptr);				break;
		case FMOB_MPC_MEM_REQ:            	fmapi_prnt_mpc_mem_req(ptr);				break;
		case FMOB_MPC_MEM_RSP:            	fmapi_prnt_mpc_mem_rsp(ptr);				break;
		case FMOB_MCC_INFO_RSP:        	  	fmapi_prnt_mcc_info_rsp(ptr);				break;
		case FMOB_MCC_ALLOC_BLK:          	fmapi_prnt_mcc_alloc_blk(ptr);				break;
		case FMOB_MCC_ALLOC_GET_REQ:      	fmapi_prnt_mcc_alloc_get_req(ptr);			break;
		case FMOB_MCC_ALLOC_GET_RSP:      	fmapi_prnt_mcc_alloc_get_rsp(ptr);			break;
		case FMOB_MCC_ALLOC_SET_REQ:      	fmapi_prnt_mcc_alloc_set_req(ptr);			break;
		case FMOB_MCC_ALLOC_SET_RSP:      	fmapi_prnt_mcc_alloc_set_rsp(ptr);			break;
		case FMOB_MCC_QOS_CTRL:           	fmapi_prnt_mcc_qos_ctrl(ptr);				break;
		case FMOB_MCC_QOS_STAT_RSP:       	fmapi_prnt_mcc_qos_stat_rsp(ptr);			break;
		case FMOB_MCC_QOS_BW_GET_REQ:	  	fmapi_prnt_mcc_qos_bw_alloc_get_req(ptr); 	break;
		case FMOB_MCC_QOS_BW_ALLOC:       	fmapi_prnt_mcc_qos_bw_alloc(ptr);			break;
		case FMOB_MCC_QOS_BW_LIMIT_GET_REQ: fmapi_prnt_mcc_qos_bw_limit_get_req(ptr);	break;
		case FMOB_MCC_QOS_BW_LIMIT: 		fmapi_prnt_mcc_qos_bw_limit(ptr);     		break;
		case FMOB_ISC_ID_RSP: 				fmapi_prnt_isc_id_rsp(ptr); 				break;
		case FMOB_ISC_BOS: 					fmapi_prnt_isc_bos(ptr);					break;
		case FMOB_ISC_MSG_LIMIT: 			fmapi_prnt_isc_msg_limit(ptr); 				break;
		default: break;
	}
}

void fmapi_prnt_hdr(void *ptr)
{
	struct fmapi_hdr *o = (struct fmapi_hdr*) ptr;
	printf("fmapi_hdr:\n");
	printf("Category:          0x%02x\n", o->category);
	printf("Tag:               0x%02x\n", o->tag);
	printf("Opcode:            0x%04x\n", o->opcode);
	printf("Len:               0x%06x\n", o->len);
	printf("Background:        0x%02x\n", o->background);
	printf("Return Code:       0x%04x\n", o->return_code);
	printf("Extended Status:   0x%04x\n", o->ext_status);
}

void fmapi_prnt_isc_id_rsp(void *ptr)
{
	struct fmapi_isc_id_rsp *o = (struct fmapi_isc_id_rsp*) ptr;
	printf("fmapi_isc_id_rsp:\n");
	printf("PCIe Vendor ID:           0x%x\n", o->vid);
	printf("PCIe Device ID:           0x%x\n", o->did);
	printf("PCIe Subsystem Vendor ID: 0x%x\n", o->svid);
	printf("PCIe Subsystem ID:        0x%x\n", o->ssid);
	printf("SN:                       0x%llx\n", o->sn);
	printf("Max Msg Size n of 2^n:    %d\n", o->size);
}

void fmapi_prnt_isc_bos(void *ptr)
{
	struct fmapi_isc_bos *o = (struct fmapi_isc_bos*) ptr;
	printf("fmapi_isc_bos:\n");
	printf("Background Op. Running:   %d\n", o->running);
	printf("Percent Complete:         %d%%\n", o->pcnt);
	printf("Command Opcode:           0x%04x\n", o->opcode);
	printf("Return Code:              0x%04x\n", o->rc);
	printf("Vendor Specific Status:   0x%04x\n", o->ext);
}

void fmapi_prnt_isc_msg_limit(void *ptr)
{
	struct fmapi_isc_msg_limit *o = (struct fmapi_isc_msg_limit*) ptr;
	printf("fmapi_isc_msg_limit:\n");
	printf("Limit:                    %d\n", o->limit);
}

void fmapi_prnt_psc_id_rsp(void *ptr)
{
	struct fmapi_psc_id_rsp *o = (struct fmapi_psc_id_rsp*) ptr;
	printf("fmapi_psc_id_rsp:\n");
	printf("Ingress Port ID:          %d\n", o->ingress_port);
	printf("Num Physical Ports:       %u\n", o->num_ports);
	printf("Num VCSs:                 %u\n", o->num_vcss);
	printf("Num VPPBs:                %u\n", o->num_vppbs);
	printf("Num Active VPPBs:         %u\n", o->active_vppbs); 
	printf("Num HDM Decoders:         %u\n", o->num_decoders);
	for ( int i = 0 ; i < 32 ; i++) 
		printf("Active Port Bitmask:      0x%02x\n", o->active_ports[i]);
	for ( int i = 0 ; i < 32 ; i++) 
		printf("Active VCS Bitmask:       0x%02x\n", o->active_vcss[i]);
}

void fmapi_prnt_psc_port_req(void *ptr)
{
	struct fmapi_psc_port_req *o = (struct fmapi_psc_port_req*) ptr;
	printf("fmapi_psc_port_req:\n");
	printf("Num Ports:                %d\n", o->num);
	for ( int i = 0 ; i < o->num ; i++)
		printf("Ports[%03d]:              %d\n", i, o->ports[i]);
}

void fmapi_prnt_psc_port_info(void *ptr)
{
	struct fmapi_psc_port_info *o = (struct fmapi_psc_port_info*) ptr;
	printf("fmapi_psc_port_info:\n");
	printf("Port ID:                       %u\n", 			o->ppid);
	printf("Current Port state;            %d - %s\n", 		o->state, 	fmps(o->state));
	printf("Connected Device CXL version:  %d - %s\n", 		o->dv, 		fmdv(o->dv));
	printf("Connected Device Type:         %d - %s\n", 		o->dt, 		fmdt(o->dt));
	printf("Connected device CXL Version:  %d - %s\n", 		o->cv, 		fmvc(o->cv));
	printf("Max link width:                %d\n", 			o->mlw);
	printf("Negotiated link width:         0x%x - %s\n", 	o->nlw, 	fmnw(o->nlw));
	printf("Supported Link speeds vector:  0x%x - %s\n", 	o->speeds,	fmss(o->speeds));
	printf("Max Link Speed:                0x%x - %s\n", 	o->mls, 	fmms(o->mls));
	printf("Current Link Speed:            0x%x - %s\n", 	o->cls,		fmms(o->cls));
	printf("LTSSM State:                   %d - %s\n", 		o->ltssm, 	fmls(o->ltssm));
	printf("First negotiated lane number:  %d\n",			o->lane);
	printf("Lane Reversal State            %d\n", 			o->lane_rev);
	printf("PCIe Reset State               %d\n", 			o->perst);
	printf("Port Presence pin state        %d\n", 			o->prsnt);
	printf("Power Control State            %d\n", 			o->pwrctrl);
	printf("Supported LD count:            %d\n", 			o->num_ld);
}

void fmapi_prnt_psc_port_rsp(void *ptr)
{
	struct fmapi_psc_port_rsp *o = (struct fmapi_psc_port_rsp*) ptr;
	printf("fmapi_psc_port_rsp:\n");
	printf("Number of Ports:               %d\n", o->num);
	for (int i = 0 ; i < o->num ; i++)
		fmapi_prnt_psc_port_info(&o->list[i]);
}

void fmapi_prnt_psc_port_ctrl_req(void *ptr)
{
	struct fmapi_psc_port_ctrl_req *o = (struct fmapi_psc_port_ctrl_req*) ptr;
	printf("fmapi_psc_port_ctrl_req:\n");
	printf("PPID:                 0x%02x\n", o->ppid);
	printf("Port Opcode:          0x%02x\n", o->opcode);	
}

void fmapi_prnt_psc_cfg_req(void *ptr)
{
	struct fmapi_psc_cfg_req *o = (struct fmapi_psc_cfg_req*) ptr;
	printf("fmapi_psc_cfg_req:\n");
	printf("PPID:                        0x%02x\n", o->ppid);
	printf("Register Number:             0x%02x\n", o->reg);
	printf("Extended Register Number:    0x%02x\n", o->ext);
	printf("First DWord Byte Enable:     0x%02x\n", o->fdbe); 		
	printf("Transation type:             0x%x\n", o->type);
}

void fmapi_prnt_psc_cfg_rsp(void *ptr)
{
	struct fmapi_psc_cfg_rsp *o = (struct fmapi_psc_cfg_rsp*) ptr;
	printf("fmapi_psc_cfg_rsp:\n");
	printf("Transaction Data:            0x%02x %02x %02x %02x\n", o->data[0], o->data[1], o->data[2], o->data[3]);
}

void fmapi_prnt_vsc_info_req(void *ptr)
{
	struct fmapi_vsc_info_req *o = (struct fmapi_vsc_info_req*) ptr;
	printf("fmapi_vsc_info_req:\n");
	printf("vPPBID Start:                 %d\n", o->vppbid_start);
	printf("vPPBID Limit:                 %d\n", o->vppbid_limit);
	printf("Number of VCSs:               %d\n", o->num);
	for (int i = 0 ; i < o->num ; i++)
		printf("VCSs[%03d]:                    %d\n", i, o->vcss[i]);
}

void fmapi_prnt_vsc_ppb_stat_blk(void *ptr)
{
	struct fmapi_vsc_ppb_stat_blk *o = (struct fmapi_vsc_ppb_stat_blk*) ptr;
	printf("fmapi_vsc_ppb_stat_blk:\n");
	printf("bind_status:                 0x%02x - %s\n", 	o->status, fmbs(o->status));	
	printf("ppid:                        0x%02x\n", o->ppid);
	printf("ldid:                        0x%02x\n", o->ldid);
}

void fmapi_prnt_vsc_info_blk(void *ptr)
{
	struct fmapi_vsc_info_blk *o = (struct fmapi_vsc_info_blk*) ptr;
	printf("fmapi_vsc_info_blk:\n");
	printf("Virtual CXL Switch ID:       0x%02x\n", 		o->vcsid);
	printf("VCS State:                   0x%02x - %s\n", 	o->state, 	fmvs(o->state) );
	printf("Upstream port ID:            0x%02x\n", o->uspid);		
	printf("Total num vPPB in VCS:       0x%02x\n", o->num);
	printf("Num vPPB in this object:     0x%02x\n", o->num);
	for ( int i = 0 ; i < o->num ; i++ )
		fmapi_prnt_vsc_ppb_stat_blk(&o->list[i]);
}

void fmapi_prnt_vsc_info_rsp(void *ptr)
{
	struct fmapi_vsc_info_rsp *o = (struct fmapi_vsc_info_rsp*) ptr;
	printf("fmapi_vsc_info_rsp:\n");
	printf("Number of VCSs:              0x%02x\n", o->num);
	for ( int i = 0 ; i < o->num ; i++ )
		fmapi_prnt_vsc_info_blk(&o->list[i]);
}

void fmapi_prnt_vsc_bind_req(void *ptr)
{
	struct fmapi_vsc_bind_req *o = (struct fmapi_vsc_bind_req*) ptr;
	printf("fmapi_vsc_bind_req:\n");
	printf("VCS ID:                      0x%02x\n", o->vcsid);
	printf("vPPB ID:                     0x%02x\n", o->vppbid);
	printf("PPID:                        0x%02x\n", o->ppid);
	printf("LD ID:                       0x%04x\n", o->ldid);
}

void fmapi_prnt_vsc_unbind_req(void *ptr)
{
	struct fmapi_vsc_unbind_req *o = (struct fmapi_vsc_unbind_req*) ptr;
	printf("fmapi_vsc_unbind_req:\n");
	printf("VCS ID:                      0x%02x\n", o->vcsid);
	printf("vPPB ID:                     0x%02x\n", o->vppbid);
	printf("Unbind Option:               0x%02x\n", o->option);
}

void fmapi_prnt_vsc_aer_req(void *ptr)
{
	struct fmapi_vsc_aer_req *o = (struct fmapi_vsc_aer_req*) ptr;
	printf("fmapi_vsc_aer_req:\n");
	printf("VCS ID:                      0x%02x\n", o->vcsid);
	printf("vPPB ID:                     0x%02x\n", o->vppbid);
	printf("AER Error Type:              0x%02x\n", o->error_type);
	printf("AER Header:                  0x");
	for ( int i = 0 ; i < 32 ; i++ )
		printf("%02x ", o->header[i]);	
	printf("\n");
}

void fmapi_prnt_mpc_tmc_req(void *ptr)
{
	struct fmapi_mpc_tmc_req *o = (struct fmapi_mpc_tmc_req*) ptr;
	printf("fmapi_mpc_tmc_req:\n");
	printf("PPID:                        0x%02x\n", o->ppid);
	printf("Command Size:                0x%04x\n", o->len);
	printf("MCTP Type:                   0x%02x\n", o->type);
	printf("MCTP Command:                0x");
	for ( int i = 0 ; i < o->len ; i++ )
		printf("%02x ", o->msg[i]);	
	printf("\n");
}

void fmapi_prnt_mpc_tmc_rsp(void *ptr)
{
	struct fmapi_mpc_tmc_rsp *o = (struct fmapi_mpc_tmc_rsp*) ptr;
	printf("fmapi_mpc_tmc_rsp:\n");
	printf("Response Length:             0x%04x\n", o->len);
	printf("MCTP Type:                   0x%02x\n", o->type);
	printf("MCTP Response:               0x");
	for ( int i = 0 ; i < o->len ; i++ )
		printf("%02x ", o->msg[i]);	
	printf("\n");
}

void fmapi_prnt_mpc_cfg_req(void *ptr)
{
	struct fmapi_mpc_cfg_req *o = (struct fmapi_mpc_cfg_req*) ptr;
	printf("fmapi_mpc_cfg_req:\n");
	printf("PPID:                        0x%04x\n", o->ppid);
	printf("LDID:                        0x%04x\n", o->ldid);
	printf("Register Number:             0x%04x\n", o->reg);
	printf("Extended Register Num:       0x%04x\n", o->ext);
	printf("First Dword byte enable:     0x%04x\n", o->fdbe);
	printf("Transaction Type:            0x%04x\n", o->type);
	printf("Transaction Data:            0x");
	for ( int i = 0 ; i < 4 ; i++ )
		printf("%02x ", o->data[i]);	
	printf("\n");
}

void fmapi_prnt_mpc_cfg_rsp(void *ptr)
{
	struct fmapi_mpc_cfg_rsp *o = (struct fmapi_mpc_cfg_rsp*) ptr;
	printf("fmapi_mpc_cfg_rsp:\n");
	printf("Transaction Data:            0x");
	for ( int i = 0 ; i < 4 ; i++ )
		printf("%02x ", o->data[i]);	
	printf("\n");
}

void fmapi_prnt_mpc_mem_req(void *ptr)
{
	struct fmapi_mpc_mem_req *o = (struct fmapi_mpc_mem_req*) ptr;
	printf("fmapi_mpc_mem_req:\n");
	printf("PPID:                        0x%02x\n", o->ppid);
	printf("LDID:                        0x%04x\n", o->ldid);
	printf("First Dword Byte Enable:     0x%02x\n", o->fdbe);
	printf("Last Dword Byte Enable:      0x%02x\n", o->ldbe);
	printf("Transaction Type:            0x%02x\n", o->type);
	printf("Transaction Length:          0x%04x\n", o->len);
	printf("Transaction Offset:          0x%08llx\n", o->offset);
	printf("Transaction Data:            0x");
	for ( int i = 0 ; i < o->len ; i++ )
		printf("%02x ", o->data[i]);	
	printf("\n");
}

void fmapi_prnt_mpc_mem_rsp(void *ptr)
{
	struct fmapi_mpc_mem_rsp *o = (struct fmapi_mpc_mem_rsp*) ptr;
	printf("fmapi_mpc_mem_rsp:\n");
	printf("Return Size:                 0x%04x\n", o->len);
	printf("Transaction Data:            0x");
	for ( int i = 0 ; i < o->len ; i++ )
		printf("%02x ", o->data[i]);	
	printf("\n");
}

void fmapi_prnt_mcc_info_rsp(void *ptr)
{
	struct fmapi_mcc_info_rsp *o = (struct fmapi_mcc_info_rsp*) ptr;
	printf("fmapi_mcc_info_rsp:\n");
	printf("Memory Size:                 0x%016llx\n", o->size);
	printf("LD Count:                    0x%04x\n", o->num);
	printf("Egress Port Congestion En    %d\n", o->epc);
	printf("Temp Throughput Reduction En %d\n", o->ttr);
}

void fmapi_prnt_mcc_alloc_blk(void *ptr)
{
	struct fmapi_mcc_alloc_blk *o = (struct fmapi_mcc_alloc_blk*) ptr;
	printf("fmapi_mcc_alloc_blk:\n");
	printf("Range 1 Multiplier:          0x%016llx\n", o->rng1);
	printf("Range 2 Multiplier:          0x%016llx\n", o->rng2);
}

void fmapi_prnt_mcc_alloc_get_req(void *ptr)
{
	struct fmapi_mcc_alloc_get_req *o = (struct fmapi_mcc_alloc_get_req*) ptr;
	printf("fmapi_mcc_alloc_get_req:\n");
	printf("Start LD ID:                 0x%02x\n", o->start);
	printf("Max num limit:               0x%02x\n", o->limit);
}

void fmapi_prnt_mcc_alloc_get_rsp(void *ptr)
{
	struct fmapi_mcc_alloc_get_rsp *o = (struct fmapi_mcc_alloc_get_rsp*) ptr;
	printf("fmapi_mcc_alloc_get_rsp:\n");
	printf("Total num LDs Supported:     0x%02x\n", o->total);
	printf("Memory Granularity:          0x%02x\n", o->granularity);
	printf("Start LD ID:                 0x%02x\n", o->start);
	printf("Number of LDs:               0x%02x\n", o->num);
	for ( int i = 0 ; i < o->num ; i++ )
		fmapi_prnt_mcc_alloc_blk(&o->list[i]);
}

void fmapi_prnt_mcc_alloc_set_req(void *ptr)
{
	struct fmapi_mcc_alloc_set_req *o = (struct fmapi_mcc_alloc_set_req*) ptr;
	printf("fmapi_mcc_alloc_set_req:\n");
	printf("Number of LDs:               0x%02x\n", o->num);
	printf("Start LD ID:                 0x%02x\n", o->start);
	for ( int i = 0 ; i < o->num ; i++ )
		fmapi_prnt_mcc_alloc_blk(&o->list[i]);
}

void fmapi_prnt_mcc_alloc_set_rsp(void *ptr)
{
	struct fmapi_mcc_alloc_set_rsp *o = (struct fmapi_mcc_alloc_set_rsp*) ptr;
	printf("fmapi_mcc_alloc_set_rsp:\n");
	printf("Number of LDs:               0x%02x\n", o->num);
	printf("Start LD ID:                 0x%02x\n", o->start);
	for ( int i = 0 ; i < o->num ; i++ )
		fmapi_prnt_mcc_alloc_blk(&o->list[i]);
}

void fmapi_prnt_mcc_qos_ctrl(void *ptr)
{
	struct fmapi_mcc_qos_ctrl *o = (struct fmapi_mcc_qos_ctrl*) ptr;
	printf("fmapi_mcc_qos_ctrl:\n");
	printf("Egress Port Congestion En:   0x%02x\n", o->epc_en);
	printf("Temporary Throughput Reduce: 0x%02x\n", o->ttr_en);
	printf("Egress Moderagte Percent:    0x%02x\n", o->egress_mod_pcnt);
	printf("Egress Severe Percent:       0x%02x\n", o->egress_sev_pcnt);
	printf("Backpressure Sample Interval:0x%02x\n", o->sample_interval);
	printf("Request Completion Basis:    0x%04x\n", o->rcb);
	printf("Completion Correction Intvl: 0x%02x\n", o->comp_interval);
}

void fmapi_prnt_mcc_qos_stat_rsp(void *ptr)
{
	struct fmapi_mcc_qos_stat_rsp *o = (struct fmapi_mcc_qos_stat_rsp*) ptr;
	printf("fmapi_mcc_qos_stat_rsp:\n");
	printf("Backpressure Avg Percent:    0x%02x\n", o->bp_avg_pcnt);
}

void fmapi_prnt_mcc_qos_bw_alloc_get_req(void *ptr)
{
	struct fmapi_mcc_qos_bw_alloc_get_req *o = (struct fmapi_mcc_qos_bw_alloc_get_req*) ptr;
	printf("fmapi_mcc_qos_bw_alloc_get_req:\n");
	printf("Num LD:                      %d\n", o->num); 
	printf("Start LD ID:                 %d\n", o->start); 
}

void fmapi_prnt_mcc_qos_bw_alloc(void *ptr)
{
	struct fmapi_mcc_qos_bw_alloc *o = (struct fmapi_mcc_qos_bw_alloc*) ptr;
	printf("fmapi_mcc_qos_bw_alloc:\n");
	printf("Num LD:                      %d\n", o->num); 
	printf("Start LD ID:                 %d\n", o->start); 
	printf("QoS Allocation Fraction:     0x"); 
	for ( int i = 0 ; i < o->num ; i++) 
		printf("%02x ", o->list[i]);
	printf("\n");
}

void fmapi_prnt_mcc_qos_bw_limit_get_req(void *ptr)
{
	struct fmapi_mcc_qos_bw_limit_get_req *o = (struct fmapi_mcc_qos_bw_limit_get_req*) ptr;
	printf("fmapi_mcc_qos_bw_limit_get_req:\n");
	printf("Num LD:                      %d\n", o->num); 
	printf("Start LD ID:                 %d\n", o->start); 
}

void fmapi_prnt_mcc_qos_bw_limit(void *ptr)
{
	struct fmapi_mcc_qos_bw_limit *o = (struct fmapi_mcc_qos_bw_limit*) ptr;
	printf("fmapi_mcc_qos_bw_limit:\n");
	printf("Num LD:                      %d\n", o->num); 
	printf("Start LD ID:                 %d\n", o->start); 
	printf("QoS Limit Fraction:          0x"); 
	for ( int i = 0 ; i < o->num ; i++) 
		printf("%02x ", o->list[i]);
	printf("\n");
}

