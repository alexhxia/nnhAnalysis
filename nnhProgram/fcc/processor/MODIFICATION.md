# Modification between `ilcsoft` and `fcc`

## Marlin to Gaudi

I was using the tools in `https://github.com/key4hep/k4MarlinWrapper`
```
git clone https://github.com/key4hep/k4MarlinWrapper
```
The script `convertMarlinSteeringToGaudi.py`
```
python3 k4MarlinWrapper/k4MarlinWrapper/scripts/convertMarlinSteeringToGaudi.py /path/to/MarlinInputFile.xml /path/to/GaudiOutputFile.py
```
> Warning: you don't have a comment in Marlin file.
