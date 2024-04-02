/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file 		fmapi.h 
 *
 * @brief 		Header file for CXL Fabric Management API commands
 *
 * @details 	As per CXL specification, all registers are little-endian
 *
 * @copyright 	Copyright (C) 2024 Jackrabbit Founders LLC. All rights reserved.
 *
 * @date 		Jan 2024
 * @author 		Barrett Edwards <code@jrlabs.io>
 *
 * Macro / Enumeration Prefixes 
 * FMCT - CXL.io Configuration Request Type
 * FMDT	- CXL Device Type
 * FMDV	- CXL version for the connected device
 * FMEL	- Event Logs
 * FMET - Physical Switch Event Record - Event Type (ET)
 * FMLF - Link Flags - Bitmask Flags for Link State for CXL Swithc Port info struct
 * FMLN - Serialized Length of each FM API Object (struct) (LN)
 * FMLO	- PCIe Lane reverse ordering state as defined in CXL 2.0 table 92
 * FMLS	- PCIe Link state
 * FMMG - Memory Granularity for Get LD Allocation
 * FMMS - Encoding for PCIe Max Link Speeds
 * FMMR - MLD Port Event Record - Event Type (MR)
 * FMMT - FM API Header Message Catgories
 * FMNW - Negotiated Link Width (NW)
 * FMOB - Types of FM API Objects (OB)
 * FMOP - FM API Header Opcodes 
 * FMPO	- Port Control Op Code
 * FMPS - CXL Port State possibilities
 * FMQT	- QoS Telemetry Capability Bitmask
 * FMRC	- FM API Return Codes
 * FMSS - Bitmask fields for PCIe Supported Link Speeds
 * FMUB - Unbind options for vPPB Unbind Command
 * FMVS - VCS State
 * FMVT - Virtual CXL Switch Event Record - Event Type (VT)
 * 
 */
#ifndef _FMAPI_H
#define _FMAPI_H

/* INCLUDES ==================================================================*/

/**
 * For PCIe Supported link speed macros 
 */
#include <linux/pci_regs.h>

/**
 * For __u8, __u16, __u32, __u64 types
 */
#include <linux/types.h>

/* MACROS ====================================================================*/

/**
 * Version of this API
 */
#define FMAPI_VERSION 1

/**
 * The CXL 2.0 FM API has an 8-bit field for port id so there is a maximum of 256 ports
 */
#define FM_MAX_PORTS 256 			

/**
 * The CXL 2.0 FM API has an 8-bit field for VCS id so there is a maximum of 256 ports
 */
#define FM_MAX_VCS 256

/**
 *The CXL 2.0 FM API has an 8-bit field for port id so there is a maximum of 256 ports
 */
#define FM_MAX_VPPBS FM_MAX_PORTS

/**
 * The maximum number of VCS Info blocks returned in a Get VCS Info Response
 */
#define FM_MAX_VCS_PER_RSP 7

/**
 * Maximum Message Body Size in Bytes
 */
#define FM_MAX_MSG_LEN 65536

/**
 * Send LD CXL.io Memory Request Data payload length 
 * CXL 2.0 v1.0 Table 108 
 */
#define FM_LD_MEM_REQ_LEN 4096
	
/**
 * Maximum number of Logical Devices supported by an MLD
 */
#define FM_MAX_NUM_LD 16

/** 
 * Length of a PCIe TLP Header in bytes
 */
#define FM_TLP_HEADER 32 

/**
 * Serialized Length in bytes of each FM API Object (struct) (LN)
 *
 * This length excludes any variable length data that comes after the object
 */
#define FMLN_HDR						12  	//!< struct fmapi_hdr
#define FMLN_MSG 						8192 	//!< Maximum length of a FM API Message Body (HDR + payload)
#define FMLN_PAYLOAD 					(FMLN_MSG - FMLN_HDR)  //!< Maximum length of the FM API Message Payload 

#define FMLN_PSC_IDENTIFY_SWITCH		93  	//!< struct fmapi_psc_id_rsp
#define FMLN_PSC_GET_PHY_PORT_REQ 		1   	//!< struct fmapi_psc_port_req
#define FMLN_PSC_GET_PHY_PORT_INFO		16  	//!< struct fmapi_psc_port_info
#define FMLN_PSC_GET_PHY_PORT_RESP		4		//!< struct fmapi_psc_port_rsp
#define FMLN_PSC_PHY_PORT_CTRL    		2   	//!< struct fmapi_psc_port_ctrl_req
#define FMLN_PSC_PPB_IO_CFG_REQ			8   	//!< struct fmapi_psc_cfg_req
#define FMLN_PSC_PPB_IO_CFG_RESP		4   	//!< struct fmapi_psc_cfg_rsp 

#define FMLN_VSC_GET_INFO_REQ			3 		//!< struct fmapi_vsc_info_req
#define FMLN_VSC_PPB_STATUS				4   	//!< struct fmapi_vsc_ppb_stat_blk
#define FMLN_VSC_INFO 					4 		//!< struct fmapi_vsc_info_blk
#define FMLN_VSC_GET_INFO_RESP 			4		//!< struct fmapi_vsc_info_rsp
#define FMLN_VSC_BIND					6   	//!< struct fmapi_vsc_bind_req
#define FMLN_VSC_UNBIND 				3   	//!< struct fmapi_vsc_unbind_req
#define FMLN_VSC_GEN_AER 				40  	//!< struct fmapi_vsc_aer_req

#define FMLN_MPC_TUNNEL_CMD_REQ			5   	//!< struct fmapi_mpc_tmc_req includes MCTP Type Byte
#define FMLN_MPC_TUNNEL_CMD_RESP		5   	//!< struct fmapi_mpc_tmc_rsp includes MCTP Type Byte
#define FMLN_MPC_TUNNEL_PAYLOAD 		(FMLN_PAYLOAD - FMLN_MPC_TUNNEL_CMD_REQ)
#define FMLN_MPC_LD_IO_CFG_REQ			12  	//!< struct fmapi_mpc_cfg_req
#define FMLN_MPC_LD_IO_CFG_RESP			4   	//!< struct fmapi_mpc_cfg_rsp
#define FMLN_MPC_LD_MEM_REQ				16  	//!< struct fmapi_mpc_mem_req
#define FMLN_MPC_LD_MEM_RESP			4   	//!< struct fmapi_mpc_mem_rsp

#define FMLN_MCC_GET_LD_INFO			11  	//!< struct fmapi_mcc_info_rsp
#define FMLN_MCC_LD_ALLOC_ENTRY			16  	//!< struct fmapi_mcc_alloc_blk
#define FMLN_MCC_GET_LD_ALLOC_REQ		2   	//!< struct fmapi_mcc_alloc_get_req
#define FMLN_MCC_GET_LD_ALLOC_RSP		4   	//!< struct fmapi_mcc_alloc_get_rsp
#define FMLN_MCC_SET_LD_ALLOC_REQ		4   	//!< struct fmapi_mcc_alloc_set_req
#define FMLN_MCC_SET_LD_ALLOC_RSP		4   	//!< struct fmapi_mcc_alloc_set_rsp
#define FMLN_MCC_QOS_CTRL				7   	//!< struct fmapi_mcc_qos_ctrl
#define FMLN_MCC_QOS_STATUS				1   	//!< struct fmapi_mcc_qos_stat_rsp
#define FMLN_MCC_GET_QOS_BW_REQ   		2 		//!< struct fmapi_mcc_qos_bw_alloc_get_req 
#define FMLN_MCC_QOS_BW_ALLOC       	2 		//!< struct fmapi_mcc_qos_bw_alloc 
#define FMLN_MCC_GET_QOS_BW_LIMIT_REQ 	2 		//!< struct fmapi_mcc_qos_bw_limit_get_req 
#define FMLN_MCC_QOS_BW_LIMIT       	2 		//!< struct fmapi_mcc_qos_bw_limit

#define FMLN_ISC_ID_RSP 				17 		//!< struct fmapi_isc_id
#define FMLN_ISC_MSG_LIMIT 				1 		//!< struct fmapi_isc_msg_limit
#define FMLN_ISC_BOS 					8 		//!< struct fmapi_isc_bos

/* ENUMERATIONS ==============================================================*/

/**
 * Types of FM API Objects (OB)
 *
 * The primary purpose of this enum is for serialization/deserialization.
 * This is not an enumeration defined by the CXL FM API
 */
enum _FMOB {
	FMOB_NULL						=  0,
	FMOB_HDR						=  1, //!< struct fmapi_hdr
	FMOB_PSC_ID_RSP					=  2, //!< struct fmapi_psc_id_rsp
	FMOB_PSC_PORT_REQ 				=  3, //!< struct fmapi_psc_port_req
	FMOB_PSC_PORT_INFO				=  4, //!< struct fmapi_psc_port_info
	FMOB_PSC_PORT_RSP				=  5, //!< struct fmapi_psc_port_rsp
	FMOB_PSC_PORT_CTRL_REQ    		=  6, //!< struct fmapi_psc_port_ctrl_req
	FMOB_PSC_CFG_REQ				=  7, //!< struct fmapi_psc_cfg_req
	FMOB_PSC_CFG_RSP				=  8, //!< struct fmapi_psc_cfg_rsp 
	FMOB_VSC_INFO_REQ				=  9, //!< struct fmapi_vsc_info_req
	FMOB_VSC_PPB_STAT_BLK			= 10, //!< struct fmapi_vsc_ppb_stat_blk
	FMOB_VSC_INFO_BLK 				= 11, //!< struct fmapi_vsc_info_blk
	FMOB_VSC_INFO_RSP 				= 12, //!< struct fmapi_vsc_info_rsp
	FMOB_VSC_BIND_REQ				= 13, //!< struct fmapi_vsc_bind_req
	FMOB_VSC_UNBIND_REQ 			= 14, //!< struct fmapi_vsc_unbind_req
	FMOB_VSC_AER_REQ 				= 15, //!< struct fmapi_vsc_aer_req
	FMOB_MPC_TMC_REQ				= 16, //!< struct fmapi_mpc_tmc_req
	FMOB_MPC_TMC_RSP				= 17, //!< struct fmapi_mpc_tmc_rsp
	FMOB_MPC_CFG_REQ				= 18, //!< struct fmapi_mpc_cfg_req
	FMOB_MPC_CFG_RSP				= 19, //!< struct fmapi_mpc_cfg_rsp
	FMOB_MPC_MEM_REQ				= 20, //!< struct fmapi_mpc_mem_req
	FMOB_MPC_MEM_RSP				= 21, //!< struct fmapi_mpc_mem_rsp
	FMOB_MCC_INFO_RSP				= 22, //!< struct fmapi_mcc_info_rsp
	FMOB_MCC_ALLOC_BLK				= 23, //!< struct fmapi_mcc_alloc_blk
	FMOB_MCC_ALLOC_GET_REQ			= 24, //!< struct fmapi_mcc_alloc_get_req
	FMOB_MCC_ALLOC_GET_RSP			= 25, //!< struct fmapi_mcc_alloc_get_rsp
	FMOB_MCC_ALLOC_SET_REQ			= 26, //!< struct fmapi_mcc_alloc_set_req
	FMOB_MCC_ALLOC_SET_RSP			= 27, //!< struct fmapi_mcc_alloc_set_rsp
	FMOB_MCC_QOS_CTRL				= 28, //!< struct fmapi_mcc_qos_ctrl
	FMOB_MCC_QOS_STAT_RSP			= 29, //!< struct fmapi_mcc_qos_stat_rsp
	FMOB_MCC_QOS_BW_GET_REQ 		= 30, //!< struct fmapi_mcc_qos_bw_alloc_get_req 
	FMOB_MCC_QOS_BW_ALLOC			= 31, //!< struct fmapi_mcc_qos_bw_alloc
	FMOB_MCC_QOS_BW_LIMIT_GET_REQ 	= 32, //!< struct fmapi_mcc_qos_bw_limit_get_req 
	FMOB_MCC_QOS_BW_LIMIT			= 33, //!< struct fmapi_mcc_qos_bw_limit
	FMOB_ISC_ID_RSP 				= 34, //!< struct fmapi_isc_id_rsp
	FMOB_ISC_MSG_LIMIT 				= 35, //!< struct fmapi_isc_msg_limit
	FMOB_ISC_BOS        			= 36, //!< struct fmapi_isc_bos
	FMOB_MAX                    
};

/**
 * FM API Command Message Category Types (MT)
 *
 * CXL 2.0 v1.0 Table 84
 */
enum _FMMT {
	FMMT_REQ 		= 0,
	FMMT_RESP 		= 1,
	FMMT_MAX
};

/**
 * FM API Command Opcodes (OP)
 *
 * CXL 2.0 v1.0 Table 205
 */
enum _FMOP {
	FMOP_PSC_ID							= 0x5100,
	FMOP_PSC_PORT						= 0x5101,
	FMOP_PSC_PORT_CTRL					= 0x5102,
	FMOP_PSC_CFG						= 0x5103,
	FMOP_VSC_INFO						= 0x5200,
	FMOP_VSC_BIND						= 0x5201,
	FMOP_VSC_UNBIND						= 0x5202,
	FMOP_VSC_AER						= 0x5203,
	FMOP_MPC_TMC						= 0x5300,
	FMOP_MPC_CFG						= 0x5301,
	FMOP_MPC_MEM						= 0x5302,
	FMOP_MCC_INFO						= 0x5400,
	FMOP_MCC_ALLOC_GET					= 0x5401,
	FMOP_MCC_ALLOC_SET					= 0x5402,
	FMOP_MCC_QOS_CTRL_GET				= 0x5403,
	FMOP_MCC_QOS_CTRL_SET				= 0x5404,
	FMOP_MCC_QOS_STAT					= 0x5405,
	FMOP_MCC_QOS_BW_ALLOC_GET			= 0x5406,
	FMOP_MCC_QOS_BW_ALLOC_SET			= 0x5407,
	FMOP_MCC_QOS_BW_LIMIT_GET			= 0x5408,
	FMOP_MCC_QOS_BW_LIMIT_SET			= 0x5409,
	FMOP_ISC_ID 						= 0x0001,
	FMOP_ISC_BOS 						= 0x0002,
	FMOP_ISC_MSG_LIMIT_GET 				= 0x0003,
	FMOP_ISC_MSG_LIMIT_SET 				= 0x0004,
	FMOP_MAX
};

/**
 * FM API Command Return Codes (RC)
 *
 * CXL 2.0 v1.0 Table 150
 */
enum _FMRC {
	FMRC_SUCCESS 						= 0x0000,
	FMRC_BACKGROUND_OP_STARTED			= 0x0001,
	FMRC_INVALID_INPUT					= 0x0002, 
	FMRC_UNSUPPORTED 					= 0x0003,
	FMRC_INTERNAL_ERROR					= 0x0004,
	FMRC_RETRY_REQUIRED 				= 0x0005,
	FMRC_BUSY							= 0x0006,
	FMRC_MEDIA_DISABLED					= 0x0007,
	FMRC_FW_TXFR_IN_PROGRESS			= 0x0008,
	FMRC_FW_TXFR_OUT_OF_ORDER			= 0x0009,
	FMRC_FW_AUTH_FAILED					= 0x000A,
	FMRC_FW_INVALID_SLOT				= 0x000B,
	FMRC_FW_ACTV_FAILED_ROLLED_BACK		= 0x000C,
	FMRC_FW_ACTV_FAILED_RESET_REQ		= 0x000D,
	FMRC_INVALID_HANDLE 				= 0x000E,
	FMRC_INVALID_PHY_ADDR				= 0x000F,
	FMRC_POISON_LIMIT_REACHED			= 0x0010,
	FMRC_MEDIA_FAILURE 					= 0x0011, 
	FMRC_ABORTED 						= 0x0012, 
	FMRC_INVALID_SECURITY_STATE			= 0x0013, 
	FMRC_INCORRECT_PASSPHRASE 			= 0x0014, 
	FMRC_UNSUPPORTED_MAILBOX			= 0x0015,
	FMRC_INVALID_PAYLOAD_LEN			= 0x0016,
	FMRC_MAX
};

/**
 * Event Log (EL)
 *
 * CXL 2.0 v1.0 Table 87
 */
enum _FMEL {
	FMEL_INFO	= 0x00,
	FMEL_WARN	= 0x01,
	FMEL_FAIL	= 0x02,
	FMEL_FATAL 	= 0x03,
	FMEL_MAX
};

/**
 * Current Port Configuration State (PS)
 *
 * CXL 2.0 v1.0 Table 92
 */
enum _FMPS {
	FMPS_DISABLED	= 0,
	FMPS_BINDING	= 1,
	FMPS_UNBINDING	= 2, 
	FMPS_DSP		= 3,
	FMPS_USP		= 4,
	FMPS_FABRIC		= 5, 
	FMPS_MAX 		= 6,
	FMPS_INVALID	= 0x0F
};

/**
 * Connected Device CXL version (DV)
 *
 * CXL 2.0 v1.0 Table 92
 */
enum _FMDV {
	FMDV_NOT_CXL	= 0,
	FMDV_CXL1_1		= 1,
	FMDV_CXL2_0		= 2,
	FMDV_MAX
};

/**
 * Connected Device Type (DT)
 *
 * CXL 2.0 v1.0 Table 92
 */
enum _FMDT {
	FMDT_NONE				= 0,
	FMDT_PCIE				= 1,
	FMDT_CXL_TYPE_1			= 2,
	FMDT_CXL_TYPE_2			= 3,
	FMDT_CXL_TYPE_3			= 4,
	FMDT_CXL_TYPE_3_POOLED	= 5,
	FMDT_CXL_SWITCH			= 6,
	FMDT_MAX
};

/*
 * Connected CXL Version (VC) 
 *
 * CXL 2.0 v1.0 Table 92
 * Bitmask fields for CXL version in fm_psc_port_info struct
 */
enum _FMVC {
	FMCV_CXL1_1 	= 0x01,
	FMCV_CXL2_0 	= 0x02,
	FMCV_MAX
};

/**
 * Negotiated Link Width (NW)
 *
 * CXL 2.0 v1.0 Table 92
 * Also defined in linux/pci_regs.h
 * - PCI_EXP_LNKSTA_NLW_X1
 */
enum _FMNW {
	FMNW_X1			= PCI_EXP_LNKSTA_NLW_X1, // 0x0010
	FMNW_X2			= PCI_EXP_LNKSTA_NLW_X2, // 0x0020
	FMNW_X4 		= PCI_EXP_LNKSTA_NLW_X4, // 0x0040 
	FMNW_X8			= PCI_EXP_LNKSTA_NLW_X8, // 0x0080
	FMNW_MAX
};

/**
 * PCIe Supported Link Speeds (SS) - Bitmask fields
 *
 * CXL 2.0 v1.0 Table 92 
 * Also defined in linux/pci_regs.h - PCI_EXP_LNKCAP2_SLS_XXXX
 */
enum _FMSS {
	FMSS_PCIE1 		= PCI_EXP_LNKCAP2_SLS_2_5GB, 	// 0x02
	FMSS_PCIE2 		= PCI_EXP_LNKCAP2_SLS_5_0GB, 	// 0x04
	FMSS_PCIE3 		= PCI_EXP_LNKCAP2_SLS_8_0GB, 	// 0x08
	FMSS_PCIE4 		= PCI_EXP_LNKCAP2_SLS_16_0GB,	// 0x10
	FMSS_PCIE5 		= PCI_EXP_LNKCAP2_SLS_32_0GB,	// 0x20
	FMSS_PCIE6		= PCI_EXP_LNKCAP2_SLS_64_0GB	// 0x40
};

/**
 * PCIe Max Link Speed (MS)
 *
 * CXL 2.0 v1.0 Table 92
 * Also matches fields for Current Link Speed 
 * Also defined in linux/pci_regs.h 
 * - PCI_EXP_LNKCAP_SLS_2_5GB - Link Capacity 
 * - PCI_EXP_LNKSTA_CLS_2_5GB - Current Link Speed
 */
enum _FMMS {
	FMMS_PCIE1	= PCI_EXP_LNKCAP_SLS_2_5GB, 	// 1
	FMMS_PCIE2	= PCI_EXP_LNKCAP_SLS_5_0GB, 	// 2
	FMMS_PCIE3	= PCI_EXP_LNKCAP_SLS_8_0GB, 	// 3
	FMMS_PCIE4	= PCI_EXP_LNKCAP_SLS_16_0GB, 	// 4
	FMMS_PCIE5	= PCI_EXP_LNKCAP_SLS_32_0GB, 	// 5
	FMMS_PCIE6	= PCI_EXP_LNKCAP_SLS_64_0GB, 	// 6
	FMMS_MAX
};

/**
 * LTSSM State - PCIe Link State (LS)
 *
 * CXL 2.0 v1.0 Table 92
 */
enum _FMLS {
	FMLS_DETECT 		= 0,
	FMLS_POOLING		= 1,
	FMLS_CONFIGURATION	= 2,
	FMLS_RECOVERY		= 3,
	FMLS_L0				= 4,
	FMLS_L0S			= 5, 
	FMLS_L1				= 6, 
	FMLS_L2				= 7,
	FMLS_DISABLED 		= 8,
	FMLS_LOOPBACK		= 9, 
	FMLS_HOT_RESET		= 10,
	FMLS_MAX
};

/**
 * Link State Flags (LF) - Bitmask Shift 
 *
 * CXL 2.0 v1.0 Table 92
 */
enum _FMLF {
	FMLF_LANE_REVERSAL_BIT 		= 0,
	FMLF_PERST_STATE_BIT 		= 1,
	FMLF_PRSNT_STATE_BIT 		= 2,
	FMLF_PWRCTL_STATE_BIT 		= 3
};

/**
 * PCIe Lane Reverse Ordering State (LO)
 *
 * CXL 2.0 v1.0 Table 92
 */
enum _FMLO {
	FMLO_LANE_STANDARD_ORDERING = 0,
	FMLO_LANE_REVERSE_ORDERING 	= 1,
	FMLO_MAX
};

/**
 * Port Opcode - Actions for Physical Port Control (PO)
 *
 * CXL 2.0 v1.0 Table 93
 */
enum _FMPO {
	FMPO_ASSERT_PERST	= 0x00,
	FMPO_DEASSERT_PERST	= 0x01,
	FMPO_RESET_PPB		= 0x02,
	FMPO_MAX	
};

/**
 * VCS State (VS)
 *
 * CXL 2.0 v1.0 Table 99
 */
enum _FMVS {
	FMVS_DISABLED 	= 0,
	FMVS_ENABLED	= 1,
	FMVS_INVALID	= 0xFF
};
#define FMVS_NUM 3

/**
 * PBB Binding Status (BS)
 *
 * CXL 2.0 v1.0 Table 99
 */
enum _FMBS {
	FMBS_UNBOUND 	= 0,
	FMBS_INPROGRESS	= 1, 
	FMBS_BOUND_PORT	= 2, 
	FMBS_BOUND_LD	= 3,
	FMBS_MAX
};

/**
 * Unbind option - For Unbind vPPB Command (UB)
 *
 * CXL 2.0 v1.0 Table 101
 */
enum _FMUB {
	FMUB_WAIT					= 0, 
	FMUB_MANAGED_HOT_REMOVE		= 1,
	FMUB_SURPRISE_HOT_REMOVE 	= 2,
	FMUB_MAX
};

/**
 * Memory Granularity - For Get LD Allocation Command (MG)
 *
 * CXL 2.0 v1.0 Table 112
 */
enum _FMMG {
	FMMG_256MB 	= 0,
	FMMG_512MB	= 1, 
	FMMG_1GB	= 2,
	FMMG_MAX
};

/**
 * QoS Telemetry Control (QT) - Bitmask
 *
 * CXL 2.0 v1.0 Table 116
 */
enum _FMQT {
	FMQT_EGRESS_PORT_CONGESTION_BIT 	= 0x00,
	FMQT_TEMP_THROUGHPUT_REDUCTION_BIT 	= 0x01,
	FMQT_MAX
};

/**
 * Physical Switch Event Record - Event Type (ET)
 *
 * CXL 2.0 v1.0 Table 120 
 */
enum _FMET {
	FMET_LINK_STATUS_CHANGE				= 0,
	FMET_SLOT_STATUS_REGISTER_UPDATED	= 1,
	FMET_MAX
};

/**
 * Virtual CXL Switch Event Record - Event Type (VT)
 *
 * CXL 2.0 v1.0 Table 121 
 */
enum _FMVT {
	FMVT_BINDING_CHANGE					= 0,
	FMVT_SECONDARY_BUS_RESET			= 1,
	FMVT_LINK_CONTROL_REGISTER_UPDATED	= 2, 
	FMVT_SLOT_CONTROL_REGISTER_UPDATED	= 3,
	FMVT_MAX
};

/**
 * MLD Port Event Record - Event Type (MR)
 *
 * CXL 2.0 v1.0 Table 121 
 */
enum _FMMR {
	FMMR_CORRECTABLE_MSG_RECEIVED 	= 0,
	FMMR_NONFATAL_MSG_RECEIVED		= 1,
	FMMR_FATAL_MSG_RECEIVED 		= 2,
	FMMR_MAX
};

/**
 * CXL.io Configuration Request Type (CT)
 *
 * CXL 2.0 v1.0 Table 94
 */
enum _FMCT {
	FMCT_READ 	= 0,
	FMCT_WRITE 	= 1,
	FMCT_MAX
};

/* STRUCTS ===================================================================*/

/** 
 * FM API Protocol Header
 *
 * CXL 2.0 v1 Table 84
 */
struct fmapi_hdr 
{
	__u8 category		: 4;	//!< Type of FM API message [FMMT]
	__u8 tag;					//!< Tag used to track response messages 
	__u16 opcode;    			//!< enum _FMOP
	__u8 background 	: 1;	//!< Run operation in the background
	__u32 len 			: 21;	//!< Payload length in bytes
	__u16 return_code;			//!< enum _FMRC
	__u16 ext_status;			//!< Vendor Specific Extended Status  
};

/**
 * Identify (Opcode 0001h)
 *
 * Retrieves status information about the component, including whether it is ready to process
 * commands. A component that is not ready to process commands shall return ‘Retry Required’.
 * 
 * CXL 2.0 Errata Table X
 */
struct fmapi_isc_id_rsp
{
	__u16 vid;                  //!< PCIe Vendor ID 
	__u16 did;                  //!< PCIe Device ID 
	__u16 svid;                 //!< PCIe Subsystem Vendor ID 
	__u16 ssid;                 //!< PCIe Subsystem ID 
	__u64 sn;                   //!< Device Serial Number
	__u8 size;					//!< Max msg size n where size = 2^n. Min 2^8=256B, Max 2^20=1MB 
};

/**
 * Get Response Message Limit - Response (Opcode 0003h)
 *
 * Retrieves the current configured response message limit used by the component.
 * 
 * CXL 2.0 Errata Table Z
 */
struct fmapi_isc_msg_limit
{
	__u8 limit;                  //!< Response Message Limit. n of 2^n. 
};

/**
 * Background Operation Status - Response (Opcode 0002h)
 *
 * Retrieves the current configured response message limit used by the component.
 * 
 * CXL 2.0 Errata Table Y
 */
struct fmapi_isc_bos
{
	__u8 running;		//!< Indicates if a Background Operation is running. 0=not, 1=running
	__u8 pcnt;			//!< Percent Complete [0-100]
	__u16 opcode;		//!< Command Opcode of last command executed in the background
	__u16 rc;			//!< Return Code of last operation run in the back ground. Valid when pcnt=100
	__u16 ext;			//!< Vendor Specific Extedned Status. Valid when pcnt=100
};

/**
 * Identify Switch Device (Opcode 5100h) 
 * 
 * CXL 2.0 v1.0 Table 89
 *
 * Physical Switch Command Set (PSC)
 */
struct fmapi_psc_id_rsp 
{
	__u8 ingress_port;					//!< Ingress Port ID 
	__u8 num_ports;						//!< Total number of physical ports
	__u8 num_vcss; 						//!< Max number of VCSs
	__u8 active_ports[FM_MAX_PORTS/8];	//!< Active physical port bitmask: enabled (1), disabled (0)
	__u8 active_vcss[FM_MAX_VCS/8]; 	//!< Active VCS bitmask: enabled (1), disabled (0)
	__u16 num_vppbs;					//!< Max number of vPPBs 
	__u16 active_vppbs;					//!< Number of active vPPBs 
	__u8 num_decoders;					//!< Number of HDM decoders available per USP 
}; 

/**
 * Get Physical Port State - Request (Opcode 5101h)
 *
 * CXL 2.0 v1.0 Table 90
 *
 * Physical Switch Command Set (PSC)
 */
struct fmapi_psc_port_req 
{
	__u8 num; 					//!< Number of ports requested
	__u8 ports[FM_MAX_PORTS];	//!< Variable length list __u8 port IDs
};

/** 
 * Get Physical Port State Port Info Block
 *
 * CXL 2.0 v1.0 Table 92
 *
 * Physical Switch Command Set (PSC)
 */
struct fmapi_psc_port_info 
{
	__u8 ppid;		//!< Physical Port ID
	__u8 state;		//!< Current Port Configuration State [FMPS]
	__u8 dv;		//!< Connected Device CXL Version [FMDV]
	__u8 dt;		//!< Connected Device Type [FMDT]
	__u8 cv;		//!< Connected device CXL Version [FMCV]
	__u8 mlw;		//!< Max link width
	__u8 nlw;		//!< Negotiated link width [FMNW]
	__u8 speeds;	//!< Supported Link speeds vector [FMSS]
	__u8 mls;		//!< Max Link Speed [FMMS]
	__u8 cls;		//!< Current Link Speed [FMMS] 
	__u8 ltssm;		//!< LTSSM State [FMLS]
	__u8 lane;		//!< First negotiated lane number

	/* Link State Flags [FMLF] and [FMLO] */
	__u8 lane_rev; 	//!< Lane reversal state. 0=standard, 1=rev [FMLO]
	__u8 perst;		//!< PCIe Reset State PERST#	
	__u8 prsnt;		//!< Port Presence pin state PRSNT#
	__u8 pwrctrl;	//!< Power Control State (PWR_CTRL)

	__u8 num_ld;	//!< Supported Logical Device (LDs) count 
};

/**
 * Get Physical Port State - Response (Opcode 5101h)
 *
 * CXL 2.0 v1.0 Table 91
 *
 * Physical Switch Command Set (PSC)
 */
struct fmapi_psc_port_rsp 
{
	__u8 num;		 								//!< num ports returned in this command
	struct fmapi_psc_port_info list[FM_MAX_PORTS];	//!< Variable list of fm_psc_port_info structs
};

/**
 * Physical Port Control (Opcode 5102h) 
 *
 * CXL 2.0 v1.0 Table 93
 *
 * Physical Switch Command Set (PSC)
 */
struct fmapi_psc_port_ctrl_req 
{
	__u8 ppid;		//!< Physical PPD ID which is the physical port number 
	__u8 opcode;	//!< Port Opcode [FMPO]
};

/*
 * Send PPB CXL.io Configuration - Request (Opcode 5103h)
 *
 * CXL 2.0 v1.0 Table 94
 *
 * Physical Switch Command Set (PSC)
 */
struct fmapi_psc_cfg_req 
{
	__u8 ppid;	 			//!< PPB ID: Target PPB physical port
	__u32 reg 		: 8 ; 	//!< Register Number as defined in PCIe spec 
	__u32 ext 		: 4 ;	//!< Extended Register Number as defined in PCIe spec 
	__u32 fdbe 		: 4 ; 	//!< First DWord Byte Enable as defined in PCIe spec 
	__u32 type 		: 1 ;	//!< Transation type [FMCT]
	__u8 data[4];			//!< Transaction Data: Write data. Only valid for write transactions
};

/**
 * Send PPB CXL.io Configuration - Response (Opcode 5103h)
 *
 * CXL 2.0 v1.0 Table 95
 *
 * Physical Switch Command Set (PSC)
 */
struct fmapi_psc_cfg_rsp 
{
	__u8 data[4];			//!< Return Data: Read data. Only valid for read transactions
};

/**
 * Get Virtual CXL Switch Info - Request (Opcode 5200h) 
 *
 * CXL 2.0 v1.0 Table 97
 *
 * Virtual Switch Command Set (VSC) 
 */
struct fmapi_vsc_info_req 
{
	__u8 vppbid_start;		//!< Starting vPPB IDa
	__u8 vppbid_limit;		//!< Max num of vPPB entries to include in the response 
	__u8 num;				//!< Number of VCSs requested 
	__u8 vcss[FM_MAX_VCS];	//!< Variable list of VCS IDs: 1 byte ID of requested VCS, repeated num_vcs times
};

/**
 * Get Virtual CXL Switch PPB Status Block
 *
 * CXL 2.0 v1.0 Table 99
 *
 * Virtual Switch Command Set (VSC) 
 */
struct fmapi_vsc_ppb_stat_blk 
{
	__u8 status;		//!< PBB Binding Status. [FMBS]
	__u8 ppid;				//!< PBB Physical Port ID
	__u8 ldid;				//!< PBB Bound LD ID. 0xFF if not bound to an LD
};

/**
 * Get Virtual CXL Switch Info Block
 *
 * CXL 2.0 v1.0 Table 99
 *
 * Virtual Switch Command Set (VSC) 
 */
struct fmapi_vsc_info_blk 
{
	__u8 vcsid;		//!< Virtual CXL Switch ID
	__u8 state;		//!< VCS State [FMVS]
	__u8 uspid;		//!< USP ID. Upstream physical port ID
	__u8 total;		//!< Total Number of vPPBs in this VCS
	__u8 num;		//!< Number of vPPBs in this object 
	
	//!< Variable array of PPB Status Blocks: 
	struct fmapi_vsc_ppb_stat_blk list[FM_MAX_VPPBS];
};

/**
 * Get Virtual CXL Switch Info - Response (Opcode 5200h) 
 *
 * CXL 2.0 v1.0 Table 98
 *
 * Virtual Switch Command Set (VSC) 
 */
struct fmapi_vsc_info_rsp 
{
	__u8 num;				//!< Number of VCSs returned

	//!< Variable Array of vcs info blocks
	struct fmapi_vsc_info_blk list[FM_MAX_VCS_PER_RSP];
};

/**
 * Bind vPPB (Opcode 5201h) 
 *
 * CXL 2.0 v1.0 Table 100
 *
 * Virtual Switch Command Set (VSC) 
 */
struct fmapi_vsc_bind_req 
{
	__u8 vcsid;		//!< Virtual CXL Switch ID 
	__u8 vppbid;	//!< vPPB ID: Index of the vPPB within the VCS specified in VCS_ID
	__u8 ppid;		//!< Physical Port ID 
	__u16 ldid;		//!< LD ID if target port is an MLD. 0xFFFF otherwise 
}; 

/**
 * Unbind vPPB (Opcode 5202h)
 *
 * CXL 2.0 v1.0 Table 101
 *
 * Virtual Switch Command Set (VSC) 
 */
struct fmapi_vsc_unbind_req 
{
	__u8 vcsid;				//!< Virtual CXL Switch ID 
	__u8 vppbid;			//!< vPPB ID: Index of the vPPB within the VCS specified in VCS_ID
	__u8 option 	: 4;	//!< Unbind Option [FMUB]
}; 

/**
 * Generate AER Event (Opcode 5203h)
 *
 * CXL 2.0 v1.0 Table 102
 *
 * Virtual Switch Command Set (VSC) 
 */
struct fmapi_vsc_aer_req 
{
	__u8 vcsid;					//!< Virtual CXL Switch ID
	__u8 vppbid;				//!< vPPB ID: Index of the vPPB within the VCS specified in VCS_ID
	__u32 error_type;			//!< AER error type, as defined in the PCIe Specification
	__u8 header[FM_TLP_HEADER];	//!< TLP Header to place in AER registers, as defined in the PCIe specification
};

/**
 * Tunnel Management Command - Request (Opcode 5300h)
 *
 * CXL 2.0 v1.0 Table 104
 *
 * MLD Port Command Set (MPC)
 */
struct fmapi_mpc_tmc_req 
{
	__u8 ppid;					//!< Egress port ID
	__u16 len;					//!< Command Size: Number of valid bytes in Management Command
	__u8 type; 					//!< MCTP Message Type 
	__u8 msg[FMLN_MPC_TUNNEL_PAYLOAD];	//!< Management Command: Raw Message Body after MCPT Type Byte
};

/**
 * Tunnel Management Command - Response (Opcode 5300h)
 *
 * CXL 2.0 v1.0 Table 105
 *
 * MLD Port Command Set (MPC)
 */
struct fmapi_mpc_tmc_rsp 
{
	__u8 type; 					//!< MCTP Message Type 
	__u16 len;					//!< Response Length: Number of valid bytes in Response Message.
	__u8 msg[FMLN_MPC_TUNNEL_PAYLOAD];	//!< Response Message: Sent by MLD. Variable in Length follows this struct
};

/**
 * Send LD CXL.io Configuration - Request (Opcode 5301h)
 *
 * CXL 2.0 v1.0 Table 106
 *
 * MLD Port Command Set (MPC)
 */
struct fmapi_mpc_cfg_req 
{
	__u8 ppid;				//!< PPB ID: Target PPB physical port
	__u8 reg;            	//!< Register Number as defined in PCIe spec 
	__u8 ext 		: 4 ;	//!< Extended Register Number as defined in PCIe spec 
	__u8 fdbe 		: 4 ; 	//!< First DWord Byte Enable as defined in PCIe spec 
	__u8 type 		: 1 ;	//!< Transation type [FMCT]
	__u16 ldid;				//!< Target LD ID
	__u8 data[4];			//!< Transaction Data. Only valid for write transactions
};

/**
 * Send LD CXL.io Configuration - Response (Opcode 5301h)
 *
 * CXL 2.0 v1.0 Table 107
 *
 * MLD Port Command Set (MPC)
 */
struct fmapi_mpc_cfg_rsp 
{
	__u8 data[4];			//!< Return Data. Only valid for read transacitons 
};

/**
 * Send LD CXL.io Memory - Request (Opcode 5302h)
 *
 * CXL 2.0 v1.0 Table 108
 *
 * MLD Port Command Set (MPC)
 */
struct fmapi_mpc_mem_req 
{
	__u8 ppid;						//!< Port ID. Target MLD port
	__u32 fdbe 	: 4 ; 				//!< First DWord Byte Enable as defined in PCIe spec 
	__u32 ldbe 	: 4 ; 				//!< Last DWord Byte Enable as defined in PCIe spec 
	__u32 type 	: 1 ;				//!< Transation type [FMCT]
	__u16 ldid;						//!< Target LD ID
	__u16 len;						//!< Transaction Length in bytes, max of 4 kB
	__u64 offset;					//!< Transaction Offset into target device mem space
	__u8 data[FM_LD_MEM_REQ_LEN];	//!< Transacrtion Data. Only valid for write transactions 
};

/**
 * Send LD CXL.io Memory - Response (Opcode 5302h)
 *
 * CXL 2.0 v1.0 Table 109
 *
 * MLD Port Command Set (MPC)
 */
struct fmapi_mpc_mem_rsp 
{
	__u16 len;						//!< Return Size: Number of successfully transferred bytes
	__u8 data[FM_LD_MEM_REQ_LEN];	//!< Transacrtion Data. Only valid for read transactions 
};

/**
 * Get LD Info (Opcode 5400h)
 *
 * CXL 2.0 v1.0 Table 111
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 */
struct fmapi_mcc_info_rsp 
{
	__u64 size;			//!< Memory Size: Total device memory capacity
	__u16 num;			//!< LD Count: Number of logical devices supported

	/* QoS Telemetry Fields */
	__u8 epc;			//!< Egress Port Congestion Supported [FMQT]
	__u8 ttr;			//!< Temporary Throughput Reduction Supported [FMQT]
};

/**
 * Get LD Allocations List Format (Opcode 5401h)
 *
 * CXL 2.0 v1.0 Table 113
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 */
struct fmapi_mcc_alloc_blk 
{
	__u64 rng1;			//!< Range 1 Allocation Multiplier: Memory allocation range 1 for LD. This value is multiplied with Memory Granularity to calculate memory allocation range in bytes.
	__u64 rng2;			//!< Range 2 Allocation Multiplier: Memory allocation range 2 for LD. This value is multiplied with Memory Granularity to calculate memory allocation range in bytes.
};

/**
 * Get LD Allocations - Request (Opcode 5401h)
 *
 * CXL 2.0 Errata Table Z
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 */
struct fmapi_mcc_alloc_get_req
{
	__u8 start;			//!< LD ID of the first LD in the allocation list
	__u8 limit;			//!< Max num of LD information blocks to return 
};

/**
 * Get LD Allocations - Response (Opcode 5401h)
 *
 * CXL 2.0 v1.0 Table 112
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 */
struct fmapi_mcc_alloc_get_rsp 
{
	__u8 total;			//!< Total number of Logical Deviecs supported by this device 
	__u8 granularity;	//!< Memory Granularity: Specifies the granularity of the memory sizes configured for each LD: [FMMG]
	__u8 start;			//!< LD ID of the first LD in the list
	__u8 num;			//!< Num LDs returned in this object 

	//!< Variable number of LD Allocation List Blocks 
	struct fmapi_mcc_alloc_blk list[FM_MAX_NUM_LD];
};

/**
 * Set LD Allocations - Request (Opcode 5402h) 
 *
 * CXL 2.0 v1.0 Table 114
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 */
struct fmapi_mcc_alloc_set_req 
{
	__u8 num;			//!< Number of LDs: Number of LDs to configure
	__u8 start;			//!< LD ID of the first entry in the list 

	//!< Variable number of LD Allocation List Blocks 
	struct fmapi_mcc_alloc_blk list[FM_MAX_NUM_LD];
};

/**
 * Set LD Allocations - Response (Opcode 5402h) 
 *
 * CXL 2.0 v1.0 Table 115
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 */
struct fmapi_mcc_alloc_set_rsp 
{
	__u8 num;			//!< Number of LDs: Number of LDs configured
	__u8 start;			//!< LD ID of the first entry in the list 

	//!< Variable number of LD Allocation List Blocks 
	struct fmapi_mcc_alloc_blk list[FM_MAX_NUM_LD];
};

/**
 * QoS Control Payload (Opcodes 5403h, 5404h, 5405h)
 *
 * CXL 2.0 v1.0 Table 116
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 * This structure is the same for multiple request and response payloads 
 */
struct fmapi_mcc_qos_ctrl 
{
	__u8 epc_en; //!< QoS Telem: Egress Port Congestion Enable. Bitfield [FMQT]
	__u8 ttr_en; //!< QoS Telem: Temporary Throuhput Reduction Enable. Bitfield [FMQT]
	__u8 egress_mod_pcnt;	//!< Egress Moderate Percentage: Threshold in percent for Egress Port Congestion mechanism to indicate moderate congestion. Valid range is 1-100. Default is 10.
	__u8 egress_sev_pcnt;	//!< Egress Severe Percentage: Threshold in percent for Egress Port Congestion mechanism to indicate severe congestion. Valid range is 1-100. Default is 25
	__u8 sample_interval;	//!< Backpressure Sample Interval: Interval in ns for Egress Port Congestion mechanism to take samples. Valid range is 0-15. Default is 8 (800 ns of history). Value of 0 disables the mechanism.
	__u16 rcb;	//!< ReqCmpBasis. Estimated maximum sustained sum of requests and recent responses across the entire device, serving as the basis for QoS Limit Fraction. Valid range is 0-65,535. Value of 0 disables the mechanism. Default is 0.
	__u8 comp_interval;		//!< Completion Collection Interval: Interval in ns for Completion Counting mechanism to collect the number of transmitted responses in a single counter. Valid range is 0-255. Default is 64
};
#define fmapi_mcc_get_qos_ctrl_rsp fmapi_mcc_qos_ctrl
#define fmapi_mcc_set_qos_ctrl_req fmapi_mcc_qos_ctrl
#define fmapi_mcc_set_qos_ctrl_rsp fmapi_mcc_qos_ctrl

/**
 * Get QoS Status (Opcode 5405h) 
 *
 * CXL 2.0 v1.0 Table 117
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 */
struct fmapi_mcc_qos_stat_rsp 
{
	__u8 bp_avg_pcnt;	//!< Backpressure Average Percentage: Current snapshot of the measured Egress Port average congestion.
};

/**
 * Get QoS Allocated BW - Request (Opcode 5406h)
 *
 * CXL 2.0 Errata Table A
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 * The payload of this command is just an array of __u8 bytes 
 */
struct fmapi_mcc_qos_bw_alloc_get_req 
{
	__u8 num;			//!< Number of LDs requested
	__u8 start;			//!< LD ID of the first entry in the list 
};

/**
 * QoS Allocated BW Payload (Opcodes 5406h, 5407h)
 *
 * CXL 2.0 v1.0 Table 118
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 * The payload of this command is just an array of __u8 bytes 
 */
struct fmapi_mcc_qos_bw_alloc 
{
	__u8 num;					//!< Number of LDs
	__u8 start;					//!< LD ID of the first entry in the list 
	__u8 list[FM_MAX_NUM_LD];	//!< QoS Allocation Fraction: Byte array of allocated bandwidth fractions, where n = LD Count, as returned by the Get LD Info command. The valid range of each array element is 0-255. Default value is 0. Value in each byte is the fraction multiplied by 256.
};

/**
 * Get QoS BW Limit - Request (Opcode 5408h)
 *
 * CXL 2.0 Errata Table B
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 * The payload of this command is just an array of __u8 bytes 
 */
struct fmapi_mcc_qos_bw_limit_get_req 
{
	__u8 num;			//!< Number of LDs requested
	__u8 start;			//!< LD ID of the first entry in the list 
};

/**
 * QoS BW Limit payload (Opcodes 5408h, 5409h) 
 *
 * CXL 2.0 v1.0 Table 119
 *
 * MLD Component Command Set (MCC) 
 * These commands are passed to the MLD using the Tunnel Management Command
 * The payload of this command is just an array of __u8 bytes 
 */
struct fmapi_mcc_qos_bw_limit 
{
	__u8 num;
	__u8 start;
	__u8 list[FM_MAX_NUM_LD];	//!< QoS Limit Fraction: Byte array of allocated bandwidth limit fractions, where n = LD Count, as returned by the Get QoS BW command. The valid range of each array element is 0-255. Default value is 0. Value in each byte is the fraction multiplied by 256.
};

/**
 * This struct is to store the serialized FM API header and object 
 */
struct __attribute__((__packed__)) fmapi_buf
{
	__u8 hdr[FMLN_HDR];				//!< Buffer to store serialized FM API Header 
	__u8 payload[FMLN_PAYLOAD];		//!< Buffer to store serialized FM API Message Payload 
};

/**
 * FM API Message 
 */
struct fmapi_msg
{
	struct fmapi_hdr hdr;			//!< FM API Header 
	struct fmapi_buf *buf;			//!< Pointer to use to cast a mctp_msg buffer 
	//!< This union is to store the deserialized object 
	union 
	{
		struct fmapi_isc_id_rsp                    isc_id_rsp;
		struct fmapi_isc_msg_limit                 isc_msg_limit;
		struct fmapi_isc_bos                       isc_bos;
		struct fmapi_psc_id_rsp                    psc_id_rsp;
		struct fmapi_psc_port_req                  psc_port_req;
		struct fmapi_psc_port_rsp                  psc_port_rsp;
		struct fmapi_psc_port_ctrl_req             psc_port_ctrl_req;
		struct fmapi_psc_cfg_req                   psc_cfg_req;
		struct fmapi_psc_cfg_rsp                   psc_cfg_rsp;
		struct fmapi_vsc_info_req                  vsc_info_req;
		struct fmapi_vsc_info_rsp                  vsc_info_rsp;
		struct fmapi_vsc_bind_req                  vsc_bind_req;
		struct fmapi_vsc_unbind_req                vsc_unbind_req;
		struct fmapi_vsc_aer_req                   vsc_aer_req;
		struct fmapi_mpc_tmc_req                   mpc_tmc_req;
		struct fmapi_mpc_tmc_rsp                   mpc_tmc_rsp;
		struct fmapi_mpc_cfg_req                   mpc_cfg_req;
		struct fmapi_mpc_cfg_rsp                   mpc_cfg_rsp;
		struct fmapi_mpc_mem_req                   mpc_mem_req;
		struct fmapi_mpc_mem_rsp                   mpc_mem_rsp;
		struct fmapi_mcc_info_rsp                  mcc_info_rsp;
		struct fmapi_mcc_alloc_get_req             mcc_alloc_get_req;
		struct fmapi_mcc_alloc_get_rsp             mcc_alloc_get_rsp;
		struct fmapi_mcc_alloc_set_req             mcc_alloc_set_req;
		struct fmapi_mcc_alloc_set_rsp             mcc_alloc_set_rsp;
		struct fmapi_mcc_qos_ctrl                  mcc_qos_ctrl;
		struct fmapi_mcc_qos_stat_rsp              mcc_qos_stat_rsp;
		struct fmapi_mcc_qos_bw_alloc_get_req      mcc_qos_bw_alloc_get_req;
		struct fmapi_mcc_qos_bw_alloc              mcc_qos_bw_alloc;
		struct fmapi_mcc_qos_bw_limit_get_req      mcc_qos_bw_limit_get_req;
		struct fmapi_mcc_qos_bw_limit              mcc_qos_bw_limit;
	} obj;	
};

/* GLOBAL VARIABLES ==========================================================*/

/* PROTOTYPES ================================================================*/

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
int fmapi_deserialize(void *dst, __u8 *src, unsigned type, void *param);

/**
 * Convenience function to populate a fmapi_hdr object 
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
);


int fmapi_fill_isc_id(struct fmapi_msg *m);
int fmapi_fill_isc_bos(struct fmapi_msg *m);
int fmapi_fill_isc_get_msg_limit(struct fmapi_msg *m);
int fmapi_fill_isc_set_msg_limit(struct fmapi_msg *m, __u8 limit);

int fmapi_fill_mcc_get_alloc(struct fmapi_msg *m, int start, int limit);
int fmapi_fill_mcc_get_info(struct fmapi_msg *m);
int fmapi_fill_mcc_get_qos_alloc(struct fmapi_msg *m, int start, int limit);
int fmapi_fill_mcc_get_qos_ctrl(struct fmapi_msg *m);
int fmapi_fill_mcc_get_qos_limit(struct fmapi_msg *m, int start, int limit);
int fmapi_fill_mcc_get_qos_status(struct fmapi_msg *m);

int fmapi_fill_mcc_set_alloc(struct fmapi_msg *m, int start, int num, __u64 *rng1, __u64 *rng2);
int fmapi_fill_mcc_set_qos_alloc(struct fmapi_msg *m, int start, int num, __u8 *list);
int fmapi_fill_mcc_set_qos_ctrl(struct fmapi_msg *m, int epc, int ttr, int mod, int sev, int si, int rcb, int ci);
int fmapi_fill_mcc_set_qos_limit(struct fmapi_msg *m, int start, int num, __u8 *list);

int fmapi_fill_mpc_tmc(struct fmapi_msg *m, int ppid, int type, struct fmapi_msg *sub);
int fmapi_fill_mpc_cfg(struct fmapi_msg *m, int ppid, int ldid, int reg, int ext, int fdbe, int type, __u8 *data);
int fmapi_fill_mpc_mem(struct fmapi_msg *m, int ppid, int ldid, __u64 offset, int len, int fdbe, int ldbe,  int type, __u8 *data);

int fmapi_fill_psc_cfg(struct fmapi_msg *m, int ppid, int reg, int ext, int fdbe, int type, __u8 *data);
int fmapi_fill_psc_id(struct fmapi_msg *m);
int fmapi_fill_psc_get_all_ports(struct fmapi_msg *m);
int fmapi_fill_psc_get_port(struct fmapi_msg *m, __u8 port);
int fmapi_fill_psc_get_ports(struct fmapi_msg *m, int num, __u8 *list);
int fmapi_fill_psc_port_ctrl(struct fmapi_msg *m, int ppid, int opcode);

int fmapi_fill_vsc_aer(struct fmapi_msg *m, int vcsid, int vppbid, __u32 type, __u8 *header);
int fmapi_fill_vsc_bind(struct fmapi_msg *m, int vcsid, int vppbid, int ppid, int ldid); 
int fmapi_fill_vsc_get_vcs(struct fmapi_msg *m, int vcsid, int start, int limit);
int fmapi_fill_vsc_unbind(struct fmapi_msg *m, int vcsid, int vppbid, int option); 

/**
 * Determine the Request Object Identifier [FMOB] for an FM API Message Opcode [FMOP]
 *
 * @param	opcode 	This is an FM API Opcode [FMOP]
 * @return	int		Returns the FM API Object Identifier [FMOB] used in a Request
 */
int fmapi_fmob_req(unsigned int opcode);

/**
 * Determine the Response Object Identifier [FMOB] for an FM API Message Opcode [FMOP]
 *
 * @param	opcode 	This is an FM API Opcode [FMOP]
 * @return	int		Returns the FM API Object Identifier [FMOB] used in a Response 
 */
int fmapi_fmob_rsp(unsigned int opcode);

/**
 * @brief Convert an object into Little Endian byte array format
 * 
 * @param[out] dst void Pointer to destination unsigned char array
 * @param[in] src void Pointer to object to serialize
 * @param[in] type unsigned enum _FMOB representing type of object to serialize
 * @return number of serialized bytes, 0 if error 
 */
int fmapi_serialize(__u8 *dst, void *src, unsigned type);

/**
 * Print an object to the screen
 *
 * @param ptr A pointer to the object to print 
 * @param type The type of object to be printed from enum _FMOB
 */
void fmapi_prnt(void *ptr, unsigned type);        

/* Functions to return a string representation of an object*/
const char *fmbs(unsigned int u);
const char *fmct(unsigned int u);
const char *fmdt(unsigned int u);
const char *fmdv(unsigned int u);
const char *fmet(unsigned int u);
const char *fmlf(unsigned int u);
const char *fmlo(unsigned int u);
const char *fmls(unsigned int u);
const char *fmmg(unsigned int u);
const char *fmmr(unsigned int u);
const char *fmms(unsigned int u);
const char *fmmt(unsigned int u);
const char *fmnw(unsigned int u);
const char *fmop(unsigned int u);
const char *fmpo(unsigned int u);
const char *fmps(unsigned int u);
const char *fmqt(unsigned int u);
const char *fmrc(unsigned int u);
const char *fmss(unsigned int u);
const char *fmub(unsigned int u);
const char *fmvc(unsigned int u);
const char *fmvs(unsigned int u);
const char *fmvt(unsigned int u);

#endif //ifndef _FMAPI_H
