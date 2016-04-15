#ifndef _VPR_PROJECT_H_
#define _VPR_PROJECT_H_


#define NET_DVR_NOERROR 					0	//û�д���
#define NET_DVR_PASSWORD_ERROR 				1	//�û����������
#define NET_DVR_NOENOUGHPRI 				2	//Ȩ�޲���
#define NET_DVR_NOINIT 						3	//û�г�ʼ��
#define NET_DVR_CHANNEL_ERROR 				4	//ͨ���Ŵ���
#define NET_DVR_OVER_MAXLINK 				5	//���ӵ�DVR�Ŀͻ��˸����������
#define NET_DVR_VERSIONNOMATCH				6	//�汾��ƥ��
#define NET_DVR_NETWORK_FAIL_CONNECT		7	//���ӷ�����ʧ��
#define NET_DVR_NETWORK_SEND_ERROR			8	//�����������ʧ��
#define NET_DVR_NETWORK_RECV_ERROR			9	//�ӷ�������������ʧ��
#define NET_DVR_NETWORK_RECV_TIMEOUT		10	//�ӷ������������ݳ�ʱ
#define NET_DVR_NETWORK_ERRORDATA			11	//���͵���������
#define NET_DVR_ORDER_ERROR					12	//���ô������
#define NET_DVR_OPERNOPERMIT				13	//�޴�Ȩ��
#define NET_DVR_COMMANDTIMEOUT				14	//DVR����ִ�г�ʱ
#define NET_DVR_ERRORSERIALPORT				15	//���ںŴ���
#define NET_DVR_ERRORALARMPORT				16	//�����˿ڴ���
#define NET_DVR_PARAMETER_ERROR 			17  //��������
#define NET_DVR_CHAN_EXCEPTION				18	//������ͨ�����ڴ���״̬
#define NET_DVR_NODISK						19	//û��Ӳ��
#define NET_DVR_ERRORDISKNUM				20	//Ӳ�̺Ŵ���
#define NET_DVR_DISK_FULL					21	//������Ӳ����
#define NET_DVR_DISK_ERROR					22	//������Ӳ�̳���
#define NET_DVR_NOSUPPORT					23	//��������֧��
#define NET_DVR_BUSY						24	//������æ
#define NET_DVR_MODIFY_FAIL					25	//�������޸Ĳ��ɹ�
#define NET_DVR_PASSWORD_FORMAT_ERROR		26	//���������ʽ����ȷ
#define NET_DVR_DISK_FORMATING				27	//Ӳ�����ڸ�ʽ����������������
#define NET_DVR_DVRNORESOURCE				28	//DVR��Դ����
#define	NET_DVR_DVROPRATEFAILED				29  //DVR����ʧ��
#define NET_DVR_OPENHOSTSOUND_FAIL 			30  //��PC����ʧ��
#define NET_DVR_DVRVOICEOPENED 				31  //�����������Խ���ռ��
#define	NET_DVR_TIMEINPUTERROR				32  //ʱ�����벻��ȷ
#define	NET_DVR_NOSPECFILE					33  //�ط�ʱ������û��ָ�����ļ�
#define NET_DVR_CREATEFILE_ERROR			34	//�����ļ�����
#define	NET_DVR_FILEOPENFAIL				35  //���ļ�����
#define	NET_DVR_OPERNOTFINISH				36  //�ϴεĲ�����û�����
#define	NET_DVR_GETPLAYTIMEFAIL				37  //��ȡ��ǰ���ŵ�ʱ�����
#define	NET_DVR_PLAYFAIL					38  //���ų���
#define NET_DVR_FILEFORMAT_ERROR			39  //�ļ���ʽ����ȷ
#define NET_DVR_DIR_ERROR					40	//·������
#define NET_DVR_ALLOC_RESOURCE_ERROR		41  //��Դ�������
#define NET_DVR_AUDIO_MODE_ERROR			42	//����ģʽ����
#define NET_DVR_NOENOUGH_BUF				43	//������̫С
#define NET_DVR_CREATESOCKET_ERROR		 	44	//����SOCKET����
#define NET_DVR_SETSOCKET_ERROR				45	//����SOCKET����
#define NET_DVR_MAX_NUM						46	//�����ﵽ���
#define NET_DVR_USERNOTEXIST				47	//�û�������
#define NET_DVR_WRITEFLASHERROR				48  //дFLASH����
#define NET_DVR_UPGRADEFAIL					49  //DVR����ʧ��
#define NET_DVR_CARDHAVEINIT				50  //���뿨�Ѿ���ʼ����
#define NET_DVR_PLAYERFAILED				51	//���ò��ſ���ĳ������ʧ��
#define NET_DVR_MAX_USERNUM					52  //�豸���û����ﵽ���
#define NET_DVR_GETLOCALIPANDMACFAIL		53  //��ÿͻ��˵�IP��ַ�������ַʧ��
#define NET_DVR_NOENCODEING					54	//��ͨ��û�б���
#define NET_DVR_IPMISMATCH					55	//IP��ַ��ƥ��
#define NET_DVR_MACMISMATCH					56	//MAC��ַ��ƥ��
#define NET_DVR_UPGRADELANGMISMATCH			57	//�����ļ����Բ�ƥ��
#define NET_DVR_MAX_PLAYERPORT				58	//������·���ﵽ���
#define NET_DVR_NOSPACEBACKUP				59	//�����豸��û���㹻�ռ���б���
#define NET_DVR_NODEVICEBACKUP				60	//û���ҵ�ָ���ı����豸
#define NET_DVR_PICTURE_BITS_ERROR			61	//ͼ����λ����������24ɫ
#define NET_DVR_PICTURE_DIMENSION_ERROR		62	//ͼƬ��*���ޣ� ��128*256
#define NET_DVR_PICTURE_SIZ_ERROR			63	//ͼƬ��С���ޣ���100K
#define NET_DVR_LOADPLAYERSDKFAILED			64	//���뵱ǰĿ¼��Player Sdk����
#define NET_DVR_LOADPLAYERSDKPROC_ERROR		65	//�Ҳ���Player Sdk��ĳ���������
#define NET_DVR_LOADDSSDKFAILED				66	//���뵱ǰĿ¼��DSsdk����
#define NET_DVR_LOADDSSDKPROC_ERROR		    67	//�Ҳ���DsSdk��ĳ���������
#define NET_DVR_DSSDK_ERROR					68	//����Ӳ�����DsSdk��ĳ������ʧ��
#define NET_DVR_VOICEMONOPOLIZE				69	//��������ռ
#define NET_DVR_JOINMULTICASTFAILED			70	//����ಥ��ʧ��
#define NET_DVR_CREATEDIR_ERROR				71	//������־�ļ�Ŀ¼ʧ��
#define NET_DVR_BINDSOCKET_ERROR			72	//���׽���ʧ��
#define NET_DVR_SOCKETCLOSE_ERROR			73	//socket�����жϣ��˴���ͨ�������������жϻ�Ŀ�ĵز��ɴ�
#define NET_DVR_USERID_ISUSING			    74	//ע��ʱ�û�ID���ڽ���ĳ����
#define NET_DVR_SOCKETLISTEN_ERROR			75	//����ʧ��
#define NET_DVR_PROGRAM_EXCEPTION			76	//�����쳣
#define NET_DVR_WRITEFILE_FAILED			77	//д�ļ�ʧ��
#define NET_DVR_FORMAT_READONLY				78  //��ֹ��ʽ��ֻ��Ӳ��
#define NET_DVR_WITHSAMEUSERNAME		    79  //�û����ýṹ�д�����ͬ���û���
#define NET_DVR_DEVICETYPE_ERROR            80  /*�������ʱ�豸�ͺŲ�ƥ��*/
#define NET_DVR_LANGUAGE_ERROR              81  /*�������ʱ���Բ�ƥ��*/
#define NET_DVR_PARAVERSION_ERROR           82  /*�������ʱ����汾��ƥ��*/
#define NET_DVR_IPCHAN_NOTALIVE             83  /*Ԥ��ʱ���IPͨ��������*/
#define NET_DVR_RTSP_SDK_ERROR				84	/*���ظ���IPCͨѶ��StreamTransClient.dllʧ��*/
#define NET_DVR_CONVERT_SDK_ERROR			85	/*����ת���ʧ��*/
#define NET_DVR_IPC_COUNT_OVERFLOW			86  /*��������ip����ͨ����*/
#define NET_DVR_MAX_ADD_NUM                 87  /*��ӱ�ǩ(һ���ļ�Ƭ��64)�ȸ����ﵽ���*/
#define NET_DVR_PARAMMODE_ERROR             88 //ͼ����ǿ�ǣ�����ģʽ��������Ӳ������ʱ���ͻ��˽����������ʱ����ֵ��
#define NET_DVR_CODESPITTER_OFFLINE			89 //��Ƶ�ۺ�ƽ̨�������������
#define NET_DVR_BACKUP_COPYING				90  //�豸���ڱ���
#define NET_DVR_CHAN_NOTSUPPORT             91  // ͨ����֧�ָò���
#define NET_DVR_CALLINEINVALID              92  // �߶���λ��̫���л򳤶��߲�����б 
#define NET_DVR_CALCANCELCONFLICT           93  // ȡ���궨��ͻ����������˹���ȫ�ֵ�ʵ�ʴ�С�ߴ����
#define NET_DVR_CALPOINTOUTRANGE            94 	// �궨�㳬����Χ 
#define NET_DVR_FILTERRECTINVALID           95  // �ߴ������������Ҫ��
#define NET_DVR_DDNS_DEVOFFLINE             96  //�豸û��ע�ᵽddns��
#define NET_DVR_DDNS_INTER_ERROR            97  //DDNS �������ڲ�����
#define NET_DVR_FUNCTION_NOT_SUPPORT_OS     98 //�˹��ܲ�֧�ָò���ϵͳ
#define NET_DVR_DEC_CHAN_REBIND             99 //����ͨ������ʾ�����������
#define NET_DVR_INTERCOM_SDK_ERROR          100 //���ص�ǰĿ¼�µ������Խ���ʧ��  
#define NET_DVR_ALIAS_DUPLICATE				150	//�����ظ�  //2011-08-31 ͨ�������������к��������豸���°汾ddns������

#define NET_DVR_FINDWITHINDEX_FAILED        151 //����ʧ��
// 2010-5-28
// ���д�����
#define RAID_ERROR_INDEX	                200
#define NET_DVR_NAME_NOT_ONLY               (RAID_ERROR_INDEX + 0)  // �����Ѵ���
#define NET_DVR_OVER_MAX_ARRAY              (RAID_ERROR_INDEX + 1 ) // ���дﵽ����
#define NET_DVR_OVER_MAX_VD                 (RAID_ERROR_INDEX + 2 ) // ������̴ﵽ����
#define NET_DVR_VD_SLOT_EXCEED              (RAID_ERROR_INDEX + 3 ) // ������̲�λ����
#define NET_DVR_PD_STATUS_INVALID           (RAID_ERROR_INDEX + 4 ) // �ؽ����������������״̬����
#define NET_DVR_PD_BE_DEDICATE_SPARE        (RAID_ERROR_INDEX + 5 ) // �ؽ����������������Ϊָ���ȱ�
#define NET_DVR_PD_NOT_FREE                 (RAID_ERROR_INDEX + 6 ) // �ؽ���������������̷ǿ���
#define NET_DVR_CANNOT_MIG2NEWMODE          (RAID_ERROR_INDEX + 7 ) // ���ܴӵ�ǰ����������Ǩ�Ƶ��µ���������
#define NET_DVR_MIG_PAUSE                   (RAID_ERROR_INDEX + 8 ) // Ǩ�Ʋ�������ͣ
#define NET_DVR_MIG_CANCEL                  (RAID_ERROR_INDEX + 9 ) // ����ִ�е�Ǩ�Ʋ�����ȡ��
#define NET_DVR_EXIST_VD                    (RAID_ERROR_INDEX + 10) // �����������ϴ���������̣��޷�ɾ������
#define NET_DVR_TARGET_IN_LD_FUNCTIONAL     (RAID_ERROR_INDEX + 11) // �����������Ϊ���������ɲ����ҹ�������
#define NET_DVR_HD_IS_ASSIGNED_ALREADY      (RAID_ERROR_INDEX + 12) // ָ����������̱�����Ϊ�������
#define NET_DVR_INVALID_HD_COUNT            (RAID_ERROR_INDEX + 13) // �������������ָ����RAID�ȼ���ƥ��
#define NET_DVR_LD_IS_FUNCTIONAL            (RAID_ERROR_INDEX + 14) // �����������޷��ؽ�
#define NET_DVR_BGA_RUNNING                 (RAID_ERROR_INDEX + 15) // ��������ִ�еĺ�̨����
#define NET_DVR_LD_NO_ATAPI                 (RAID_ERROR_INDEX + 16) // �޷���ATAPI�̴����������
#define NET_DVR_MIGRATION_NOT_NEED          (RAID_ERROR_INDEX + 17) // ��������Ǩ��
#define NET_DVR_HD_TYPE_MISMATCH            (RAID_ERROR_INDEX + 18) // ������̲�����ͬ������
#define NET_DVR_NO_LD_IN_DG                 (RAID_ERROR_INDEX + 19) // ��������̣��޷����д������
#define NET_DVR_NO_ROOM_FOR_SPARE           (RAID_ERROR_INDEX + 20) // ���̿ռ��С���޷���ָ��Ϊ�ȱ���
#define NET_DVR_SPARE_IS_IN_MULTI_DG        (RAID_ERROR_INDEX + 21) // �����ѱ�����Ϊĳ�����ȱ���
#define NET_DVR_DG_HAS_MISSING_PD           (RAID_ERROR_INDEX + 22) // ����ȱ����

// x86 64bit nvr���� 2012-02-04
#define NET_DVR_NAME_EMPTY					(RAID_ERROR_INDEX + 23) /*����Ϊ��*/
#define NET_DVR_INPUT_PARAM					(RAID_ERROR_INDEX + 24) /*�����������*/
#define NET_DVR_PD_NOT_AVAILABLE			(RAID_ERROR_INDEX + 25) /*������̲�����*/
#define NET_DVR_ARRAY_NOT_AVAILABLE			(RAID_ERROR_INDEX + 26) /*���в�����*/
#define NET_DVR_PD_COUNT					(RAID_ERROR_INDEX + 27) /*�������������ȷ*/
#define NET_DVR_VD_SMALL					(RAID_ERROR_INDEX + 28) /*�������̫С*/
#define NET_DVR_NO_EXIST					(RAID_ERROR_INDEX + 29) /*������*/
#define NET_DVR_NOT_SUPPORT					(RAID_ERROR_INDEX + 30) /*��֧�ָò���*/
#define NET_DVR_NOT_FUNCTIONAL	 			(RAID_ERROR_INDEX + 31) /*����״̬��������״̬*/
#define NET_DVR_DEV_NODE_NOT_FOUND			(RAID_ERROR_INDEX + 32) /*��������豸�ڵ㲻����*/
#define NET_DVR_SLOT_EXCEED					(RAID_ERROR_INDEX + 33) /*��λ�ﵽ����*/ 
#define NET_DVR_NO_VD_IN_ARRAY				(RAID_ERROR_INDEX + 34) /*�����ϲ������������*/
#define NET_DVR_VD_SLOT_INVALID				(RAID_ERROR_INDEX + 35) /*������̲�λ��Ч*/
#define NET_DVR_PD_NO_ENOUGH_SPACE			(RAID_ERROR_INDEX + 36) /*����������̿ռ䲻��*/
#define NET_DVR_ARRAY_NONFUNCTION			(RAID_ERROR_INDEX + 37) /*ֻ�д�������״̬�����в��ܽ���Ǩ��*/
#define NET_DVR_ARRAY_NO_ENOUGH_SPACE		(RAID_ERROR_INDEX + 38) /*���пռ䲻��*/
#define NET_DVR_STOPPING_SCANNING_ARRAY		(RAID_ERROR_INDEX + 39) /*����ִ�а�ȫ���̻�����ɨ��*/
#define NET_DVR_NOT_SUPPORT_16T             (RAID_ERROR_INDEX + 40) /*��֧�ִ�������16T������*/


// ���ܴ����� 
#define VCA_ERROR_INDEX                     300 // ���ܴ���������
#define NET_DVR_ID_ERROR                    (VCA_ERROR_INDEX + 0)   // ����ID������
#define NET_DVR_POLYGON_ERROR               (VCA_ERROR_INDEX + 1)   // ����β�����Ҫ��
#define NET_DVR_RULE_PARAM_ERROR            (VCA_ERROR_INDEX + 2)   // �������������
#define NET_DVR_RULE_CFG_CONFLICT           (VCA_ERROR_INDEX + 3)   // ������Ϣ��ͻ
#define NET_DVR_CALIBRATE_NOT_READY         (VCA_ERROR_INDEX + 4)   // ��ǰû�б궨��Ϣ
#define NET_DVR_CAMERA_DATA_ERROR           (VCA_ERROR_INDEX + 5)   // ���������������
#define NET_DVR_CALIBRATE_DATA_UNFIT        (VCA_ERROR_INDEX + 6)	// ���Ȳ�����б�������ڱ궨
#define NET_DVR_CALIBRATE_DATA_CONFLICT     (VCA_ERROR_INDEX + 7)	// �궨������Ϊ���е㹲�߻���λ��̫����
#define NET_DVR_CALIBRATE_CALC_FAIL         (VCA_ERROR_INDEX + 8)	// ������궨����ֵ����ʧ��
#define	NET_DVR_CALIBRATE_LINE_OUT_RECT		(VCA_ERROR_INDEX + 9)	// ����������궨�߳�����������Ӿ��ο�
#define NET_DVR_ENTER_RULE_NOT_READY		(VCA_ERROR_INDEX + 10)	// û�����ý�������
#define NET_DVR_AID_RULE_NO_INCLUDE_LANE	(VCA_ERROR_INDEX + 11)	// ��ͨ�¼�������û�а�����������ֵӵ�º����У�
#define NET_DVR_LANE_NOT_READY				(VCA_ERROR_INDEX + 12)	// ��ǰû�����ó���
#define NET_DVR_RULE_INCLUDE_TWO_WAY		(VCA_ERROR_INDEX + 13)	// �¼������а���2�ֲ�ͬ����
#define NET_DVR_LANE_TPS_RULE_CONFLICT      (VCA_ERROR_INDEX + 14)  // ���������ݹ����ͻ
#define NET_DVR_NOT_SUPPORT_EVENT_TYPE      (VCA_ERROR_INDEX + 15)  // ��֧�ֵ��¼�����
#define NET_DVR_LANE_NO_WAY                 (VCA_ERROR_INDEX + 16)  // ����û�з���
#define NET_DVR_SIZE_FILTER_ERROR           (VCA_ERROR_INDEX + 17)  // �ߴ���˿򲻺���
#define NET_DVR_LIB_FFL_NO_FACE             (VCA_ERROR_INDEX + 18) // �����㶨λʱ�����ͼ��û������
#define NET_DVR_LIB_FFL_IMG_TOO_SMALL       (VCA_ERROR_INDEX + 19) // �����㶨λʱ�����ͼ��̫С
#define NET_DVR_LIB_FD_IMG_NO_FACE          (VCA_ERROR_INDEX + 20) // ����ͼ���������ʱ�����ͼ��û������
#define NET_DVR_LIB_FACE_TOO_SMALL          (VCA_ERROR_INDEX + 21) // ��ģʱ����̫С
#define NET_DVR_LIB_FACE_QUALITY_TOO_BAD    (VCA_ERROR_INDEX + 22) // ��ģʱ����ͼ������̫��
#define NET_DVR_KEY_PARAM_ERR               (VCA_ERROR_INDEX + 23) //�߼��������ô���
#define NET_DVR_CALIBRATE_DATA_ERR          (VCA_ERROR_INDEX + 24) //�궨������Ŀ���󣬻�����ֵ���󣬻������㳬����ƽ��
#define NET_DVR_CALIBRATE_DISABLE_FAIL      (VCA_ERROR_INDEX + 25) //�����ù�������ȡ���궨
#define NET_DVR_VCA_LIB_FD_SCALE_OUTRANGE   (VCA_ERROR_INDEX + 26) //�����˿�Ŀ����Сֵ������С���˿�Ŀ�����ֵ��������
#define NET_DVR_LIB_FD_REGION_TOO_LARGE     (VCA_ERROR_INDEX + 27) //��ǰ�������Χ���󡣼�������Ϊͼ���2/3
#define NET_DVR_TRIAL_OVERDUE               (VCA_ERROR_INDEX + 28) //���ð��������ѽ���
#define NET_DVR_CONFIG_FILE_CONFLICT        (VCA_ERROR_INDEX + 29) //�豸�����������ļ���ͻ�����ܹ����������з��������ò�����������ʾ��
//�㷨����ش�����
#define NET_DVR_FR_FPL_FAIL                 (VCA_ERROR_INDEX + 30)   // ���������㶨λʧ��
#define NET_DVR_FR_IQA_FAIL                 (VCA_ERROR_INDEX + 31)   // ��������ʧ��
#define NET_DVR_FR_FEM_FAIL                 (VCA_ERROR_INDEX + 32)   // ����������ȡʧ��
#define NET_DVR_FPL_DT_CONF_TOO_LOW         (VCA_ERROR_INDEX + 33)   // �����㶨λʱ����������Ŷȹ���
#define NET_DVR_FPL_CONF_TOO_LOW            (VCA_ERROR_INDEX + 34)   // �����㶨λ���Ŷȹ���
#define NET_DVR_E_DATA_SIZE                 (VCA_ERROR_INDEX + 35)  // ���ݳ��Ȳ�ƥ��
#define NET_DVR_FR_MODEL_VERSION_ERR        (VCA_ERROR_INDEX + 36)  // ����ģ�������е�ģ�Ͱ汾����
#define NET_DVR_FR_FD_FAIL                  (VCA_ERROR_INDEX + 37)  // ʶ������������ʧ��
#define NET_DVR_FA_NORMALIZE_ERR            (VCA_ERROR_INDEX + 38)  // ������һ������
//����������
#define NET_DVR_DOG_PUSTREAM_NOT_MATCH      (VCA_ERROR_INDEX + 39)  // ���ܹ���ǰ��ȡ���豸���Ͳ�ƥ��
#define NET_DVR_DEV_PUSTREAM_NOT_MATCH      (VCA_ERROR_INDEX + 40)  // ǰ��ȡ���豸�汾��ƥ��
#define NET_DVR_PUSTREAM_ALREADY_EXISTS     (VCA_ERROR_INDEX + 41)  // �豸������ͨ���Ѿ���ӹ���ǰ���豸
#define NET_DVR_SEARCH_CONNECT_FAILED       (VCA_ERROR_INDEX + 42)  // ���Ӽ���������ʧ��
#define NET_DVR_INSUFFICIENT_DISK_SPACE     (VCA_ERROR_INDEX + 43)  // �ɴ洢��Ӳ�̿ռ䲻��
#define NET_DVR_DATABASE_CONNECTION_FAILED  (VCA_ERROR_INDEX + 44)  // ���ݿ�����ʧ��
#define NET_DVR_DATABASE_ADM_PW_ERROR       (VCA_ERROR_INDEX + 45)  // ���ݿ��û������������
#define NET_DVR_DECODE_YUV                  (VCA_ERROR_INDEX + 46)  // ����ʧ��

#define NET_DVR_RTSP_ERROR_NOENOUGHPRI          401  //��Ȩ�ޣ�����������401ʱ��ת�����������
#define NET_DVR_RTSP_ERROR_ALLOC_RESOURCE       402  //������Դʧ��
#define NET_DVR_RTSP_ERROR_PARAMETER            403  //��������
#define NET_DVR_RTSP_ERROR_NO_URL               404  //ָ����URL��ַ�����ڣ�����������404ʱ��ת�����������
#define NET_DVR_RTSP_ERROR_FORCE_STOP           406  //�û���;ǿ���˳�

#define NET_DVR_RTSP_GETPORTFAILED			    407  //rtsp �õ��˿ڴ���
#define NET_DVR_RTSP_DESCRIBERROR			    410  //rtsp decribe ��������
#define NET_DVR_RTSP_DESCRIBESENDTIMEOUT		411  //rtsp decribe ���ͳ�ʱ
#define NET_DVR_RTSP_DESCRIBESENDERROR			412  //rtsp decribe ����ʧ��
#define NET_DVR_RTSP_DESCRIBERECVTIMEOUT		413  //rtsp decribe ���ճ�ʱ
#define NET_DVR_RTSP_DESCRIBERECVDATALOST		414  //rtsp decribe �������ݴ���
#define NET_DVR_RTSP_DESCRIBERECVERROR			415  //rtsp decribe ����ʧ��
#define NET_DVR_RTSP_DESCRIBESERVERERR			416  //rtsp decribe ����������401:�û���������������Ȩ��,���� 501:�豸�ڲ���Դ����ʧ�� 

#define NET_DVR_RTSP_SETUPERROR			        420  //rtsp setup ��������
#define NET_DVR_RTSP_SETUPSENDTIMEOUT			421  //rtsp setup ���ͳ�ʱ
#define NET_DVR_RTSP_SETUPSENDERROR				422  //rtsp setup ���ʹ���
#define NET_DVR_RTSP_SETUPRECVTIMEOUT			423  //rtsp setup ���ճ�ʱ
#define NET_DVR_RTSP_SETUPRECVDATALOST			424  //rtsp setup �������ݴ���
#define NET_DVR_RTSP_SETUPRECVERROR				425  //rtsp setup ����ʧ��
#define NET_DVR_RTSP_OVER_MAX_CHAN				426  //rtsp setup ����������401,501�ȴ���,�������������

#define NET_DVR_RTSP_PLAYERROR			        430  //rtsp play ��������
#define NET_DVR_RTSP_PLAYSENDTIMEOUT			431  //rtsp play ���ͳ�ʱ
#define NET_DVR_RTSP_PLAYSENDERROR				432  //rtsp play ���ʹ���
#define NET_DVR_RTSP_PLAYRECVTIMEOUT			433  //rtsp play ���ճ�ʱ
#define NET_DVR_RTSP_PLAYRECVDATALOST			434  //rtsp play �������ݴ���
#define NET_DVR_RTSP_PLAYRECVERROR				435  //rtsp play ����ʧ��
#define NET_DVR_RTSP_PLAYSERVERERR				436  //rtsp play ����������401,501�ȴ���

#define NET_DVR_RTSP_TEARDOWNERROR			    440  //rtsp teardown ��������
#define NET_DVR_RTSP_TEARDOWNSENDTIMEOUT		441  //rtsp teardown ���ͳ�ʱ
#define NET_DVR_RTSP_TEARDOWNSENDERROR			442  //rtsp teardown ���ʹ���
#define NET_DVR_RTSP_TEARDOWNRECVTIMEOUT		443  //rtsp teardown ���ճ�ʱ
#define NET_DVR_RTSP_TEARDOWNRECVDATALOST		444  //rtsp teardown �������ݴ���
#define NET_DVR_RTSP_TEARDOWNRECVERROR			445  //rtsp teardown ����ʧ��
#define NET_DVR_RTSP_TEARDOWNSERVERERR			446  //rtsp teardown ����������401,501�ȴ���

#define  NET_PLAYM4_NOERROR					    500	//no error
#define	 NET_PLAYM4_PARA_OVER				    501	//input parameter is invalid;
#define  NET_PLAYM4_ORDER_ERROR				    502	//The order of the function to be called is error.
#define	 NET_PLAYM4_TIMER_ERROR				    503	//Create multimedia clock failed;
#define  NET_PLAYM4_DEC_VIDEO_ERROR			    504	//Decode video data failed.
#define  NET_PLAYM4_DEC_AUDIO_ERROR			    505	//Decode audio data failed.
#define	 NET_PLAYM4_ALLOC_MEMORY_ERROR		    506	//Allocate memory failed.
#define  NET_PLAYM4_OPEN_FILE_ERROR			    507	//Open the file failed.
#define  NET_PLAYM4_CREATE_OBJ_ERROR		    508	//Create thread or event failed
#define  NET_PLAYM4_CREATE_DDRAW_ERROR		    509	//Create DirectDraw object failed.
#define  NET_PLAYM4_CREATE_OFFSCREEN_ERROR      510	//failed when creating off-screen surface.
#define  NET_PLAYM4_BUF_OVER			        511	//buffer is overflow
#define  NET_PLAYM4_CREATE_SOUND_ERROR	        512	//failed when creating audio device.	
#define	 NET_PLAYM4_SET_VOLUME_ERROR	        513	//Set volume failed
#define  NET_PLAYM4_SUPPORT_FILE_ONLY	        514	//The function only support play file.
#define  NET_PLAYM4_SUPPORT_STREAM_ONLY	        515	//The function only support play stream.
#define  NET_PLAYM4_SYS_NOT_SUPPORT		        516	//System not support.
#define  NET_PLAYM4_FILEHEADER_UNKNOWN          517	//No file header.
#define  NET_PLAYM4_VERSION_INCORRECT	        518	//The version of decoder and encoder is not adapted.  
#define  NET_PALYM4_INIT_DECODER_ERROR          519	//Initialize decoder failed.
#define  NET_PLAYM4_CHECK_FILE_ERROR	        520	//The file data is unknown.
#define  NET_PLAYM4_INIT_TIMER_ERROR	        521	//Initialize multimedia clock failed.
#define	 NET_PLAYM4_BLT_ERROR			        522	//Blt failed.
#define  NET_PLAYM4_UPDATE_ERROR		        523	//Update failed.
#define  NET_PLAYM4_OPEN_FILE_ERROR_MULTI       524   //openfile error, streamtype is multi
#define  NET_PLAYM4_OPEN_FILE_ERROR_VIDEO       525   //openfile error, streamtype is video
#define  NET_PLAYM4_JPEG_COMPRESS_ERROR         526   //JPEG compress error
#define  NET_PLAYM4_EXTRACT_NOT_SUPPORT         527	//Don't support the version of this file.
#define  NET_PLAYM4_EXTRACT_DATA_ERROR          528	//extract video data failed.

#define NET_QOS_OK								 700					//no error
#define NET_QOS_ERROR							 (NET_QOS_OK - 1)		//qos error
#define NET_QOS_ERR_INVALID_ARGUMENTS			 (NET_QOS_OK - 2)		//invalid arguments 
#define NET_QOS_ERR_SESSION_NOT_FOUND			 (NET_QOS_OK - 3)		//session net found
#define NET_QOS_ERR_LIB_NOT_INITIALIZED          (NET_QOS_OK - 4)		//lib not initialized
#define NET_QOS_ERR_OUTOFMEM                     (NET_QOS_OK - 5)		//outtofmem
#define NET_QOS_ERR_PACKET_UNKNOW                (NET_QOS_OK - 10)		//packet unknow
#define NET_QOS_ERR_PACKET_VERSION               (NET_QOS_OK - 11)		//packet version error
#define NET_QOS_ERR_PACKET_LENGTH                (NET_QOS_OK - 12)		//packet length error
#define NET_QOS_ERR_PACKET_TOO_BIG               (NET_QOS_OK - 13)		//packet too big
#define NET_QOS_ERR_SCHEDPARAMS_INVALID_BANDWIDTH (NET_QOS_OK - 20)		//schedparams invalid bandwidth
#define NET_QOS_ERR_SCHEDPARAMS_BAD_FRACTION      (NET_QOS_OK - 21)		//schedparams bad fraction
#define NET_QOS_ERR_SCHEDPARAMS_BAD_MINIMUM_INTERVAL (NET_QOS_OK - 22)	//schedparams bad minimum interval

#define NET_ERROR_TRUNK_LINE						711 //��ϵͳ�ѱ���ɸ���
#define NET_ERROR_MIXED_JOINT						712 //���ܽ��л��ƴ��
#define NET_ERROR_DISPLAY_SWITCH					713 //���ܽ�����ʾͨ���л�
#define NET_ERROR_USED_BY_BIG_SCREEN				714 //������Դ������ռ��
#define NET_ERROR_USE_OTHER_DEC_RESOURCE			715 //����ʹ������������ϵͳ��Դ
#define NET_ERROR_DISP_MODE_SWITCH                  716 //��ʾͨ����ʾ״̬�л���
#define NET_ERROR_SCENE_USING						717	//��������ʹ��

#define NET_DVR_DEV_NET_OVERFLOW		            800	//�������������豸��������
#define NET_DVR_STATUS_RECORDFILE_WRITING_NOT_LOCK  801 //¼���ļ���¼���޷������� 
#define NET_DVR_STATUS_CANT_FORMAT_LITTLE_DISK      802 //����Ӳ��̫С�޷���ʽ��

//N+1������
#define NET_SDK_ERR_REMOTE_DISCONNECT				803 //Զ���޷�����
#define NET_SDK_ERR_RD_ADD_RD						804 //����������ӱ���
#define NET_SDK_ERR_BACKUP_DISK_EXCEPT				805 //�������쳣
#define NET_SDK_ERR_RD_LIMIT						806 //�������Ѵ�����
#define NET_SDK_ERR_ADDED_RD_IS_WD					807 //��ӵı����ǹ�����
#define NET_SDK_ERR_ADD_ORDER_WRONG					808 //���˳���������û�б����������Ϊ����������ӹ�����
#define NET_SDK_ERR_WD_ADD_WD						809 //������������ӹ�����
#define NET_SDK_ERR_WD_SERVICE_EXCETP				810 //������CVR�����쳣
#define NET_SDK_ERR_RD_SERVICE_EXCETP				811 //����CVR�����쳣
#define NET_SDK_ERR_ADDED_WD_IS_RD					812 //��ӵĹ������Ǳ���
#define NET_SDK_ERR_PERFORMANCE_LIMIT				813 //���ܴﵽ����
#define NET_SDK_ERR_ADDED_DEVICE_EXIST				814 //��ӵ��豸�Ѿ�����



//2011-10-25���������������루900-950��
#define  NET_ERR_WINCHAN_IDX				        901	// ����ͨ���Ŵ���
#define  NET_ERR_WIN_LAYER						    902	// ���ڲ������󣬵�����Ļ����า�ǵĴ��ڲ���
#define  NET_ERR_WIN_BLK_NUM			            903	// ���ڵĿ������󣬵������ڿɸ��ǵ���Ļ����
#define  NET_ERR_OUTPUT_RESOLUTION	                904	// ����ֱ��ʴ���
#define  NET_ERR_LAYOUT							    905	// ���ֺŴ���
#define  NET_ERR_INPUT_RESOLUTION    	            906 // ����ֱ��ʲ�֧��
#define  NET_ERR_SUBDEVICE_OFFLINE                  907 // ���豸������
#define  NET_ERR_NO_DECODE_CHAN                     908 // û�п��н���ͨ��
#define  NET_ERR_MAX_WINDOW_ABILITY                 909 // ������������
#define  NET_ERR_ORDER_ERROR                        910 // ����˳������
#define  NET_ERR_PLAYING_PLAN    					911 //����ִ��Ԥ��
#define  NET_ERR_DECODER_USED						912 //���������ʹ��
#define	 NET_ERR_OUTPUT_BOARD_DATA_OVERFLOW			913	// ���������������
#define	 NET_ERR_SAME_USER_NAME						914	// �û�����ͬ
#define	 NET_ERR_INVALID_USER_NAME					915	// ��Ч�û���
#define	 NET_ERR_MATRIX_USING						916	// �����������ʹ��
#define	 NET_ERR_DIFFERENT_CHAN_TYPE				917	// ͨ�����Ͳ�ͬ���������ͨ���Ϳ�����������Ϊ��ͬ�����ͣ�
#define	 NET_ERR_INPUT_CHAN_BINDED					918	// ����ͨ���Ѿ������������
#define	 NET_ERR_BINDED_OUTPUT_CHAN_OVERFLOW		919	// ����ʹ�õľ������ͨ����������������������󶨵�ͨ������
#define	 NET_ERR_MAX_SIGNAL_NUM						920	// �����ź�Դ�����ﵽ����
#define  NET_ERR_INPUT_CHAN_USING					921	// ����ͨ������ʹ��
#define  NET_ERR_MANAGER_LOGON 					    922	//����Ա�Ѿ���½������ʧ��
#define  NET_ERR_USERALREADY_LOGON 				    923	//���û��Ѿ���½������ʧ��
#define  NET_ERR_LAYOUT_INIT 						924	//�������ڳ�ʼ��������ʧ��
#define	 NET_ERR_BASEMAP_SIZE_NOT_MATCH				925	//��ͼ��С���� 

//�����������루951-999��
#define  NET_ERR_MAX_WIN_OVERLAP                    951 //�ﵽ��󴰿��ص���
//�����������������
#define XML_ABILITY_NOTSUPPORT                      1000  //��֧�������ڵ��ȡ
#define	XML_ANALYZE_NOENOUGH_BUF				    1001		//����ڴ治��
#define	XML_ANALYZE_FIND_LOCALXML_ERROR			    1002		//�޷��ҵ���Ӧ�ı���xml
#define	XML_ANALYZE_LOAD_LOCALXML_ERROR			    1003		//���ر���xml����
#define	XML_NANLYZE_DVR_DATA_FORMAT_ERROR		    1004		//�豸�������ݸ�ʽ����
#define	XML_ANALYZE_TYPE_ERROR					    1005		//���������ʹ���
#define	XML_ANALYZE_XML_NODE_ERROR				    1006		//XML�����ڵ��ʽ����
#define XML_INPUT_PARAM_ERROR                       1007  //���������XML�ڵ�ֵ����

//2012-10-16 �����豸�����루1200~1300��
#define NET_ERR_SEARCHING_MODULE                    1201 // �����������ģ��
#define NET_ERR_REGISTERING_MODULE                  1202 // ����ע�����ģ��
#define NET_ERR_GETTING_ZONES						1203 // ���ڻ�ȡ��������
#define NET_ERR_GETTING_TRIGGERS					1204 // ���ڻ�ȡ������
#define NET_ERR_ARMED_STATUS						1205 // ϵͳ���ڲ���״̬
#define	NET_ERR_PROGRAM_MODE_STATUS					1206 // ϵͳ���ڱ��ģʽ
#define	NET_ERR_WALK_TEST_MODE_STATUS				1207 // ϵͳ���ڲ���ģʽ
#define	NET_ERR_BYPASS_STATUS						1208 // ��·״̬
#define NET_ERR_DISABLED_MODULE_STATUS				1209 // ����δʹ��
#define	NET_ERR_NOT_SUPPORT_OPERATE_ZONE			1210 // ������֧�ָò���
#define NET_ERR_NOT_SUPPORT_MOD_MODULE_ADDR			1211 // ģ���ַ���ܱ��޸�
#define NET_ERR_UNREGISTERED_MODULE					1212 // ģ��δע��
#define NET_ERR_PUBLIC_SUBSYSTEM_ASSOCIATE_SELF		1213 // ������ϵͳ��������
#define NET_ERR_EXCEEDS_ASSOCIATE_SUBSYSTEM_NUM		1214 // ����������ϵͳ����������
#define NET_ERR_BE_ASSOCIATED_BY_PUBLIC_SUBSYSTEM	1215 // ��ϵͳ������������ϵͳ����
#define	NET_ERR_ZONE_FAULT_STATUS					1216 // �������ڹ���״̬
#define  NET_ERR_SAME_EVENT_TYPE					1217 //�¼�������������������¼�������������ر�������ͬ�¼�����	
#define NET_ERR_ZONE_ALARM_STATUS					1218 //�������ڱ���״̬
#define NET_ERR_EXPANSION_BUS_SHORT_CIRCUIT			1219 //��չ���߶�·

//2012-12-20ץ�Ļ������루1400-1499��
#define NET_DVR_ERR_LANENUM_EXCEED                  1400  //��������������
#define NET_DVR_ERR_PRAREA_EXCEED                   1401  //��ʶ�������
#define NET_DVR_ERR_LIGHT_PARAM                     1402  //�źŵƽ����������
#define NET_DVR_ERR_LANE_LINE_INVALID               1403  //���������ô���
#define NET_DVR_ERR_STOP_LINE_INVALID               1404  //ֹͣ�����ô���
#define NET_DVR_ERR_LEFTORRIGHT_LINE_INVALID        1405  //��/��ת�ֽ������ô���
#define NET_DVR_ERR_LANE_NO_REPEAT                  1406  //���ӳ������ظ�
#define NET_DVR_ERR_PRAREA_INVALID                  1407  //��ʶ����β�����Ҫ��
#define NET_DVR_ERR_LIGHT_NUM_EXCEED                1408  //��Ƶ��⽻ͨ���źŵ���Ŀ�������ֵ
#define NET_DVR_ERR_SUBLIGHT_NUM_INVALID            1409  //��Ƶ��⽻ͨ���źŵ��ӵ���Ŀ���Ϸ�
#define NET_DVR_ERR_LIGHT_AREASIZE_INVALID          1410  //��Ƶ��⽻ͨ�������źŵƿ��С���Ϸ�
#define NET_DVR_ERR_LIGHT_COLOR_INVALID             1411  //��Ƶ��⽻ͨ�������źŵ���ɫ���Ϸ�
#define NET_DVR_ERR_LIGHT_DIRECTION_INVALID         1412  //��Ƶ��⽻ͨ������Ʒ������Բ��Ϸ�

//2013-3-6VQD�����루1500��1550��
#define NET_ERR_VQD_TIME_CONFLICT					1500 //VQD���ʱ��γ�ͻ
#define NET_ERR_VQD_PLAN_NO_EXIST					1501 //VQD��ϼƻ�������
#define NET_ERR_VQD_CHAN_NO_EXIST					1502 //VQD��ص㲻����
#define NET_ERR_VQD_CHAN_MAX						1503 //VQD�ƻ����Ѵ�����
#define NET_ERR_VQD_TASK_MAX						1504 //VQD�������Ѵ�����

/*******************ȫ�ִ����� end**********************/

/******************************************
Func:VPR_NotifyFunc
Description:	����ʶ���¼��ص��������� 
Input:      uIndex��������ţ�������֤���ơ�ͼ���һ���ԣ���ȡץ�Ľ��ʱ���ݸ���Ӧ������
Output:      ��
Return:		��
******************************************/
typedef void (__stdcall *VPR_NotifyFunc)(unsigned int uIndex);


/******************************************
Func:VPR_Connect
Description:	���ӳ���ʶ��ϵͳ 
Input:      lpszConnPara1�����Ӳ���1�����ͨ���������ӣ����Ӳ�������ʽ������IP��ַ������˿ںţ�
                  �磺"192.168.0.1:3000"�����ͨ���������ӣ����Ӳ�������Ϊ�˿ںţ���"COM1"��"1"��
			lpszConnPara2�����Ӳ���2��һ��Ϊ���ַ�������������Ҫ���ö���������豸��
Output:      ��
Return:		0-�ɹ�������-ʧ��
******************************************/
	int VPR_Connect(const char* lpszConnPara1, const char* lpszConnPara2);


/******************************************
Func:VPR_Disconnect
Description:	�Ͽ��복��ʶ��ϵͳ������ 
Input:      ��
Output:     ��
Return:		0-�ɹ�������-ʧ��
******************************************/
int VPR_Disconnect();


/******************************************
Func:VPR_Capture
Description:������������ʶ��ϵͳץ��ͼ��ʶ���ƣ�����Ϊ��������ʽ�����ú��������أ�
            ץ��ʶ������ɲ�׼�������ݺ�ͨ����Ϣ�ķ�ʽ֪ͨ�ϲ�Ӧ�� 
Input:      ��
Output:      ��
Return:		0-�ɹ�������-ʧ��
******************************************/
int VPR_Capture();


/******************************************
Func:VPR_GetPlate
Description:	��ȡָ����ų�����ɫ�����ƺš�
				������ɫ����Ϊ��0-���ƣ�1-���ƣ�2-���ƣ�3-���ƣ�-1-����ɫ
				���Ƹ�ʽ���磺"³A12345"
Input:   nIndex��������ţ�
		 
Output:  nColor: ������ɫ���룻
		 lpszPlate�����ƺţ�Ϊ0�����ַ�����������Ӧ��20�ֽ����ڣ���δʶ������ƻ����޳�������㳤���ַ�����
		 nRating�����ƵĿ��Ŷȣ�Ϊ����������nRating=85����ʾ���Ŷ�Ϊ85%��
Return:		0-�ɹ�������-ʧ��
******************************************/
int VPR_GetPlate(unsigned int nIndex, int &nColor, const char* lpszPlate, unsigned int &nRating);



/******************************************
Func:VPR_GetBigImg
Description:	��ȡָ����ŵĳ�����ͼ 
Input:      nIndex��������ţ�
			lpImgBuf��ͼ�����ݻ�����;
			nSize�� ����ʱΪ�ϲ�Ӧ�÷���Ļ�������С�����ʱΪͼ��ʵ�ʴ�С��
					��ͼ���С���ڻ�������С������lpImgBufΪ��ָ�룩�����ͼ��ʵ�ʴ�С
Output:      ��
Return:		0-�ɹ���1-ͼ���С���ڻ����� 2-��ͼ�� ����-ʧ�ܣ�
******************************************/
int VPR_GetBigImg(unsigned int nIndex, unsigned char* lpImgBuf, unsigned int &nSize);



/******************************************
Func:VPR_GetSmallImg
Description:	��ȡָ����ŵĳ���Сͼ 
Input:      nIndex��������ţ�
			lpImgBuf��ͼ�����ݻ�����;
			nSize�� ����ʱΪ�ϲ�Ӧ�÷���Ļ�������С�����ʱΪͼ��ʵ�ʴ�С��
					��ͼ���С���ڻ�������С������lpImgBufΪ��ָ�룩�����ͼ��ʵ�ʴ�С��
Output:      ��
Return:		0-�ɹ���1-ͼ���С���ڻ����� 2-��ͼ�� ����-ʧ�ܣ�
******************************************/
int VPR_GetSmallImg(unsigned nIndex, unsigned char* lpImgBuf,unsigned &nSize);


/******************************************
Func:VPR_GetBinImg
Description:	��ȡָ����ŵĳ��ƶ�ֵ��ͼ
Input:      nIndex��������ţ�
			lpImgBuf��ͼ�����ݻ�����;
			nWeidth������Ķ�ֵ��ͼ���ȣ�
			nHeight������Ķ�ֵ��ͼ��߶ȣ�
			nSize�� ����ʱΪ�ϲ�Ӧ�÷���Ļ�������С�����ʱΪͼ��ʵ�ʴ�С��
					��ͼ���С���ڻ�������С������lpImgBufΪ��ָ�룩�����ͼ��ʵ�ʴ�С��
Output:      ��
Return:		0-�ɹ���1-ͼ���С���ڻ����� 2-��ͼ�� ����-ʧ�ܣ�
******************************************/
int VPR_GetBinImg(unsigned int nIndex, unsigned char* lpImgBuf, unsigned int &nSize, unsigned int &nWidth, unsigned &nHeight);



typedef struct
 {
	int nColor;     //������ɫ���� 0-���ƣ�1-���ƣ�2-���ƣ�3-���ƣ�-1-����ɫ
	char *lpszPlate;  //���ƺ� 20�ֽ�����
	unsigned int nRating;  //���ƵĿ��Ŷ�  0-100
	unsigned char *lpBigImgBuf;   //������ͼͼ�����ݻ�����  �������ΪNULL�������ͼ��
	unsigned int nBigImgSize;     //������ͼͼ���С ����ʱΪ��������С�����ʱΪʵ��ͼ���С
	unsigned char *lpSmallImgBuf; //����Сͼͼ�����ݻ�����
	unsigned int nSmallImgSize;   //����Сͼͼ���С
	unsigned char *lpBinImgBuf;   //��ֵ��ͼ�����ݻ�����
	unsigned int nBinImgSize;     //����Сͼͼ���С
	unsigned int nBinImgWidth;	  //��ֵ��ͼ����
	unsigned int nBinImgHeight;   //��ֵ��ͼ��߶�
}CAutoDetectedVehInfo, *LPAutoDetectedVehInfo;

/******************************************
Func:VPR_GetAllVehInfo
Description:	��ȡ���ơ���ͼ������Сͼ����ֵ��ͼ������
Input:      nIndex��������ţ�
			LPAutoDetectedVehInfo: ����ʶ�����ṹ
Output:     AutoDetectedVehInfo: ����ʶ�����ṹ
Return:		0-�ɹ�������-ʧ�ܣ�
******************************************/
int VPR_GetAllVehInfo(unsigned int nIndex, LPAutoDetectedVehInfo pAuto);


/******************************************
Func:VPR_GetVer
Description:	��ȡ��̬���ӿ�İ汾�źͳ�����Ϣ
Input:     lpszVer����̬���ӿ�汾�ţ�����ǿ�ָ�룬����д��20�ֽڣ�
		   lpszVender���������ƣ�����ǿ�ָ�룬����д��50�ֽڣ�
		   lpszDevType���豸�ͺţ�����ǿ�ָ�룬����д��50�ֽڣ�
Output:    lpszVer����̬���ӿ�汾�ţ�����ǿ�ָ�룬����д��20�ֽڣ�
		   lpszVender���������ƣ�����ǿ�ָ�룬����д��50�ֽڣ�
		   lpszDevType���豸�ͺţ�����ǿ�ָ�룬��
Return:	    0-�ɹ�������-ʧ�ܣ�
******************************************/
int VPR_GetVer(char *lpszVer, char *lpszVender, char *lpszDevType);


/******************************************
Func:VPR_RegisterCallback
Description:	���ûص����� 
Input:      func:����ʶ�����ص�����
Output:      ��
Return:		��
******************************************/
int VPR_RegisterCallback(VPR_NotifyFunc func);

/******************************************
Func:myVideoInit
Description:	��ʼ����Ƶ���� 
Input:      left����Ƶ���ھ�����Ļ��߿�ľ��룬��λ��px;
			top����Ƶ���ھ�����Ļ���߿�ľ��룬��λ��px;
			width����Ƶ���ڵĿ�ȣ�
			height����Ƶ���ڵĸ߶�

Return:		0����ɹ����������������룻
******************************************/
int  myVideoInit (int left,int top,int width,int height);

/******************************************
Func:myVideoStart
Description:	������Ƶ���ڵĴ�С
Input:      left����Ƶ���ھ�����Ļ��߿�ľ��룬��λ��px;
			top����Ƶ���ھ�����Ļ���߿�ľ��룬��λ��px;
			width����Ƶ���ڵĿ�ȣ�
			height����Ƶ���ڵĸ߶�

Return:		0����ɹ����������������룻
******************************************/
int    myVideoStart( int left,int top,int width,int height);

/******************************************
Func:myVideoInitCapture
Description: ��Ƶ����ץ��ͼƬ
Input:      fileName���ļ���
			quality��ץ��ͼƬ����0-100��Խ��Խ�ã�


Return:		0����ɹ����������������룻
******************************************/
int  myVideoInitCapture (char * fileName, int quality);

/******************************************
Func:Vdm_Init
Description: �������ĳ�ʼ����
Input:     
Return:		0����ɹ����������������룻
******************************************/
int Vdm_Init();

/******************************************
Func:Vdm_line
Description: �����ַ�
Input:      Line���кţ��к����϶��´���������кŴ����Ҵ��������
			Coloum���кţ��Ӵ��п�ʼ����һ���ַ���
			Nlen:	    ����,�������ַ��ĳ��ȣ�
			VdmChar:	���ݣ��������ַ��������ݣ�
Return:	    0����ɹ����������������룻
******************************************/
int Vdm_line (unsigned char line , unsigned char coloum, unsigned char nlen, char *VdmChar);

 
/******************************************
Func:Sync_Vdm_Time
Description: ���������ӵ�ǰʱ��

Return:		Sync_Vdm_Time
******************************************/
int  Sync_Vdm_Time();

/******************************************
Func:Vdm_Close
Description: �ر��ַ�������

����ֵ��0����ɹ����������������룻

******************************************/
int  Vdm_Close();
#endif