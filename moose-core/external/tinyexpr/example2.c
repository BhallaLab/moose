#include "tinyexpr.h"
#include <stdio.h>
#include <math.h>

double H( double a)
{
    return pow(a, 3.0);
}

int main(int argc, char *argv[])
{
    const char* expr = "p + g + l + len + dia + h(1-l)";
    printf("Evaluating:\n\t%s\n", expr);

    /* This shows an example where the variables
     * x and y are bound at eval-time. */
    double p, g, L, len, dia;
    te_variable vars[] = {{"p", &p}, {"g", &g}, {"l", &L}, {"len", &len}, {"dia", &dia}
        , { "h", H, TE_FUNCTION1}
    };


    /* This will compile the expression and check for errors. */
    int err;
    te_expr *n = te_compile(expr, vars, 6, &err);

    if (n) {
        /* The variables can be changed here, and eval can be called as many
         * times as you like. This is fairly efficient because the parsing has
         * already been done. */
        p = 1; g = 0.1; L = 10; len = 2; dia = 0.2;
        const double r = te_eval(n); printf("Result:\n\t%f\n", r);

        te_free(n);
    } else {
        /* Show the user where the error is at. */
        printf("\t%*s^\nError near here", err-1, "");
    }


    return 0;
}
