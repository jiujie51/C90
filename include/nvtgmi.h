/* =================================================================
 *
 *      nvtgmi.h
 *
 *      General Multimedia Interface header file
 *
 * =================================================================
 */
#ifndef _NVTGMI_H
#define _NVTGMI_H

#ifdef __cplusplus
extern "C" {
#endif


#define GM_VERSION_CODE 0x0052

/* return value definition */
#define GM_SUCCESS               0
#define GM_FD_NOT_EXIST         -1
#define GM_BS_BUF_TOO_SMALL     -2
#define GM_EXTRA_BUF_TOO_SMALL  -3
#define GM_TIMEOUT              -4
#define GM_DUPLICATE_FD         -5
#define GM_MD_MAX_SUB_REGION_NUM 4
#define GM_MD_USR_MAX_OBJ_NUM 128
#define GM_MD_USR_MAX_EVT_SIZE 10800
#define GM_CUSTOM_BC_USR_MAX_EVT_SIZE 120*90
#define GM_BC_MAX_NUM  1024
/***********************************
 *        General Structure
 ***********************************/

/* Dimention definition */
typedef struct gm_dim {
	int width;
	int height;
} gm_dim_t;

/* Rectancle definition */
typedef struct gm_rect {
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
} gm_rect_t;

typedef struct gm_quadrangle {
	unsigned int x[4];
	unsigned int y[4];
} gm_quadrangle_t;

typedef enum {
	GM_CAP_OBJECT = 0xFEFE0001,         ///< Capture object type
	GM_ENCODER_OBJECT = 0xFEFE0002,     ///< Encoder object type
	GM_WIN_OBJECT = 0xFEFE0003,         ///< Window object type
	GM_FILE_OBJECT = 0xFEFE0004,        ///< File object type
	GM_AUDIO_GRAB_OBJECT = 0xFEFE0005,     ///< Audio grabber object type
	GM_AUDIO_ENCODER_OBJECT = 0xFEFE0006,  ///< Audio encoder object type
	GM_AUDIO_RENDER_OBJECT = 0xFEFE0007,   ///< Audio renderer object type
} gm_obj_type_t;

typedef enum {
	GM_H264 = 0,
	GM_H265 = 1,
} gm_codec_type_t;

/***********************************
 *        System Structure
 ***********************************/
typedef enum {
	GM_INTERLACED,
	GM_PROGRESSIVE,
} gm_video_scan_method_t;

typedef enum {
	GM_AUTO = 0,
	GM_YUV420,
	GM_YUV422,
	GM_RGB565,
	GM_RGB1555,
	GM_RGB4444,
} gm_format_t;


#define FEATURE_UNDEFINED        0
#define FEATURE_ENCODE_ONLY      1
#define FEATURE_LIVEVIEW_ONLY    2
typedef struct {
	int valid;
	unsigned int number_of_path;  		///< Not in use
	gm_video_scan_method_t scan_method;	///< Not in use
	int framerate;
	gm_dim_t dim;
	int feature;             ///< 0: undefined 1: encode only 2: liveview only
	int sensor_id;
	int ime_id;
	int max_bitrate_kbit;
	int sensor_front_fps;
	int ime_compressed;
	int ime_tmnr;
	int ime_fps;
	unsigned int buffer_count;
	unsigned int path_id;
	int reserved[9];         ///< Reserved words
} gm_cap_sys_info_t;

typedef struct {
	int valid;
	int framerate;
	gm_dim_t dim;
	unsigned int chipid;	///< Not in use
	unsigned int is_channel_zero;
	unsigned int vout_id;
	int reserved[10];         ///< Reserved words
} gm_lcd_sys_info_t;

#define AU_INFO_SAMPE_RATE_8K       (1 << 0)    /*  8000 Hz */
#define AU_INFO_SAMPE_RATE_11K      (1 << 1)    /* 11025 Hz */
#define AU_INFO_SAMPE_RATE_12K      (1 << 2)    /* 12000 Hz */
#define AU_INFO_SAMPE_RATE_16K      (1 << 3)    /* 16000 Hz */
#define AU_INFO_SAMPE_RATE_22K      (1 << 4)    /* 22050 Hz */
#define AU_INFO_SAMPE_RATE_24K      (1 << 5)    /* 24000 Hz */
#define AU_INFO_SAMPE_RATE_32K      (1 << 6)    /* 32000 Hz */
#define AU_INFO_SAMPE_RATE_44K      (1 << 7)    /* 44100 Hz */
#define AU_INFO_SAMPE_RATE_48K      (1 << 8)    /* 48000 Hz */

#define AU_INFO_SAMPE_SIZE_8BIT     (1 << 0)    /* 8 Bit   */
#define AU_INFO_SAMPE_SIZE_16BIT    (1 << 1)    /* 16 bits */
#define AU_INFO_SAMPE_SIZE_32BIT    (1 << 2)    /* 32 bits */

#define AU_INFO_CHANNEL_LEFT        (1 << 0)
#define AU_INFO_CHANNEL_RIGHT       (1 << 1)
#define AU_INFO_CHANNEL_STEREO      (1 << 2)
 typedef struct {
	int valid;
	int sample_rate_support_bmp;
	int sample_size_support_bmp;
	int channel_support_bmp;
	int reserved[10];
} gm_audio_grab_sys_info_t;

typedef struct {
	int valid;
	int sample_rate_support_bmp;
	int sample_size_support_bmp;
	int channel_support_bmp;
	int reserved[10];
} gm_audio_render_sys_info_t;

#define	MAX_ENV_INFO_STRING_LEN	31
typedef struct {
	char sensor_name[MAX_ENV_INFO_STRING_LEN + 1];
	char display_name[MAX_ENV_INFO_STRING_LEN + 1];
	char embedded_memory_name[MAX_ENV_INFO_STRING_LEN + 1];
	char dsp_feature[MAX_ENV_INFO_STRING_LEN + 1];
	char build_date[MAX_ENV_INFO_STRING_LEN + 1];      //uitron build date
	char build_date_lib[MAX_ENV_INFO_STRING_LEN + 1];  //gmlib build date
	char reserved_string[5][MAX_ENV_INFO_STRING_LEN + 1];

	struct {
		unsigned long  addr;
		unsigned long  size;
	} mem_partition[8];	//0:ipc 1:linux 2:uboot 3:dsp1 4:uitron 5:ramdisk 6:loader 7:extdram
	struct {
		unsigned long  addr;
		unsigned long  size;
		unsigned long  free;
		unsigned long  max_free_block;
	} mem_status[2];  //0:ddr-1, 1:ddr-2
	unsigned int  fd_facescale;

	unsigned int  direct_mode : 1;
	unsigned int  define_product : 4;
	unsigned int  define_new_trigger : 1;
	unsigned int  define_ipl_direct : 1;
	unsigned int  define_ipl_3dnr : 1;
	unsigned int  define_enc_3dnr : 1;
	unsigned int  define_disp_wait : 1;
	unsigned int  define_dis : 1;
	unsigned int  define_md_func : 1;
	unsigned int  define_smartroi : 1;
	unsigned int  define_vqa_func : 1;
	unsigned int  define_pd_func : 1;
	unsigned int  define_fd_func : 1;
	unsigned int  define_aec_func : 1;
	unsigned int  define_anr_func : 1;
	unsigned int  define_ltr_func : 1;
	unsigned int  define_wdr_func : 1;
	unsigned int  define_hdr_func : 1;
	unsigned int  define_ftg_func : 1;
	unsigned int  define_buf_level : 4;
	unsigned int  define_reserved : 6;
	unsigned int  md_path;

	unsigned int  max_video_buf_time_ms;
	unsigned int  max_audio_in_buf_time_ms;
	unsigned int  max_audio_out_buf_time_ms;

	unsigned int  audio_enc_pcm : 1;
	unsigned int  audio_enc_aac : 1;
	unsigned int  audio_enc_alaw : 1;
	unsigned int  audio_enc_ulaw : 1;
	unsigned int  audio_enc_reserved : 12;
	unsigned int  audio_dec_pcm : 1;
	unsigned int  audio_dec_aac : 1;
	unsigned int  audio_dec_alaw : 1;
	unsigned int  audio_dec_ulaw : 1;
	unsigned int  audio_dec_reserved : 12;

	int reserved[15];
} gm_environment_sys_info;

#define CAPTURE_VCH_NUMBER      8
#define LCD_VCH_NUMBER          1
#define AUDIO_GRAB_VCH_NUMBER   4
#define AUDIO_RENDER_VCH_NUMBER 4

typedef struct {
	unsigned int graph_type;
	unsigned int gm_lib_version;
	gm_cap_sys_info_t cap[CAPTURE_VCH_NUMBER];
	gm_lcd_sys_info_t lcd[LCD_VCH_NUMBER];
	gm_audio_grab_sys_info_t au_grab[AUDIO_GRAB_VCH_NUMBER];
	gm_audio_render_sys_info_t au_render[AUDIO_RENDER_VCH_NUMBER];
	gm_environment_sys_info env;
	int reserved[20];         ///< Reserved words
} gm_system_t;


/***********************************
 *       Poll Structure
 ***********************************/
#define GM_POLL_READ            1
#define GM_POLL_WRITE           2

typedef struct {
	unsigned int event;
	unsigned int bs_len;
	unsigned int extra_len;
	unsigned int keyframe;  // 1: keyframe, 0: non-keyframe
} gm_ret_event_t;

#define GM_FD_MAX_PRIVATE_DATA         5
typedef struct {
	void *bindfd;    ///< The return value of gm_bind
	unsigned int event;     ///< The poll event ID,
	gm_ret_event_t revent;  ///< The returned event value
	int fd_private[GM_FD_MAX_PRIVATE_DATA];  ///< Library internal data, don't use it!
} gm_pollfd_t;

/***********************************
 *  Encode/Decode Bitstream Structure
 ***********************************/
#define GM_FLAG_NEW_SETTING (1 << 0) ///< Indicate the bitstream of new setting
#define GM_FLAG_NEW_FRAME_RATE  (1 << 3) ///< Not in use
#define GM_FLAG_NEW_GOP         (1 << 4) ///< Not in use
#define GM_FLAG_NEW_DIM         (1 << 6) ///< Not in use
#define GM_FLAG_NEW_BITRATE     (1 << 7) ///< Not in use

typedef struct gm_enc_bitstream {
	/* provide by application */
	char *bs_buf;             ///< Bitstream buffer pointer
	unsigned int bs_buf_len;  ///< AP provide bs_buf max length
	char *extra_buf;          ///< NULL indicate no needed extra-data
	unsigned int extra_buf_len;  ///< NULL indicate no needed extra-data

	/* value return by gm_recv_bitstream() */
	unsigned int bs_len;      ///< Bitstream length
	unsigned int extra_len;   ///< Extra data length
	int keyframe;             ///< 0:non-keyframe 1:keyframe  2:LTR frame
	unsigned int timestamp;   ///< Encode bitstream timestamp
	unsigned int newbs_flag;  ///< Flag notification of new seting, such as GM_FLAG_NEW_BITRATE
	unsigned int checksum;             ///< Not in use
	int ref_frame;            ///< 1: reference frame, CANNOT skip, 0: not reference frame, can skip,
	unsigned int slice_offset[3];      ///< Not in use
	int reserved[5];          ///< Reserved words
} gm_enc_bitstream_t;

typedef struct gm_enc_multi_bitstream {
	void *bindfd;
	gm_enc_bitstream_t bs;
	int retval;  ///< less than 0: recv bistream fail.
	int reserved[6];         ///< Reserved words

#define GM_ENC_MAX_PRIVATE_DATA  29
	int enc_private[GM_ENC_MAX_PRIVATE_DATA]; ///< Library internal data, don't use it!
} gm_enc_multi_bitstream_t;

typedef enum {
	TIME_ALIGN_ENABLE  = 0xFEFE01FE,
	TIME_ALIGN_DISABLE = 0xFEFE07FE,
	TIME_ALIGN_USER    = 0xFEFE09FE
} time_align_t;

/* NOTE: need 8bytes align for (* vpdDinBs_t)->timestamp */
typedef struct gm_dec_multi_bitstream {
	void *bindfd;
	char *bs_buf;
	unsigned int bs_buf_len;
	int retval;  ///< less than 0: send bistream fail.
	/* time_align:
	    TIME_ALIGN_ENABLE(default): playback time align by LCD period (ex. 60HZ is 33333us)
	    TIME_ALIGN_DISABLE: play timestamp by gm_send_multi_bitstreams called
	    TIME_ALIGN_USER: start to play at previous play point + time_diff(us)
	 */
	time_align_t time_align;	///< Not in use
	unsigned int time_diff; ///< Not in use
	int reserved[4];         ///< Reserved words

#define GM_DEC_MAX_PRIVATE_DATA         23
	int dec_private[GM_DEC_MAX_PRIVATE_DATA];   ///< Library internal(vpd_put_copy_din_t) data, don't use it!
} gm_dec_multi_bitstream_t;

/***********************************
 *        Clear Window Structure
 ***********************************/
typedef enum {
	GM_ACTIVE_BY_APPLY,
	GM_ACTIVE_IMMEDIATELY,
} gm_clear_window_mode_t;

typedef struct {
	int in_w;   ///< minimun input dim: 64x32
	int in_h;   ///< minimun input dim: 64x32
	gm_format_t in_fmt;
	unsigned char *buf;
	int out_x;
	int out_y;
	int out_w;
	int out_h;
	gm_clear_window_mode_t  mode;
	int reserved[5];
} gm_clear_window_t;

/***********************************
 *        Snapshot Structure
 ***********************************/
typedef struct snapshot { /* encode snapshot JPEG */
	void *bindfd;
	int image_quality;  ///< Not in use
	char *bs_buf;
	unsigned int bs_buf_len; ///< User given parepred bs_buf length, gm_lib returns actual length
	int bs_width;   ///< bitstream width, support range 128 ~ 720
	int bs_height;  ///< bitstream height, support range 96 ~ 576

	int reserved1[3];
	unsigned int timestamp;   ///< Not in use
	int reserved2[2];
} snapshot_t;

/***********************************
 *       Encode rate control
 ***********************************/
typedef enum {
    GM_CBR = 1,
    GM_VBR,
    GM_FIX_QP,
    GM_EVBR
} gm_enc_ratecontrol_mode_t;


typedef struct {
    unsigned int bitrate;
    unsigned int gop;
    unsigned int init_quant;
    unsigned int min_quant;
    unsigned int max_quant;
    int          ip_weight;
    struct {
    	unsigned int enabled;
    	unsigned int qp_range;
    	unsigned int qp_step;
    } row_rc;
    unsigned int static_time;
    unsigned int reserved1[8];
    unsigned int init_p_quant;
    unsigned int min_p_quant;
    unsigned int max_p_quant;
    unsigned int reserved2[6];
} gm_enc_cbr_t;


typedef struct {
    unsigned int bitrate;
    unsigned int gop;
    unsigned int init_quant;
    unsigned int min_quant;
    unsigned int max_quant;
    int ip_weight;
    struct {
    	unsigned int enabled;
    	unsigned int qp_range;
    	unsigned int qp_step;
    } row_rc;
    unsigned int static_time;
    unsigned int reserved1[7];
    unsigned int change_pos;
    unsigned int init_p_quant;
    unsigned int min_p_quant;
    unsigned int max_p_quant;
    unsigned int reserved2[6];
} gm_enc_vbr_t;


typedef struct {
	unsigned int bitrate;
	unsigned int gop;
	unsigned int init_quant;
	unsigned int min_quant;
	unsigned int max_quant;
	int          ip_weight;
    struct {
		unsigned int enabled;
		unsigned int qp_range;
		unsigned int qp_step;
    } row_rc;
    unsigned int static_time;
    unsigned int key_p_period;
    int          key_p_weight;
    int          motion_aq_strength;
    unsigned int still_frame_count;
    unsigned int motion_ratio_threadshold;
    unsigned int i_psnr_condition;
    unsigned int p_psnr_condition;
    unsigned int key_p_psnr_condition;

    unsigned int init_p_quant;
    unsigned int min_p_quant;
    unsigned int max_p_quant;
    unsigned int reserved2[6];
} gm_enc_evbr_t;


typedef struct {
	unsigned int bitrate;
	unsigned int gop;
	unsigned int init_quant;
	unsigned int reserved1[15];
	unsigned int init_p_quant;
	unsigned int reserved2[8];
} gm_enc_fixqp_t;



/***********************************
 *        OSG Structure
 ***********************************/
typedef struct {
    unsigned int osg_image_idx;      ///< identified image index for osg, Range:0 ~ 15
    int exist;                       ///< indicated non_exist:0, exist:1
    char *buf;                       ///< image buffer pointer
    unsigned int buf_len;            ///< image buffer length
    gm_format_t buf_type;            ///< image buffer type
    gm_dim_t dim;                    ///< the dimension of image
    int reserved[5];                 ///< Reserved words
} gm_osg_image_t;

typedef enum {
    GM_ALPHA_0 = 0, ///< alpha 0%
    GM_ALPHA_25,    ///< alpha 25%
    GM_ALPHA_37_5,  ///< alpha 37.5%
    GM_ALPHA_50,    ///< alpha 50%
    GM_ALPHA_62_5,  ///< alpha 62.5%
    GM_ALPHA_75,    ///< alpha 75%
    GM_ALPHA_87_5,  ///< alpha 87.5%
    GM_ALPHA_100    ///< alpha 100%
} gm_alpha_t;

#define MAX_OSG_WINDOW_NUM 16   ///< per encode object
typedef struct {
    int win_idx;    ///< Range:0 ~ 15
    int enabled;    ///< 0:enable, 1:disable
    int osg_img_idx;///< range:0 ~ 15
    int x;          ///< range:0 ~ max_width.
    int y;          ///< range:0 ~ max_height.
    gm_alpha_t alpha;    ///< Only RGB1555, RGB4444 supported
    int reserved[5];
} gm_osg_window_t;

/***********************************
 *        MASK Structure
 ***********************************/
#define MAX_CAP_MASK_NUM 8   ///< per cap_vch
typedef struct {
    int win_idx;    ///< Capture object per channel (shared by all paths): 0 ~ 7
    int enabled;    ///< 1:enable, 0:disable
    gm_dim_t    virtual_bg_dim; ///< user-defined background plane.
    gm_rect_t   virtual_rect;   ///< user-defined area based on the virtual_bg_dim.
    gm_alpha_t alpha;
	unsigned int color; ///< color, RBG888 format
    int reserved[6];
} gm_mask_t;

typedef struct {
    int win_idx;    ///< Capture object per channel (shared by all paths): 0 ~ 7
    int enabled;    ///< 1:enable, 0:disable
    gm_dim_t    	virtual_bg_dim; ///< user-defined background plane.
    gm_quadrangle_t virtual_quadrangle; ///< user-defined 4 vertex points based on the virtual_bg_dim.
    									///< using minimal x as first point then put other three points in clockwise order.
    gm_alpha_t alpha;
	unsigned int color; ///< color, RBG888 format
    int reserved[6];
} gm_quadrangle_mask_t;

/***************************
*MD parameter
****************************/
#define MD_USR_MAX_REGION 4
#define MD_USR_MAX_LV_NUM 4
typedef struct{
	unsigned int  x;
	unsigned int  y;
	unsigned int  width;
	unsigned int  height;
	unsigned int  level;
} gm_user_district;

typedef struct {
	unsigned int  is_enabled;
	unsigned int  start_block_x;
	unsigned int  start_block_y;
	unsigned int  end_block_x;
	unsigned int  end_block_y;
	unsigned int  alarm_th;
	unsigned int  alarm;
} md_user_rect;

typedef struct {
	md_user_rect  sub_region[GM_MD_MAX_SUB_REGION_NUM];
	unsigned int  sub_region_num;


	unsigned int  scene_change_th;


	unsigned int  u_height;           /*  user define height suggest not change default value    */
	unsigned int  u_width;            /*  user define width suggest not change default value    */
	unsigned int  u_mb_height;        /*  user define mb height (suggest value = k_mb_widthh * 2)too large would affect the accuracy of motion detection */
	unsigned int  u_mb_width;         /*  user define mb width (suggest value = k_mb_widthh * 2)too large would affect the accuracy of motion detection */
	unsigned int  frame_count;
	unsigned int  training_time;      /*  Needed training frames number for motion detection application  */
	unsigned int  md_type;
	unsigned int  rotate;
	unsigned int  flip;

	unsigned int  enable_type;
	unsigned int  mb_w_num;
	unsigned int  mb_h_num;
	unsigned int  alarm_th;

	unsigned int  object_size;        /*  object size threshold	*/	
	unsigned int  cross_type;         /*  none:0 cross_Entrance:1 cross_region:2  */
	unsigned int  overlap_th;         /*  object overlapping threshold  */		
	unsigned int  blk1_x;
	unsigned int  blk1_y;
	unsigned int  blk2_x;
	unsigned int  blk2_y;
	unsigned int  blk3_x;
	unsigned int  blk3_y;
	unsigned int  blk4_x;
	unsigned int  blk4_y;

	unsigned int  single_people_size;
	unsigned int  single_w;
	unsigned int  single_h;
	unsigned int  event_postprocessing;
    unsigned int  tamper_th;
	unsigned int  tamper_frm_num;
	unsigned int  md_rst;
	unsigned int  ref_cell_en;
	unsigned char   cell_map[GM_MD_USR_MAX_EVT_SIZE];
	unsigned int	reserved[6];
}md_user_global_info;

typedef struct {
    unsigned int	alpha;
	unsigned int	tb;
	unsigned int	sigma;
	unsigned int	init_val;
	unsigned int	tg;
	unsigned int	lum_diff_th;
	unsigned int	tex_diff_th;
	unsigned int	tex_th;
	unsigned int	tex_ratio_th;
	unsigned int	lvl_gm_md2_th;
    unsigned int reserved[6];
} md_user_lv_info;
typedef struct {
    unsigned int	alpha;
	unsigned int	tb;
	unsigned int	sigma;
	unsigned int	init_val;
	unsigned int	tg;
	unsigned int	lum_diff_th;
	unsigned int	tex_diff_th;
	unsigned int	tex_th;
	unsigned int	tex_ratio_th;
	unsigned int	reserved[6];
}md_usr_md1_lv_info;

typedef struct {
	unsigned int	   tbg;
    md_usr_md1_lv_info lv_info[MD_USR_MAX_LV_NUM];
	unsigned int	   reserved[6];
}md_user_md1_info;

typedef struct {
	unsigned int  time_period;
    unsigned int  lv_gm_md2_th[MD_USR_MAX_LV_NUM];
	unsigned int  reserved[6];
}md_user_md2_info;

typedef struct {
	unsigned int  update_prob;
    unsigned int  update_neighbor_en;
	unsigned int  deghost_en;
	unsigned int  th_feature;
	unsigned int  sensi;
	unsigned int  reserved[6];
}md_user_md3_info;

typedef struct {
    md_user_global_info  global_info;
	md_user_md1_info     md1_info;
	md_user_md2_info     md2_info;
	md_user_md3_info     md3_info;
    unsigned int reserved[6];
} gm_md_user_param_info;

// md_mode:
//		0 --> check motion alarm
//      1 --> check sub region motion alarm
//      2 --> size filter
//      3 --> cross line detection
//      4 --> forbidden zone detection
typedef struct {
	unsigned int fps; //fps=0 means turn off
	unsigned int md_mode;
	unsigned int dist_region_nu;
	gm_user_district dist_region[MD_USR_MAX_REGION];
	gm_md_user_param_info param_info;
    unsigned int reserved[6];
} gm_md_param_t;

typedef struct {
	unsigned int  bc_width;
	unsigned int  bc_height;
	unsigned char  bc_reset;
	unsigned char  bc_rslt_new;
	unsigned char  bc_update_prob;
	unsigned char  bc_update_neighbor_en;
	unsigned char  bc_deghost_en;
	unsigned int bc_max_fg_frm_num;
	unsigned int bc_min_obj_area_pct;
	unsigned int bc_th_cross_frm_num;
	unsigned char  bc_th_feature;
	unsigned char  bc_sensi;
	unsigned int reserved[6];
} gm_bc_user_param_t;

typedef struct {
	unsigned int fps; //fps=0 means turn off
	unsigned int bc_mode;
	gm_bc_user_param_t user_param;
    unsigned int reserved[6];
} gm_bc_param_t;

typedef struct {
	unsigned int targetnum;
	gm_rect_t target[GM_BC_MAX_NUM];
	unsigned int evt_width;
	unsigned int evt_height;
	unsigned char evt_map_addr[GM_CUSTOM_BC_USR_MAX_EVT_SIZE];
} gm_bc_rst_t;

typedef struct {
	char bc_event;
	unsigned int timestamp;
	gm_bc_rst_t bc_reslt;
    unsigned int reserved[6];
} gm_bc_data_t;

typedef struct {
	int start_x;
	int start_y;
	int end_x;
	int end_y;
	int label;
    unsigned int reserved[6];
} gm_md_usr_obj_t;

typedef struct {
	unsigned int motion_alarm;        ///< alarm
	unsigned int in_num;
	unsigned int out_num;
	unsigned int obj_num;
	unsigned char status;
	//unsigned int cross_decision;       ///< cross decision info
	unsigned int sub_motion_alarm[GM_MD_MAX_SUB_REGION_NUM]; ///< sub alarm
	gm_md_usr_obj_t obj[GM_MD_USR_MAX_OBJ_NUM]; ///< obj info
	unsigned char evt_map_addr[GM_MD_USR_MAX_EVT_SIZE];        ///< evt map
    unsigned int tp_result;
    unsigned int reserved[5];
} gm_md_usr_rst_t;

typedef struct {
	char md_event;
	unsigned int timestamp;
	gm_md_usr_rst_t md_reslt;
    unsigned int reserved[5];
} gm_md_data_t;


typedef struct {
	unsigned int	tbg;
	unsigned int 	time_period;
	unsigned int	u_height;           /*  user define height suggest not change default value    */
	unsigned int	u_width;            /*  user define width suggest not change default value    */
	unsigned int	u_mb_height;        /*  user define mb height (suggest value = k_mb_widthh * 2)too large would affect the accuracy of motion detection */
	unsigned int	u_mb_width;         /*  user define mb width (suggest value = k_mb_widthh * 2)too large would affect the accuracy of motion detection */
	unsigned int	md_type;
	unsigned int	mb_w_num;
	unsigned int	mb_h_num;
	unsigned int    event_postprocessing;
    unsigned int reserved[5];
}roi_md_user_global_info;


typedef struct {
    roi_md_user_global_info  roi_md_global_info;
	md_user_lv_info      lv_info[MD_USR_MAX_REGION];
    unsigned int reserved[6];
} gm_roi_md_user_param_info;

/***************************
*Smart Roi MD parameter
****************************/
typedef struct {
	unsigned int enable;
	unsigned int dist_region_nu;
	gm_user_district dist_region[MD_USR_MAX_REGION];
	gm_roi_md_user_param_info roi_md_param_info;
    unsigned int reserved[6];
} gm_roi_md_param_t;

/***************************
*FD parameter
****************************/
#define FD_NUM_MAX	36
typedef struct {
     unsigned int max_face_num;//maximum face number
     unsigned int face_scale;//the minimal face to detect releative to the maximum of image width and weight
     unsigned int process_rate;//process rate of frame, set 3 means 3 frames detect FD 1 time.
     unsigned int draw_face;//draw rectangle in enc data
} gm_fd_setting;

typedef struct {
	unsigned int timestamp;
	unsigned int face_number;
	gm_rect_t fd_rect[FD_NUM_MAX];
} gm_fd_result;
/***************************
*FTG parameter
****************************/
#define FTG_MAX_LANDMARK	68
typedef struct {
     unsigned int ftg_max_num;//Maximum number of tracking faces. Range: 1~36
     unsigned int ftg_scale;//The ratio of minimum face size to maximum image size. If the value is 15, face size is 80*80. Range: 0~15
     unsigned int ftg_ada_boost_type;// not support now, default value is 3
     unsigned int ftg_process_rate;//config od the process rate of frame, set 3 means 3 frames detect OD 1 time.
     unsigned int ftg_score_th;//landmark score threshold range 
     unsigned int ftg_ot_life;//config max obj life OT:0~5
     unsigned int ftg_od_life;//config max obj life OD:0~30
     unsigned int ftg_outobj_scale;//config output obj context range, can be set to 0~100, e.g. set 15 means output obj box with 1.5 times the original size and context range.
     unsigned int ftg_landmark_frq;//How many frame to get landmark info
} gm_ftg_setting;

typedef struct
{
    unsigned int x;//< result x
    unsigned int y;//< result y
    unsigned int w;//< result w
    unsigned int h;//< result h
    unsigned int id;//< result id
    // FTG clarity
    unsigned short luma;//< result face average luma range [0, 255]
	unsigned short clarity;//< result face average clarity range [0, 255]
	unsigned short contrast;//< result face average contrast range [0, 255]
	// FTG landmark
	unsigned int lstate;//< if landmark success is 1, failed is -1;
	unsigned int yaw;//< result face yaw angle
	unsigned int pitch;//< result face pitch angle
	unsigned int roll;//< result face roll angle
	unsigned int lscore;//< result face landmark score range [-1024 1024]
	unsigned int lnumber;//< result face landmark number
	unsigned short lpoints[FTG_MAX_LANDMARK*2];//< result face landmark data[x1,y1,x2,y2,...xn,yn]
} ftg_info;

typedef struct
{
	unsigned int timestamp;
    unsigned char  has_land_mark;
	unsigned int face_num;
	ftg_info info[10];
} gm_ftg_result;
/***********************************
 *       Video Quality Analysis Structure
 ***********************************/
typedef struct {
	int en_auto_adj_param;
	int en_ref_md;
	int en_too_light;
	int en_too_dark;
	int en_blur;
    unsigned int reserved[6];
} vqa_enable;

typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned int mb_x_size;
	unsigned int mb_y_size;
	unsigned int auto_adj_period;
	unsigned int g_alarm_frame_num;
    unsigned int reserved[6];
} vqa_global;

typedef struct {
	unsigned int too_light_strength_th;
	unsigned int too_light_cover_th;
	unsigned int too_light_alarm_times;
	unsigned int too_dark_strength_th;
	unsigned int too_dark_cover_th;
	unsigned int too_dark_alarm_times;
    unsigned int reserved[6];
} vqa_light;

typedef struct {
	unsigned int blur_strength;
	unsigned int blur_cover_th;
	unsigned int blur_alarm_times;
    unsigned int reserved[6];
} vqa_contrast;

typedef struct {
	unsigned int fps; //fps=0 means turn off
	vqa_enable enable_param;
	vqa_global global_param;
	vqa_light light_param;
	vqa_contrast contrast_param;
    unsigned int reserved[6];
} gm_vqa_param_t;

typedef struct {
	unsigned int timestamp;
	int is_too_light;
	int is_too_dark;
	int is_blur;
} gm_vqa_result_t;

typedef struct {
	unsigned int fps; //fps=0 means turn off
    unsigned int reserved[9];
} gm_pd_param_t;

#define PD_MAX_NUM	32
typedef struct gm_pd_info {
	unsigned int id;
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
} gm_pd_info_t;

typedef struct {
	unsigned int timestamp;
	unsigned int people_counts;
	gm_pd_info_t people[PD_MAX_NUM];
} gm_pd_result_t;

/***********************************
 *       Attribute Structure
 ***********************************/
typedef struct {
	int data[8];
} gm_priv_t;


typedef enum {
	GM_MONO = 1,     //if GM_LEFT and GM_RIGHT are available,
					 //  GM_MONO will use left(GM_LEFT) as the default channel
	GM_STEREO = 2,
	GM_LEFT = 3,
	GM_RIGHT = 4,
} gm_audio_channel_type_t;

typedef struct {
	gm_priv_t priv; ///< Library internal data, don't use it!
	int vch;        ///< File virtual channel given by application
	int max_width;  ///< Not in use
	int max_height; ///< Not in use
	int max_fps;    ///< Not in use
	unsigned short sample_rate;     ///< for audio data, specify its sample rate
	unsigned short sample_size;     ///< for audio data, specify its sample size
	gm_audio_channel_type_t channel_type;   ///< for audio data, specify its channel counts
	gm_format_t data_format;
	int reserved[2];         ///< Reserved words
} gm_file_attr_t;

typedef struct  {
	gm_priv_t priv; ///< Library internal data, don't use it!
	int cap_vch;    ///< Capture virtual channel
	int path;		///< Not in use
	int sub_path;   ///< Not in use
	unsigned int md_enable; ///< (NOT READY, please don't use it), capture motion data enable/disable
	unsigned int dma_path;  ///< Not in use
	unsigned int external_scaler; ///< Not in use
	unsigned int extra_buffers;   ///< Not in use
	int reserved3; ///< Reserved words,
} gm_cap_attr_t;


typedef enum {
	GM_CAP_METHOD_FRAME,        ///> Get an even/odd interlacing frame
	GM_CAP_METHOD_ONE_FIELD,    ///> Get a field, skip another
	GM_CAP_METHOD_TWO_FIELD     ///> Get and output every field
} gm_cap_method_t;

typedef struct {
	gm_priv_t priv; ///< Library internal data, don't use it!
	int out_dim_enable;
	gm_dim_t out_dim;   ///< indicate liveview capture output dim
	gm_cap_method_t method; ///> the method to get an interlace frame (no use for progressive)
	int reserved[6];
} gm_cap_advanced_attr_t;

typedef struct  {
	gm_priv_t priv; ///< Library internal data, don't use it!
	int enabled;
	gm_dim_t virtual_bg_dim;
	gm_rect_t virtual_rect;
	char reserved[20];         ///< Reserved words
} gm_crop_attr_t;

typedef enum {
	GM_ROTATE_NONE,
	GM_ROTATE_LEFT_90_DEGREE,
	GM_ROTATE_RIGHT_90_DEGREE
} gm_rotation_t;

typedef enum {
	GM_FLIP_NONE,
	GM_FLIP_HORIZONTAL,
	GM_FLIP_VERTICAL,
	GM_FLIP_VERTICAL_AND_HORIZONTAL
} gm_flip_t;

typedef struct  {
	gm_priv_t priv; ///< Library internal data, don't use it!
	gm_rotation_t direction;///< Active sequence: flip-->rotate direction
	gm_flip_t flip;         ///< Active sequence: flip-->rotate direction
	int reserved[5];        ///< Reserved words
} gm_rotation_attr_t;

#define GM_LCD0      0  ///< Indicate lcd_vch value
#define GM_LCD1      1  ///< Indicate lcd_vch value
#define GM_LCD2      2  ///< Indicate lcd_vch value
#define GM_LCD3      3  ///< Indicate lcd_vch value
#define GM_LCD4      4  ///< Indicate lcd_vch value
#define GM_LCD5      5  ///< Indicate lcd_vch value

typedef enum {
	WIN_LAYER1 = 0,         // background
	WIN_LAYER2,             // second layer
	COUNT_OF_WIN_LAYERS
} gm_win_layer_t;

typedef struct {
	gm_priv_t priv; ///< Library internal data, don't use it!
	int lcd_vch;   ///< Display virtual channel
	int visible;
	gm_rect_t rect;
	gm_win_layer_t layer;
	int reserved[6];         ///< Reserved words
} gm_win_attr_t;

typedef struct {
	gm_priv_t priv;
	int enabled;
	int palette_idx;  ///< Display window border color, GM8210 palette index 0~7
	int reserved[5];         ///< Reserved words
} gm_win_aspect_ratio_attr_t;

typedef enum {
	GM_H26X_DEFAULT_PROFILE = 0,
	GM_H26X_BASELINE_PROFILE = 66,
	GM_H26X_MAIN_PROFILE = 77,
	GM_H26X_HIGH_PROFILE = 100
} gm_h26x_profile_t;

typedef enum {
	GM_H26X_DEFAULT_LEVEL = 0,
	GM_H26X_LEVEL_1 = 10,
	GM_H26X_LEVEL_1_1 = 11,
	GM_H26X_LEVEL_1_2 = 12,
	GM_H26X_LEVEL_1_3 = 13,
	GM_H26X_LEVEL_2 = 20,
	GM_H26X_LEVEL_2_1 = 21,
	GM_H26X_LEVEL_2_2 = 22,
	GM_H26X_LEVEL_3 = 30,
	GM_H26X_LEVEL_3_1 = 31,
	GM_H26X_LEVEL_3_2 = 32,
	GM_H26X_LEVEL_4 = 40,
	GM_H26X_LEVEL_4_1 = 41,
	GM_H26X_LEVEL_4_2 = 42,
	GM_H26X_LEVEL_5 = 50,
	GM_H26X_LEVEL_5_1 = 51
} gm_h26x_level_t;

typedef enum {
	GM_H26X_DEFAULT_CONFIG = 0,
	GM_H26X_PERFORMANCE_CONFIG = 1,
	GM_H26X_LIGHT_QUALITY_CONFIG = 2,
	GM_H26X_QUALITY_CONFIG = 3
} gm_h26x_config_t;

typedef enum {
	GM_H26X_DEFAULT_CODING = 0,
	GM_H26X_CAVLC_CODING = 1,
	GM_H26X_CABAC_CODING = 2,
} gm_h26x_coding_t;

typedef enum  {
	GM_CHECKSUM_NONE = 0x0,           // no checksum
	GM_CHECKSUM_ALL_CRC = 0x101,      // all frames use crc method
	GM_CHECKSUM_ALL_SUM = 0x0102,     // all frames use sum method
	GM_CHECKSUM_ALL_4_BYTE = 0x103,   // all frames use 4 bytes sum method
	GM_CHECKSUM_ONLY_I_CRC = 0x201,   // only I frames use crc method
	GM_CHECKSUM_ONLY_I_SUM = 0x0202,  // only I frames use sum method
	GM_CHECKSUM_ONLY_I_4_BYTE = 0x203 // only I frames use 4 bytes sum method
} gm_checksum_type_t;

typedef enum  {
	GM_FASTFORWARD_NONE = 0,           // no fastforward
	GM_FASTFORWARD_1_FRAME = 2,        // skip 1 frame
	GM_FASTFORWARD_3_FRAMES = 4        // skip 3 frames
} gm_fast_forward_t;

typedef enum {
	GM_LTR_REF_TO_I_FRAME = 0,
	GM_LTR_REF_TO_PREV_LTR = 1
} gm_ltr_method_t;

typedef struct {
	gm_priv_t priv; ///< Library internal data, don't use it!
	gm_codec_type_t codec_type;
	gm_dim_t dim;
	union {
		int framerate;
		struct {
			int numerator: 16;	 ///< Not supported, don't use it!
			int denominator: 16; ///< Not supported, don't use it!
		} fps_ratio; ///< Not supported, don't use it!
	} frame_info;
	gm_enc_ratecontrol_mode_t rc_mode;
	union {
		gm_enc_cbr_t    cbr;
		gm_enc_vbr_t    vbr;
		gm_enc_evbr_t   evbr;
		gm_enc_fixqp_t  fixqp;
	} rc;
	struct {
		unsigned int    interval;   //For EVBR:
									//    0: LTR off, >0: must be equal to the value 'key_p_period'
						 			//For other rc_mode:
									//    0: LTR off, >0: LTR interval can be any value (0~4095)
		gm_ltr_method_t method;
	} ltr;
	struct {
		gm_h26x_profile_t profile: 8;
		gm_h26x_level_t level: 8;	///< Not in use
		gm_h26x_config_t config: 8;	///< Not in use
		gm_h26x_coding_t coding: 8;	///< Not in use
	} profile_setting;
	int reserved1[1];
	gm_checksum_type_t checksum_type;	///< Not in use
	gm_fast_forward_t fast_forward;	///< Not in use
	int reserved2[6];         ///< Reserved words
} gm_h26x_enc_attr_t;

#define GM_MAX_ROI_QP_NUMBER   10
typedef struct  {
    gm_priv_t priv;
    struct {
		int enabled;
		gm_rect_t rect;
		int qp_value;     ///< fixed the QP value in the ROI window
    } region[GM_MAX_ROI_QP_NUMBER];
    int reserved[5];
} gm_h26x_roi_qp_attr_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    gm_dim_t dim;
    union {
        int framerate;
        struct {
            int numerator:16;	///< Not supported, don't use it!
            int denominator:16;	///< Not supported, don't use it!
        } fps_ratio;
    } frame_info;
    int quality;   ///< Not supported, don't use it!
    int bitrate;
    int reserved[6];         ///< Reserved words
} gm_mjpeg_enc_attr_t;

typedef struct  {
	gm_priv_t priv; ///< Library internal data, don't use it!
	int vch;
	int sample_rate;
	int sample_size;
	gm_audio_channel_type_t channel_type;
	int reserved[5];         ///< Reserved words
} gm_audio_grab_attr_t;

typedef struct  {
	gm_priv_t priv;
	int enabled;             ///< Enable/Disable the ANR (default: Off)
	int suppress_level;      ///< Maximum suppression level of noise
	int HPF_cut_off_freq;    ///< Cut-off frequency of HPF pre-filtering
	int bias_sensitive;      ///< Bias sensitive
	char reserved[20];
} gm_anr_attr_t;

typedef enum {
	GM_PCM = 1,
	GM_AAC,
	GM_G711_ALAW,
	GM_G711_ULAW
} gm_audio_encode_type_t;

typedef struct  {
	gm_priv_t priv;
	gm_audio_encode_type_t encode_type;
	int bitrate;
	int frame_samples;       ///< only for G.711a/u
	int reserved[4];         ///< Reserved words
} gm_audio_enc_attr_t;

typedef struct  {
	gm_priv_t priv;
	int enabled;               ///< Enable/Disable the AEC (default: Off)
	int leak_estimate_enabled; ///< Enable/Disable the AEC leak estimate
	int leak_estimate_value;   ///< Initial Condition of the leak estimate (Range 25 ~ 99)
	int noise_cancel_level;    ///< Suggest value range -40 ~ -3. Unit in dB.
	int echo_cancel_level;     ///< Suggest value range -60 ~ -30. Unit in dB.
	int filter_length;         ///< AEC internal Filter Length. Set to 0 is using default value.
	int frame_size;            ///< AEC internal Frame Size. Set to 0 is using default value.
	int notch_radius;          ///< Set the Notch filter Radius. Value range 0 ~ 1000.
	char reserved[20];
} gm_aec_attr_t;

typedef struct  {
	gm_priv_t priv;
	int vch;
	gm_audio_encode_type_t encode_type;
	int frame_samples;       ///< Reserved
	int sync_with_lcd_vch;   ///< Reserved
	int reserved[4];         ///< Reserved words
} gm_audio_render_attr_t;

/***********************************
 * gmlib internal use
 ***********************************/
int gm_init_private(int version);  ///< Library internal use, don't use it!
void gm_init_attr(void *name, const char *struct_str, int struct_size, int version); ///< Library internal use, don't use it!


/***********************************
 *       gmlib API
 ***********************************/

/*!
 * @fn int gm_init(void)
 * @brief Init gmlib
 * @return 0 on success, negative value on error
 */
static inline int gm_init(void)
{
	return (gm_init_private(GM_VERSION_CODE));
}

/*!
 * @fn int gm_release(void)
 * @brief Release gmlib
 * @return 0 on success, negative value on error
 */
int gm_release(void);


/*!
 * @fn int gm_get_sysinfo(gm_system_t *system_info)
 * @brief Get system information
 * @param system_info Input pointer, application must prepare this memory.
 * @return 0 on success, negative value on error
 */
int gm_get_sysinfo(gm_system_t *system_info);

/*!
 * @fn void *gm_new_obj(gm_obj_type_t obj_type)
 * @brief Create a new object by obj_type
 * @param obj_type Type of object
 * @return object pointer
*/
void *gm_new_obj(gm_obj_type_t obj_type);

/*!
 * @fn int gm_delete_obj(void *obj)
 * @brief Delete a exist object
 * @param obj Pointer of object
 * @return 0 on success, negative value on error
*/
int gm_delete_obj(void *obj);

/*!
 * @fn void *gm_bind(void *groupfd, void *in_obj, void *out_obj)
 * @brief Bind in object to out object to groupfd
 * @param groupfd The fd of group
 * @param in_obj Input object pointer
 * @param out_obj Out object pointer
 * @return The fd of binding result
 */
void *gm_bind(void *groupfd, void *in_obj, void *out_obj);

/*!
 * @fn int gm_unbind(void *bindfd)
 * @brief Unbind the bindfd
 * @param bindfd The fd of gm_bind
 * @return 0 on success, negative value on error
 */
int gm_unbind(void *bindfd);

/*!
 * @fn void *gm_new_groupfd(void)
 * @brief Create a new group
 * @return The fd of group
 */
void *gm_new_groupfd(void);

/*!
 * @fn void gm_delete_groupfd(void *groupfd)
 * @brief Delete a group by groupfd
 * @param groupfd The fd of group
 */
int gm_delete_groupfd(void *groupfd);

/*!
 * @fn void DECLARE_ATTR(var, structure)
 * @brief Declare a attribyte
 * @param var AP definied variable
 * @param structure Structure of attribute
 */
#define DECLARE_ATTR(var, structure) \
	structure var = ({gm_init_attr(&var, #structure, sizeof(structure), GM_VERSION_CODE); var;})

/*!
 * @fn int gm_set_attr(void *obj, void *attr)
 * @brief Set object's attribute for object
 * @param obj Object pointer
 * @param attr Attribute pointer
 * @return 0 on success, negative value on error
 */
int gm_set_attr(void *obj, void *attr);

/*!
 * @fn int gm_apply(void *groupfd)
 * @brief Apply a groupfd
 * @param groupfd The fd of group
 * @return 0 on success, negative value on error
 */
int gm_apply(void *groupfd);

/*!
 * @fn int gm_apply_attr(void *bindfd, void *attr)
 * @brief dynamicly update attribute for the bindfd.
 * @param bindfd The return fd of gm_bind()
 * @param attr Attribute pointer
 * @return 0 on success, negative value on error
 */
int gm_apply_attr(void *bindfd, void *attr);

/*!
 * @fn int gm_poll(gm_pollfd_t *poll_fds, int num_fds, int timeout_millisec)
 * @brief Blocking to poll a bitstream
 * @param poll_fds The poll fd sets
 * @param num_fds Indicate number of fd sets
 * @param timeout_millisec Timeout value (millisec)
 * @return 0 on success, -4 on timeout, -1 on error
 */
int gm_poll(gm_pollfd_t poll_fds[], int num_fds, int timeout_millisec);

/*!
 * @fn int gm_send_multi_bitstreams(gm_dec_multi_bitstream_t *multi_bs, int num_bs, int timeout_millisec)
 * @brief Send multiple bitstream to playback
 * @param multi_bs the Structure of decode bitstream
 * @param num_bs Indicate number of bs sets
 * @param timeout_millisec Timeout value to send bitstream (millisecond)
 * @return 0 on success, -4 on timeout, -1 on error
 */
int gm_send_multi_bitstreams(gm_dec_multi_bitstream_t multi_bs[], int num_bs, int timeout_millisec);

/*!
 * @fn int gm_recv_multi_bitstreams(gm_enc_multi_bitstream_t *multi_bs, int num_bs)
 * @brief Get multiple encode bitstream
 * @param multi_bs the Structure of decode bitstream
 * @param num_bs Indicate number of bs sets
 * @return 0 on success, -1 on error, -2 on bs_buf too small, -3 on extra_buf too small, -4 on timeout
 */
int gm_recv_multi_bitstreams(gm_enc_multi_bitstream_t multi_bs[], int num_bs);

/*!
 * @fn int gm_request_snapshot(snapshot_t *snapshot, int timeout_millisec);
 * @brief Request a snapshot JPEG
 * @param snapshot The snapshot_t structure of snapshot
 * @param timeout_millisec Timeout value to snapshot a JPEG
 * @return positive value on success(JPEG length), -1 on error, -2 on buf too small, -4 on timeout
 */
int gm_request_snapshot(snapshot_t *snapshot, int timeout_millisec);

/*!
 * @fn gm_request_keyframe(void *bindfd)
 * @brief Request a keyframe
 * @param bindfd The return fd of gm_bind()
 * @return 0 on success, negative value on error
 */
int gm_request_keyframe(void *bindfd);

/*!
 * @fn int int gm_init_osg_buffer(unsigned int size);
 * @brief Set osg buffer size
 * @param size The size of osg buffer
 * @return 0 on success, -1 on error
 */
int gm_init_osg_buffer(unsigned int size);

/*!
 * @fn int gm_set_osg_images(gm_osg_image_t *images, int num_images);
 * @brief Set image to osg
 * @param images The multiple of image for osg
 * @param num_imgages Number of images
 * @return 0 on success, -1 on error
 */
int gm_set_osg_images(gm_osg_image_t images[], int num_images);

/*!
 * @fn int gm_set_osg_windows(void *obj, gm_osg_window_t osg_windows[], int num_osg_windows)
 * @brief Paste osg image on screen for encode
 * @param obj Object pointer of GM_CAP_OBJECT
 * @param osg_windows The multiple of osg window for osg
 * @param num_osg_windows Number of osg windows
 * @return 0 on success, -1 on error
 */
int gm_set_osg_windows(void *obj, gm_osg_window_t osg_windows[], int num_osg_windows);

/*!
 * @fn int gm_set_masks(void *obj, gm_mask_t masks[], int num_masks)
 * @brief Paste mask on screen for liveview or encode
 * @param obj Object pointer of GM_CAP_OBJECT
 * @param masks The multiple of mask for mask
 * @param num_masks Number of mask
 * @return 0 on success, -1 on error
 */
int gm_set_masks(void *obj, gm_mask_t masks[], int num_masks);

/*!
 * @fn int gm_set_quadrangle_masks(void *obj, gm_quadrangle_mask_t quadrangle_masks[], int num_masks)
 * @brief Paste mask on screen for liveview or encode
 * @param obj Object pointer of GM_CAP_OBJECT
 * @param masks The multiple of mask for mask
 * @param num_masks Number of mask
 * @return 0 on success, -1 on error
 */
int gm_set_quadrangle_masks(void *obj, gm_quadrangle_mask_t quadrangle_masks[], int num_masks);

/*!
 * @fn int gm_set_display_rate(int lcd_vch, int display_rate)
 * @brief Set frame rate to display
 * @param lcd_vch The vch of LCD display (GM_LCD0, GM_LCD1...)
 * @param display_rate Target frame rate
 * @return 0 on success, -1 on error
 */
int gm_set_display_rate(int lcd_vch, int display_rate);


/*!
 * @fn int gm_set_display_mode(int lcd_vch, int dup_from_lcd_vch, display_region_t disp_region)
 * @brief Set display mode (same function as "/proc/videograph/display_mode" command)
 * @param lcd_vch The vch of LCD display (GM_LCD0, GM_LCD1...)
 * @param dup_from_lcd_vch The vch of LCD display as the source to be duplicated
 * @param disp_region The region that will be duplicated from lcd_vch
 * @return 0 on success, -1 on error
 */
typedef enum {
	GM_DISP_WHOLE,
	GM_DISP_LEFT,
	GM_DISP_RIGHT,
} display_region_t;

int gm_set_display_mode(int lcd_vch, int dup_from_lcd_vch, display_region_t disp_region);

/*!
 * @fn unsigned int gm_get_timestamp(void)
 * @brief get current timestamp
 * @return timestamp value
*/
unsigned int gm_get_timestamp(void);

/*!
 * @fn int gm_set_vqa_param(gm_vqa_param_t vqa_param)
 * @brief set vqa parameters
 * @param vqa_param vqa parameters
 * @return 0 on success, -1 on error
*/
int gm_set_vqa_param(gm_vqa_param_t *vqa_param);

/*!
 * @fn int gm_set_vqa_param(gm_vqa_result_t *vqa_result)
 * @brief get vqa parameters
 * @param vqa_result VQA result
 * @param timeout_millisec Timeout value (millisec)
 * @return 0 on success, -1 on error
*/
int gm_get_vqa_result(gm_vqa_result_t *vqa_result, int timeout_millisec);

/*!
 * @fn int gm_vqa_exit(void)
 * @brief exit get vqa result
 * @return 0 on success, -1 on error
*/
int gm_vqa_exit(void);

/*!
 * @fn int gm_set_pd_param(gm_pd_param_t pd_param)
 * @brief set pd parameters
 * @param pd_param pd parameters
 * @return 0 on success, -1 on error
*/
int gm_set_pd_param(gm_pd_param_t *pd_param);

/*!
 * @fn int gm_set_pd_param(gm_pd_result_t *pd_result)
 * @brief get pd parameters
 * @param pd_result VQA result
 * @param timeout_millisec Timeout value (millisec)
 * @return 0 on success, -1 on error
*/
int gm_get_pd_result(gm_pd_result_t *pd_result, int timeout_millisec);

/*!
 * @fn int gm_pd_exit(void)
 * @brief exit get people detect result
 * @return 0 on success, -1 on error
*/
int gm_pd_exit(void);

/*!
 * @fn int gm_set_md_param(gm_md_param_t *md_param)
 * @brief set motion parameters
 * @param md_param motion detection parameters
 * @return 0 on success, -1 on error
*/
int gm_set_md_param(gm_md_param_t *md_param);

/*!
 * @fn int gm_poll_md_data(char *md_data, int timeout_millisec)
 * @param md_data pointer to current motion result
 * @param timeout_millisec timeout in millisecond
 * @return timestamp value
*/
int gm_poll_md_data(gm_md_data_t *md_data, int timeout_millisec);
/*
* stop poll md data
*/
void gm_md_poll_exit(void);
/*!
 * @fn int gm_set_bc_param(gm_bc_param_t *bc_param)
 * @brief set bc parameters
 * @param bc_param background calculation parameters
 * @return 0 on success, -1 on error
*/
int gm_set_bc_param(gm_bc_param_t *bc_param);

/*!
 * @fn int gm_get_bc_result(gm_bc_data_t *md_data, int timeout_millisec)
 * @param bc_result pointer to current bc result
 * @param timeout_millisec timeout in millisecond
 * @return timestamp value
*/
int gm_get_bc_result(gm_bc_data_t *bc_result, int timeout_millisec);
/*!
 * @fn int gm_exit_bc(void)
 * @brief exit get background calculation function
 * @return 0 on success, -1 on error
*/
int gm_exit_bc(void);
/*
* stop poll md data
*/
void gm_md_poll_exit(void);

/*!
 * @fn int gm_start_fd(void)
 * @brief start face detect func
 * @return 0 on success, -1 on error
*/
int gm_start_fd(void);
/*!
 * @fn int gm_stop_fd(void
 * @brief stop face detect func
 * @return 0 on success, -1 on error
*/
int gm_stop_fd(void);
/*!
 * @fn int gm_set_fd_param(gm_fd_setting *fd_setting)
 * @brief setup face detect paramters
 * @param fd_setting pointer to setup face detect parameters
 * @return 0 on success, -1 on error
*/
int gm_set_fd_param(gm_fd_setting *fd_setting);
/*!
 * @fn int gm_get_fd_param(gm_fd_setting *fd_setting)
 * @brief get current face detect paramters
 * @param fd_setting pointer to current face detect parameters
 * @return 0 on success, -1 on error
*/
int gm_get_fd_param(gm_fd_setting *fd_setting);
/*!
 * @fn int gm_get_fd_result(gm_fd_result *fd_result, int timeout_millisec)
 * @brief get face detect result
 * @param fd_result pointer to face detect result
 * @param timeout_millisec timeout in millisecond
 * @return 0 on success, -1 on error
*/
int gm_get_fd_result(gm_fd_result *fd_result, int timeout_millisec);
/*!
 * @fn int gm_fd_exit(void)
 * @brief exit get face detect result
 * @return 0 on success, -1 on error
*/
int gm_fd_exit(void);

/*!
 * @fn int gm_set_roi_md_param(gm_roi_md_param_t *roi_md_param)
 * @brief set smart roi motion parameters
 * @param roi_md_param set roi md parameters
 * @return 0 on success, -1 on error
*/
int gm_set_roi_md_param(gm_roi_md_param_t *roi_md_param);

/*!
 * @fn int gm_start_ftg(void)
 * @brief start face tracking & grading func
 * @return 0 on success, -1 on error
*/
int gm_start_ftg(void);
/*!
 * @fn int gm_stop_ftg(void
 * @brief stop face tracking & grading func
 * @return 0 on success, -1 on error
*/
int gm_stop_ftg(void);
/*!
 * @fn int gm_set_ftg_param(gm_ftg_setting *ftg_setting)
 * @brief setup face tracking & grading paramters
 * @param ftg_setting pointer to setup face tracking & grading parameters
 * @return 0 on success, -1 on error
*/
int gm_set_ftg_param(gm_ftg_setting *ftg_setting);
/*!
 * @fn int gm_get_ftg_param(gm_ftg_setting *ftg_setting)
 * @brief get current face tracking & grading paramters
 * @param ftg_setting pointer to current face tracking & grading parameters
 * @return 0 on success, -1 on error
*/
int gm_get_ftg_param(gm_ftg_setting *ftg_setting);
/*!
 * @fn int gm_get_ftg_result(gm_ftg_result *ftg_result, int timeout_millisec)
 * @brief get face tracking & grading result
 * @param ftg_result pointer to face tracking & grading result
 * @param timeout_millisec timeout in millisecond
 * @return 0 on success, -1 on error
*/
int gm_get_ftg_result(gm_ftg_result *ftg_result, int timeout_millisec);
/*!
 * @fn int gm_ftg_exit(void)
 * @brief exit get face tracking & grading result
 * @return 0 on success, -1 on error
*/
int gm_ftg_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* _NVTGMI_H */
