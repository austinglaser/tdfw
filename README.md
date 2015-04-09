# TDFW

An autonomous air-hockey table

# MDS

The Mallet Drive System ("MDS") is the low-level subsystem which controls the
table's mallet

## Interface

### Commands

Mnemonic              | Syntax                            
----------------------|-----------------------------------
Set pa**r**ameters    | `MR[X,Y]:<P>,<I>,<D>,<S>(,)\n`
Set **l**ocation      | `ML:<X>,<Y>(,)\n`
**S**tart             | `MS\n`
Sto**p**              | `MP\n`
**C**alibrate         | `MC\n`
Calibration **d**one  | `MD\n`

### Responses

Mnemonic                    | Syntax                            
----------------------------|-----------------------------------
MDS **o**n                  | `MO\n`
Command **a**cknowledged    | `MA\n`
**E**rror                   | `ME:<"error string">\n`
Calibration l**i**mits      | `MI:<Xmin>,<Xmax>,<Ymin>,<Ymax>\n`

### Syntax Reference

Syntax        |   Explanation
--------------|--------------------------------------------------------
`(c)`         | 'c' is optional, and does not affect syntax
`[c1,...,cn]` | one of 'c1' to 'cn' is required
`<n>`         | floating-point representation of 'n' is required
`<"string">`  | a string (perhaps including spaces) is required. Does not include quotation marks

All other characters are required verbatim
