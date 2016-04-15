#ifndef _VPR_PROJECT_H_
#define _VPR_PROJECT_H_


#define NET_DVR_NOERROR 					0	//没有错误
#define NET_DVR_PASSWORD_ERROR 				1	//用户名密码错误
#define NET_DVR_NOENOUGHPRI 				2	//权限不足
#define NET_DVR_NOINIT 						3	//没有初始化
#define NET_DVR_CHANNEL_ERROR 				4	//通道号错误
#define NET_DVR_OVER_MAXLINK 				5	//连接到DVR的客户端个数超过最大
#define NET_DVR_VERSIONNOMATCH				6	//版本不匹配
#define NET_DVR_NETWORK_FAIL_CONNECT		7	//连接服务器失败
#define NET_DVR_NETWORK_SEND_ERROR			8	//向服务器发送失败
#define NET_DVR_NETWORK_RECV_ERROR			9	//从服务器接收数据失败
#define NET_DVR_NETWORK_RECV_TIMEOUT		10	//从服务器接收数据超时
#define NET_DVR_NETWORK_ERRORDATA			11	//传送的数据有误
#define NET_DVR_ORDER_ERROR					12	//调用次序错误
#define NET_DVR_OPERNOPERMIT				13	//无此权限
#define NET_DVR_COMMANDTIMEOUT				14	//DVR命令执行超时
#define NET_DVR_ERRORSERIALPORT				15	//串口号错误
#define NET_DVR_ERRORALARMPORT				16	//报警端口错误
#define NET_DVR_PARAMETER_ERROR 			17  //参数错误
#define NET_DVR_CHAN_EXCEPTION				18	//服务器通道处于错误状态
#define NET_DVR_NODISK						19	//没有硬盘
#define NET_DVR_ERRORDISKNUM				20	//硬盘号错误
#define NET_DVR_DISK_FULL					21	//服务器硬盘满
#define NET_DVR_DISK_ERROR					22	//服务器硬盘出错
#define NET_DVR_NOSUPPORT					23	//服务器不支持
#define NET_DVR_BUSY						24	//服务器忙
#define NET_DVR_MODIFY_FAIL					25	//服务器修改不成功
#define NET_DVR_PASSWORD_FORMAT_ERROR		26	//密码输入格式不正确
#define NET_DVR_DISK_FORMATING				27	//硬盘正在格式化，不能启动操作
#define NET_DVR_DVRNORESOURCE				28	//DVR资源不足
#define	NET_DVR_DVROPRATEFAILED				29  //DVR操作失败
#define NET_DVR_OPENHOSTSOUND_FAIL 			30  //打开PC声音失败
#define NET_DVR_DVRVOICEOPENED 				31  //服务器语音对讲被占用
#define	NET_DVR_TIMEINPUTERROR				32  //时间输入不正确
#define	NET_DVR_NOSPECFILE					33  //回放时服务器没有指定的文件
#define NET_DVR_CREATEFILE_ERROR			34	//创建文件出错
#define	NET_DVR_FILEOPENFAIL				35  //打开文件出错
#define	NET_DVR_OPERNOTFINISH				36  //上次的操作还没有完成
#define	NET_DVR_GETPLAYTIMEFAIL				37  //获取当前播放的时间出错
#define	NET_DVR_PLAYFAIL					38  //播放出错
#define NET_DVR_FILEFORMAT_ERROR			39  //文件格式不正确
#define NET_DVR_DIR_ERROR					40	//路径错误
#define NET_DVR_ALLOC_RESOURCE_ERROR		41  //资源分配错误
#define NET_DVR_AUDIO_MODE_ERROR			42	//声卡模式错误
#define NET_DVR_NOENOUGH_BUF				43	//缓冲区太小
#define NET_DVR_CREATESOCKET_ERROR		 	44	//创建SOCKET出错
#define NET_DVR_SETSOCKET_ERROR				45	//设置SOCKET出错
#define NET_DVR_MAX_NUM						46	//个数达到最大
#define NET_DVR_USERNOTEXIST				47	//用户不存在
#define NET_DVR_WRITEFLASHERROR				48  //写FLASH出错
#define NET_DVR_UPGRADEFAIL					49  //DVR升级失败
#define NET_DVR_CARDHAVEINIT				50  //解码卡已经初始化过
#define NET_DVR_PLAYERFAILED				51	//调用播放库中某个函数失败
#define NET_DVR_MAX_USERNUM					52  //设备端用户数达到最大
#define NET_DVR_GETLOCALIPANDMACFAIL		53  //获得客户端的IP地址或物理地址失败
#define NET_DVR_NOENCODEING					54	//该通道没有编码
#define NET_DVR_IPMISMATCH					55	//IP地址不匹配
#define NET_DVR_MACMISMATCH					56	//MAC地址不匹配
#define NET_DVR_UPGRADELANGMISMATCH			57	//升级文件语言不匹配
#define NET_DVR_MAX_PLAYERPORT				58	//播放器路数达到最大
#define NET_DVR_NOSPACEBACKUP				59	//备份设备中没有足够空间进行备份
#define NET_DVR_NODEVICEBACKUP				60	//没有找到指定的备份设备
#define NET_DVR_PICTURE_BITS_ERROR			61	//图像素位数不符，限24色
#define NET_DVR_PICTURE_DIMENSION_ERROR		62	//图片高*宽超限， 限128*256
#define NET_DVR_PICTURE_SIZ_ERROR			63	//图片大小超限，限100K
#define NET_DVR_LOADPLAYERSDKFAILED			64	//载入当前目录下Player Sdk出错
#define NET_DVR_LOADPLAYERSDKPROC_ERROR		65	//找不到Player Sdk中某个函数入口
#define NET_DVR_LOADDSSDKFAILED				66	//载入当前目录下DSsdk出错
#define NET_DVR_LOADDSSDKPROC_ERROR		    67	//找不到DsSdk中某个函数入口
#define NET_DVR_DSSDK_ERROR					68	//调用硬解码库DsSdk中某个函数失败
#define NET_DVR_VOICEMONOPOLIZE				69	//声卡被独占
#define NET_DVR_JOINMULTICASTFAILED			70	//加入多播组失败
#define NET_DVR_CREATEDIR_ERROR				71	//建立日志文件目录失败
#define NET_DVR_BINDSOCKET_ERROR			72	//绑定套接字失败
#define NET_DVR_SOCKETCLOSE_ERROR			73	//socket连接中断，此错误通常是由于连接中断或目的地不可达
#define NET_DVR_USERID_ISUSING			    74	//注销时用户ID正在进行某操作
#define NET_DVR_SOCKETLISTEN_ERROR			75	//监听失败
#define NET_DVR_PROGRAM_EXCEPTION			76	//程序异常
#define NET_DVR_WRITEFILE_FAILED			77	//写文件失败
#define NET_DVR_FORMAT_READONLY				78  //禁止格式化只读硬盘
#define NET_DVR_WITHSAMEUSERNAME		    79  //用户配置结构中存在相同的用户名
#define NET_DVR_DEVICETYPE_ERROR            80  /*导入参数时设备型号不匹配*/
#define NET_DVR_LANGUAGE_ERROR              81  /*导入参数时语言不匹配*/
#define NET_DVR_PARAVERSION_ERROR           82  /*导入参数时软件版本不匹配*/
#define NET_DVR_IPCHAN_NOTALIVE             83  /*预览时外接IP通道不在线*/
#define NET_DVR_RTSP_SDK_ERROR				84	/*加载高清IPC通讯库StreamTransClient.dll失败*/
#define NET_DVR_CONVERT_SDK_ERROR			85	/*加载转码库失败*/
#define NET_DVR_IPC_COUNT_OVERFLOW			86  /*超出最大的ip接入通道数*/
#define NET_DVR_MAX_ADD_NUM                 87  /*添加标签(一个文件片段64)等个数达到最大*/
#define NET_DVR_PARAMMODE_ERROR             88 //图像增强仪，参数模式错误（用于硬件设置时，客户端进行软件设置时错误值）
#define NET_DVR_CODESPITTER_OFFLINE			89 //视频综合平台，码分器不在线
#define NET_DVR_BACKUP_COPYING				90  //设备正在备份
#define NET_DVR_CHAN_NOTSUPPORT             91  // 通道不支持该操作
#define NET_DVR_CALLINEINVALID              92  // 高度线位置太集中或长度线不够倾斜 
#define NET_DVR_CALCANCELCONFLICT           93  // 取消标定冲突，如果设置了规则及全局的实际大小尺寸过滤
#define NET_DVR_CALPOINTOUTRANGE            94 	// 标定点超出范围 
#define NET_DVR_FILTERRECTINVALID           95  // 尺寸过滤器不符合要求
#define NET_DVR_DDNS_DEVOFFLINE             96  //设备没有注册到ddns上
#define NET_DVR_DDNS_INTER_ERROR            97  //DDNS 服务器内部错误
#define NET_DVR_FUNCTION_NOT_SUPPORT_OS     98 //此功能不支持该操作系统
#define NET_DVR_DEC_CHAN_REBIND             99 //解码通道绑定显示输出次数受限
#define NET_DVR_INTERCOM_SDK_ERROR          100 //加载当前目录下的语音对讲库失败  
#define NET_DVR_ALIAS_DUPLICATE				150	//别名重复  //2011-08-31 通过别名或者序列号来访问设备的新版本ddns的配置

#define NET_DVR_FINDWITHINDEX_FAILED        151 //查找失败
// 2010-5-28
// 阵列错误码
#define RAID_ERROR_INDEX	                200
#define NET_DVR_NAME_NOT_ONLY               (RAID_ERROR_INDEX + 0)  // 名称已存在
#define NET_DVR_OVER_MAX_ARRAY              (RAID_ERROR_INDEX + 1 ) // 阵列达到上限
#define NET_DVR_OVER_MAX_VD                 (RAID_ERROR_INDEX + 2 ) // 虚拟磁盘达到上限
#define NET_DVR_VD_SLOT_EXCEED              (RAID_ERROR_INDEX + 3 ) // 虚拟磁盘槽位已满
#define NET_DVR_PD_STATUS_INVALID           (RAID_ERROR_INDEX + 4 ) // 重建阵列所需物理磁盘状态错误
#define NET_DVR_PD_BE_DEDICATE_SPARE        (RAID_ERROR_INDEX + 5 ) // 重建阵列所需物理磁盘为指定热备
#define NET_DVR_PD_NOT_FREE                 (RAID_ERROR_INDEX + 6 ) // 重建阵列所需物理磁盘非空闲
#define NET_DVR_CANNOT_MIG2NEWMODE          (RAID_ERROR_INDEX + 7 ) // 不能从当前的阵列类型迁移到新的阵列类型
#define NET_DVR_MIG_PAUSE                   (RAID_ERROR_INDEX + 8 ) // 迁移操作已暂停
#define NET_DVR_MIG_CANCEL                  (RAID_ERROR_INDEX + 9 ) // 正在执行的迁移操作已取消
#define NET_DVR_EXIST_VD                    (RAID_ERROR_INDEX + 10) // 阵列上阵列上存在虚拟磁盘，无法删除阵列
#define NET_DVR_TARGET_IN_LD_FUNCTIONAL     (RAID_ERROR_INDEX + 11) // 对象物理磁盘为虚拟磁盘组成部分且工作正常
#define NET_DVR_HD_IS_ASSIGNED_ALREADY      (RAID_ERROR_INDEX + 12) // 指定的物理磁盘被分配为虚拟磁盘
#define NET_DVR_INVALID_HD_COUNT            (RAID_ERROR_INDEX + 13) // 物理磁盘数量与指定的RAID等级不匹配
#define NET_DVR_LD_IS_FUNCTIONAL            (RAID_ERROR_INDEX + 14) // 阵列正常，无法重建
#define NET_DVR_BGA_RUNNING                 (RAID_ERROR_INDEX + 15) // 存在正在执行的后台任务
#define NET_DVR_LD_NO_ATAPI                 (RAID_ERROR_INDEX + 16) // 无法用ATAPI盘创建虚拟磁盘
#define NET_DVR_MIGRATION_NOT_NEED          (RAID_ERROR_INDEX + 17) // 阵列无需迁移
#define NET_DVR_HD_TYPE_MISMATCH            (RAID_ERROR_INDEX + 18) // 物理磁盘不属于同意类型
#define NET_DVR_NO_LD_IN_DG                 (RAID_ERROR_INDEX + 19) // 无虚拟磁盘，无法进行此项操作
#define NET_DVR_NO_ROOM_FOR_SPARE           (RAID_ERROR_INDEX + 20) // 磁盘空间过小，无法被指定为热备盘
#define NET_DVR_SPARE_IS_IN_MULTI_DG        (RAID_ERROR_INDEX + 21) // 磁盘已被分配为某阵列热备盘
#define NET_DVR_DG_HAS_MISSING_PD           (RAID_ERROR_INDEX + 22) // 阵列缺少盘

// x86 64bit nvr新增 2012-02-04
#define NET_DVR_NAME_EMPTY					(RAID_ERROR_INDEX + 23) /*名称为空*/
#define NET_DVR_INPUT_PARAM					(RAID_ERROR_INDEX + 24) /*输入参数有误*/
#define NET_DVR_PD_NOT_AVAILABLE			(RAID_ERROR_INDEX + 25) /*物理磁盘不可用*/
#define NET_DVR_ARRAY_NOT_AVAILABLE			(RAID_ERROR_INDEX + 26) /*阵列不可用*/
#define NET_DVR_PD_COUNT					(RAID_ERROR_INDEX + 27) /*物理磁盘数不正确*/
#define NET_DVR_VD_SMALL					(RAID_ERROR_INDEX + 28) /*虚拟磁盘太小*/
#define NET_DVR_NO_EXIST					(RAID_ERROR_INDEX + 29) /*不存在*/
#define NET_DVR_NOT_SUPPORT					(RAID_ERROR_INDEX + 30) /*不支持该操作*/
#define NET_DVR_NOT_FUNCTIONAL	 			(RAID_ERROR_INDEX + 31) /*阵列状态不是正常状态*/
#define NET_DVR_DEV_NODE_NOT_FOUND			(RAID_ERROR_INDEX + 32) /*虚拟磁盘设备节点不存在*/
#define NET_DVR_SLOT_EXCEED					(RAID_ERROR_INDEX + 33) /*槽位达到上限*/ 
#define NET_DVR_NO_VD_IN_ARRAY				(RAID_ERROR_INDEX + 34) /*阵列上不存在虚拟磁盘*/
#define NET_DVR_VD_SLOT_INVALID				(RAID_ERROR_INDEX + 35) /*虚拟磁盘槽位无效*/
#define NET_DVR_PD_NO_ENOUGH_SPACE			(RAID_ERROR_INDEX + 36) /*所需物理磁盘空间不足*/
#define NET_DVR_ARRAY_NONFUNCTION			(RAID_ERROR_INDEX + 37) /*只有处于正常状态的阵列才能进行迁移*/
#define NET_DVR_ARRAY_NO_ENOUGH_SPACE		(RAID_ERROR_INDEX + 38) /*阵列空间不足*/
#define NET_DVR_STOPPING_SCANNING_ARRAY		(RAID_ERROR_INDEX + 39) /*正在执行安全拔盘或重新扫描*/
#define NET_DVR_NOT_SUPPORT_16T             (RAID_ERROR_INDEX + 40) /*不支持创建大于16T的阵列*/


// 智能错误码 
#define VCA_ERROR_INDEX                     300 // 智能错误码索引
#define NET_DVR_ID_ERROR                    (VCA_ERROR_INDEX + 0)   // 配置ID不合理
#define NET_DVR_POLYGON_ERROR               (VCA_ERROR_INDEX + 1)   // 多边形不符合要求
#define NET_DVR_RULE_PARAM_ERROR            (VCA_ERROR_INDEX + 2)   // 规则参数不合理
#define NET_DVR_RULE_CFG_CONFLICT           (VCA_ERROR_INDEX + 3)   // 配置信息冲突
#define NET_DVR_CALIBRATE_NOT_READY         (VCA_ERROR_INDEX + 4)   // 当前没有标定信息
#define NET_DVR_CAMERA_DATA_ERROR           (VCA_ERROR_INDEX + 5)   // 摄像机参数不合理
#define NET_DVR_CALIBRATE_DATA_UNFIT        (VCA_ERROR_INDEX + 6)	// 长度不够倾斜，不利于标定
#define NET_DVR_CALIBRATE_DATA_CONFLICT     (VCA_ERROR_INDEX + 7)	// 标定出错，以为所有点共线或者位置太集中
#define NET_DVR_CALIBRATE_CALC_FAIL         (VCA_ERROR_INDEX + 8)	// 摄像机标定参数值计算失败
#define	NET_DVR_CALIBRATE_LINE_OUT_RECT		(VCA_ERROR_INDEX + 9)	// 输入的样本标定线超出了样本外接矩形框
#define NET_DVR_ENTER_RULE_NOT_READY		(VCA_ERROR_INDEX + 10)	// 没有设置进入区域
#define NET_DVR_AID_RULE_NO_INCLUDE_LANE	(VCA_ERROR_INDEX + 11)	// 交通事件规则中没有包括车道（特值拥堵和逆行）
#define NET_DVR_LANE_NOT_READY				(VCA_ERROR_INDEX + 12)	// 当前没有设置车道
#define NET_DVR_RULE_INCLUDE_TWO_WAY		(VCA_ERROR_INDEX + 13)	// 事件规则中包含2种不同方向
#define NET_DVR_LANE_TPS_RULE_CONFLICT      (VCA_ERROR_INDEX + 14)  // 车道和数据规则冲突
#define NET_DVR_NOT_SUPPORT_EVENT_TYPE      (VCA_ERROR_INDEX + 15)  // 不支持的事件类型
#define NET_DVR_LANE_NO_WAY                 (VCA_ERROR_INDEX + 16)  // 车道没有方向
#define NET_DVR_SIZE_FILTER_ERROR           (VCA_ERROR_INDEX + 17)  // 尺寸过滤框不合理
#define NET_DVR_LIB_FFL_NO_FACE             (VCA_ERROR_INDEX + 18) // 特征点定位时输入的图像没有人脸
#define NET_DVR_LIB_FFL_IMG_TOO_SMALL       (VCA_ERROR_INDEX + 19) // 特征点定位时输入的图像太小
#define NET_DVR_LIB_FD_IMG_NO_FACE          (VCA_ERROR_INDEX + 20) // 单张图像人脸检测时输入的图像没有人脸
#define NET_DVR_LIB_FACE_TOO_SMALL          (VCA_ERROR_INDEX + 21) // 建模时人脸太小
#define NET_DVR_LIB_FACE_QUALITY_TOO_BAD    (VCA_ERROR_INDEX + 22) // 建模时人脸图像质量太差
#define NET_DVR_KEY_PARAM_ERR               (VCA_ERROR_INDEX + 23) //高级参数设置错误
#define NET_DVR_CALIBRATE_DATA_ERR          (VCA_ERROR_INDEX + 24) //标定样本数目错误，或数据值错误，或样本点超出地平线
#define NET_DVR_CALIBRATE_DISABLE_FAIL      (VCA_ERROR_INDEX + 25) //所配置规则不允许取消标定
#define NET_DVR_VCA_LIB_FD_SCALE_OUTRANGE   (VCA_ERROR_INDEX + 26) //最大过滤框的宽高最小值超过最小过滤框的宽高最大值两倍以上
#define NET_DVR_LIB_FD_REGION_TOO_LARGE     (VCA_ERROR_INDEX + 27) //当前检测区域范围过大。检测区最大为图像的2/3
#define NET_DVR_TRIAL_OVERDUE               (VCA_ERROR_INDEX + 28) //试用版评估期已结束
#define NET_DVR_CONFIG_FILE_CONFLICT        (VCA_ERROR_INDEX + 29) //设备类型与配置文件冲突（加密狗类型与现有分析仪配置不符错误码提示）
//算法库相关错误码
#define NET_DVR_FR_FPL_FAIL                 (VCA_ERROR_INDEX + 30)   // 人脸特征点定位失败
#define NET_DVR_FR_IQA_FAIL                 (VCA_ERROR_INDEX + 31)   // 人脸评分失败
#define NET_DVR_FR_FEM_FAIL                 (VCA_ERROR_INDEX + 32)   // 人脸特征提取失败
#define NET_DVR_FPL_DT_CONF_TOO_LOW         (VCA_ERROR_INDEX + 33)   // 特征点定位时人脸检测置信度过低
#define NET_DVR_FPL_CONF_TOO_LOW            (VCA_ERROR_INDEX + 34)   // 特征点定位置信度过低
#define NET_DVR_E_DATA_SIZE                 (VCA_ERROR_INDEX + 35)  // 数据长度不匹配
#define NET_DVR_FR_MODEL_VERSION_ERR        (VCA_ERROR_INDEX + 36)  // 人脸模型数据中的模型版本错误
#define NET_DVR_FR_FD_FAIL                  (VCA_ERROR_INDEX + 37)  // 识别库中人脸检测失败
#define NET_DVR_FA_NORMALIZE_ERR            (VCA_ERROR_INDEX + 38)  // 人脸归一化出错
//其他错误码
#define NET_DVR_DOG_PUSTREAM_NOT_MATCH      (VCA_ERROR_INDEX + 39)  // 加密狗与前端取流设备类型不匹配
#define NET_DVR_DEV_PUSTREAM_NOT_MATCH      (VCA_ERROR_INDEX + 40)  // 前端取流设备版本不匹配
#define NET_DVR_PUSTREAM_ALREADY_EXISTS     (VCA_ERROR_INDEX + 41)  // 设备的其他通道已经添加过该前端设备
#define NET_DVR_SEARCH_CONNECT_FAILED       (VCA_ERROR_INDEX + 42)  // 连接检索服务器失败
#define NET_DVR_INSUFFICIENT_DISK_SPACE     (VCA_ERROR_INDEX + 43)  // 可存储的硬盘空间不足
#define NET_DVR_DATABASE_CONNECTION_FAILED  (VCA_ERROR_INDEX + 44)  // 数据库连接失败
#define NET_DVR_DATABASE_ADM_PW_ERROR       (VCA_ERROR_INDEX + 45)  // 数据库用户名、密码错误
#define NET_DVR_DECODE_YUV                  (VCA_ERROR_INDEX + 46)  // 解码失败

#define NET_DVR_RTSP_ERROR_NOENOUGHPRI          401  //无权限：服务器返回401时，转成这个错误码
#define NET_DVR_RTSP_ERROR_ALLOC_RESOURCE       402  //分配资源失败
#define NET_DVR_RTSP_ERROR_PARAMETER            403  //参数错误
#define NET_DVR_RTSP_ERROR_NO_URL               404  //指定的URL地址不存在：服务器返回404时，转成这个错误码
#define NET_DVR_RTSP_ERROR_FORCE_STOP           406  //用户中途强行退出

#define NET_DVR_RTSP_GETPORTFAILED			    407  //rtsp 得到端口错误
#define NET_DVR_RTSP_DESCRIBERROR			    410  //rtsp decribe 交互错误
#define NET_DVR_RTSP_DESCRIBESENDTIMEOUT		411  //rtsp decribe 发送超时
#define NET_DVR_RTSP_DESCRIBESENDERROR			412  //rtsp decribe 发送失败
#define NET_DVR_RTSP_DESCRIBERECVTIMEOUT		413  //rtsp decribe 接收超时
#define NET_DVR_RTSP_DESCRIBERECVDATALOST		414  //rtsp decribe 接收数据错误
#define NET_DVR_RTSP_DESCRIBERECVERROR			415  //rtsp decribe 接收失败
#define NET_DVR_RTSP_DESCRIBESERVERERR			416  //rtsp decribe 服务器返回401:用户名密码错误或者无权限,或者 501:设备内部资源分配失败 

#define NET_DVR_RTSP_SETUPERROR			        420  //rtsp setup 交互错误
#define NET_DVR_RTSP_SETUPSENDTIMEOUT			421  //rtsp setup 发送超时
#define NET_DVR_RTSP_SETUPSENDERROR				422  //rtsp setup 发送错误
#define NET_DVR_RTSP_SETUPRECVTIMEOUT			423  //rtsp setup 接收超时
#define NET_DVR_RTSP_SETUPRECVDATALOST			424  //rtsp setup 接收数据错误
#define NET_DVR_RTSP_SETUPRECVERROR				425  //rtsp setup 接收失败
#define NET_DVR_RTSP_OVER_MAX_CHAN				426  //rtsp setup 服务器返回401,501等错误,超过最大连接数

#define NET_DVR_RTSP_PLAYERROR			        430  //rtsp play 交互错误
#define NET_DVR_RTSP_PLAYSENDTIMEOUT			431  //rtsp play 发送超时
#define NET_DVR_RTSP_PLAYSENDERROR				432  //rtsp play 发送错误
#define NET_DVR_RTSP_PLAYRECVTIMEOUT			433  //rtsp play 接收超时
#define NET_DVR_RTSP_PLAYRECVDATALOST			434  //rtsp play 接收数据错误
#define NET_DVR_RTSP_PLAYRECVERROR				435  //rtsp play 接收失败
#define NET_DVR_RTSP_PLAYSERVERERR				436  //rtsp play 服务器返回401,501等错误

#define NET_DVR_RTSP_TEARDOWNERROR			    440  //rtsp teardown 交互错误
#define NET_DVR_RTSP_TEARDOWNSENDTIMEOUT		441  //rtsp teardown 发送超时
#define NET_DVR_RTSP_TEARDOWNSENDERROR			442  //rtsp teardown 发送错误
#define NET_DVR_RTSP_TEARDOWNRECVTIMEOUT		443  //rtsp teardown 接收超时
#define NET_DVR_RTSP_TEARDOWNRECVDATALOST		444  //rtsp teardown 接收数据错误
#define NET_DVR_RTSP_TEARDOWNRECVERROR			445  //rtsp teardown 接收失败
#define NET_DVR_RTSP_TEARDOWNSERVERERR			446  //rtsp teardown 服务器返回401,501等错误

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

#define NET_ERROR_TRUNK_LINE						711 //子系统已被配成干线
#define NET_ERROR_MIXED_JOINT						712 //不能进行混合拼接
#define NET_ERROR_DISPLAY_SWITCH					713 //不能进行显示通道切换
#define NET_ERROR_USED_BY_BIG_SCREEN				714 //解码资源被大屏占用
#define NET_ERROR_USE_OTHER_DEC_RESOURCE			715 //不能使用其他解码子系统资源
#define NET_ERROR_DISP_MODE_SWITCH                  716 //显示通道显示状态切换中
#define NET_ERROR_SCENE_USING						717	//场景正在使用

#define NET_DVR_DEV_NET_OVERFLOW		            800	//网络流量超过设备能力上限
#define NET_DVR_STATUS_RECORDFILE_WRITING_NOT_LOCK  801 //录像文件在录像，无法被锁定 
#define NET_DVR_STATUS_CANT_FORMAT_LITTLE_DISK      802 //由于硬盘太小无法格式化

//N+1错误码
#define NET_SDK_ERR_REMOTE_DISCONNECT				803 //远端无法连接
#define NET_SDK_ERR_RD_ADD_RD						804 //备机不能添加备机
#define NET_SDK_ERR_BACKUP_DISK_EXCEPT				805 //备份盘异常
#define NET_SDK_ERR_RD_LIMIT						806 //备机数已达上限
#define NET_SDK_ERR_ADDED_RD_IS_WD					807 //添加的备机是工作机
#define NET_SDK_ERR_ADD_ORDER_WRONG					808 //添加顺序出错，比如没有被工作机添加为备机，就添加工作机
#define NET_SDK_ERR_WD_ADD_WD						809 //工作机不能添加工作机
#define NET_SDK_ERR_WD_SERVICE_EXCETP				810 //工作机CVR服务异常
#define NET_SDK_ERR_RD_SERVICE_EXCETP				811 //备机CVR服务异常
#define NET_SDK_ERR_ADDED_WD_IS_RD					812 //添加的工作机是备机
#define NET_SDK_ERR_PERFORMANCE_LIMIT				813 //性能达到上限
#define NET_SDK_ERR_ADDED_DEVICE_EXIST				814 //添加的设备已经存在



//2011-10-25多屏控制器错误码（900-950）
#define  NET_ERR_WINCHAN_IDX				        901	// 开窗通道号错误
#define  NET_ERR_WIN_LAYER						    902	// 窗口层数错误，单个屏幕上最多覆盖的窗口层数
#define  NET_ERR_WIN_BLK_NUM			            903	// 窗口的块数错误，单个窗口可覆盖的屏幕个数
#define  NET_ERR_OUTPUT_RESOLUTION	                904	// 输出分辨率错误
#define  NET_ERR_LAYOUT							    905	// 布局号错误
#define  NET_ERR_INPUT_RESOLUTION    	            906 // 输入分辨率不支持
#define  NET_ERR_SUBDEVICE_OFFLINE                  907 // 子设备不在线
#define  NET_ERR_NO_DECODE_CHAN                     908 // 没有空闲解码通道
#define  NET_ERR_MAX_WINDOW_ABILITY                 909 // 开窗能力上限
#define  NET_ERR_ORDER_ERROR                        910 // 调用顺序有误
#define  NET_ERR_PLAYING_PLAN    					911 //正在执行预案
#define  NET_ERR_DECODER_USED						912 //解码板正在使用
#define	 NET_ERR_OUTPUT_BOARD_DATA_OVERFLOW			913	// 输出板数据量超限
#define	 NET_ERR_SAME_USER_NAME						914	// 用户名相同
#define	 NET_ERR_INVALID_USER_NAME					915	// 无效用户名
#define	 NET_ERR_MATRIX_USING						916	// 输入矩阵正在使用
#define	 NET_ERR_DIFFERENT_CHAN_TYPE				917	// 通道类型不同（矩阵输出通道和控制器的输入为不同的类型）
#define	 NET_ERR_INPUT_CHAN_BINDED					918	// 输入通道已经被其他矩阵绑定
#define	 NET_ERR_BINDED_OUTPUT_CHAN_OVERFLOW		919	// 正在使用的矩阵输出通道个数超过矩阵与控制器绑定的通道个数
#define	 NET_ERR_MAX_SIGNAL_NUM						920	// 输入信号源个数达到上限
#define  NET_ERR_INPUT_CHAN_USING					921	// 输入通道正在使用
#define  NET_ERR_MANAGER_LOGON 					    922	//管理员已经登陆，操作失败
#define  NET_ERR_USERALREADY_LOGON 				    923	//该用户已经登陆，操作失败
#define  NET_ERR_LAYOUT_INIT 						924	//布局正在初始化，操作失败
#define	 NET_ERR_BASEMAP_SIZE_NOT_MATCH				925	//底图大小不符 

//解码器错误码（951-999）
#define  NET_ERR_MAX_WIN_OVERLAP                    951 //达到最大窗口重叠数
//能力集解析库错误码
#define XML_ABILITY_NOTSUPPORT                      1000  //不支持能力节点获取
#define	XML_ANALYZE_NOENOUGH_BUF				    1001		//输出内存不足
#define	XML_ANALYZE_FIND_LOCALXML_ERROR			    1002		//无法找到对应的本地xml
#define	XML_ANALYZE_LOAD_LOCALXML_ERROR			    1003		//加载本地xml出错
#define	XML_NANLYZE_DVR_DATA_FORMAT_ERROR		    1004		//设备能力数据格式错误
#define	XML_ANALYZE_TYPE_ERROR					    1005		//能力集类型错误
#define	XML_ANALYZE_XML_NODE_ERROR				    1006		//XML能力节点格式错误
#define XML_INPUT_PARAM_ERROR                       1007  //输入的能力XML节点值错误

//2012-10-16 报警设备错误码（1200~1300）
#define NET_ERR_SEARCHING_MODULE                    1201 // 正在搜索外接模块
#define NET_ERR_REGISTERING_MODULE                  1202 // 正在注册外接模块
#define NET_ERR_GETTING_ZONES						1203 // 正在获取防区参数
#define NET_ERR_GETTING_TRIGGERS					1204 // 正在获取触发器
#define NET_ERR_ARMED_STATUS						1205 // 系统处于布防状态
#define	NET_ERR_PROGRAM_MODE_STATUS					1206 // 系统处于编程模式
#define	NET_ERR_WALK_TEST_MODE_STATUS				1207 // 系统处于步测模式
#define	NET_ERR_BYPASS_STATUS						1208 // 旁路状态
#define NET_ERR_DISABLED_MODULE_STATUS				1209 // 功能未使能
#define	NET_ERR_NOT_SUPPORT_OPERATE_ZONE			1210 // 防区不支持该操作
#define NET_ERR_NOT_SUPPORT_MOD_MODULE_ADDR			1211 // 模块地址不能被修改
#define NET_ERR_UNREGISTERED_MODULE					1212 // 模块未注册
#define NET_ERR_PUBLIC_SUBSYSTEM_ASSOCIATE_SELF		1213 // 公共子系统关联自身
#define NET_ERR_EXCEEDS_ASSOCIATE_SUBSYSTEM_NUM		1214 // 超过公共子系统最大关联个数
#define NET_ERR_BE_ASSOCIATED_BY_PUBLIC_SUBSYSTEM	1215 // 子系统被其他公共子系统关联
#define	NET_ERR_ZONE_FAULT_STATUS					1216 // 防区处于故障状态
#define  NET_ERR_SAME_EVENT_TYPE					1217 //事件触发报警输出开启和事件触发报警输出关闭中有相同事件类型	
#define NET_ERR_ZONE_ALARM_STATUS					1218 //防区处于报警状态
#define NET_ERR_EXPANSION_BUS_SHORT_CIRCUIT			1219 //扩展总线短路

//2012-12-20抓拍机错误码（1400-1499）
#define NET_DVR_ERR_LANENUM_EXCEED                  1400  //车道数超出能力
#define NET_DVR_ERR_PRAREA_EXCEED                   1401  //牌识区域过大
#define NET_DVR_ERR_LIGHT_PARAM                     1402  //信号灯接入参数错误
#define NET_DVR_ERR_LANE_LINE_INVALID               1403  //车道线配置错误
#define NET_DVR_ERR_STOP_LINE_INVALID               1404  //停止线配置错误
#define NET_DVR_ERR_LEFTORRIGHT_LINE_INVALID        1405  //左/右转分界线配置错误
#define NET_DVR_ERR_LANE_NO_REPEAT                  1406  //叠加车道号重复
#define NET_DVR_ERR_PRAREA_INVALID                  1407  //牌识多边形不符合要求
#define NET_DVR_ERR_LIGHT_NUM_EXCEED                1408  //视频检测交通灯信号灯数目超出最大值
#define NET_DVR_ERR_SUBLIGHT_NUM_INVALID            1409  //视频检测交通灯信号灯子灯数目不合法
#define NET_DVR_ERR_LIGHT_AREASIZE_INVALID          1410  //视频检测交通灯输入信号灯框大小不合法
#define NET_DVR_ERR_LIGHT_COLOR_INVALID             1411  //视频检测交通灯输入信号灯颜色不合法
#define NET_DVR_ERR_LIGHT_DIRECTION_INVALID         1412  //视频检测交通灯输入灯方向属性不合法

//2013-3-6VQD错误码（1500～1550）
#define NET_ERR_VQD_TIME_CONFLICT					1500 //VQD诊断时间段冲突
#define NET_ERR_VQD_PLAN_NO_EXIST					1501 //VQD诊断计划不存在
#define NET_ERR_VQD_CHAN_NO_EXIST					1502 //VQD监控点不存在
#define NET_ERR_VQD_CHAN_MAX						1503 //VQD计划数已达上限
#define NET_ERR_VQD_TASK_MAX						1504 //VQD任务数已达上限

/*******************全局错误码 end**********************/

/******************************************
Func:VPR_NotifyFunc
Description:	车牌识别事件回调函数定义 
Input:      uIndex：数据序号，用来保证车牌、图像的一致性，在取抓拍结果时传递给相应函数。
Output:      无
Return:		无
******************************************/
typedef void (__stdcall *VPR_NotifyFunc)(unsigned int uIndex);


/******************************************
Func:VPR_Connect
Description:	连接车牌识别系统 
Input:      lpszConnPara1：连接参数1。如果通过网络连接，连接参数的形式可能是IP地址加网络端口号，
                  如："192.168.0.1:3000"。如果通过串口连接，连接参数可能为端口号，如"COM1"或"1"。
			lpszConnPara2：连接参数2。一般为空字符串，仅用于需要配置额外参数的设备。
Output:      无
Return:		0-成功；其它-失败
******************************************/
	int VPR_Connect(const char* lpszConnPara1, const char* lpszConnPara2);


/******************************************
Func:VPR_Disconnect
Description:	断开与车牌识别系统的连接 
Input:      无
Output:     无
Return:		0-成功；其它-失败
******************************************/
int VPR_Disconnect();


/******************************************
Func:VPR_Capture
Description:主动触发车牌识别系统抓怕图像并识别车牌，函数为非阻塞方式，调用后立即返回，
            抓拍识别工作完成并准备好数据后通过消息的方式通知上层应用 
Input:      无
Output:      无
Return:		0-成功；其它-失败
******************************************/
int VPR_Capture();


/******************************************
Func:VPR_GetPlate
Description:	获取指定序号车牌颜色、车牌号。
				车牌颜色定义为：0-蓝牌；1-黄牌；2-黑牌；3-白牌；-1-无颜色
				车牌格式例如："鲁A12345"
Input:   nIndex：数据序号；
		 
Output:  nColor: 车牌颜色代码；
		 lpszPlate：车牌号，为0结束字符串，输出结果应在20字节以内，如未识别出车牌或车辆无车牌输出零长度字符串；
		 nRating：车牌的可信度，为整数，例如nRating=85，表示可信度为85%。
Return:		0-成功；其它-失败
******************************************/
int VPR_GetPlate(unsigned int nIndex, int &nColor, const char* lpszPlate, unsigned int &nRating);



/******************************************
Func:VPR_GetBigImg
Description:	获取指定序号的车辆大图 
Input:      nIndex：数据序号；
			lpImgBuf：图像数据缓冲区;
			nSize： 输入时为上层应用分配的缓冲区大小，输出时为图像实际大小；
					如图像大小大于缓冲区大小（或者lpImgBuf为空指针），输出图像实际大小
Output:      无
Return:		0-成功；1-图像大小大于缓冲区 2-无图像 其它-失败；
******************************************/
int VPR_GetBigImg(unsigned int nIndex, unsigned char* lpImgBuf, unsigned int &nSize);



/******************************************
Func:VPR_GetSmallImg
Description:	获取指定序号的车牌小图 
Input:      nIndex：数据序号；
			lpImgBuf：图像数据缓冲区;
			nSize： 输入时为上层应用分配的缓冲区大小，输出时为图像实际大小；
					如图像大小大于缓冲区大小（或者lpImgBuf为空指针），输出图像实际大小。
Output:      无
Return:		0-成功；1-图像大小大于缓冲区 2-无图像 其它-失败；
******************************************/
int VPR_GetSmallImg(unsigned nIndex, unsigned char* lpImgBuf,unsigned &nSize);


/******************************************
Func:VPR_GetBinImg
Description:	获取指定序号的车牌二值化图
Input:      nIndex：数据序号；
			lpImgBuf：图像数据缓冲区;
			nWeidth：输出的二值化图像宽度；
			nHeight：输出的二值化图像高度；
			nSize： 输入时为上层应用分配的缓冲区大小，输出时为图像实际大小；
					如图像大小大于缓冲区大小（或者lpImgBuf为空指针），输出图像实际大小。
Output:      无
Return:		0-成功；1-图像大小大于缓冲区 2-无图像 其它-失败；
******************************************/
int VPR_GetBinImg(unsigned int nIndex, unsigned char* lpImgBuf, unsigned int &nSize, unsigned int &nWidth, unsigned &nHeight);



typedef struct
 {
	int nColor;     //车牌颜色代码 0-蓝牌；1-黄牌；2-黑牌；3-白牌；-1-无颜色
	char *lpszPlate;  //车牌号 20字节以内
	unsigned int nRating;  //车牌的可信度  0-100
	unsigned char *lpBigImgBuf;   //车辆大图图像数据缓冲区  如果输入为NULL，不输出图像
	unsigned int nBigImgSize;     //车辆大图图像大小 输入时为缓冲区大小，输出时为实际图像大小
	unsigned char *lpSmallImgBuf; //车牌小图图像数据缓冲区
	unsigned int nSmallImgSize;   //车牌小图图像大小
	unsigned char *lpBinImgBuf;   //二值化图像数据缓冲区
	unsigned int nBinImgSize;     //车牌小图图像大小
	unsigned int nBinImgWidth;	  //二值化图像宽度
	unsigned int nBinImgHeight;   //二值化图像高度
}CAutoDetectedVehInfo, *LPAutoDetectedVehInfo;

/******************************************
Func:VPR_GetAllVehInfo
Description:	获取车牌、大图、车牌小图、二值化图等数据
Input:      nIndex：数据序号；
			LPAutoDetectedVehInfo: 车牌识别结果结构
Output:     AutoDetectedVehInfo: 车牌识别结果结构
Return:		0-成功；其它-失败；
******************************************/
int VPR_GetAllVehInfo(unsigned int nIndex, LPAutoDetectedVehInfo pAuto);


/******************************************
Func:VPR_GetVer
Description:	获取动态链接库的版本号和厂家信息
Input:     lpszVer：动态链接库版本号，如果非空指针，可以写入20字节；
		   lpszVender：厂商名称，如果非空指针，可以写入50字节；
		   lpszDevType：设备型号，如果非空指针，可以写入50字节；
Output:    lpszVer：动态链接库版本号，如果非空指针，可以写入20字节；
		   lpszVender：厂商名称，如果非空指针，可以写入50字节；
		   lpszDevType：设备型号，如果非空指针，可
Return:	    0-成功；其它-失败；
******************************************/
int VPR_GetVer(char *lpszVer, char *lpszVender, char *lpszDevType);


/******************************************
Func:VPR_RegisterCallback
Description:	设置回调函数 
Input:      func:车牌识别结果回调函数
Output:      无
Return:		无
******************************************/
int VPR_RegisterCallback(VPR_NotifyFunc func);

/******************************************
Func:myVideoInit
Description:	初始化视频窗口 
Input:      left：视频窗口距离屏幕左边框的距离，单位：px;
			top：视频窗口距离屏幕顶边框的距离，单位：px;
			width：视频窗口的宽度；
			height：视频窗口的高度

Return:		0代表成功，其他代表错误代码；
******************************************/
int  myVideoInit (int left,int top,int width,int height);

/******************************************
Func:myVideoStart
Description:	设置视频窗口的大小
Input:      left：视频窗口距离屏幕左边框的距离，单位：px;
			top：视频窗口距离屏幕顶边框的距离，单位：px;
			width：视频窗口的宽度；
			height：视频窗口的高度

Return:		0代表成功，其他代表错误代码；
******************************************/
int    myVideoStart( int left,int top,int width,int height);

/******************************************
Func:myVideoInitCapture
Description: 视频保存抓拍图片
Input:      fileName：文件名
			quality：抓拍图片质量0-100（越大越好）


Return:		0代表成功，其他代表错误代码；
******************************************/
int  myVideoInitCapture (char * fileName, int quality);

/******************************************
Func:Vdm_Init
Description: 叠加器的初始化；
Input:     
Return:		0代表成功，其他代表错误代码；
******************************************/
int Vdm_Init();

/******************************************
Func:Vdm_line
Description: 叠加字符
Input:      Line：行号，行号自上而下从零递增，列号从左到右从零递增；
			Coloum：列号，从此列开始叠加一行字符；
			Nlen:	    长度,所叠加字符的长度；
			VdmChar:	内容，所叠加字符串的内容；
Return:	    0代表成功，其他代表错误代码；
******************************************/
int Vdm_line (unsigned char line , unsigned char coloum, unsigned char nlen, char *VdmChar);

 
/******************************************
Func:Sync_Vdm_Time
Description: 叠加器叠加当前时间

Return:		Sync_Vdm_Time
******************************************/
int  Sync_Vdm_Time();

/******************************************
Func:Vdm_Close
Description: 关闭字符叠加器

返回值：0代表成功，其他代表错误代码；

******************************************/
int  Vdm_Close();
#endif