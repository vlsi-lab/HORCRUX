//////////////////////////////////////////////////////////////////////////////////////////
// Engineer:      Alessandra Dolmeta - alessandra.dolmeta@polito.it                     //
//                Valeria Piscopo    - valeria.piscopo@polito.it                        //
//                                                                                      //
// Design Name:    Tightly-coupled Accelerator Module                                   //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


module restr_to_val_rom (
  input  wire [2:0] x,          // assumes FZ_ELEM ∈ [0..7]
  output reg  [7:0] y           // FP_ELEM is 8-bit here
);


  always @* begin
    case (x)
      3'd0: y = 8'h01;
      3'd1: y = 8'h02;
      3'd2: y = 8'h04;
      3'd3: y = 8'h08;
      3'd4: y = 8'h10;
      3'd5: y = 8'h20;
      3'd6: y = 8'h40;
      3'd7: y = 8'h01;          // wrap
    endcase
  end
endmodule
