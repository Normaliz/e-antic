/*
    Copyright (C) 2018 Vincent Delecroix

    This fire is part of e-antic

    e-antic is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3.0 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "flint/fmpq_poly.h"
#include "e-antic/poly_extra.h"
#include <ctype.h>
#include <string.h>
#include <errno.h>

int _monomial_set_str(fmpq_t coeff, slong * pow, const char * var, char * w)
{
    char tmp;
    char * x = w;
    char * xx;
    char * y;
    int sign = 1;
    int ans;

    /* ignore spaces */
    while (*x == ' ') x++;

    if (*x == '\0')
        return -1;

    /* 1. sign */
    do
    {
        if (*x == '\0')
            return -1;
        else if (*x == '-')
            sign *= -1;
        else if (*x != ' ' && *x != '+')
            break;
        x++;
    } while (1);

    /* 2. coefficient */
    if (isdigit(*x))
    {
        y = x + 1;
        while (isdigit(*y) || (*y == '/')) y++;

        tmp = *y;
        *y = '\0';
        ans = fmpq_set_str(coeff, x, 10);
        *y = tmp;

        if (ans)
            return ans;
        x = y;
    }
    else
        fmpq_one(coeff);

    if (sign == -1)
        fmpq_neg(coeff, coeff);

    while (*x == ' ') x++;

    if (*x == '\0')
    {
        *pow = 0;
        return 0;
    }

    /* 3. multiplication sign (can also be implicit) */

    if (*x == '*')
    {
        do
        {
            x++;
        } while (*x == ' ');

        if (*x == '\0')
            return -1;
    }

    /* 4. variable */
    if (strncmp(x, var, strlen(var)))
        return -1;

    x += strlen(var);

    /* 5. carret */
    if (!isdigit(*x))
    {
        while (*x == ' ') x++;

        if (*x == '\0')
        {
            *pow = 1;
            return 0;
        }

        if (*x != '^')
            return -1;
        do
        {
            x++;
        } while (*x == ' ');
    }

    /* exponent */
    if (*x == '\0' || !isdigit(*x))
        return -1;

    {
        errno = 0;
        *pow = strtol(x, &y, 10);
        if (errno)
            return -1;

        while (*y == ' ') y++;

        if (*y != '\0')
            return -1;

        return 0;
    }
}

int str_is_varname(const char* var)
{
    if (isalpha(*var) || *var == '_')
        var++;
    else
        return 0;

    while (*var != '\0')
    {
        if (!isalnum(*var) && *var != '_')
            return 0;
        var++;
    }

    return 1;
}

int fmpz_poly_set_str_pretty(fmpz_poly_t p, const char * s, const char * var)
{
    if (!str_is_varname(var))
        return -1;

    size_t n = strlen(var);
    char * z = (char *) flint_malloc((strlen(s) + 1) * sizeof(char));

    fmpq_t coeff;
    slong pow;
    fmpz_poly_t mon;

    fmpz_poly_init(mon);
    fmpq_init(coeff);
    const char * start = s;

    fmpz_poly_zero(p);

    while (*start != '\0')
    {
        while (*start == ' ') start++;

        int ans;
        const char * end = start;

        /* jump over sign */
        while (*end == '+' || *end == ' ' || *end == '-') end++;

        /* jump over non sign */
        while (*end != '\0' && *end != '+' && *end != '-') end++;

        strncpy(z, start, end - start);
        z[end - start] = '\0';

        ans = _monomial_set_str(coeff, &pow, var, z);
        if (ans)
        {
            flint_free(z);
            fmpq_clear(coeff);
            fmpz_poly_clear(mon);
            return -1;
        }
        else if (!fmpz_is_one(fmpq_denref(coeff)))
        {
            flint_free(z);
            fmpq_clear(coeff);
            fmpz_poly_clear(mon);
            return -2;
        }
        fmpz_poly_zero(mon);
        fmpz_poly_set_coeff_fmpz(mon, pow, fmpq_numref(coeff));
        fmpz_poly_add(p, p, mon);

        start = end;
    }

    flint_free(z);
    fmpq_clear(coeff);
    fmpz_poly_clear(mon);
    return 0;
}

int fmpq_poly_set_str_pretty(fmpq_poly_t p, const char * s, const char * var)
{
    if (!str_is_varname(var))
        return -1;

    size_t n = strlen(var);
    char * z = (char *) flint_malloc((strlen(s) + 1) * sizeof(char));

    fmpq_t coeff;
    slong pow;
    fmpq_poly_t mon;

    fmpq_poly_init(mon);
    fmpq_poly_zero(p);
    fmpq_init(coeff);
    const char * start = s;

    while (*start != '\0')
    {
        while (*start == ' ') start++;

        int ans;
        const char * end = start;

        /* jump over sign */
        while (*end == '+' || *end == ' ' || *end == '-') end++;

        /* jump over non sign */
        while (*end != '\0' && *end != '+' && *end != '-') end++;

        strncpy(z, start, end - start);
        z[end - start] = '\0';

        ans = _monomial_set_str(coeff, &pow, var, z);
        if (ans)
        {
            flint_free(z);
            fmpq_clear(coeff);
            fmpq_poly_clear(mon);
            return -1;
        }
        fmpq_poly_zero(mon);
        fmpq_poly_set_coeff_fmpq(mon, pow, coeff);
        fmpq_poly_add(p, p, mon);

        start = end;
    }

    flint_free(z);
    fmpq_clear(coeff);
    fmpq_poly_clear(mon);
    return 0;
}
