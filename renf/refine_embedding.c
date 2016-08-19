/*
    Copyright (C) 2016 Vincent Delecroix

    This file is part of e-antic

    e-antic is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "poly_extra.h"
#include "renf.h"

/* TODO: use bisection if it fails */
/* TODO: use multiple Newton if possible */

void renf_refine_embedding(renf_t nf, slong prec)
{
    arb_t tmp;
    arb_init(tmp);
#ifdef DEBUG
    printf("[_renf_refine_embedding]: before emb=");
    arb_printd(nf->emb, 10);
    printf("\n");
#endif
    _fmpz_poly_newton_step_arb(tmp,
            fmpq_poly_numref(nf->nf->pol),
            nf->der->coeffs,
            fmpq_poly_length(nf->nf->pol),
            nf->emb,
            prec);
    arb_swap(tmp, nf->emb);
    arb_clear(tmp);
#ifdef DEBUG
    printf("[_renf_refine_embedding]: after emb=");
    arb_printd(nf->emb, 10);
    printf("\n");
#endif
}
