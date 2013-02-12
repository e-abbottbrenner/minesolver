#ifndef ENCAPSULATION_H
#define ENCAPSULATION_H

#define getter(ret_type, ret_expr, getter_name) \
    ret_type getter_name() const {return ret_expr;}

#define setter(var_type, var_name, setter_name) \
    void setter_name(const var_type& var_name) {this->var_name = var_name;}

#endif // ENCAPSULATION_H
