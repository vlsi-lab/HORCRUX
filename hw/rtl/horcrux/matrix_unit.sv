//////////////////////////////////////////////////////////////////////////////////////////
// Engineer:      Alessandra Dolmeta - alessandra.dolmeta@polito.it                     //
//                Valeria Piscopo    - valeria.piscopo@polito.it                        //
//                                                                                      //
// Design Name:    Tightly-coupled Accelerator Module                                   //
// File name:      matrix_unit.sv                                                       //
// Language:       SystemVerilog                                                        //
//                                                                                      //
// Description:    Performs all operation of fp_arith.h                                 //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

module matrix_unit
import horcrux_pkg::*;
(
    input  wire [15:0] rs1,    // e[i]
    input  wire [15:0] rs2,    // V_tr[i][j]
    input  wire [15:0] rs3,    // res[i][j]
    input  horcrux_insn insn_i,
    output reg  [31:0] result // computed_res[i][j]
);

    // Internal signals
    logic [31:0] mul;
    logic [31:0] sum;
    logic [15:0] tmp1, tmp2;

    logic [15:0] a, b, c;
    logic [7:0] restr_to_val_rs1;

    logic [31:0] input_norm;
    logic [31:0] output_norm;

    logic [31:0] fpred_opposite;
    logic [31:0] adder2;


    restr_to_val_rom restr_to_val_rom_inst (
        .x(rs1[2:0]),          
        .y(restr_to_val_rs1)          
    );

    always @(*) begin
        case (insn_i)
            vec_by_matrix, synd_minus_vec_scaled: begin
                a = rs1; // e[i]        or s[i]
                b = rs2; // V_tr[i][j]  or chall_1
                c = rs3; // res[i][j]   or synd[i]
            end

            restr_vec_by_matrix: begin
                a = restr_to_val_rs1; // RESTR_TO_VAL(e[i])
                b = rs2;              // V_tr[i][j]
                c = rs3;              // res[i][j]
            end

            vec_by_vec_pointwise: begin
                a = rs1;  // in1
                b = rs2;  // in2
                c = '0;
            end

            default: begin
                a = '0;
                b = '0;
                c = '0;
            end
        endcase
    end

    assign mul = a * b;

    assign input_norm =
        (insn_i == double_zero_norm)         ? ({24'b0, rs1[7:0]}) :
        (insn_i == synd_minus_vec_scaled) ? (mul) :
                                                32'd0;

    fp_double_zero_norm fp_double_zero_norm_inst (
        .x(input_norm),
        .y(output_norm)
    );

    assign fpred_opposite = output_norm ^ 16'h007F;
    assign adder2 =
        (insn_i == synd_minus_vec_scaled) ? fpred_opposite : mul;
   
    assign sum = adder2 + {16'b0, c};


    always @(*) begin
        case (insn_i)
            vec_by_matrix, restr_vec_by_matrix, vec_by_vec_pointwise: begin
                tmp1 = ((sum[15:0] & 16'h007F) + (sum[15:0] >> 7));
                tmp2 = ((tmp1 & 16'h007F) + (tmp1 >> 7));
            end
            synd_minus_vec_scaled: begin
                tmp1 = '0;
                tmp2 = ((sum[15:0] & 16'h007F) + (sum[15:0] >> 7));
            end
            double_zero_norm: begin
                tmp1 = '0;
                tmp2 = output_norm;
            end
            default: begin
                tmp1 = 16'b0;
                tmp2 = 16'b0;
            end
        endcase

        result = {tmp1,tmp2};
    end

endmodule
