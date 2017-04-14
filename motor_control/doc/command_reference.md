# Reference syntax

Syntax       |   Explanation
-------------|--------------------------------------------------------
`(c)`        |   'c' is optional, and does not affect syntax
`[c1,...,cn]`|   one of 'c1' to 'cn' is required
`<n>`        |   floating-point representation of 'n' is required

All other characters are required verbatim

# MDS Commands

Mnemonic              | Syntax
----------------------|-----------------------------------
set pa**R**ameters    | `GR[X,Y]:<P>,<I>,<D>,<S>(,)\n`
set **L**ocation      | `GL:<X>,<Y>(,)\n`
**S**tart             | `GS\n`
sto**P**              | `GP\n`
**C**alibrate         | `GC\n`
calibration **D**one    | `GD\n`

# MDS Responses

Mnemonic               | Syntax
-----------------------|-----------------------------------
mds **O**n               | `GO\n`
command **A**cknowledged | `GA\n`
**E**rror                | `GE:<Error>\n`
calibration l**I**mits   | `GI:<Xmin>,<Xmax>,<Ymin>,<Ymax>\n`
