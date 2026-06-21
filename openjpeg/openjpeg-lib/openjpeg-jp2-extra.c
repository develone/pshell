 
opj_jp2_t* opj_jp2_create(OPJ_BOOL p_is_decoder)
{
    opj_jp2_t *jp2 = (opj_jp2_t*)opj_calloc(1, sizeof(opj_jp2_t));
    if (jp2) {

        /* create the J2K codec */
        if (! p_is_decoder) {
            jp2->j2k = opj_j2k_create_compress();
        } else {
            jp2->j2k = opj_j2k_create_decompress();
        }

        if (jp2->j2k == 00) {
            opj_jp2_destroy(jp2);
            return 00;
        }

        /* Color structure */
        jp2->color.icc_profile_buf = NULL;
        jp2->color.icc_profile_len = 0;
        jp2->color.jp2_cdef = NULL;
        jp2->color.jp2_pclr = NULL;
        jp2->color.jp2_has_colr = 0;

        /* validation list creation */
        jp2->m_validation_list = opj_procedure_list_create();
        if (! jp2->m_validation_list) {
            opj_jp2_destroy(jp2);
            return 00;
        }

        /* execution list creation */
        jp2->m_procedure_list = opj_procedure_list_create();
        if (! jp2->m_procedure_list) {
            opj_jp2_destroy(jp2);
            return 00;
        }
    }

    return jp2;
}

void opj_jp2_destroy(opj_jp2_t *jp2)
{
    if (jp2) {
        /* destroy the J2K codec */
        opj_j2k_destroy(jp2->j2k);
        jp2->j2k = 00;

        if (jp2->comps) {
            opj_free(jp2->comps);
            jp2->comps = 00;
        }

        if (jp2->cl) {
            opj_free(jp2->cl);
            jp2->cl = 00;
        }

        if (jp2->color.icc_profile_buf) {
            opj_free(jp2->color.icc_profile_buf);
            jp2->color.icc_profile_buf = 00;
        }

        if (jp2->color.jp2_cdef) {
            if (jp2->color.jp2_cdef->info) {
                opj_free(jp2->color.jp2_cdef->info);
                jp2->color.jp2_cdef->info = NULL;
            }

            opj_free(jp2->color.jp2_cdef);
            jp2->color.jp2_cdef = 00;
        }

        if (jp2->color.jp2_pclr) {
            if (jp2->color.jp2_pclr->cmap) {
                opj_free(jp2->color.jp2_pclr->cmap);
                jp2->color.jp2_pclr->cmap = NULL;
            }
            if (jp2->color.jp2_pclr->channel_sign) {
                opj_free(jp2->color.jp2_pclr->channel_sign);
                jp2->color.jp2_pclr->channel_sign = NULL;
            }
            if (jp2->color.jp2_pclr->channel_size) {
                opj_free(jp2->color.jp2_pclr->channel_size);
                jp2->color.jp2_pclr->channel_size = NULL;
            }
            if (jp2->color.jp2_pclr->entries) {
                opj_free(jp2->color.jp2_pclr->entries);
                jp2->color.jp2_pclr->entries = NULL;
            }

            opj_free(jp2->color.jp2_pclr);
            jp2->color.jp2_pclr = 00;
        }

        if (jp2->m_validation_list) {
            opj_procedure_list_destroy(jp2->m_validation_list);
            jp2->m_validation_list = 00;
        }

        if (jp2->m_procedure_list) {
            opj_procedure_list_destroy(jp2->m_procedure_list);
            jp2->m_procedure_list = 00;
        }

        opj_free(jp2);
    }
}

OPJ_BOOL opj_jp2_decode(opj_jp2_t *jp2,
                        opj_stream_private_t *p_stream,
                        opj_image_t* p_image,
                        opj_event_mgr_t * p_manager)
{
    if (!p_image) {
        return OPJ_FALSE;
    }

    /* J2K decoding */
    if (! opj_j2k_decode(jp2->j2k, p_stream, p_image, p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Failed to decode the codestream in the JP2 file\n");
        return OPJ_FALSE;
    }

    if (!jp2->ignore_pclr_cmap_cdef) {
        if (!opj_jp2_check_color(p_image, &(jp2->color), p_manager)) {
            return OPJ_FALSE;
        }

        /* Set Image Color Space */
        if (jp2->enumcs == 16) {
            p_image->color_space = OPJ_CLRSPC_SRGB;
        } else if (jp2->enumcs == 17) {
            p_image->color_space = OPJ_CLRSPC_GRAY;
        } else if (jp2->enumcs == 18) {
            p_image->color_space = OPJ_CLRSPC_SYCC;
        } else if (jp2->enumcs == 24) {
            p_image->color_space = OPJ_CLRSPC_EYCC;
        } else if (jp2->enumcs == 12) {
            p_image->color_space = OPJ_CLRSPC_CMYK;
        } else {
            p_image->color_space = OPJ_CLRSPC_UNKNOWN;
        }

        if (jp2->color.jp2_pclr) {
            /* Part 1, I.5.3.4: Either both or none : */
            if (!jp2->color.jp2_pclr->cmap) {
                opj_jp2_free_pclr(&(jp2->color));
            } else {
                if (!opj_jp2_apply_pclr(p_image, &(jp2->color), p_manager)) {
                    return OPJ_FALSE;
                }
            }
        }

        /* Apply the color space if needed */
        if (jp2->color.jp2_cdef) {
            opj_jp2_apply_cdef(p_image, &(jp2->color), p_manager);
        }

        if (jp2->color.icc_profile_buf) {
            p_image->icc_profile_buf = jp2->color.icc_profile_buf;
            p_image->icc_profile_len = jp2->color.icc_profile_len;
            jp2->color.icc_profile_buf = NULL;
        }
    }

    return OPJ_TRUE;
}

static OPJ_BOOL opj_jp2_check_color(opj_image_t *image, opj_jp2_color_t *color,
                                    opj_event_mgr_t *p_manager)
{
    OPJ_UINT16 i;

    /* testcase 4149.pdf.SIGSEGV.cf7.3501 */
    if (color->jp2_cdef) {
        opj_jp2_cdef_info_t *info = color->jp2_cdef->info;
        OPJ_UINT16 n = color->jp2_cdef->n;
        OPJ_UINT32 nr_channels =
            image->numcomps; /* FIXME image->numcomps == jp2->numcomps before color is applied ??? */

        /* cdef applies to cmap channels if any */
        if (color->jp2_pclr && color->jp2_pclr->cmap) {
            nr_channels = (OPJ_UINT32)color->jp2_pclr->nr_channels;
        }

        for (i = 0; i < n; i++) {
            if (info[i].cn >= nr_channels) {
                opj_event_msg(p_manager, EVT_ERROR, "Invalid component index %d (>= %d).\n",
                              info[i].cn, nr_channels);
                return OPJ_FALSE;
            }
            if (info[i].asoc == 65535U) {
                continue;
            }

            if (info[i].asoc > 0 && (OPJ_UINT32)(info[i].asoc - 1) >= nr_channels) {
                opj_event_msg(p_manager, EVT_ERROR, "Invalid component index %d (>= %d).\n",
                              info[i].asoc - 1, nr_channels);
                return OPJ_FALSE;
            }
        }

        /* issue 397 */
        /* ISO 15444-1 states that if cdef is present, it shall contain a complete list of channel definitions. */
        while (nr_channels > 0) {
            for (i = 0; i < n; ++i) {
                if ((OPJ_UINT32)info[i].cn == (nr_channels - 1U)) {
                    break;
                }
            }
            if (i == n) {
                opj_event_msg(p_manager, EVT_ERROR, "Incomplete channel definitions.\n");
                return OPJ_FALSE;
            }
            --nr_channels;
        }
    }

    /* testcases 451.pdf.SIGSEGV.f4c.3723, 451.pdf.SIGSEGV.5b5.3723 and
       66ea31acbb0f23a2bbc91f64d69a03f5_signal_sigsegv_13937c0_7030_5725.pdf */
    if (color->jp2_pclr && color->jp2_pclr->cmap) {
        OPJ_UINT16 nr_channels = color->jp2_pclr->nr_channels;
        opj_jp2_cmap_comp_t *cmap = color->jp2_pclr->cmap;
        OPJ_BOOL *pcol_usage, is_sane = OPJ_TRUE;

        /* verify that all original components match an existing one */
        for (i = 0; i < nr_channels; i++) {
            if (cmap[i].cmp >= image->numcomps) {
                opj_event_msg(p_manager, EVT_ERROR, "Invalid component index %d (>= %d).\n",
                              cmap[i].cmp, image->numcomps);
                is_sane = OPJ_FALSE;
            }
        }

        pcol_usage = (OPJ_BOOL *) opj_calloc(nr_channels, sizeof(OPJ_BOOL));
        if (!pcol_usage) {
            opj_event_msg(p_manager, EVT_ERROR, "Unexpected OOM.\n");
            return OPJ_FALSE;
        }
        /* verify that no component is targeted more than once */
        for (i = 0; i < nr_channels; i++) {
            OPJ_UINT16 pcol = cmap[i].pcol;
            /* See ISO 15444-1 Table I.14 – MTYPi field values */
            if (cmap[i].mtyp != 0 && cmap[i].mtyp != 1) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Invalid value for cmap[%d].mtyp = %d.\n", i,
                              cmap[i].mtyp);
                is_sane = OPJ_FALSE;
            } else if (pcol >= nr_channels) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Invalid component/palette index for direct mapping %d.\n", pcol);
                is_sane = OPJ_FALSE;
            } else if (pcol_usage[pcol] && cmap[i].mtyp == 1) {
                opj_event_msg(p_manager, EVT_ERROR, "Component %d is mapped twice.\n", pcol);
                is_sane = OPJ_FALSE;
            } else if (cmap[i].mtyp == 0 && cmap[i].pcol != 0) {
                /* I.5.3.5 PCOL: If the value of the MTYP field for this channel is 0, then
                 * the value of this field shall be 0. */
                opj_event_msg(p_manager, EVT_ERROR, "Direct use at #%d however pcol=%d.\n", i,
                              pcol);
                is_sane = OPJ_FALSE;
            } else {
                pcol_usage[pcol] = OPJ_TRUE;
            }
        }
        /* verify that all components are targeted at least once */
        for (i = 0; i < nr_channels; i++) {
            if (!pcol_usage[i] && cmap[i].mtyp != 0) {
                opj_event_msg(p_manager, EVT_ERROR, "Component %d doesn't have a mapping.\n",
                              i);
                is_sane = OPJ_FALSE;
            }
        }
        /* Issue 235/447 weird cmap */
        if (1 && is_sane && (image->numcomps == 1U)) {
            for (i = 0; i < nr_channels; i++) {
                if (!pcol_usage[i]) {
                    is_sane = 0U;
                    opj_event_msg(p_manager, EVT_WARNING,
                                  "Component mapping seems wrong. Trying to correct.\n", i);
                    break;
                }
            }
            if (!is_sane) {
                is_sane = OPJ_TRUE;
                for (i = 0; i < nr_channels; i++) {
                    cmap[i].mtyp = 1U;
                    cmap[i].pcol = (OPJ_BYTE) i;
                }
            }
        }
        opj_free(pcol_usage);
        if (!is_sane) {
            return OPJ_FALSE;
        }
    }

    return OPJ_TRUE;
}
