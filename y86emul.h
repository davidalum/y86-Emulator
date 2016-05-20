typedef enum{

  AOK,
  HLT,
  ADR,
INS

} number;
number status;



typedef struct {

  unsigned char upperHalf: 4; /*The upper half of the bitfield (the first four bits)*/
  unsigned char lowerHalf: 4; /*The lower half of the bitfield (the last four bits)*/

} bytes;
