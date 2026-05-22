interface pf_stream_if #(
		    parameter int WIDTH = 32
		);

    import pf_types_pkg::*;
		import pf_logic_pkg::*;

    // Core stream
		logic valid;
		logic ready;
		logic [WIDTH-1:0] data;

    // Frame control
		logic last;

		// Optional sideband signals
		logic error;
		logic [3:0] keep;
		logic [7:0] user;

		// Direction control
    modport master (
        output valid,
        output data,
        output last,
        output error,
        output keep,
        output user,
        input  ready
    );

    modport slave (
        input  valid,
        input  data,
        input  last,
        input  error,
        input  keep,
        input  user,
        output ready
    );

endinterface