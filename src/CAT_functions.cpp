#include "item_functions.h"

void info_crits(double &val, const S4 &item, const vector<double> &Theta, const int &criteria, 
	const arma::colvec &w, const arma::mat &inv_priorvar)
{
   	arma::mat info_mat = Info(item, Theta);
    if(criteria != 1){
    	info_mat = info_mat + inv_priorvar;
    	arma::mat acov;
    	switch(criteria){
    		case 2 : //D
    			val = arma::as_scalar(det(info_mat));
    			break;
    		case 3 : //T
    			val = arma::as_scalar(trans(w) * info_mat.diag());
    			break;
			case 4 : //A
				acov = pinv(info_mat);
        		val = arma::as_scalar(trans(w) * acov.diag());
    			break;
    		case 5 : //W
    			val = arma::as_scalar(trans(w) * info_mat * w);
    			break;
    		case 6 : //E
    			val = arma::as_scalar(min(arma::eig_sym(info_mat)));
    			break;
    	}
    } else {
    	val = info_mat(0,0);
    }
}

RcppExport SEXP ComputeCriteria(SEXP Rpars, SEXP RTheta, SEXP Rwhich_not_answered, 
	SEXP Rcriteria, SEXP Rw, SEXP Rinv_priorvar)
{
    BEGIN_RCPP

    const List pars(Rpars);
    const vector<double> Theta = as< vector<double> >(RTheta);
    const vector<int> which_not_answered = as< vector<int> >(Rwhich_not_answered);
    const NumericVector rw(Rw);
    const int criteria = as<int>(Rcriteria);
    const int len = which_not_answered.size();
    const int nfact = Theta.size();
    NumericMatrix rinv_priorvar(Rinv_priorvar);
    arma::colvec w(rw.begin(), rw.size(), false);
    arma::mat inv_priorvar(rinv_priorvar.begin(), nfact, nfact, false);
    vector<double> crit(len);

    for(int pick = 0; pick < len; ++pick){
    	const int whc = which_not_answered[pick];
    	const S4 item = pars[whc-1];
        
    	/* criteria:
    		1-6 = Fisher, Drule, Trule, Arule, Wrule, Erule
    	*/

    	if(criteria <= 6){
    		info_crits(crit[pick], item, Theta, criteria, w, inv_priorvar);
    	} else {
    		Rprintf("Criteria invalid in compiled code.\n");
    	}
    }

    return(wrap(crit));
    END_RCPP
}