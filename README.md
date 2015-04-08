# TDFW

An autonomous air-hockey table

# MDS

The Mallet Drive System ("MDS") is the low-level subsystem which controls the
table's mallet

## Interface

### Commands

Mnemonic              | Syntax                            
----------------------|-----------------------------------
Set pa**r**ameters    | `GR[X,Y]:<P>,<I>,<D>,<S>(,)\n`
Set **l**ocation      | `GL:<X>,<Y>(,)\n`
**S**tart             | `GS\n`
Sto**p**              | `GP\n`
**C**alibrate         | `GC\n`
Calibration **d**one  | `GD\n`

### Responses

Mnemonic                    | Syntax                            
----------------------------|-----------------------------------
MDS **o**n                  | `GO\n`
Command **a**cknowledged    | `GA\n`
**E**rror                   | `GE:<"error string">\n`
Calibration l**i**mits      | `GI:<Xmin>,<Xmax>,<Ymin>,<Ymax>\n`

### Syntax Reference

Syntax        |   Explanation
--------------|--------------------------------------------------------
`(c)`         | 'c' is optional, and does not affect syntax
`[c1,...,cn]` | one of 'c1' to 'cn' is required
`<n>`         | floating-point representation of 'n' is required
`<"string">`  | a string (perhaps including spaces) is required. Does not include quotation marks

All other characters are required verbatim
