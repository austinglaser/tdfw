# Reference syntax

Syntax      |   Explanation
------------|--------------------------------------------------------
(c)         |   'c' is optional, and does not affect syntax
[c1,...,cn] |   one of 'c1' to 'cn' is required
<n>         |   floating-point representation of 'n' is required

All other characters are required verbatim

# MDS Commands

Mnemonic              | Syntax                            
----------------------|-----------------------------------
set paRameters        | GR[X,Y]:<P>,<I>,<D>,<S>(,)\n      
set Location          | GL:<X>,<Y>(,)\n                   
Start                 | GS\n                              
stoP                  | GP\n                              
Calibrate             | GC\n                              
calibration Done      | GD\n                              

# MDS Responses

Mnemonic              | Syntax                            
----------------------|-----------------------------------
mds On                | GO\n
command Acknowledged  | GA\n                              
Error                 | GE:<Error>\n                      
calibration lImits    | GI:<Xmin>,<Xmax>,<Ymin>,<Ymax>\n  
