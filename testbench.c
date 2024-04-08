/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file 		testbench.c
 *
 * @brief 		Code file for testing the FM API commands
 *
 * @details 	This program has a series of tests that can be run to 
 *              demonstrate the functionality of the FM API
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

#include <stdlib.h>

/* memset()
 */
#include <string.h>

/* au_prnt_buf()
 */
#include <arrayutils.h>

#include "main.h"

/* MACROS ====================================================================*/

/* ENUMERATIONS ==============================================================*/

/* STRUCTS ===================================================================*/

/* GLOBAL VARIABLES ==========================================================*/

/* PROTOTYPES ================================================================*/

void print_strings()
{
	int i; 
	i = 0;
	printf("fmop %d: %s\n", i++, fmop(FMOP_PSC_ID		));
	printf("fmop %d: %s\n", i++, fmop(FMOP_PSC_PORT		));
	printf("fmop %d: %s\n", i++, fmop(FMOP_PSC_PORT_CTRL		));
	printf("fmop %d: %s\n", i++, fmop(FMOP_PSC_CFG			));
	printf("fmop %d: %s\n", i++, fmop(FMOP_VSC_INFO	));
	printf("fmop %d: %s\n", i++, fmop(FMOP_VSC_BIND					));
	printf("fmop %d: %s\n", i++, fmop(FMOP_VSC_UNBIND					));
	printf("fmop %d: %s\n", i++, fmop(FMOP_VSC_AER			));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MPC_TMC	));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MPC_CFG			));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MPC_MEM			));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_INFO					));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_ALLOC_GET			));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_ALLOC_SET			));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_QOS_CTRL_GET				));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_QOS_CTRL_SET				));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_QOS_STAT				));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_QOS_BW_ALLOC_GET			));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_QOS_BW_ALLOC_SET			));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_QOS_BW_LIMIT_GET				));
	printf("fmop %d: %s\n", i++, fmop(FMOP_MCC_QOS_BW_LIMIT_SET				));
	
	for ( int i = 0 ; i < FMMT_MAX; i++ )
		printf("fmmt %d: %s\n", i, fmmt(i));	
	
	for ( int i = 0 ; i < FMRC_MAX; i++ )
		printf("fmrc %d: %s\n", i, fmrc(i));	
	
	for ( int i = 0 ; i < FMPS_MAX; i++ )
		printf("fmps %d: %s\n", i, fmps(i));	
	
	for ( int i = 0 ; i < FMDV_MAX; i++ )
		printf("fmdv %d: %s\n", i, fmdv(i));	
	
	for ( int i = 0 ; i < FMDT_MAX; i++ )
		printf("fmdt %d: %s\n", i, fmdt(i));	
	
	for ( int i = 0 ; i < FMMS_MAX; i++ )
		printf("fmms %d: %s\n", i, fmms(i));	
	
	for ( int i = 0 ; i < FMLS_MAX; i++ )
		printf("fmls %d: %s\n", i, fmls(i));	
	
	printf("fmvs %d: %s\n", i, fmvs(FMVS_DISABLED));	
	printf("fmvs %d: %s\n", i, fmvs(FMVS_ENABLED));	
	printf("fmvs %d: %s\n", i, fmvs(FMVS_INVALID));	
	
	for ( int i = 0 ; i < FMBS_MAX; i++ )
		printf("fmbs %d: %s\n", i, fmbs(i));	
}

int verify_object(void *obj, unsigned obj_len, unsigned type, unsigned buf_len)
{
	__u8 *data;

	/* STEPS 
	 * 1: Allocate Memory
	 * 3: Clear memory
	 * 4: Fill in object with test data
	 * 5: Print Object 
	 * 6: Serialize Object
	 * 7: Print the buffer
	 * 8: Clear the object 
	 * 9: Deserialize buffer into object
	 * 10: Free memory
	 */

	// STEP 1: Allocate Memory
	data = (__u8*) malloc(buf_len);

	// STEP 2: Clear memory
	memset(data, 0 , buf_len);

	// STEP 5: Print Object 
	fmapi_prnt(obj, type);

	// STEP 6: Serialize Object
	fmapi_serialize(data, obj, type);

	// STEP 6: Print the buffer
	autl_prnt_buf(data, buf_len, 4, 1);
	
	// STEP 7: Clear the object 
	memset(obj, 0 , obj_len);

	// STEP 8: Deserialize buffer into object
	fmapi_deserialize(obj, data, type, &buf_len);

	// STEP 9: Print object
	fmapi_prnt(obj, type);

	// STEP 10: Free memory
	free(data);

	return 0;
}

int verify_hdr()
{
	struct fmapi_hdr obj; 

	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.category = FMMT_RESP;
	obj.tag = 0x42;
	obj.opcode = 0xABCD; 
	obj.len = 0x1FFFFF; 
	obj.background = 1;
	obj.return_code = 0xABCD;
	obj.ext_status = 0x1234;
	
	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_HDR, FMLN_HDR);
}

int verify_identify_switch()
{
	struct fmapi_psc_id_rsp obj;	

	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.ingress_port = 0x1;
	obj.num_ports = 0xFF;
	obj.num_vcss = 128;
	obj.num_vppbs = 256;
	obj.active_vppbs = 123;
	obj.num_decoders = 21;
	
	for ( int i = 0 ; i < 32 ; i++) 
		obj.active_ports[i] = i;
	for ( int i = 0 ; i < 32 ; i++) 
		obj.active_vcss[i] = i;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_PSC_ID_RSP, FMLN_PSC_IDENTIFY_SWITCH);
}

int verify_psc_get_port_req()
{
	struct fmapi_psc_port_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num = 1;
	obj.ports[0] = 0x23;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_PSC_PORT_REQ, FMLN_PSC_GET_PHY_PORT_REQ + obj.num);
}

int verify_psc_port_info()
{
	struct fmapi_psc_port_info obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.ppid 	= 1;				//!< Physical Port ID
	obj.state 	= FMPS_DSP;			//!< Current Port Configuration State [FMPS]
	obj.dv		= FMDV_CXL1_1;		//!< Connected Device CXL Version [FMDV]
	obj.dt		= FMDT_CXL_TYPE_1;	//!< Connected Device Type [FMDT]
	obj.cv		= FMCV_CXL1_1;		//!< Connected device CXL Version [FMCV]
	obj.mlw		= 16;				//!< Max link width
	obj.nlw		= FMNW_X8;			//!< Negotiated link width [FMNW]
	obj.speeds	= FMSS_PCIE5;		//!< Supported Link speeds vector [FMSS]
	obj.mls		= FMMS_PCIE5;		//!< Max Link Speed [FMMS]
	obj.cls		= FMMS_PCIE5;		//!< Current Link Speed [FMMS] 
	obj.ltssm	= FMLS_L0;			//!< LTSSM State [FMLS]
	obj.lane	= 1;				//!< First negotiated lane number
	obj.lane_rev=0;					//!< Link State Flags [FMLF] and [FMLO]
	obj.perst   = 0;				//!< Link State Flags [FMLF] and [FMLO]
	obj.prsnt   = 1;				//!< Link State Flags [FMLF] and [FMLO]
	obj.pwrctrl = 0;				//!< Link State Flags [FMLF] and [FMLO]
	obj.num_ld	= 16;				//!< Supported Logical Device (LDs) count 

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_PSC_PORT_INFO, FMLN_PSC_GET_PHY_PORT_INFO);
}

int verify_psc_get_port_resp()
{
	struct fmapi_psc_port_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num = 2;

	obj.list[0].ppid 	= 1;				//!< Physical Port ID
	obj.list[0].state 	= FMPS_DSP;			//!< Current Port Configuration State [FMPS]
	obj.list[0].dv		= FMDV_CXL1_1;		//!< Connected Device CXL Version [FMDV]
	obj.list[0].dt		= FMDT_CXL_TYPE_1;	//!< Connected Device Type [FMDT]
	obj.list[0].cv		= FMCV_CXL1_1;		//!< Connected device CXL Version [FMCV]
	obj.list[0].mlw		= 16;				//!< Max link width
	obj.list[0].nlw		= FMNW_X8;			//!< Negotiated link width [FMNW]
	obj.list[0].speeds	= FMSS_PCIE5;		//!< Supported Link speeds vector [FMSS]
	obj.list[0].mls		= FMMS_PCIE5;		//!< Max Link Speed [FMMS]
	obj.list[0].cls		= FMMS_PCIE5;		//!< Current Link Speed [FMMS] 
	obj.list[0].ltssm	= FMLS_L0;			//!< LTSSM State [FMLS]
	obj.list[0].lane	= 1;				//!< First negotiated lane number
	obj.list[0].lane_rev=0;					//!< Link State Flags [FMLF] and [FMLO]
	obj.list[0].perst   = 0;				//!< Link State Flags [FMLF] and [FMLO]
	obj.list[0].prsnt   = 1;				//!< Link State Flags [FMLF] and [FMLO]
	obj.list[0].pwrctrl = 0;				//!< Link State Flags [FMLF] and [FMLO]
	obj.list[0].num_ld	= 16;				//!< Supported Logical Device (LDs) count 

	obj.list[1].ppid 	= 2;				//!< Physical Port ID
	obj.list[1].state 	= FMPS_DSP;			//!< Current Port Configuration State [FMPS]
	obj.list[1].dv		= FMDV_CXL1_1;		//!< Connected Device CXL Version [FMDV]
	obj.list[1].dt		= FMDT_CXL_TYPE_1;	//!< Connected Device Type [FMDT]
	obj.list[1].cv		= FMCV_CXL1_1;		//!< Connected device CXL Version [FMCV]
	obj.list[1].mlw		= 16;				//!< Max link width
	obj.list[1].nlw		= FMNW_X8;			//!< Negotiated link width [FMNW]
	obj.list[1].speeds	= FMSS_PCIE5;		//!< Supported Link speeds vector [FMSS]
	obj.list[1].mls		= FMMS_PCIE5;		//!< Max Link Speed [FMMS]
	obj.list[1].cls		= FMMS_PCIE5;		//!< Current Link Speed [FMMS] 
	obj.list[1].ltssm	= FMLS_L0;			//!< LTSSM State [FMLS]
	obj.list[1].lane	= 1;				//!< First negotiated lane number
	obj.list[1].lane_rev=0;					//!< Link State Flags [FMLF] and [FMLO]
	obj.list[1].perst   = 0;				//!< Link State Flags [FMLF] and [FMLO]
	obj.list[1].prsnt   = 1;				//!< Link State Flags [FMLF] and [FMLO]
	obj.list[1].pwrctrl = 0;				//!< Link State Flags [FMLF] and [FMLO]
	obj.list[1].num_ld	= 16;				//!< Supported Logical Device (LDs) count 

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_PSC_PORT_RSP, FMLN_PSC_GET_PHY_PORT_RESP + obj.num * FMLN_PSC_GET_PHY_PORT_INFO);
}

int verify_psc_port_control()
{
	struct fmapi_psc_port_ctrl_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.ppid = 0x42;
	obj.opcode = FMPO_RESET_PPB;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_PSC_PORT_CTRL_REQ, FMLN_PSC_PHY_PORT_CTRL);
}

int verify_psc_ppb_config_req()
{
	struct fmapi_psc_cfg_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.ppid 	= 0x42; 	//!< PPB ID: Target PPB physical port
	obj.reg 	= 0xab; 	//!< Register Number as defined in PCIe spec 
	obj.ext 	= 0x0c;		//!< Extended Register Number as defined in PCIe spec 
	obj.fdbe 	= 0x0d; 	//!< First DWord Byte Enable as defined in PCIe spec 
	obj.type    = 0x1 ;		//!< Transation type [FMCT]
	obj.data[0] = 0;		//!< Transaction Data: Write data. Only valid for write transactions
	obj.data[1] = 1;		//!< Transaction Data: Write data. Only valid for write transactions
	obj.data[2] = 2;		//!< Transaction Data: Write data. Only valid for write transactions
	obj.data[3] = 3;		//!< Transaction Data: Write data. Only valid for write transactions

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_PSC_CFG_REQ, FMLN_PSC_PPB_IO_CFG_REQ);
}

int verify_psc_ppb_config_resp()
{
	struct fmapi_psc_cfg_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.data[0] = 0;		//!< Transaction Data: Write data. Only valid for write transactions
	obj.data[1] = 1;		//!< Transaction Data: Write data. Only valid for write transactions
	obj.data[2] = 2;		//!< Transaction Data: Write data. Only valid for write transactions
	obj.data[3] = 3;		//!< Transaction Data: Write data. Only valid for write transactions

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_PSC_CFG_RSP, FMLN_PSC_PPB_IO_CFG_RESP);
}

int verify_vsc_info_req()
{
	struct fmapi_vsc_info_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.vppbid_start = 2;
	obj.vppbid_limit = 6;
	obj.num = 1;
	obj.vcss[0] = 0x07;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_VSC_INFO_REQ, FMLN_VSC_GET_INFO_REQ + obj.num);
}

int verify_vsc_ppb_status_block()
{
	struct fmapi_vsc_ppb_stat_blk obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.status 	= FMBS_BOUND_PORT;
	obj.ppid	= 0x42;
	obj.ldid 	= 0x07;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_VSC_PPB_STAT_BLK, FMLN_VSC_PPB_STATUS);
}

int verify_vsc_info_block()
{
	struct fmapi_vsc_info_blk obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.vcsid = 0x42;
	obj.state = FMVS_ENABLED;
	obj.uspid = 0x01;
	obj.num = 2;

	obj.list[0].status 	= FMBS_BOUND_PORT;
	obj.list[0].ppid	= 0x21;
	obj.list[0].ldid 	= 0x07;

	obj.list[1].status 	= FMBS_BOUND_PORT;
	obj.list[1].ppid	= 0x22;
	obj.list[1].ldid 	= 0x05;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_VSC_INFO_BLK, FMLN_VSC_INFO + obj.num * FMLN_VSC_PPB_STATUS);
}

int verify_vsc_info_resp()
{
	struct fmapi_vsc_info_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num = 2;

	// VCS 0
	obj.list[0].vcsid = 0x42;
	obj.list[0].state = FMVS_ENABLED;
	obj.list[0].uspid = 0x01;
	obj.list[0].num = 2;

	// VCS 0 vPPB 0
	obj.list[0].list[0].status 	= FMBS_BOUND_PORT;
	obj.list[0].list[0].ppid	= 0x21;
	obj.list[0].list[0].ldid 	= 0x07;

	// VCS 0 vPPB 1
	obj.list[0].list[1].status 	= FMBS_BOUND_PORT;
	obj.list[0].list[1].ppid	= 0x22;
	obj.list[0].list[1].ldid 	= 0x05;
	
	// VCS 1
	obj.list[1].vcsid = 0x43;
	obj.list[1].state = FMVS_ENABLED;
	obj.list[1].uspid = 0x09;
	obj.list[1].num = 2;

	// VCS 1 1vPPB 0
	obj.list[1].list[0].status 	= FMBS_BOUND_PORT;
	obj.list[1].list[0].ppid	= 0x23;
	obj.list[1].list[0].ldid 	= 0x03;

	// VCS 1 1vPPB 1
	obj.list[1].list[1].status 	= FMBS_BOUND_PORT;
	obj.list[1].list[1].ppid	= 0x24;
	obj.list[1].list[1].ldid 	= 0x0a;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_VSC_INFO_RSP, FMLN_VSC_GET_INFO_RESP + 2 * FMLN_VSC_INFO + 4 * FMLN_VSC_PPB_STATUS);
}

int verify_vsc_bind()
{
	struct fmapi_vsc_bind_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.vcsid 		= 0x42;
	obj.vppbid 		= 0x0A;	
	obj.ppid 		= 0x0B;	
	obj.ldid		= 0x0C0D;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_VSC_BIND_REQ, FMLN_VSC_BIND);
}

int verify_vsc_unbind()
{
	struct fmapi_vsc_unbind_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.vcsid 	= 0x42;
	obj.vppbid  = 0x0A;
	obj.option  = FMUB_SURPRISE_HOT_REMOVE;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_VSC_UNBIND_REQ, FMLN_VSC_UNBIND);
}

int verify_vsc_gen_aer()
{
	struct fmapi_vsc_aer_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.vcsid 		= 0x42;
	obj.vppbid 		= 0x0A;
	obj.error_type 	= 0x0b0c0d0e;
	for ( int i = 0 ; i < 32 ; i++ )
		obj.header[i] = i;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_VSC_AER_REQ, FMLN_VSC_GEN_AER);
}

int verify_mpc_tmc_req()
{
	struct fmapi_mpc_tmc_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.ppid = 0x42;
	obj.len   = 68; 
	for ( int i = 0 ; i < obj.len ; i++ )
		obj.msg[i] = i;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MPC_TMC_REQ, FMLN_MPC_TUNNEL_CMD_REQ + obj.len);
}

int verify_mpc_tmc_resp()
{
	struct fmapi_mpc_tmc_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.len   = 68; 
	for ( int i = 0 ; i < obj.len ; i++ )
		obj.msg[i] = i;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MPC_TMC_RSP, FMLN_MPC_TUNNEL_CMD_RESP + obj.len);
}

int verify_mpc_config_req()
{
	struct fmapi_mpc_cfg_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.ppid 	= 0x42;
	obj.reg  	= 0x0A;
	obj.ext 	= 0x0B;
	obj.fdbe 	= 0x0C;
	obj.type 	= FMCT_WRITE;
	obj.ldid    = 0x0D0E;
	obj.data[0] = 0x01;
	obj.data[1] = 0x02;
	obj.data[2] = 0x03;
	obj.data[3] = 0x04;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MPC_CFG_REQ, FMLN_MPC_LD_IO_CFG_REQ);
}

int verify_mpc_config_resp()
{
	struct fmapi_mpc_cfg_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.data[0] = 0x01;
	obj.data[1] = 0x02;
	obj.data[2] = 0x03;
	obj.data[3] = 0x04;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MPC_CFG_RSP, FMLN_MPC_LD_IO_CFG_RESP);
}

int verify_mpc_mem_req()
{
	struct fmapi_mpc_mem_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.ppid 	= 0x42;
	obj.fdbe 	= 0x0a; 		
	obj.ldbe 	= 0x0b; 	
	obj.type 	= FMCT_WRITE;
	obj.ldid    = 0x0c0d;
	obj.len     = 68;		
	obj.offset  = 0xa1a2a3a4a5a6a7a8;
	for ( int i = 0 ; i < obj.len ; i++ )
		obj.data[i] = i;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MPC_MEM_REQ, FMLN_MPC_LD_MEM_REQ + obj.len);
}

int verify_mpc_mem_resp()
{
	struct fmapi_mpc_mem_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.len = 68;
	for ( int i = 0 ; i < obj.len ; i++ )
		obj.data[i] = i;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MPC_MEM_RSP, FMLN_MPC_LD_MEM_RESP + obj.len);
}

int verify_mcc_ld_info()
{
	struct fmapi_mcc_info_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.size 		= 0xa1a2a3a4a5a6a7a8;
	obj.num 	= 16;
	obj.epc  		= 1;
	obj.ttr 		= 1;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_INFO_RSP, FMLN_MCC_GET_LD_INFO);
}

int verify_mcc_ld_alloc_entry()
{
	struct fmapi_mcc_alloc_blk obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.rng1 = 0xa1a2a3a4a5a6a7a8;	
	obj.rng2 = 0xb1b2b3b4b5b6b7b8;	

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_ALLOC_BLK, FMLN_MCC_LD_ALLOC_ENTRY);
}

int verify_mcc_get_ld_alloc_req()
{
	struct fmapi_mcc_alloc_get_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.start = 2;
	obj.limit = 5;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_ALLOC_GET_REQ, FMLN_MCC_GET_LD_ALLOC_REQ);
}

int verify_mcc_get_ld_alloc_rsp()
{
	struct fmapi_mcc_alloc_get_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.total = 5;
	obj.granularity = FMMG_1GB;
	obj.start = 1;
	obj.num = 3;
	for ( int i = 0 ; i < obj.num ; i++ ) {
		obj.list[i].rng1 = 0xa1a2a3a4a5a6a7a8;	
		obj.list[i].rng2 = 0xb1b2b3b4b5b6b7b8;	
	}

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_ALLOC_GET_RSP, FMLN_MCC_GET_LD_ALLOC_RSP + obj.num * FMLN_MCC_LD_ALLOC_ENTRY);
}

int verify_mcc_set_ld_alloc_req()
{
	struct fmapi_mcc_alloc_set_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num = 2;
	obj.start = 3;
	for ( int i = 0 ; i < obj.num ; i++ ) {
		obj.list[i].rng1 = 0xa1a2a3a4a5a6a7a8;	
		obj.list[i].rng2 = 0xb1b2b3b4b5b6b7b8;	
	}

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_ALLOC_SET_REQ, FMLN_MCC_SET_LD_ALLOC_REQ + obj.num * FMLN_MCC_LD_ALLOC_ENTRY);
}

int verify_mcc_set_ld_alloc_resp()
{
	struct fmapi_mcc_alloc_set_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num = 2;
	obj.start = 3;
	for ( int i = 0 ; i < obj.num ; i++ ) {
		obj.list[i].rng1 = 0xa1a2a3a4a5a6a7a8;	
		obj.list[i].rng2 = 0xb1b2b3b4b5b6b7b8;	
	}

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_ALLOC_SET_RSP, FMLN_MCC_SET_LD_ALLOC_RSP + obj.num * FMLN_MCC_LD_ALLOC_ENTRY);
}

int verify_mcc_qos_ctrl()
{
	struct fmapi_mcc_qos_ctrl obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.epc_en 				= 1;
	obj.ttr_en 				= 1;
	obj.egress_mod_pcnt		= 10;
	obj.egress_sev_pcnt		= 25;
	obj.sample_interval		= 8;
	obj.rcb					= 0xabcd;
	obj.comp_interval		= 64;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_QOS_CTRL, FMLN_MCC_QOS_CTRL);
}

int verify_mcc_qos_status()
{
	struct fmapi_mcc_qos_stat_rsp obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.bp_avg_pcnt = 0x42;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_QOS_STAT_RSP, FMLN_MCC_QOS_STATUS);
}

int verify_mcc_get_qos_alloc_bw_req()
{
	struct fmapi_mcc_qos_bw_alloc_get_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num   = 7;
	obj.start = 3;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_QOS_BW_GET_REQ, FMLN_MCC_GET_QOS_BW_REQ);
}

int verify_mcc_qos_alloc_bw()
{
	struct fmapi_mcc_qos_bw_alloc obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num = FM_MAX_NUM_LD;
	obj.start = 1;
	for ( int i = 0 ; i < obj.num; i++)
		obj.list[i] = i;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_QOS_BW_ALLOC, FMLN_MCC_QOS_BW_ALLOC + obj.num);
}

int verify_mcc_get_qos_limit_bw_req()
{
	struct fmapi_mcc_qos_bw_limit_get_req obj;	
	
	/* STEPS 
	 * 1: Clear memory
	 * 2: Fill in object with test data
	 * 3: Verify object
	 */

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num   = 7;
	obj.start = 3;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_QOS_BW_LIMIT_GET_REQ, FMLN_MCC_GET_QOS_BW_LIMIT_REQ);
}

int verify_mcc_qos_bw_limit()
{
	struct fmapi_mcc_qos_bw_limit obj;	
	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.num = FM_MAX_NUM_LD;
	obj.start = 7;
	for ( int i = 0 ; i < obj.num; i++)
		obj.list[i] = i;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_MCC_QOS_BW_LIMIT, FMLN_MCC_QOS_BW_LIMIT + obj.num);
}

int verify_isc_id_rsp()
{
	struct fmapi_isc_id_rsp obj;	

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.vid = 0x1aed;
	obj.did = 0x1234;
	obj.svid = 0xabcd;
	obj.ssid = 0xb1b2;
	obj.sn = 0xa1a2a3a4a5a6a7a8;
	obj.size = 13;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_ISC_ID_RSP, FMLN_ISC_ID_RSP);
}

int verify_isc_msg_limit()
{
	struct fmapi_isc_msg_limit obj;	

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.limit = 13;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_ISC_MSG_LIMIT, FMLN_ISC_MSG_LIMIT);
}

int verify_isc_bos()
{
	struct fmapi_isc_bos obj;	

	// STEP 1: Clear memory
	memset(&obj, 0 , sizeof(obj));

	// STEP 2: Fill in object with test data
	obj.running = 1;
	obj.pcnt 	= 15;
	obj.opcode 	= 0xa1a2;
	obj.rc 		= 0xb1b2;
	obj.ext 	= 0xc1c2;

	// STEP 3: Verify object
	return verify_object(&obj, sizeof(obj), FMOB_ISC_BOS, FMLN_ISC_BOS);
}

int verify_sizes()
{
	printf("Sizeof:\n");
	printf("struct fmapi_hdr:                        %lu\n", sizeof(struct fmapi_hdr));
	printf("struct fmapi_psc_id_rsp:                 %lu\n", sizeof(struct fmapi_psc_id_rsp));
	printf("struct fmapi_psc_port_req:               %lu\n", sizeof(struct fmapi_psc_port_req));
	printf("struct fmapi_psc_port_info:              %lu\n", sizeof(struct fmapi_psc_port_info));
	printf("struct fmapi_psc_port_rsp:               %lu\n", sizeof(struct fmapi_psc_port_rsp));
	printf("struct fmapi_psc_port_ctrl_req:          %lu\n", sizeof(struct fmapi_psc_port_ctrl_req));
	printf("struct fmapi_psc_cfg_req:                %lu\n", sizeof(struct fmapi_psc_cfg_req));
	printf("struct fmapi_psc_cfg_rsp:                %lu\n", sizeof(struct fmapi_psc_cfg_rsp));
	printf("struct fmapi_vsc_info_req:               %lu\n", sizeof(struct fmapi_vsc_info_req));
	printf("struct fmapi_vsc_ppb_stat_blk:           %lu\n", sizeof(struct fmapi_vsc_ppb_stat_blk));
	printf("struct fmapi_vsc_info_blk:               %lu\n", sizeof(struct fmapi_vsc_info_blk));
	printf("struct fmapi_vsc_info_rsp:               %lu\n", sizeof(struct fmapi_vsc_info_rsp));
	printf("struct fmapi_vsc_bind_req:               %lu\n", sizeof(struct fmapi_vsc_bind_req));
	printf("struct fmapi_vsc_unbind_req:             %lu\n", sizeof(struct fmapi_vsc_unbind_req));
	printf("struct fmapi_vsc_aer_req:                %lu\n", sizeof(struct fmapi_vsc_aer_req));
	printf("struct fmapi_mpc_tmc_req:                %lu\n", sizeof(struct fmapi_mpc_tmc_req));
	printf("struct fmapi_mpc_tmc_rsp:                %lu\n", sizeof(struct fmapi_mpc_tmc_rsp));
	printf("struct fmapi_mpc_cfg_req:                %lu\n", sizeof(struct fmapi_mpc_cfg_req));
	printf("struct fmapi_mpc_cfg_rsp:                %lu\n", sizeof(struct fmapi_mpc_cfg_rsp));
	printf("struct fmapi_mpc_mem_req:                %lu\n", sizeof(struct fmapi_mpc_mem_req));
	printf("struct fmapi_mpc_mem_rsp:                %lu\n", sizeof(struct fmapi_mpc_mem_rsp));
	printf("struct fmapi_mcc_info_rsp:               %lu\n", sizeof(struct fmapi_mcc_info_rsp));
	printf("struct fmapi_mcc_alloc_blk:              %lu\n", sizeof(struct fmapi_mcc_alloc_blk));
	printf("struct fmapi_mcc_alloc_get_req:          %lu\n", sizeof(struct fmapi_mcc_alloc_get_req));
	printf("struct fmapi_mcc_alloc_get_rsp:          %lu\n", sizeof(struct fmapi_mcc_alloc_get_rsp));
	printf("struct fmapi_mcc_alloc_set_req:          %lu\n", sizeof(struct fmapi_mcc_alloc_set_req));
	printf("struct fmapi_mcc_alloc_set_rsp:          %lu\n", sizeof(struct fmapi_mcc_alloc_set_rsp));
	printf("struct fmapi_mcc_qos_ctrl:               %lu\n", sizeof(struct fmapi_mcc_qos_ctrl));
	printf("struct fmapi_mcc_qos_stat_rsp:           %lu\n", sizeof(struct fmapi_mcc_qos_stat_rsp));
	printf("struct fmapi_mcc_qos_bw_alloc_get_req:   %lu\n", sizeof(struct fmapi_mcc_qos_bw_alloc_get_req));
	printf("struct fmapi_mcc_qos_bw_alloc:           %lu\n", sizeof(struct fmapi_mcc_qos_bw_alloc));
	printf("struct fmapi_mcc_qos_bw_limit_get_req:   %lu\n", sizeof(struct fmapi_mcc_qos_bw_limit_get_req));
	printf("struct fmapi_mcc_qos_bw_limit:           %lu\n", sizeof(struct fmapi_mcc_qos_bw_limit));
	printf("struct fmapi_isc_id_rsp:                 %lu\n", sizeof(struct fmapi_isc_id_rsp));
	printf("struct fmapi_isc_msg_limit:              %lu\n", sizeof(struct fmapi_isc_msg_limit));
	printf("struct fmapi_isc_bos:                    %lu\n", sizeof(struct fmapi_isc_bos));
	printf("struct fmapi_buf:                    	 %lu\n", sizeof(struct fmapi_buf));
	printf("struct fmapi_msg:                    	 %lu\n", sizeof(struct fmapi_msg));
	return 0;
}

int main(int argc, char **argv)
{
	int i, max;
	const char *names[] = {
		"",
		"fmapi_hdr",						// 1

		"fmapi_psc_id_rsp",					// 2
		"fmapi_psc_port_req",				// 3
		"fmapi_psc_port_info",				// 4
		"fmapi_psc_port_rsp",				// 5
		"fmapi_psc_port_ctrl_req",			// 6
		"fmapi_psc_ppb_cfg_req",			// 7
		"fmapi_psc_ppb_cfg_rsp", 			// 8

		"fmapi_vsc_info_req", 				// 9
		"fmapi_vsc_ppb_stat_blk",			// 10
		"fmapi_vsc_info_blk",				// 11
		"fmapi_vsc_info_rsp",				// 12
		"fmapi_vsc_bind_req",				// 13
		"fmapi_vsc_unbind_req",				// 14
		"fmapi_vsc_aer_req",				// 15

		"fmapi_mpc_tmc_req",				// 16
		"fmapi_mpc_tmc_rsp",				// 17
		"fmapi_mpc_cfg_req",				// 18
		"fmapi_mpc_cfg_resp",				// 19
		"fmapi_mpc_mem_req",				// 20
		"fmapi_mpc_mem_rsp",				// 21

		"fmapi_mcc_ld_info_rsp",			// 22
		"fmapi_mcc_alloc_blk",				// 23
		"fmapi_mcc_alloc_get_req",			// 24
		"fmapi_mcc_alloc_get_rsp",			// 25
		"fmapi_mcc_alloc_set_req",			// 26
		"fmapi_mcc_alloc_set_rsp",			// 27  
                                        	     
		"fmapi_mcc_qos_ctrl",				// 28
		"fmapi_mcc_qos_stat_rsp", 			// 29
		"fmapi_mcc_qos_bw_alloc_get_req",   // 30
		"fmapi_mcc_qos_bw_alloc",			// 31
		"fmapi_mcc_qos_bw_limit_get_req",   // 32
		"fmapi_mcc_qos_bw_limit", 			// 33
		"fmapi_isc_id_rsp",					// 34
		"fmapi_isc_msg_limit",				// 35
		"fmapi_isc_bos",					// 36
		"sizeof()"
	};

	max = FMOB_MAX;

	if (argc > 1)
		i = atoi(argv[1]);
	else {
		for ( i = 0 ; i <= max ; i++ )
			printf("TEST %d: %s\n", i, names[i]);
		goto end;
	}
	if (i > max)
		goto end;

	printf("TEST %d: %s\n", i, names[i]);

	switch(i)
	{
		case FMOB_HDR						: verify_hdr(); 						break;	// 1,  //!< struct fmapi_hdr
		case FMOB_PSC_ID_RSP				: verify_identify_switch(); 			break;	// 3,  //!< struct fmapi_psc_ident
		case FMOB_PSC_PORT_REQ 				: verify_psc_get_port_req();  			break;	// 4,  //!< struct fmapi_psc_get_port_req
		case FMOB_PSC_PORT_INFO				: verify_psc_port_info(); 				break;	// 5,  //!< struct fmapi_psc_port_info
		case FMOB_PSC_PORT_RSP				: verify_psc_get_port_resp();  			break;	// 6,  //!< struct fmapi_psc_get_port_resp
		case FMOB_PSC_PORT_CTRL_REQ    		: verify_psc_port_control(); 			break;	// 7,  //!< struct fmapi_psc_port_control
		case FMOB_PSC_CFG_REQ				: verify_psc_ppb_config_req(); 			break;	// 8,  //!< struct fmapi_psc_ppb_config_req
		case FMOB_PSC_CFG_RSP				: verify_psc_ppb_config_resp();			break;	// 9,  //!< struct fmapi_psc_ppb_config_resp 
		case FMOB_VSC_INFO_REQ				: verify_vsc_info_req(); 				break;	// 10, //!< struct fmapi_vsc_info_req
		case FMOB_VSC_PPB_STAT_BLK			: verify_vsc_ppb_status_block();  		break;	// 11, //!< struct fmapi_vsc_ppb_status_block
		case FMOB_VSC_INFO_BLK 				: verify_vsc_info_block();        		break;	// 12, //!< struct fmapi_vsc_info_block
		case FMOB_VSC_INFO_RSP 	 			: verify_vsc_info_resp();         		break;	// 13, //!< struct fmapi_vsc_info_resp
		case FMOB_VSC_BIND_REQ				: verify_vsc_bind();              		break;	// 14, //!< struct fmapi_vsc_bind
		case FMOB_VSC_UNBIND_REQ 			: verify_vsc_unbind();            		break;	// 15, //!< struct fmapi_vsc_unbind
		case FMOB_VSC_AER_REQ 				: verify_vsc_gen_aer();           		break;	// 16, //!< struct fmapi_vsc_gen_aer
		case FMOB_MPC_TMC_REQ				: verify_mpc_tmc_req();           		break;	// 17, //!< struct fmapi_mpc_tmc_req
		case FMOB_MPC_TMC_RSP				: verify_mpc_tmc_resp();          		break;	// 18, //!< struct fmapi_mpc_tmc_resp
		case FMOB_MPC_CFG_REQ				: verify_mpc_config_req();        		break;	// 19, //!< struct fmapi_mpc_config_req
		case FMOB_MPC_CFG_RSP				: verify_mpc_config_resp();       		break;	// 20, //!< struct fmapi_mpc_config_resp
		case FMOB_MPC_MEM_REQ				: verify_mpc_mem_req();           		break;	// 21, //!< struct fmapi_mpc_mem_req
		case FMOB_MPC_MEM_RSP				: verify_mpc_mem_resp();          		break;	// 22, //!< struct fmapi_mpc_mem_resp
		case FMOB_MCC_INFO_RSP				: verify_mcc_ld_info();           		break;	// 23, //!< struct fmapi_mcc_ld_info
		case FMOB_MCC_ALLOC_BLK				: verify_mcc_ld_alloc_entry();     		break;	// 24, //!< struct fmapi_mcc_ld_alloc_entry
		case FMOB_MCC_ALLOC_GET_REQ			: verify_mcc_get_ld_alloc_req();   		break;	// 25, //!< struct fmapi_mcc_get_ld_alloc_req
		case FMOB_MCC_ALLOC_GET_RSP			: verify_mcc_get_ld_alloc_rsp();   		break;	// 25, //!< struct fmapi_mcc_get_ld_alloc_rsp
		case FMOB_MCC_ALLOC_SET_REQ			: verify_mcc_set_ld_alloc_req();  		break;	// 26, //!< struct fmapi_mcc_alloc_set_req
		case FMOB_MCC_ALLOC_SET_RSP			: verify_mcc_set_ld_alloc_resp();  		break;	// 27, //!< struct fmapi_mcc_set_ld_alloc_resp
		case FMOB_MCC_QOS_CTRL				: verify_mcc_qos_ctrl();  				break;	// 28, //!< struct fmapi_mcc_qos_ctrl
		case FMOB_MCC_QOS_STAT_RSP			: verify_mcc_qos_status();    			break;	// 29, //!< struct fmapi_mcc_qos_status
		case FMOB_MCC_QOS_BW_GET_REQ    	: verify_mcc_get_qos_alloc_bw_req();  	break;	// 30, //!< struct fmapi_mcc_qos_bw_alloc_get_req
		case FMOB_MCC_QOS_BW_ALLOC			: verify_mcc_qos_alloc_bw();  			break;	// 31, //!< struct fmapi_mcc_qos_alloc_bw
		case FMOB_MCC_QOS_BW_LIMIT_GET_REQ 	: verify_mcc_get_qos_limit_bw_req();  	break;	// 32, //!< struct fmapi_mcc_qos_bw_limit_get_req
		case FMOB_MCC_QOS_BW_LIMIT			: verify_mcc_qos_bw_limit();  			break;	// 33, //!< struct fmapi_mcc_qos_bw_limit
		case FMOB_ISC_ID_RSP 				: verify_isc_id_rsp();        			break;	// 34, //!< struct fmapi_isc_id_rsp
		case FMOB_ISC_MSG_LIMIT 			: verify_isc_msg_limit();      			break;	// 34, //!< struct fmapi_isc_msg_limit
		case FMOB_ISC_BOS        	 		: verify_isc_bos();     	 			break;	// 36, //!< struct fmapi_isc_bos
		case FMOB_MAX 						: verify_sizes();						break;  // 37
		default 							: print_strings();						break;
	}

end:
	return 0;
}
