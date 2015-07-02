// This file is generated by bench2vlog.py by littleshamoo
//   options: -l tsmc 
module c17 ( G1gat, G2gat, G3gat, G6gat, G7gat, G22gat, G23gat);

input G1gat;
input G2gat;
input G3gat;
input G6gat;
input G7gat;

output G22gat;
output G23gat;

wire G10gat, G11gat, G16gat, G19gat;


nand (G10gat, G1gat, G3gat);
nand (G11gat, G3gat, G6gat);
nand (G16gat, G2gat, G11gat);
nand (G19gat, G11gat, G7gat);
nand (G22gat, G10gat, G16gat);
nand (G23gat, G16gat, G19gat);

endmodule
