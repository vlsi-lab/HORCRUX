//////////////////////////////////////////////////////////////////////////////////////////
// Engineer:      Alessandra Dolmeta - alessandra.dolmeta@polito.it                     //
//                Valeria Piscopo    - valeria.piscopo@polito.it                        //
//                                                                                      //
// Design Name:    Tightly-coupled Accelerator Module                                   //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

module fp_double_zero_norm (
  input  logic [31:0] x,
  output logic [31:0] y
);

  logic [31:0] r;
  logic [31:0] tmp;

  always_comb begin
    r = {2'b0, x[6:0]} + (x >> 7);
    if (r >= 9'd127) begin
      tmp = {25'b0, (r - 9'd127)};
      y = tmp[6:0];
    end else begin
      y = {25'b0, r[6:0]};
    end
  end
endmodule
