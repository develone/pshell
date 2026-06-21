opj_jp2_t* opj_jp2_create(OPJ_BOOL p_is_decoder);
 

void opj_jp2_destroy(opj_jp2_t *jp2);
 

OPJ_BOOL opj_jp2_decode(opj_jp2_t *jp2,
                        opj_stream_private_t *p_stream,
                        opj_image_t* p_image,
                        opj_event_mgr_t * p_manager);
 

static OPJ_BOOL opj_jp2_check_color(opj_image_t *image, opj_jp2_color_t *color,
                                    opj_event_mgr_t *p_manager);
 typedef struct opj_jp2 {
    /** handle to the J2K codec  */
    opj_j2k_t *j2k;
    /** list of validation procedures */
    struct opj_procedure_list * m_validation_list;
    /** list of execution procedures */
    struct opj_procedure_list * m_procedure_list;

    /* width of image */
    OPJ_UINT32 w;
    /* height of image */
    OPJ_UINT32 h;
    /* number of components in the image */
    OPJ_UINT32 numcomps;
    OPJ_UINT32 bpc;
    OPJ_UINT32 C;
    OPJ_UINT32 UnkC;
    OPJ_UINT32 IPR;
    OPJ_UINT32 meth;
    OPJ_UINT32 approx;
    OPJ_UINT32 enumcs;
    OPJ_UINT32 precedence;
    OPJ_UINT32 brand;
    OPJ_UINT32 minversion;
    OPJ_UINT32 numcl;
    OPJ_UINT32 *cl;
    opj_jp2_comps_t *comps;
    /* FIXME: The following two variables are used to save offset
      as we write out a JP2 file to disk. This mechanism is not flexible
      as codec writers will need to extand those fields as new part
      of the standard are implemented.
    */
    OPJ_OFF_T j2k_codestream_offset;
    OPJ_OFF_T jpip_iptr_offset;
    OPJ_BOOL jpip_on;
    OPJ_UINT32 jp2_state;
    OPJ_UINT32 jp2_img_state;

    opj_jp2_color_t color;

    OPJ_BOOL ignore_pclr_cmap_cdef;
    OPJ_BYTE has_jp2h;
    OPJ_BYTE has_ihdr;
}
opj_jp2_t;
