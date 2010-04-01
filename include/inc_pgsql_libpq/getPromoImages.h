/*
 * getPromoImages.h 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2003 Satoshi Nagayasu & Hideyuki Kawashima &
 *                    Sachi Osawa & Hirokazu Kondo & Satoru Satake
 */

#ifndef _getPromoImages_H
#define _getPromoImages_H

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_getPromoImages \
           "SELECT i_id,i_thumbnail FROM item "\
           "WHERE i_id = (SELECT i_related1 FROM item WHERE i_id = %lld) OR "\
                 "i_id = (SELECT i_related2 FROM item WHERE i_id = %lld) OR "\
                 "i_id = (SELECT i_related3 FROM item WHERE i_id = %lld) OR "\
                 "i_id = (SELECT i_related4 FROM item WHERE i_id = %lld) OR "\
                 "i_id = (SELECT i_related5 FROM item WHERE i_id = %lld);"

int getPromoImages(struct db_context_t *,
                   struct promotional_processing_t *);

#endif /* _getPromoImages_H */
