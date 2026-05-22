package pf_logic_pkg;

    typedef enum logic {
        PF_NO,
				PF_YES,
				PF_MAYBE
		} pf_option_t;

		typedef enum logic {
        PF_FALSE = 1'b0,
        PF_TRUE  = 1'b1
    } pf_bool_t;
	
endpackage