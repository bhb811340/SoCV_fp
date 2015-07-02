//# 4 inputs
//# 1 outputs
//# 3 D-type flipflops
//# 2 inverters
//# 8 gates (1 ANDs + 1 NANDs + 2 ORs + 4 NORs)

module s27(CK,G0,G1,G17,G2,G3);
input CK,G0,G1,G2,G3;
output G17;

  wire G5,G10,G6,G11,G7,G13,G14,G8,G15,G12,G16,G9;

  dff DFF_0(CK,G5,G10);
  dff DFF_1(CK,G6,G11);
  dff DFF_2(CK,G7,G13);
  not (G14,G0);
  not (G17,G11);
  and (G8, G14, G6);
  or (G15,G12,G8);
  or (G16,G3,G8);
  nand (G9,G16,G15);
  nor (G10,G14,G11);
  nor (G11,G5,G9);
  nor (G12,G1,G7);
  nor (G13,G2,G12);

endmodule

